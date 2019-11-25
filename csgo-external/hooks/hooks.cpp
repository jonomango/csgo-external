#include "hooks.h"

#include "../sdk/misc/constants.h"
#include "../sdk/classes/defines.h"


#include <epic/vmt_hook.h>
#include <epic/shellcode.h>

#include <thread>


namespace {
	mango::VmtHook client_mode_hook;

	uint32_t create_move_shellcode_addr;

	// IClientMode::CreateMove
	void hook_createmove() {
		using namespace sdk;

		const auto orig_create_move = globals::process.get_vfunc<uint32_t>(globals::client_mode, indices::create_move);

		// converts a direction to an angle
		// void vectorangle(vec3* direction, vec3* angle)
		mango::Shellcode vectorangle_shellcode(
			mango::Shellcode::prologue<false>(),

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// returns the amount of damage and sets position
		// int getdamage(void* localplayer, void* entity, vec3* position)
		mango::Shellcode getdamage_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == entity
			// [ebp + 0x10] == position
			// local variables:
			"\x83\xEC", uint8_t(					// sub esp, (size)
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
			"\x89\x53\x04",							// mov [ebx + 8], edx

			// return 1
			"\xB8\x01\x00\x00\x00",					// mov eax, 1
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0xC)
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
			// [ebp - 0x04] == localplayer			(uint32_t)
			// [ebp - 0x08] == most_damage			(int)
			// [ebp - 0x0C] == best_entity			(uint32_t)
			// [ebp - 0x10] == aim_position			(vec3f)
			// [ebp - 0x1C] == position/tmp vector	(vec3f)
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(uint32_t) +
				sizeof(int) + 
				sizeof(uint32_t) +
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
				11),

			// return true
			"\xB8\x01\x00\x00\x00",					// mov eax, 1
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(8),

			// store the localplayer in [ebp - 0x4]
			"\x89\x45\xFC",							// mov [ebp - 0x4], eax

			// set most_damage and best_entity to 0
			"\xC7\x45\xF8\x00\x00\x00\x00",			// mov [ebp - 0x8], 0
			"\xC7\x45\xF4\x00\x00\x00\x00",			// mov [ebp - 0xC], 0

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
			"\x8B\x55\xFC",							// mov edx, [ebp - 0x4]
			"\x8B\x92", uint32_t(					// mov edx, [edx + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x3B\x90", uint32_t(					// cmp edx, [eax + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x0F\x84", uint32_t(					// jz (end of loop)
				48),																// FIX OFFSET

			// save the entity onto the stack for use later
			"\x50",									// push eax

			// get_damage(entity, &position)
			"\x8D\x55\xE4",							// lea edx, [ebp - 0x1C]
			"\x52",									// push edx (&position)
			"\x50",									// push eax (entity)
			"\xFF\x75\xFC",							// push [ebp - 4] (localplayer)
			"\xE8", uint32_t(						// call get_damage
				-int32_t(getdamage_shellcode.size() + 5 + 175)),					// FIX OFFSET

			// pop the entity into edx
			"\x5A",									// pop edx

			// if damage <= most_damage, skip to end
			"\x3B\x45\xF8",							// cmp eax, [ebp - 0x08]
			"\x0F\x8E", uint32_t(					// jle (end of loop)
				24),																// FIX OFFSET

			// set most_damage to damage
			"\x89\x45\xF8",							// mov [ebp - 0x08], eax

			// set best_entity to entity
			"\x89\x55\xF4",							// mov [ebp - 0x0C], edx

			// set aim_position to position
			"\x8B\x45\xE4",							// mov eax, [ebp - 0x1C]
			"\x89\x45\xF0",							// mov [ebp - 0x10], eax
			"\x8B\x45\xE0",							// mov eax, [ebp - 0x20]
			"\x89\x45\xEC",							// mov [ebp - 0x14], eax
			"\x8B\x45\xDC",							// mov eax, [ebp - 0x24]
			"\x89\x45\xE8",							// mov [ebp - 0x18], eax

			// increment the index and jump to start if < 64
			"\x59",									// pop ecx
			"\x41",									// inc ecx
			"\x83\xF9\x40",							// cmp ecx, 64
			"\x0F\x8C", uint32_t(					// jl (start of loop)
				-int32_t(6 + 133)),													// FIX OFFSET

			// if best_entity is nullptr, return
			"\x83\x7D\xF4\x00",						// cmp [ebp - 0x0C], 0
			"\x75",	uint8_t(						// jne (past first return)
				11),

			// return true
			"\xB8\x01\x00\x00\x00",					// mov eax, 1
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(8),

			"\x8B\x45\xF4",							// mov eax, [ebp - 0x0C]
			"\xC6\x80", uint32_t(					// mov [eax + m_bSpotted], 1
				offsets::m_bSpotted), "\x01",

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(8)
		);

		// allocate shellcode
		create_move_shellcode_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(
			vectorangle_shellcode.size() + 
			getdamage_shellcode.size() + 
			createmove_shellcode.size(), PAGE_EXECUTE_READWRITE)));

		// vectorangle
		vectorangle_shellcode.write(globals::process, create_move_shellcode_addr);

		// getdamage
		getdamage_shellcode.write(globals::process, create_move_shellcode_addr + 
			vectorangle_shellcode.size());

		// createmove
		createmove_shellcode.write(globals::process, create_move_shellcode_addr + 
			vectorangle_shellcode.size() + 
			getdamage_shellcode.size());

		// hook the function
		client_mode_hook.hook(indices::create_move, create_move_shellcode_addr +
			vectorangle_shellcode.size() +
			getdamage_shellcode.size());
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