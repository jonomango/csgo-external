#include "hooks.h"

#include "../sdk/misc/constants.h"
#include "../sdk/classes/defines.h"

#include <epic/vmt_hook.h>
#include <epic/shellcode.h>
#include <crypto/string_encryption.h>

#include <thread>


namespace {
	mango::VmtHook client_mode_hook;

	uint32_t create_move_shellcode_addr;

	// IClientMode::CreateMove
	void hook_createmove() {
		using namespace sdk;

		const auto orig_create_move = globals::process.get_vfunc<uint32_t>(globals::client_mode, indices::create_move);
		const auto con_msg = globals::process.get_proc_addr(enc_str("tier0.dll"), enc_str("?ConMsg@@YAXPBDZZ"));

		const std::string vector_str("vector: [%f, %f, %f]\n");
		const auto vector_str_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(vector_str.size() + 1)));
		globals::process.write(vector_str_addr, vector_str.data(), vector_str.size() + 1);

		const std::string float_str("float: %f\n");
		const auto float_str_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(float_str.size() + 1)));
		globals::process.write(float_str_addr, float_str.data(), float_str.size() + 1);

		// void print_vector(vec3* vector)
		const auto print_vector = mango::Shellcode(
			mango::Shellcode::prologue<false>(),

			// mov the vector ptr into eax
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8]

			// convert vector[2] to double and push to stack 
			"\x83\xEC\x08",							// sub esp, 0x8
			"\xF3\x0F\x5A\x40\x08",					// cvtss2sd xmm0, [eax + 8]
			"\xF2\x0F\x11\x04\x24",					// movsd [esp], xmm0

			// convert vector[1] to double and push to stack 
			"\x83\xEC\x08",							// sub esp, 0x8
			"\xF3\x0F\x5A\x40\x04",					// cvtss2sd xmm0, [eax + 4]
			"\xF2\x0F\x11\x04\x24",					// movsd [esp], xmm0

			// convert vector[0] to double and push to stack 
			"\x83\xEC\x08",							// sub esp, 0x8
			"\xF3\x0F\x5A\x00",						// cvtss2sd xmm0, [eax]
			"\xF2\x0F\x11\x04\x24",					// movsd [esp], xmm0

			// call ConMsg()
			"\x68", uint32_t(						// push vector_str_addr
				vector_str_addr),
			"\xB8", uint32_t(						// mov eax, con_msg
				con_msg),
			"\xFF\xD0",								// call eax

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x4)
		).allocate_and_write(globals::process);


		// could have used fatan2 i guess but fuck x87 ya er
		// xmm0 == return value
		// xmm0 == left, xmm1 == right
		// float atan2(float left, float right)
		mango::Shellcode atan2_shellcode(
			mango::Shellcode::prologue<false>(),

			// a single variable on the stack
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(float)),

			// xmm0 == left
			// xmm1 == right
			// xmm2 == min(xmm6, xmm7) / max(xmm6, xmm7)
			// xmm3 == xmm2 * xmm2
			// xmm4 == temporary
			// xmm5 == temporary return value
			// xmm6 == abs(left)
			// xmm7 == abs(right)

			// make a mask in xmm5 to calculate the absolute value of left and right
			// basically just set the most significant to 0 (the sign bit)
			"\x66\x0F\x76\xED",						// pcmpeqd xmm5, xmm5

			// shift right 1 (drop the most significant bit)
			"\x66\x0F\x71\xD5\x01",					// psrlw  xmm5, 1

			// store abs value of xmm0 and xmm1 in xmm6 and xmm7
			"\xF3\x0F\x10\xF0",						// movss xmm6, xmm0
			"\xF3\x0F\x10\xF9",						// movss xmm7, xmm1
			"\x0F\x54\xF5",							// addps xmm6, xmm5
			"\x0F\x54\xFD",							// addps xmm7, xmm5

			// xmm2 = min(xmm6, xmm7)
			"\xF3\x0F\x10\xD6",						// movss xmm2, xmm6
			"\xF3\x0F\x5D\xD7",						// minss xmm2, xmm7

			// xmm3 = max(xmm6, xmm7)
			"\xF3\x0F\x10\xDE",						// movss xmm3, xmm6
			"\xF3\x0F\x5F\xDF",						// maxss xmm3, xmm7

			// xmm2 = xmm2 / xmm3
			"\xF3\x0F\x5E\xD3",						// divss xmm2, xmm3

			// xmm3 = xmm2 * xmm2
			"\xF3\x0F\x10\xDA",						// movss xmm3, xmm2
			"\xF3\x0F\x59\xDB",						// mulss xmm3, xmm3

			// xmm5 = xmm3
			"\xF3\x0F\x10\xEB",						// movss xmm5, xmm3

			// xmm5 *= -0.0464964749f
			"\xC7\x04\x24\x16\x73\x3E\xBD",			// mov [esp], -0.0464964749f
			"\xF3\x0F\x59\x2C\x24",					// mulss xmm5, [esp]

			// xmm5 += 0.15931422f
			"\xC7\x04\x24\x44\x23\x23\x3E",			// mov [esp], 0.15931422f
			"\xF3\x0F\x58\x2C\x24",					// addss xmm5, [esp]

			// xmm5 *= xmm3
			"\xF3\x0F\x59\xEB",						// mulss xmm5, xmm3

			// xmm5 -= 0.327622764f
			"\xC7\x04\x24\x2C\xBE\xA7\x3E",			// mov [esp], 0.327622764f
			"\xF3\x0F\x5C\x2C\x24",					// subss xmm5, [esp]

			// xmm5 *= xmm3
			"\xF3\x0F\x59\xEB",						// mulss xmm5, xmm3

			// xmm5 *= xmm2
			"\xF3\x0F\x59\xEA",						// mulss xmm5, xmm2

			// xmm5 += xmm2
			"\xF3\x0F\x58\xEA",						// addss xmm5, xmm2

			// if xmm7 < xmm6 { xmm5 = 1.5707963268f - xmm5 }
			"\x0F\x2E\xF7",							// ucomiss xmm6, xmm7
			"\x76", uint8_t(						// jbe (skip the next block)
				20),

			// xmm5 = 1.5707963268f - xmm5
			"\xF3\x0F\x10\xE5",						// movss xmm4, xmm5
			"\xC7\x04\x24\xDB\x0F\xC9\x3F",			// mov [esp], 1.5707963268f
			"\xF3\x0F\x10\x2C\x24",					// movss xmm5, [esp]
			"\xF3\x0F\x5C\xEC",						// subss xmm5, xmm4

			// if xmm1 < 0.f { xmm5 = 3.14159265359f - xmm5 }
			"\xC7\x04\x24\x00\x00\x00\x00",			// mov [esp], 0.f
			"\xF3\x0F\x10\x24\x24",					// mov xmm4, [esp]
			"\x0F\x2E\xE1",							// ucomiss xmm4, xmm1
			"\x76", uint8_t(						// jbe (skip the next block)
				20),

			// xmm5 = 3.14159265359f - xmm5
			"\xF3\x0F\x10\xE5",						// movss xmm4, xmm5
			"\xC7\x04\x24\xDB\x0F\x49\x40",			// mov [esp], 3.14159265359f
			"\xF3\x0F\x10\x2C\x24",					// movss xmm5, [esp]
			"\xF3\x0F\x5C\xEC",						// subss xmm5, xmm4

			// if xmm0 < 0.f { xmm5 = -xmm5 }
			"\xC7\x04\x24\x00\x00\x00\x00",			// mov [esp], 0.f
			"\xF3\x0F\x10\x24\x24",					// mov xmm4, [esp]
			"\x0F\x2E\xE0",							// ucomiss xmm4, xmm0
			"\x76", uint8_t(						// jbe (skip the next block)
				20),

			// xmm5 = -xmm5
			"\xF3\x0F\x10\xE5",						// movss xmm4, xmm5
			"\xC7\x04\x24\x00\x00\x00\x00",			// mov [esp], 0.f
			"\xF3\x0F\x10\x2C\x24",					// movss xmm5, [esp]
			"\xF3\x0F\x5C\xEC",						// subss xmm5, xmm4

			// xmm0 = xmm5
			"\xF3\x0F\x10\xC5",						// movss xmm0, xmm5

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret()
		);

		// converts a direction to angles
		// void vectorangle(vec3* direction, vec3* angles)
		mango::Shellcode vectorangle_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == direction
			// [ebp + 0x0C] == angles
			// local variables:
			// [ebp - 0x0C] == direction (copy)
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(mango::Vec3f)),

			// copy the direction argument into the direction local variable
			"\x8B\x45\x08",							// mov eax, [ebp + 0x08]
			"\x8B\x10",								// mov edx, [eax]
			"\x89\x55\xF4",							// mov [ebp - 0x0C], edx
			"\x8B\x50\x04",							// mov edx, [eax + 4]
			"\x89\x55\xF8",							// mov [ebp - 0x08], edx
			"\x8B\x50\x08",							// mov edx, [eax + 8]
			"\x89\x55\xFC",							// mov [ebp - 0x04], edx

			// if (direction[0] == 0.f)
			"\x83\x7D\xF4\x00",						// cmp [ebp - 0x0C], 0
			"\x75", uint8_t(						// jne (skip the next check)
				46),

			// if (direction[1] == 0.f)
			"\x83\x7D\xF8\x00",						// cmp [ebp - 0x08], 0
			"\x75", uint8_t(						// jne (skip the ret)
				40),

			// address of angles in eax
			"\x8B\x45\x0C",							// mov eax, [ebp + 0x0C]

			// angles[1] = 0.f, angles[2] = 0.f
			"\xC7\x40\x04\x00\x00\x00\x00",			// mov [eax + 4], 0
			"\xC7\x40\x08\x00\x00\x00\x00",			// mov [eax + 8], 0

			// +90 if direction[2] is positive, else negative 90
			"\x8B\x55\xFC",							// mov edx, [ebp - 0x4]

			// copy the sign mask into edx
			"\x81\xE2\x00\x00\x00\x80",				// and edx, 0x80000000 (1 << 31)

			// mov 90.f into angles[0] and copy the sign bit from direction[2]
			"\xC7\x00\x00\x00\xB4\x42",				// mov [eax], 90.f
			"\x09\x10",								// or [eax], edx

			// return
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8),

			// xmm0 = direction[1]
			// xmm1 = direction[0]
			"\xF3\x0F\x10\x45\xF8",					// movss xmm0, [ebp - 0x8]
			"\xF3\x0F\x10\x4D\xF4",					// movss xmm1, [ebp - 0xC]
			"\xE8", uint32_t(-int32_t(				// call atan2
				atan2_shellcode.size() +
				5 + 88)),

			// change result to degrees
			"\x68\xE1\x2E\x65\x42",					// push 57.2957795131f
			"\xF3\x0F\x59\x04\x24",					// mulss xmm0, [esp]
			"\x58",									// pop eax (cleanup the stack)

			// angles[1] = result
			"\x8B\x45\x0C",							// mov eax, [ebp + 0x0C]
			"\xF3\x0F\x11\x40\x04",					// movss [eax + 4], xmm0

			// xmm0 = direction[0] * direction[0]
			"\xF3\x0F\x10\x45\xF4",					// movss xmm0, [ebp - 0xC]
			"\xF3\x0F\x59\xC0",						// mulss xmm0, xmm0

			// xmm1 = direction[1] * direction[1]
			"\xF3\x0F\x10\x4D\xF8",					// movss xmm1, [ebp - 0x8]
			"\xF3\x0F\x59\xC9",						// mulss xmm1, xmm1

			// xmm0 += xmm1
			"\xF3\x0F\x58\xC1",						// addss xmm0, xmm1

			// xmm0 = sqrt(xmm0)
			"\xF3\x0F\x51\xC0",						// sqrtss xmm0, xmm0

			// xmm1 = xmm0
			"\xF3\x0F\x10\xC8",						// movss xmm1, xmm0

			// xmm0 = -direction[2]
			"\x0F\x57\xC0",							// xorps xmm0, xmm0
			"\xF3\x0F\x5C\x45\xFC",					// subss xmm0, [ebp - 0x4]

			// atan2(-direction[2], xmm1)
			"\xE8", uint32_t(-int32_t(				// call atan2
				atan2_shellcode.size() +
				5 + 150)),

			// change result to degrees
			"\x68\xE1\x2E\x65\x42",					// push 57.2957795131f
			"\xF3\x0F\x59\x04\x24",					// mulss xmm0, [esp]
			"\x58",									// pop eax (cleanup the stack)

			// angles[0] = result
			"\x8B\x45\x0C",							// mov eax, [ebp + 0x0C]
			"\xF3\x0F\x11\x00",						// movss [eax], xmm0

			// angles[2] = 0
			"\xC7\x40\x08\x00\x00\x00\x00",			// mov [eax + 0x8], 0

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// void fixmovement(CUserCmd* cmd)
		mango::Shellcode fixmovement_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == cmd
			// local variables:
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				0),

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x4)
		);

		// returns the amount of damage and sets position
		// int getdamage(CBaseEntity* localplayer, CBaseEntity* entity, vec3* position)
		mango::Shellcode getdamage_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == entity
			// [ebp + 0x10] == position
			// local variables:
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				0),

			// mov entity into eax
			"\x8B\x45\x0C",							// mov eax, [ebp + 0x0C]

			// mov position ptr into ebx
			"\x8B\x5D\x10"							// mov ebx, [ebp + 0x10]
			
			// copy m_vecOrigin[0] into position[0]
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin]
				offsets::m_vecOrigin),
			"\x89\x13",								// mov [ebx], edx
			
			// copy m_vecOrigin[1] into position[1]
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin + 4]
				offsets::m_vecOrigin + 4),
			"\x89\x53\x04",							// mov [ebx + 4], edx
			
			// copy m_vecOrigin[2] into position[2]
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin + 8]
				offsets::m_vecOrigin + 8),
			"\x89\x53\x08",							// mov [ebx + 8], edx

			// add 35.f to the z component
			"\x68", uint32_t(0x420c0000),			// push 35.f
			"\xF3\x0F\x10\x43\x08",					// movss xmm0, [ebx + 8]
			"\xF3\x0F\x58\x04\x24",					// addss xmm0, [esp]
			"\xF3\x0F\x11\x43\x08",					// movss [ebx + 8], xmm0
			"\x58",									// pop eax

			// return 1
			"\xB8\x01\x00\x00\x00",					// mov eax, 1
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0xC)
		);

		// void runaimbot(CBaseEntity* localplayer, CUserCmd* cmd)
		mango::Shellcode runaimbot_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == cmd
			// local variables:
			// [ebp - 0x04] == most_damage
			// [ebp - 0x08] == best_entity
			// [ebp - 0x14] == aim_position
			// [ebp - 0x20] == position
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(int) + 
				sizeof(uint32_t) +
				sizeof(mango::Vec3f) + 
				sizeof(mango::Vec3f)),

			// set most_damage and best_entity to 0
			"\xC7\x45\xFC\x00\x00\x00\x00",			// mov [ebp - 0x4], 0
			"\xC7\x45\xF8\x00\x00\x00\x00",			// mov [ebp - 0x8], 0

			// loop through every entity and call getdamage (above the createmove shellcode)
			// ecx will hold the entity index
			"\xB9\x01\x00\x00\x00",					// mov ecx, 1
			
			// this is the start of the loop
			"\x51",									// push ecx

			// get the entity address
			"\x51",									// push ecx (entity index)
			"\xB9", uint32_t(						// mov ecx, client_entity_list
				interfaces::client_entity_list),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x50", uint8_t(					// call [eax + (get_client_entity offset)]
				indices::get_client_entity * sizeof(uint32_t)),

			// check if entity is nullptr
			"\x85\xC0",								// test eax, eax
			"\x0F\x84", uint32_t(					// jz (end of loop)
				108),																// FIX OFFSET		

			// check if they're dead
			"\x83\xB8", uint32_t(					// cmp [eax + m_iHealth], 0
				offsets::m_iHealth), "\x00",
			"\x0F\x84", uint32_t(					// jz (end of loop)
				95),																// FIX OFFSET

			// check if they're dormant
			"\x80\xB8", uint32_t(					// cmp byte ptr [eax + m_bDormant], 0
				offsets::m_bDormant), "\x00",
			"\x0F\x85", uint32_t(					// jnz (end of loop)
				82),																// FIX OFFSET

			// check if immune
			"\x80\xB8", uint32_t(					// cmp byte ptr [eax + m_bGunGameImmunity], 0
				offsets::m_bGunGameImmunity), "\x00",
			"\x0F\x85", uint32_t(					// jnz (end of loop)
				69),																// FIX OFFSET

			// check if same team
			"\x8B\x55\x08",							// mov edx, [ebp + 0x8] (localplayer)
			"\x8B\x92", uint32_t(					// mov edx, [edx + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x3B\x90", uint32_t(					// cmp edx, [eax + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x0F\x84", uint32_t(					// jz (end of loop)
				48),																// FIX OFFSET

			// save the entity onto the stack for use later
			"\x50",									// push eax

			// get_damage(entity, &position)
			"\x8D\x55\xE0",							// lea edx, [ebp - 0x20]
			"\x52",									// push edx				(&position)
			"\x50",									// push eax				(entity)
			"\xFF\x75\x08",							// push [ebp + 0x08]	(localplayer)
			"\xE8", uint32_t(-int32_t(				// call get_damage
				getdamage_shellcode.size() +
				5 + 114)),															// FIX OFFSET

			// pop the entity into edx
			"\x5A",									// pop edx

			// if damage <= most_damage, skip to end
			"\x3B\x45\xFC",							// cmp eax, [ebp - 0x04]
			"\x0F\x8E", uint32_t(					// jle (end of loop)
				24),																// FIX OFFSET

			// set most_damage to damage
			"\x89\x45\xFC",							// mov [ebp - 0x04], eax

			// set best_entity to entity
			"\x89\x55\xF8",							// mov [ebp - 0x08], edx

			// set aim_position to position
			"\x8B\x45\xE0",							// mov eax, [ebp - 0x20]
			"\x89\x45\xEC",							// mov [ebp - 0x14], eax
			"\x8B\x45\xE4",							// mov eax, [ebp - 0x1C]
			"\x89\x45\xF0",							// mov [ebp - 0x10], eax
			"\x8B\x45\xE8",							// mov eax, [ebp - 0x18]
			"\x89\x45\xF4",							// mov [ebp - 0x0C], eax

			// increment the index and jump to start if < 64
			"\x59",									// pop ecx
			"\x41",									// inc ecx
			"\x83\xF9\x40",							// cmp ecx, 64
			"\x0F\x8C", uint32_t(					// jl (start of loop)
				-int32_t(6 + 133)),													// FIX OFFSET

			// if best_entity is nullptr, return
			"\x83\x7D\xF8\x00",						// cmp [ebp - 0x08], 0
			"\x75", uint8_t(						// jne (past first return)
				8),

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8),

			// copy localplayer m_vecOrigin into position local variable
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (localplayer)
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin]
				offsets::m_vecOrigin),
			"\x89\x55\xE0",							// mov [ebp - 0x20], edx
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin + 4]
				offsets::m_vecOrigin + 4),
			"\x89\x55\xE4",							// mov [ebp - 0x1C], edx
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin + 8]
				offsets::m_vecOrigin + 8),
			"\x89\x55\xE8",							// mov [ebp - 0x18], edx

			// add m_vecViewOffset[2] to position[2] (eax is still localplayer at this point)
			"\xF3\x0F\x10\x45\xE8",					// movss xmm0, [ebp - 0x18]
			"\xF3\x0F\x58\x80", uint32_t(			// addss xmm0, [eax + m_vecViewOffset + 8]
				offsets::m_vecViewOffset + 8),
			"\xF3\x0F\x11\x45\xE8",					// movss [ebp - 0x18], xmm0

			// position[0] = aim_position[0] - position[0]
			"\xF3\x0F\x10\x45\xEC",					// movss xmm0, [ebp - 0x14]
			"\xF3\x0F\x5C\x45\xE0",					// subss xmm0, [ebp - 0x20]
			"\xF3\x0F\x11\x45\xE0",					// movss [ebp - 0x20], xmm0
			
			// position[1] = aim_position[1] - position[1]
			"\xF3\x0F\x10\x45\xF0",					// movss xmm0, [ebp - 0x10]
			"\xF3\x0F\x5C\x45\xE4",					// subss xmm0, [ebp - 0x1C]
			"\xF3\x0F\x11\x45\xE4",					// movss [ebp - 0x1C], xmm0
			
			// position[2] = aim_position[2] - position[2]
			"\xF3\x0F\x10\x45\xF4",					// movss xmm0, [ebp - 0x0C]
			"\xF3\x0F\x5C\x45\xE8",					// subss xmm0, [ebp - 0x18]
			"\xF3\x0F\x11\x45\xE8",					// movss [ebp - 0x18], xmm0

			// vectorangle(vector, vector)
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			"\x8D\x40\x0C",							// lea eax, [eax + 0xC] (viewangles)
			"\x50",									// push eax
			"\x8D\x45\xE0",							// lea eax, [ebp - 0x20] (position)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectorangle
				vectorangle_shellcode.size() +
				fixmovement_shellcode.size() +
				getdamage_shellcode.size() +
				5 + 282)),															// FIX OFFSET

			// set IN_ATTACK button state to true
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			"\x83\x48\x30\x01",						// or [eax + 0x30], 0x1 (IN_ATTACK)

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// CreateMove hook
		mango::Shellcode createmove_shellcode(

			mango::Shellcode::prologue<false>(),

			// call the original createmove
			"\xFF\x75\x0C",							// push [ebp + 0xC]
			"\xFF\x75\x08",							// push [ebp + 0x8]
			"\xB8", orig_create_move,				// mov eax, orig_create_move
			"\xFF\xD0",								// call eax

			// local variables
			// [ebp - 0x04] == localplayer	
			// [ebp - 0x08] == most_damage	
			// [ebp - 0x0C] == best_entity	
			// [ebp - 0x18] == aim_position	
			// [ebp - 0x24] == tmp vector	
			// [ebp - 0x30] == tmp vector	
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(uint32_t) +
				sizeof(int) + 
				sizeof(uint32_t) +
				sizeof(mango::Vec3f) + 
				sizeof(mango::Vec3f) + 
				sizeof(mango::Vec3f)),

			// get the localplayer index
			"\xB9", uint32_t(						// mov ecx, engine_client
				interfaces::engine_client),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x50", uint8_t(					// call [eax + get_local_player offset]
				indices::get_local_player_index * 4), 

			// get the localplayer
			"\x50",									// push eax (localplayer index)
			"\xB9", uint32_t(						// mov ecx, client_entity_list
				interfaces::client_entity_list),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x50", uint8_t(					// call [eax + (get_client_entity offset)]
				indices::get_client_entity * sizeof(uint32_t)),

			// check if localplayer is dead
			"\x83\xB8", uint32_t(					// cmp [eax + m_iHealth], 0
				offsets::m_iHealth), "\x00",
			"\x0F\x85", uint32_t(					// jnz (skip the return)
				8),

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8),

			// store the localplayer in [ebp - 0x4]
			"\x89\x45\xFC",							// mov [ebp - 0x4], eax

			// AIMBOT:
			"\xFF\x75\x0C",							// push [ebp + 0xC] (cmd)
			"\xFF\x75\xFC",							// push [ebp - 0x4] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call runaimbot
				runaimbot_shellcode.size() +
				5 + 70)),															// FIX OFFSET

			// MOVEMENT FIX:	
			// [ebp - 0x24] = vectorangle(cmd->movement)
			"\x8D\x45\xDC",							// lea eax, [ebp - 0x24]
			"\x50",									// push eax
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			"\x8D\x40\x24",							// lea eax, [eax + 0x24] (movement)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectorangle
				vectorangle_shellcode.size() +
				fixmovement_shellcode.size() +
				getdamage_shellcode.size() +
				runaimbot_shellcode.size() +
				5 + 86)),															// FIX OFFSET
				
			// [ebp - 0x30] = Engine->GetViewAngles()
			"\x8D\x45\xD0",							// lea eax, [ebp - 0x30]
			"\x50",									// push eax
			"\xB9", uint32_t(						// mov ecx, engine_client
				interfaces::engine_client),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_view_angles offset)]
				indices::get_view_angles * 4),
			
			// xmm0 = cmd->viewangles[1]
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			"\xF3\x0F\x10\x40\x10",					// movss xmm0, [eax + 0x10]
			
			// xmm0 -= engine_angles[1]
			"\xF3\x0F\x5C\x45\xD4",					// subss xmm0, [ebp - 0x2C]
			
			// xmm0 += movement_angle[1]
			"\xF3\x0F\x58\x45\xE0",					// addss xmm0, [ebp - 0x20]
			
			// xmm0 *= 0.01745329251f (pi / 180)
			"\x68\x35\xFA\x8E\x3C",					// push 0.01745329251f
			"\xF3\x0F\x59\x04\x24",					// mulss xmm0, [esp]
			"\x58",									// pop eax
			
			// xmm1 = cmd->movement[0] * cmd->movement[0]
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			"\xF3\x0F\x10\x48\x24",					// movss xmm1, [eax + 0x24] (movement[0])
			"\xF3\x0F\x59\xC9",						// mulss xmm1, xmm1
			
			// xmm2 = cmd->movement[1] * cmd->movement[1]
			"\xF3\x0F\x10\x50\x28",					// movss xmm2, [eax + 0x28] (movement[1])
			"\xF3\x0F\x59\xD2",						// mulss xmm2, xmm2
			
			// xmm1 += xmm2
			"\xF3\x0F\x58\xCA",						// addss xmm1, xmm2
			
			// xmm2 = sqrt(xmm1)
			"\xF3\x0F\x51\xD1",						// sqrtss xmm2, xmm1
			
			// allocate space on stack
			"\x50",									// push eax
			
			// calculate the sin and cos of angle
			"\xF3\x0F\x11\x04\x24",					// movss [esp], xmm0
			"\xD9\x04\x24",							// fld [esp]
			"\xD9\xFB",								// fsincos
			
			// cos in xmm1
			"\xD9\x1C\x24",							// fstp [esp]
			"\xF3\x0F\x10\x0C\x24",					// movss xmm1, [esp]
			
			// sin in xmm0
			"\xD9\x1C\x24",							// fstp [esp]
			"\xF3\x0F\x10\x04\x24",					// movss xmm0, [esp]
			
			// clean up stack
			"\x58",									// pop eax
			
			// xmm3 = xmm2 * xmm1(cos)
			"\xF3\x0F\x10\xDA",						// movss xmm3, xmm2
			"\xF3\x0F\x59\xD9",						// mulss xmm3, xmm1
			
			// xmm2 *= xmm0(sin)
			"\xF3\x0F\x59\xD0",						// mulss xmm2, xmm0
			
			// eax holds CUserCmd*
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (CUserCmd*)
			
			// cmd->movement[0] = xmm3
			"\xF3\x0F\x11\x58\x24",					// movss [eax + 0x24], xmm3
			
			// cmd->movement[1] = xmm2
			"\xF3\x0F\x11\x50\x28",					// movss [eax + 0x28], xmm2
			
			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// allocate shellcode
		create_move_shellcode_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(
			atan2_shellcode.size() +
			vectorangle_shellcode.size() + 
			getdamage_shellcode.size() + 
			createmove_shellcode.size(), PAGE_EXECUTE_READWRITE)));

		// atan2
		atan2_shellcode.write(globals::process, create_move_shellcode_addr);

		// vectorangle
		vectorangle_shellcode.write(globals::process, create_move_shellcode_addr 
			+ atan2_shellcode.size());

		// fixmovement
		fixmovement_shellcode.write(globals::process, create_move_shellcode_addr +
			atan2_shellcode.size() +
			vectorangle_shellcode.size());

		// getdamage
		getdamage_shellcode.write(globals::process, create_move_shellcode_addr + 
			atan2_shellcode.size() +
			vectorangle_shellcode.size() +
			fixmovement_shellcode.size());

		// runaimbot
		runaimbot_shellcode.write(globals::process, create_move_shellcode_addr +
			atan2_shellcode.size() +
			vectorangle_shellcode.size() +
			fixmovement_shellcode.size() +
			getdamage_shellcode.size());

		// createmove
		createmove_shellcode.write(globals::process, create_move_shellcode_addr + 
			atan2_shellcode.size() +
			vectorangle_shellcode.size() + 
			fixmovement_shellcode.size() +
			getdamage_shellcode.size() +
			runaimbot_shellcode.size());

		// hook the function
		client_mode_hook.hook(indices::create_move, create_move_shellcode_addr +
			atan2_shellcode.size() +
			vectorangle_shellcode.size() +
			fixmovement_shellcode.size() +
			getdamage_shellcode.size() +
			runaimbot_shellcode.size());
	}
} // namespace

namespace hooks {
	// hook stuff and shibble
	void hook() {
		client_mode_hook.setup(sdk::globals::process, sdk::globals::client_mode);

		// start hookin!
		hook_createmove();
	}

	// when we're done
	void release() {
		client_mode_hook.release();

		// sleep before freeing memory
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		// free memory
		sdk::globals::process.free_virt_mem(create_move_shellcode_addr);
	}
} // namespace hooks