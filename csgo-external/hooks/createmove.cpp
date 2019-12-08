#include "createmove.h"

#include "hooks.h"
#include "../config.h"
#include "../sdk/misc/constants.h"
#include "../sdk/classes/defines.h"

#include <epic/shellcode.h>
#include <epic/vmt_helpers.h>
#include <crypto/string_encryption.h>


namespace hooks {
	uint32_t create_move_shellcode_addr = 0,
		trace_filter_addr = 0;

	// allocate and write shellcode then hook
	void hook_createmove() {
		using namespace sdk;

		const auto orig_create_move = mango::get_vfunc<uint32_t>(globals::process, globals::client_mode, indices::create_move);
		const auto con_msg = globals::process.get_proc_addr(enc_str("tier0.dll"), enc_str("?ConMsg@@YAXPBDZZ"));

		const std::string vector_str("vector: [%f, %f, %f]\n");
		const auto vector_str_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(vector_str.size() + 1)));
		globals::process.write(vector_str_addr, vector_str.data(), vector_str.size() + 1);

		const std::string hex_str("hex: %X\n");
		const auto hex_str_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(hex_str.size() + 1)));
		globals::process.write(hex_str_addr, hex_str.data(), hex_str.size() + 1);

		struct AimbotTraceFilter {
			// return (entity != this->m_ignore_entity)
			uint8_t m_should_hit_entity[19] = { 
				0x8B, 0x44, 0x24, 0x04,		// mov eax, [esp + 0x04] (arg_entity)
				0x3B, 0x41, 0x04,			// cmp eax, [ecx + 0x04] (this->m_ignore_entity)
				0x74, 0x05,					// je return_false
				0xB0, 0x01,					// mov al, 0x01
				0xC2, 0x08, 0x00,			// ret 0x08

				// return_false:
				0x30, 0xC0,					// xor al, al
				0xC2, 0x08, 0x00			// ret 0x08
			};

			// return 0 (TRACE_EVERYTHING)
			uint8_t m_get_trace_type[3] = { 
				0x31, 0xC0,					// xor eax, eax
				0xC3						// ret
			};

			// vtable stuffz
			uint32_t m_vtable[2] = { 0, 0 };
			uint32_t m_vtable_ptr = 0;

			// our traces will ignore this entity (usually localplayer)
			uint32_t m_ignore_entity = 0;
		};

		// this cant be on the stack cuz of noexecute
		trace_filter_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(sizeof(AimbotTraceFilter), PAGE_EXECUTE_READWRITE)));
		const auto trace_filter_thisptr = trace_filter_addr + offsetof(AimbotTraceFilter, m_vtable_ptr);

		// manually construct the vtable and functions for our tracefilter (i dont construct a RTTICompleteObjectLocator since it's unused (probably))
		{
			AimbotTraceFilter trace_filter;

			// set the vtable entries to point to our functions
			trace_filter.m_vtable[0] = trace_filter_addr + offsetof(AimbotTraceFilter, m_should_hit_entity);
			trace_filter.m_vtable[1] = trace_filter_addr + offsetof(AimbotTraceFilter, m_get_trace_type);

			// point to the vtable
			trace_filter.m_vtable_ptr = trace_filter_addr + offsetof(AimbotTraceFilter, m_vtable);

			// write
			globals::process.write(trace_filter_addr, trace_filter);
		}

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

		// transforms the vector in-place
		// void vectortransform(const vec3f* vector, const mat3x4* matrix)
		mango::Shellcode vectortransform_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == vector
			// [ebp + 0x0C] == matrix

			// xmm5 = vector[0]
			// xmm6 = vector[1]
			// xmm7 = vector[2]
			"\x8B\x45\x08",							// mov eax, [ebp + 0x08] (vector)
			"\xF3\x0F\x10\x28",						// movss xmm5, [eax] (vector[0])
			"\xF3\x0F\x10\x70\x04",					// movss xmm6, [eax + 0x04] (vector[1])
			"\xF3\x0F\x10\x78\x08",					// movss xmm7, [eax + 0x08] (vector[2])

			// set the iterator to 0
			"\x31\xC9",								// xor ecx, ecx

			// get the column
			// ebx = matrix[ecx]
			"\x89\xCB",								// mov ebx, ecx
			"\x6B\xDB\x10",							// imul ebx, 0x10
			"\x03\x5D\x0C",							// add ebx, [ebp + 0x0C] (matrix address)

			// xmm0 = column[0] * vector[0]
			"\xF3\x0F\x10\x03",						// movss xmm0, [ebx] (column[0])
			"\xF3\x0F\x59\xC5",						// mulss xmm0, xmm5 (vector[0])

			// xmm0 += column[1] * vector[1]
			"\xF3\x0F\x10\x4B\x04",					// movss xmm1, [ebx + 0x04] (column[1])
			"\xF3\x0F\x59\xCE",						// mulss xmm1, xmm6 (vector[1])
			"\xF3\x0F\x58\xC1",						// addss xmm0, xmm1

			// xmm0 += column[2] * vector[2]
			"\xF3\x0F\x10\x4B\x08",					// movss xmm1, [ebx + 0x08] (column[2])
			"\xF3\x0F\x59\xCF",						// mulss xmm1, xmm7 (vector[2])
			"\xF3\x0F\x58\xC1",						// addss xmm0, xmm1

			// xmm0 += column[3]
			"\xF3\x0F\x58\x43\x0C",					// addss xmm0, [ebx + 0x0C] (column[3])

			// vector[ecx] = xmm0
			"\x89\xCB",								// mov ebx, ecx
			"\x6B\xDB\x04",							// imul ebx, 0x04
			"\x03\x5D\x08",							// add ebx, [ebp + 0x08] (vector address)
			"\xF3\x0F\x11\x03",						// movss [ebx], xmm0

			// ++ecx
			// if (ecx < 3) jmp to loop start
			"\x41",									// inc ecx
			"\x83\xF9\x03",							// cmp ecx, 3
			"\x7C", uint8_t(						// jl (loop start)
				-int8_t(2 + 63)),													// FIX OFFSET

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// normalize angles in place
		// void vectornormalize(vec3f* angles)
		mango::Shellcode vectornormalize_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == angles
			// local variables:
			// [ebp - 0x04] == temp float
			// [ebp - 0x08] == temp float
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(float) +
				sizeof(float)),

			// ebx = angles
			"\x8B\x5D\x08",							// mov ebx, [ebp + 0x08] (angles)

			// set roll to 0.f
			"\xC7\x43\x08\x00\x00\x00\x00",			// mov [ebx + 0x08], 0.f (angles[2])

			// clamp pitch from [-89.f, 89.f]
			"\xF3\x0F\x10\x03",						// movss xmm0, [ebx] (angles[0])
			"\xC7\x45\xFC\x00\x00\xB2\xC2",			// mov [ebp - 0x04], -89.f
			"\xF3\x0F\x5F\x45\xFC",					// maxss xmm0, [ebp - 0x04]
			"\xC7\x45\xFC\x00\x00\xB2\x42",			// mov [ebp - 0x04], 89.f
			"\xF3\x0F\x5D\x45\xFC",					// minss xmm0, [ebp - 0x04]
			"\xF3\x0F\x11\x03",						// movss [ebx], xmm0 (angles[0])

			// normalize yaw from [-180.f, 180.f]
			"\xF3\x0F\x10\x43\x04",					// movss xmm0, [ebx + 0x04] (angles[1])
			"\xC7\x45\xF8\x00\x00\xB4\x43",			// mov [ebp - 0x08], 360.f

			// while (yaw > 180.f) yaw -= 360.f
			"\xC7\x45\xFC\x00\x00\x34\x43",			// mov [ebp - 0x04], 180.f
			"\x0F\x2E\x45\xFC",						// ucomiss xmm0, [ebp - 0x04] (180.f)
			"\xF3\x0F\x5C\x45\xF8",					// subss xmm0, [ebp - 0x08] (360.f)
			"\x77", uint8_t(						// ja (before the compare)
				-int8_t(2 + 9)),

			// while (yaw < 180.f) yaw += 360.f
			"\xC7\x45\xFC\x00\x00\x34\xC3",			// mov [ebp - 0x04], -180.f
			"\x0F\x2E\x45\xFC",						// ucomiss xmm0, [ebp - 0x04] (-180.f)
			"\xF3\x0F\x58\x45\xF8",					// addss xmm0, [ebp - 0x08] (360.f)
			"\x72", uint8_t(						// jb (before the compare)
				-int8_t(2 + 9)),

			// yaw = xmm0
			"\xF3\x0F\x11\x43\x04",					// movss [ebx + 0x04], xmm0 (angles[1])

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x4)
		);

		// void fixmovement(CUserCmd* cmd)
		mango::Shellcode fixmovement_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == cmd
			// local variables:
			// [ebp - 0x0C] == movement angles
			// [ebp - 0x18] == engine viewangles
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(mango::Vec3f) +
				sizeof(mango::Vec3f)),

			// [ebp - 0x0C] = vectorangle(cmd->movement)
			"\x8D\x45\xF4",							// lea eax, [ebp - 0x0C]
			"\x50",									// push eax
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (CUserCmd*)
			"\x8D\x40\x24",							// lea eax, [eax + 0x24] (movement)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectorangle
				vectornormalize_shellcode.size() +
				vectortransform_shellcode.size() +
				vectorangle_shellcode.size() +
				5 + 17)),															// FIX OFFSET

			// [ebp - 0x18] = Engine->GetViewAngles()
			"\x8D\x45\xE8",							// lea eax, [ebp - 0x18]
			"\x50",									// push eax
			"\xB9", uint32_t(						// mov ecx, engine_client
				interfaces::engine_client),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_view_angles offset)]
				indices::get_view_angles * 4),

			// xmm0 = cmd->viewangles[1]
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (CUserCmd*)
			"\xF3\x0F\x10\x40\x10",					// movss xmm0, [eax + 0x10]

			// xmm0 -= engine_angles[1]
			"\xF3\x0F\x5C\x45\xEC",					// subss xmm0, [ebp - 0x14]

			// xmm0 += movement_angle[1]
			"\xF3\x0F\x58\x45\xF8",					// addss xmm0, [ebp - 0x08]

			// xmm0 *= 0.01745329251f (pi / 180)
			"\x68\x35\xFA\x8E\x3C",					// push 0.01745329251f
			"\xF3\x0F\x59\x04\x24",					// mulss xmm0, [esp]
			"\x58",									// pop eax

			// xmm1 = cmd->movement[0] * cmd->movement[0]
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (CUserCmd*)
			"\xF3\x0F\x10\x48\x24",					// movss xmm1, [eax + 0x24] (cmd->movement[0])
			"\xF3\x0F\x59\xC9",						// mulss xmm1, xmm1

			// xmm2 = cmd->movement[1] * cmd->movement[1]
			"\xF3\x0F\x10\x50\x28",					// movss xmm2, [eax + 0x28] (cmd->movement[1])
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

			// mov CUserCmd* into eax
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (CUserCmd*)

			// cmd->movement[0] = xmm3
			"\xF3\x0F\x11\x58\x24",					// movss [eax + 0x24], xmm3

			// cmd->movement[1] = xmm2
			"\xF3\x0F\x11\x50\x28",					// movss [eax + 0x28], xmm2

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x4)
		);

		// void gethitboxpos(CBaseEntity* entity, int hitbox, vec3* position)
		mango::Shellcode gethitboxpos_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08]			== entity
			// [ebp + 0x0C]			== hitbox id
			// [ebp + 0x10]			== position
			// local variables:
			// [ebp - 0x04]			== matrix
			// [ebp - 0x10]			== min
			// [ebp - 0x1C]			== max
			// [ebp - 0x20]			== tmp float
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(uint32_t) +
				sizeof(mango::Vec3f) +
				sizeof(mango::Vec3f) +
				sizeof(float)),

			// get the model_t*
			"\x8B\x4D\x08",							// mov ecx, [ebp + 0x08] (entity)
			"\x83\xC1\x04",							// add ecx, 0x04
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_model offset)]
				indices::get_model * 4),

			// get the studiohdr_t*
			"\x50",									// push eax (model_t*)
			"\xB9", uint32_t(						// mov ecx, model_info
				interfaces::model_info),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_studio_hdr offset)]
				indices::get_studio_hdr * 4),

			// get the mstudiohitboxset_t*
			"\x03\x80", uint32_t(					// add eax, [eax + hitboxsetindex]
				offsetof(studiohdr_t, hitboxsetindex)),

			// finally, get the mstudiobbox_t*
			"\x03\x80", uint32_t(					// add eax, [eax + hitboxindex]
				offsetof(mstudiohitboxset_t, hitboxindex)),
			"\x8B\x55\x0C",							// mov edx, [ebp + 0x0C] (hitbox id)
			"\x69\xD2", uint32_t(					// imul edx, sizeof(mstudiobbox_t)
				sizeof(mstudiobbox_t)),
			"\x01\xD0",								// add eax, edx

			// min = bbmin
			"\x8D\x98", uint32_t(					// lea ebx, [eax + bbmin]
				offsetof(mstudiobbox_t, bbmin)),
			"\x8B\x13",								// mov edx, [ebx] (bbmin[0])
			"\x89\x55\xF0",							// mov [ebp - 0x10], edx (min[0])
			"\x8B\x53\x04",							// mov edx, [ebx + 0x04] (bbmin[1])
			"\x89\x55\xF4",							// mov [ebp - 0x0C], edx (min[1])
			"\x8B\x53\x08",							// mov edx, [ebx + 0x08] (bbmin[2])
			"\x89\x55\xF8",							// mov [ebp - 0x08], edx (min[2])

			// max = bbmax
			"\x8D\x98", uint32_t(					// lea ebx, [eax + bbmax]
				offsetof(mstudiobbox_t, bbmax)),
			"\x8B\x13",								// mov edx, [ebx] (bbmax[0])
			"\x89\x55\xE4",							// mov [ebp - 0x1C], edx (max[0])
			"\x8B\x53\x04",							// mov edx, [ebx + 0x04] (bbmax[1])
			"\x89\x55\xE8",							// mov [ebp - 0x18], edx (max[1])
			"\x8B\x53\x08",							// mov edx, [ebx + 0x08] (bbmax[2])
			"\x89\x55\xEC",							// mov [ebp - 0x14], edx (max[2])

			// eax = offset into bone matrix array
			"\x8B\x80", uint32_t(					// mov eax, [eax + bone]
				offsetof(mstudiobbox_t, bone)),
			"\x69\xC0", uint32_t(					// imul eax, sizeof(Matrix3x4f)
				sizeof(mango::Matrix3x4f)),

			// [ebp - 0x04] = bone matrix
			"\x8B\x5D\x08",							// mov ebx, [ebp + 0x08] (entity)
			"\x8B\x9B", uint32_t(					// mov ebx, [ebx + m_pBones]
				offsets::m_BoneAccessor + offsetof(BoneAccessor, m_pBones)),
			"\x01\xC3",								// add ebx, eax
			"\x89\x5D\xFC",							// mov [ebp - 0x04], ebx (matrix)

			// vectortransform(min)
			"\xFF\x75\xFC",							// push [ebp - 0x04] (matrix)
			"\x8D\x45\xF0",							// lea eax, [ebp - 0x10] (min)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectortransform
				fixmovement_shellcode.size() +
				vectornormalize_shellcode.size() +
				vectortransform_shellcode.size() +
				5 + 136)),															// FIX OFFSET

			// vectortransform(max)
			"\xFF\x75\xFC",							// push [ebp - 0x04] (matrix)
			"\x8D\x45\xE4",							// lea eax, [ebp - 0x1C] (max)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectortransform
				fixmovement_shellcode.size() +
				vectornormalize_shellcode.size() +
				vectortransform_shellcode.size() +
				5 + 148)),															// FIX OFFSET

			// ebx = &position
			"\x8B\x5D\x10",							// mov ebx, [ebp + 0x10] (position)

			// xmm1 = 0.5f
			"\xC7\x45\xE0\x00\x00\x00\x3F",			// mov [ebp - 0x20], 0.5f (tmp float)
			"\xF3\x0F\x10\x4D\xE0",					// movss xmm1, [ebp - 0x20]

			// position[0] = (min[0] + max[0]) * 0.5f
			"\xF3\x0F\x10\x45\xF0",					// movss xmm0, [ebp - 0x10] (min[0])
			"\xF3\x0F\x58\x45\xE4",					// addss xmm0, [ebp - 0x1C] (max[0])
			"\xF3\x0F\x59\xC1",						// mulss xmm0, xmm1 (0.5f)
			"\xF3\x0F\x11\x03",						// movss [ebx], xmm0 (position[0])

			// position[1] = (min[1] + max[1]) * 0.5f
			"\xF3\x0F\x10\x45\xF4",					// movss xmm0, [ebp - 0x0C] (min[1])
			"\xF3\x0F\x58\x45\xE8",					// addss xmm0, [ebp - 0x18] (max[1])
			"\xF3\x0F\x59\xC1",						// mulss xmm0, xmm1 (0.5f)
			"\xF3\x0F\x11\x43\x04",					// movss [ebx + 0x04], xmm0 (position[1])

			// position[2] = (min[2] + max[2]) * 0.5f
			"\xF3\x0F\x10\x45\xF8",					// movss xmm0, [ebp - 0x08] (min[2])
			"\xF3\x0F\x58\x45\xEC",					// addss xmm0, [ebp - 0x14] (max[2])
			"\xF3\x0F\x59\xC1",						// mulss xmm0, xmm1 (0.5f)
			"\xF3\x0F\x11\x43\x08",					// movss [ebx + 0x08], xmm0 (position[2])

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0xC)
		);

		// int getdamage(CBaseEntity* localplayer, CBaseEntity* entity, vec3* eyeposition, vec3* position)
		mango::Shellcode getdamage_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08]			== localplayer
			// [ebp + 0x0C]			== entity
			// [ebp + 0x10]			== eyeposition
			// [ebp + 0x14]			== position
			// local variables:
			// [ebp - 0x54]			== trace
			// [ebp - 0xA4] & -16	== ray (aligned to 16 bytes)
			"\x81\xEC", uint32_t(					// sub esp, (size of local variables)
				sizeof(trace_t) +
				sizeof(Ray_t) + 0xC), // the 0xC is padding, needed to align to 16-byte boundary (because of VectorAligned)

			// make AimbotTraceFilter::m_ignore_entity to point localplayer
			"\x8B\x45\x08",							// mov eax, [ebp + 0x08]
			"\xA3", uint32_t(						// mov trace_filter_addr.m_ignore_entity, eax
				trace_filter_addr + offsetof(AimbotTraceFilter, m_ignore_entity)),

			// initialize the ray
			"\x8D\x9D\x5C\xFF\xFF\xFF",				// lea ebx, [ebp - 0xA4]
			"\x83\xE3\xF0",							// and ebx, 0xFFFF'FFF0 (-16)

			// ray.m_IsRay = true
			// ray.m_IsSwept = true
			"\xC6\x83", uint32_t(					// mov [ebx + m_IsRay], 0x01
				offsetof(Ray_t, m_IsRay)), "\x01",
			"\xC6\x83", uint32_t(					// mov [ebx + m_IsSwept], 0x01
				offsetof(Ray_t, m_IsSwept)), "\x01",

			// ray.m_StartOffset[0] = 0
			// ray.m_StartOffset[1] = 0
			// ray.m_StartOffset[2] = 0
			"\xC7\x83", uint32_t(					// mov [ebx + m_StartOffset], 0
				offsetof(Ray_t, m_StartOffset)), "\x00\x00\x00\x00",
			"\xC7\x83", uint32_t(					// mov [ebx + m_StartOffset + 4], 0
				offsetof(Ray_t, m_StartOffset) + 4), "\x00\x00\x00\x00",
			"\xC7\x83", uint32_t(					// mov [ebx + m_StartOffset + 8], 0
				offsetof(Ray_t, m_StartOffset) + 8), "\x00\x00\x00\x00",

			// ray.m_Extents[0] = 0
			// ray.m_Extents[1] = 0
			// ray.m_Extents[2] = 0
			"\xC7\x83", uint32_t(					// mov [ebx + m_Extents], 0
				offsetof(Ray_t, m_Extents)), "\x00\x00\x00\x00",
			"\xC7\x83", uint32_t(					// mov [ebx + m_Extents + 4], 0
				offsetof(Ray_t, m_Extents) + 4), "\x00\x00\x00\x00",
			"\xC7\x83", uint32_t(					// mov [ebx + m_Extents + 8], 0
				offsetof(Ray_t, m_Extents) + 8), "\x00\x00\x00\x00",

			// ray.m_Start = eyeposition
			"\x8B\x45\x10",							// mov eax, [ebp + 0x10] (&eyeposition)
			"\x8B\x10",								// mov edx, [eax] (eyeposition[0])
			"\x89\x93", uint32_t(					// mov [ebx + m_Start], edx
				offsetof(Ray_t, m_Start)),
			"\x8B\x50\x04",							// mov edx, [eax + 4] (eyeposition[1])
			"\x89\x93", uint32_t(					// mov [ebx + m_Start + 4], edx
				offsetof(Ray_t, m_Start) + 4),
			"\x8B\x50\x08",							// mov edx, [eax + 8] (eyeposition[2])
			"\x89\x93", uint32_t(					// mov [ebx + m_Start + 8], edx
				offsetof(Ray_t, m_Start) + 8),

			// ray.m_Delta = position - ray.m_Start
			"\x8B\x45\x14",							// mov eax, [ebp + 0x14] (&position)
			"\xF3\x0F\x10\x00",						// movss xmm0, [eax] (position[0])
			"\xF3\x0F\x5C\x83", uint32_t(			// subss xmm0, [ebx + m_Start]
				offsetof(Ray_t, m_Start)),
			"\xF3\x0F\x11\x83", uint32_t(			// movss [ebx + m_Delta], xmm0
				offsetof(Ray_t, m_Delta)),
			"\xF3\x0F\x10\x40\x04",					// movss xmm0, [eax + 4] (position[1])
			"\xF3\x0F\x5C\x83", uint32_t(			// subss xmm0, [ebx + m_Start + 4]
				offsetof(Ray_t, m_Start) + 4),
			"\xF3\x0F\x11\x83", uint32_t(			// movss [ebx + m_Delta + 4], xmm0
				offsetof(Ray_t, m_Delta) + 4),
			"\xF3\x0F\x10\x40\x08",					// movss xmm0, [eax + 8] (position[2])
			"\xF3\x0F\x5C\x83", uint32_t(			// subss xmm0, [ebx + m_Start + 8]
				offsetof(Ray_t, m_Start) + 8),
			"\xF3\x0F\x11\x83", uint32_t(			// movss [ebx + m_Delta + 8], xmm0
				offsetof(Ray_t, m_Delta) + 8),

			// engine_trace->TraceRay(&ray, MASK_ALL, trace_filter_thisptr, &trace)
			"\x8D\x45\xAC",							// lea eax, [ebp - 0x54]
			"\x50",									// push eax (&trace)
			"\x68", uint32_t(						// push trace_filter_thisptr
				trace_filter_thisptr),
			"\x6A\xFF",								// push 0xFFFFFFFF (MASK_ALL)
			"\x8D\x85\x5C\xFF\xFF\xFF",				// lea eax, [ebp - 0xA4]
			"\x83\xE0\xF0",							// and eax, 0xFFFF'FFF0 (-16)
			"\x50",									// push eax (ray)
			"\xB9", uint32_t(						// mov ecx, engine_trace
				interfaces::engine_trace),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (trace_ray offset)]
				indices::trace_ray * 4),

			// set return value to 1
			"\xB8\x01\x00\x00\x00",					// mov eax, 1

			// ebx = &trace
			"\x8D\x5D\xAC",							// lea ebx, [ebp - 0x54]

			// check if trace.entity == entity
			"\x8B\x55\x0C",							// mov edx, [ebp + 0xC] (entity)
			"\x3B\x93", uint32_t(					// cmp edx, [ebx + entity]
				offsetof(trace_t, entity)),
			"\x74", uint8_t(						// je (past the eax = 0)
				2),

			// set return value to 0
			"\x31\xC0",								// xor eax, eax

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x10)
		);

		// returns the amount of damage and sets position
		// int getaimposition(CBaseEntity* localplayer, CBaseEntity* entity, vec3* position, eyeposition)
		mango::Shellcode getaimposition_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == entity
			// [ebp + 0x10] == position
			// [ebp + 0x14] == eyeposition
			// local variables:
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				0),

			// mov entity into eax
			"\xFF\x75\x10",							// push [ebp + 0x10] (position)
			"\x6A\x00",								// push 0 (HEAD_HITBOX)
			"\xFF\x75\x0C",							// push [ebp + 0x0C] (entity)
			"\xE8", uint32_t(-int32_t(				// call gethitboxpos
				gethitboxpos_shellcode.size() +
				getdamage_shellcode.size() +
				5 + 14)),															// FIX OFFSET

			// getdamage(localplayer, entity, eyeposition, position)
			"\xFF\x75\x10",							// push [ebp + 0x10] (position)
			"\xFF\x75\x14",							// push [ebp + 0x14] (eyeposition)
			"\xFF\x75\x0C",							// push [ebp + 0x0C] (entity)
			"\xFF\x75\x08",							// push [ebp + 0x08] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call getdamage
				getdamage_shellcode.size() +
				5 + 31)),															// FIX OFFSET

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x10)
		);

		// bool isaccurate(CBaseEntity* localplayer, CBaseEntity* target, CBaseCombatWeapon* weapon, vec3* eyeposition, vec3* aimposition)
		mango::Shellcode isaccurate_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == target
			// [ebp + 0x10] == weapon
			// [ebp + 0x14] == eyeposition
			// [ebp + 0x18] == aimposition
			// local variables:
			// [ebp - 0x04] == temp float
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(float)),

			// xmm0 = accuracy
			"\x8B\x4D\x10",							// mov ecx, [ebp + 0x10] (weapon)
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_inaccuracy offset)
				indices::get_inaccuracy * 4),
			"\xD9\x5D\xFC",							// fstp [ebp - 0x04]
			"\xF3\x0F\x10\x45\xFC",					// movss xmm0, [ebp - 0x04]
			
			// compare against our accuracy threshold
			"\xC7\x45\xFC", uint32_t(				// mov [ebp - 0x04], threshold
				*reinterpret_cast<uint32_t*>(&config::aimbot::accuracy_threshold)),
			"\x0F\x2E\x45\xFC",						// ucomiss xmm0, [ebp - 0x04]
			"\x76", uint8_t(						// jbe (return true)
				8),
			
			// return false
			"\xB0\x00",								// mov al, 0x00
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x14),

			// return true
			"\xB0\x01",								// mov al, 0x01
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x14)
		);

		// void runaimbot(CBaseEntity* localplayer, CUserCmd* cmd)
		mango::Shellcode runaimbot_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == cmd
			// local variables:
			// [ebp - 0x04] == mostdamage
			// [ebp - 0x08] == bestentity
			// [ebp - 0x14] == aimposition
			// [ebp - 0x20] == position
			// [ebp - 0x2C] == eyeposition
			// [ebp - 0x30] == activeweapon
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(int) +
				sizeof(uint32_t) +
				sizeof(mango::Vec3f) +
				sizeof(mango::Vec3f) +
				sizeof(mango::Vec3f) +
				sizeof(uint32_t)),

			// ebx = localplayer->m_hActiveWeapon
			"\x8B\x5D\x08",							// mov ebx, [ebp + 0x08] (localplayer)
			"\x8B\x9B", uint32_t(					// mov ebx, [ebx + m_hActiveWeapon]
				offsets::m_hActiveWeapon),

			// get the active weapon
			"\x81\xE3\xFF\x0F\x00\x00",				// and ebx, 0xFFF
			"\x53",									// push ebx
			"\xB9", uint32_t(						// mov ecx, client_entity_list
				interfaces::client_entity_list),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_client_entity offset)]
				indices::get_client_entity * 4),
			"\x89\x45\xD0",							// mov [ebp - 0x30], eax (activeweapon)

			// xmm0 = interval_per_tick
			"\xF3\x0F\x10\x05", uint32_t(			// movss xmm0, interval_per_tick
				globals::global_vars_base + offsetof(GlobalVarsBase, interval_per_tick)),

			// xmm1 = localplayer->m_nTickbase
			"\x8B\x5D\x08",							// mov ebx, [ebp + 0x08] (localplayer)
			"\xF3\x0F\x2A\x8B", uint32_t(			// cvtsi2ss xmm1, [ebx + m_nTickBase]
				offsets::m_nTickBase),

			// xmm0 *= xmm1
			"\xF3\x0F\x59\xC1",						// mulss xmm0, xmm1

			// ucomiss xmm0, localplayer->m_flNextPrimaryAttack
			"\x0F\x2E\x80", uint32_t(
				offsets::m_flNextPrimaryAttack),
			"\x0F\x83", uint32_t(					// jae (past return)
				8),

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8),

			// set mostdamage and bestentity to 0
			"\xC7\x45\xFC\x00\x00\x00\x00",			// mov [ebp - 0x4], 0
			"\xC7\x45\xF8\x00\x00\x00\x00",			// mov [ebp - 0x8], 0

			// copy localplayer m_vecOrigin into eyeposition local variable
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (localplayer)
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin]
				offsets::m_vecOrigin),
			"\x89\x55\xD4",							// mov [ebp - 0x2C], edx
			"\x8B\x90", uint32_t(					// mov edx, [eax + m_vecOrigin + 4]
				offsets::m_vecOrigin + 4),
			"\x89\x55\xD8",							// mov [ebp - 0x28], edx

			// make sure to also add m_vecViewOffset[2] to eye_position[2]
			"\xF3\x0F\x10\x80", uint32_t(			// movss xmm0, [eax + m_vecOrigin + 8]
				offsets::m_vecOrigin + 8),
			"\xF3\x0F\x58\x80", uint32_t(			// addss xmm0, [eax + m_vecViewOffset + 8]
				offsets::m_vecViewOffset + 8),
			"\xF3\x0F\x11\x45\xDC",					// movss [ebp - 0x24], xmm0

			// loop through every entity and call getaimposition (above the createmove shellcode)
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
				112),																// FIX OFFSET		

			// check if they're dead
			"\x83\xB8", uint32_t(					// cmp [eax + m_iHealth], 0
				offsets::m_iHealth), "\x00",
			"\x0F\x84", uint32_t(					// jz (end of loop)
				99),																// FIX OFFSET

			// check if they're dormant
			"\x80\xB8", uint32_t(					// cmp byte ptr [eax + m_bDormant], 0
				offsets::m_bDormant), "\x00",
			"\x0F\x85", uint32_t(					// jnz (end of loop)
				86),																// FIX OFFSET

			// check if immune
			"\x80\xB8", uint32_t(					// cmp byte ptr [eax + m_bGunGameImmunity], 0
				offsets::m_bGunGameImmunity), "\x00",
			"\x0F\x85", uint32_t(					// jnz (end of loop)
				73),																// FIX OFFSET

			// check if same team
			"\x8B\x55\x08",							// mov edx, [ebp + 0x8] (localplayer)
			"\x8B\x92", uint32_t(					// mov edx, [edx + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x3B\x90", uint32_t(					// cmp edx, [eax + m_iTeamNum]
				offsets::m_iTeamNum),
			"\x0F\x84", uint32_t(					// jz (end of loop)
				52),																// FIX OFFSET

			// save the entity onto the stack for use later
			"\x50",									// push eax

			// getaimposition(localplayer, entity, &position, &eyeposition)
			"\x8D\x55\xD4",							// lea edx, [ebp - 0x2C]
			"\x52",									// push edx				(&eyeposition)
			"\x8D\x55\xE0",							// lea edx, [ebp - 0x20]
			"\x52",									// push edx				(&position)
			"\x50",									// push eax				(entity)
			"\xFF\x75\x08",							// push [ebp + 0x08]	(localplayer)
			"\xE8", uint32_t(-int32_t(				// call getaimposition
				isaccurate_shellcode.size() +
				getaimposition_shellcode.size() +
				5 + 236)),															// FIX OFFSET

			// pop the entity into edx
			"\x5A",									// pop edx

			// if damage <= mostdamage, skip to end
			"\x3B\x45\xFC",							// cmp eax, [ebp - 0x04]
			"\x0F\x8E", uint32_t(					// jle (end of loop)
				24),																// FIX OFFSET

			// set mostdamage to damage
			"\x89\x45\xFC",							// mov [ebp - 0x04], eax

			// set bestentity to entity
			"\x89\x55\xF8",							// mov [ebp - 0x08], edx

			// set aimposition to position
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
				-int32_t(6 + 137)),													// FIX OFFSET

			// if bestentity is nullptr, return
			"\x83\x7D\xF8\x00",						// cmp [ebp - 0x08], 0
			"\x74", uint8_t(						// je (return false)
				24),

			// isaccurate(localplayer, bestentity, activeweapon, eyeposition, aimposition)
			"\xFF\x75\xEC",							// push [ebp - 0x14] (aimposition)
			"\xFF\x75\xD4",							// push [ebp - 0x2C] (eyeposition)
			"\xFF\x75\xD0",							// push [ebp - 0x30] (activeweapon)
			"\xFF\x75\xF8",							// push [ebp - 0x08] (bestentity)
			"\xFF\x75\x08",							// push [ebp + 0x08] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call isaccurate
				isaccurate_shellcode.size() +
				5 + 307)),															// FIX OFFSET

			// return if not accurate
			"\x84\xC0",								// test al, al
			"\x75", uint8_t(						// jnz (skip the return)
				8),

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8),
				// 324
			// position = eye_position
			"\x8B\x55\xD4",							// mov edx, [ebp - 0x2C]
			"\x89\x55\xE0",							// mov [ebp - 0x20], edx
			"\x8B\x55\xD8",							// mov edx, [ebp - 0x28]
			"\x89\x55\xE4",							// mov [ebp - 0x1C], edx
			"\x8B\x55\xDC",							// mov edx, [ebp - 0x24]
			"\x89\x55\xE8",							// mov [ebp - 0x18], edx
				// 342
			// position[0] = aimposition[0] - position[0]
			"\xF3\x0F\x10\x45\xEC",					// movss xmm0, [ebp - 0x14]
			"\xF3\x0F\x5C\x45\xE0",					// subss xmm0, [ebp - 0x20]
			"\xF3\x0F\x11\x45\xE0",					// movss [ebp - 0x20], xmm0

			// position[1] = aimposition[1] - position[1]
			"\xF3\x0F\x10\x45\xF0",					// movss xmm0, [ebp - 0x10]
			"\xF3\x0F\x5C\x45\xE4",					// subss xmm0, [ebp - 0x1C]
			"\xF3\x0F\x11\x45\xE4",					// movss [ebp - 0x1C], xmm0

			// position[2] = aimposition[2] - position[2]
			"\xF3\x0F\x10\x45\xF4",					// movss xmm0, [ebp - 0x0C]
			"\xF3\x0F\x5C\x45\xE8",					// subss xmm0, [ebp - 0x18]
			"\xF3\x0F\x11\x45\xE8",					// movss [ebp - 0x18], xmm0
				// 387
			// vectorangle(vector, vector)
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (cmd)
			"\x8D\x40\x0C",							// lea eax, [eax + 0xC] (viewangles)
			"\x50",									// push eax
			"\x8D\x45\xE0",							// lea eax, [ebp - 0x20] (position)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectorangle
				isaccurate_shellcode.size() +
				getaimposition_shellcode.size() +
				getdamage_shellcode.size() +
				gethitboxpos_shellcode.size() +
				fixmovement_shellcode.size() +
				vectornormalize_shellcode.size() +
				vectortransform_shellcode.size() +
				vectorangle_shellcode.size() +
				5 + 398)),															// FIX OFFSET

			// xmm1 = 2.f
			"\x68\x00\x00\x00\x40",					// push 2.f
			"\xF3\x0F\x10\x14\x24",					// movss xmm2, [esp]
			"\x58",									// pop eax

			// eax = localplayer
			// ebx = &cmd->viewangles
			"\x8B\x45\x08",							// mov eax, [ebp + 0x8] (localplayer)
			"\x8B\x5D\x0C",							// mov ebx, [ebp + 0xC] (cmd)
			"\x83\xC3\x0C",							// add ebx, 0xC (offsetof(CUserCmd, viewangles))

			// cmd->viewangles[0] -= localplayer->m_aimPunchAngle[0] * 2.f
			"\xF3\x0F\x10\x88", uint32_t(			// movss xmm1, [eax + m_aimPunchAngle]
				offsets::m_Local + offsets::m_aimPunchAngle),
			"\xF3\x0F\x59\xCA",						// mulss xmm1, xmm2
			"\xF3\x0F\x10\x03",						// movss xmm0, [ebx] (cmd->viewangles[0])
			"\xF3\x0F\x5C\xC1",						// subss xmm0, xmm1
			"\xF3\x0F\x11\x03",						// movss [ebx], xmm0 (cmd->viewangles[0])

			// cmd->viewangles[1] -= localplayer->m_aimPunchAngle[1] * 2.f
			"\xF3\x0F\x10\x88", uint32_t(			// movss xmm1, [eax + m_aimPunchAngle + 4]
				offsets::m_Local + offsets::m_aimPunchAngle + 4),
			"\xF3\x0F\x59\xCA",						// mulss xmm1, xmm2
			"\xF3\x0F\x10\x43\x04",					// movss xmm0, [ebx + 4] (cmd->viewangles[1])
			"\xF3\x0F\x5C\xC1",						// subss xmm0, xmm1
			"\xF3\x0F\x11\x43\x04",					// movss [ebx + 4], xmm0 (cmd->viewangles[1])

			// set IN_ATTACK button state to true
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (cmd)
			"\x83\x48\x30\x01",						// or [eax + 0x30], 0x1 (IN_ATTACK)

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// void runantiaim(CBaseEntity* localplayer, CUserCmd* cmd)
		mango::Shellcode runantiaim_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == cmd
			// local variables:
			// [ebp - 0x0C] == viewangles
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(mango::Vec3f)),

			// [ebp - 0x0C] = Engine->GetViewAngles()
			"\x8D\x45\xF4",							// lea eax, [ebp - 0x0C]
			"\x50",									// push eax
			"\xB9", uint32_t(						// mov ecx, engine_client
				interfaces::engine_client),
			"\x8B\x01",								// mov eax, [ecx]
			"\xFF\x90", uint32_t(					// call [eax + (get_view_angles offset)]
				indices::get_view_angles * 4),

			// viewangles[0] = 89.f
			"\xC7\x45\xF4\x00\x00\xB2\x42",			// mov [ebp - 0x0C], 89.f

			// xmm0 = viewangles[1]
			"\xF3\x0F\x10\x45\xF8",					// movss xmm0, [ebp - 0x08]

			// xmm0 += 180.f
			"\x68\x00\x00\x34\x43",					// push 180.f
			"\xF3\x0F\x58\x04\x24",					// addss [esp]
			"\x58",									// pop eax

			// viewangles[1] = xmm0
			"\xF3\x0F\x11\x45\xF8",					// movss [ebp - 0x08], xmm0

			// ebx = &cmd->viewangles
			"\x8B\x5D\x0C",							// mov ebx, [ebp + 0x0C]
			"\x83\xC3\x0C",							// add ebx, 0x0C

			// cmd->viewangles[0] = viewangles[0]
			"\x8B\x55\xF4",							// mov edx, [ebp - 0x0C]
			"\x89\x13",								// mov [ebx], edx

			// cmd->viewangles[1] = viewangles[1]
			"\x8B\x55\xF8",							// mov edx, [ebp - 0x08]
			"\x89\x53\x04",							// mov [ebx + 0x04], edx

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// void runbhop(CBaseEntity* localplayer, CUserCmd* cmd)
		mango::Shellcode runbhop_shellcode(
			mango::Shellcode::prologue<false>(),

			// arguments:
			// [ebp + 0x08] == localplayer
			// [ebp + 0x0C] == cmd

			// check if FL_ONGROUND flag is set
			"\x8B\x5D\x08",							// mov ebx, [ebp + 0x08]
			"\x8B\x93", uint32_t(					// mov edx, [ebx + m_fFlags]
				offsets::m_fFlags),
			"\x81\xE2", uint32_t(					// and edx, FL_ONGROUND
				1 << 0),
			"\x83\xFA\x00",							// cmp edx, 0
			"\x75", uint8_t(						// jne (end of function)
				13),

			// cmd->buttons &= ~IN_JUMP
			"\x8B\x5D\x0C",							// mov ebx, [ebp + 0x0C]
			"\x81\xA3", uint32_t(					// and [ebx + buttons], ~IN_JUMP
				offsetof(CUserCmd, buttons)),
			uint32_t(~(1 << 1)),

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// bool createmove(float frametime, CUserCmd* cmd)
		mango::Shellcode createmove_shellcode(
			mango::Shellcode::prologue<false>(),

			// call the original createmove
			"\xFF\x75\x0C",							// push [ebp + 0xC]
			"\xFF\x75\x08",							// push [ebp + 0x8]
			"\xB8", uint32_t(						// mov eax, orig_create_move
				orig_create_move),
			"\xFF\xD0",								// call eax

			// arguments:
			// [ebp + 0x08] == frametime
			// [ebp + 0x0C] == cmd
			// local variables:
			// [ebp - 0x04] == localplayer	
			"\x83\xEC", uint8_t(					// sub esp, (size of local variables)
				sizeof(uint32_t)),

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

			// runantiaim(localplayer, cmd)
			"\xFF\x75\x0C",							// push [ebp + 0xC] (cmd)
			"\xFF\x75\xFC",							// push [ebp - 0x4] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call runantiaim
				runbhop_shellcode.size() +
				runantiaim_shellcode.size() +
				5 + 70)),															// FIX OFFSET

			// runaimbot(localplayer, cmd)
			"\xFF\x75\x0C",							// push [ebp + 0xC] (cmd)
			"\xFF\x75\xFC",							// push [ebp - 0x4] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call runaimbot
				runbhop_shellcode.size() +
				runantiaim_shellcode.size() +
				runaimbot_shellcode.size() +
				5 + 81)),															// FIX OFFSET

			// runbhop(localplayer, cmd)
			"\xFF\x75\x0C",							// push [ebp + 0xC] (cmd)
			"\xFF\x75\xFC",							// push [ebp - 0x4] (localplayer)
			"\xE8", uint32_t(-int32_t(				// call runbhop
				runbhop_shellcode.size() +
				5 + 92)),															// FIX OFFSET

			// fixmovement(cmd)
			"\xFF\x75\x0C",							// push [ebp + 0xC] (cmd)
			"\xE8", uint32_t(-int32_t(				// call fixmovement
				runbhop_shellcode.size() +
				runantiaim_shellcode.size() +
				runaimbot_shellcode.size() +
				isaccurate_shellcode.size() +
				getaimposition_shellcode.size() +
				getdamage_shellcode.size() +
				gethitboxpos_shellcode.size() +
				fixmovement_shellcode.size() +
				5 + 100)),															// FIX OFFSET

			// normalize cmd->viewangles
			"\x8B\x45\x0C",							// mov eax, [ebp + 0xC] (cmd)
			"\x83\xC0\x0C",							// add eax, 0xC (viewangles offset)
			"\x50",									// push eax
			"\xE8", uint32_t(-int32_t(				// call vectornormalize
				runbhop_shellcode.size() +
				runantiaim_shellcode.size() +
				runaimbot_shellcode.size() +
				isaccurate_shellcode.size() +
				getaimposition_shellcode.size() +
				getdamage_shellcode.size() +
				gethitboxpos_shellcode.size() +
				fixmovement_shellcode.size() +
				vectornormalize_shellcode.size() +
				5 + 112)),															// FIX OFFSET

			// return false
			"\x31\xC0",								// xor eax, eax
			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(0x8)
		);

		// merge all the shellcode into one big blob :]
		mango::Shellcode total_shellcode(
			atan2_shellcode,
			vectorangle_shellcode,
			vectortransform_shellcode,
			vectornormalize_shellcode,
			fixmovement_shellcode,
			gethitboxpos_shellcode,
			getdamage_shellcode,
			getaimposition_shellcode,
			isaccurate_shellcode,
			runaimbot_shellcode,
			runantiaim_shellcode,
			runbhop_shellcode,
			createmove_shellcode
		);

		create_move_shellcode_addr = uint32_t(total_shellcode.allocate_and_write(globals::process));

		// hook the function
		hooks::client_mode_vmt_hook.hook(indices::create_move, create_move_shellcode_addr + 
			(total_shellcode.size() - createmove_shellcode.size()));

		mango::logger.success(enc_str("Hooked createmove: 0x"), std::hex, 
			std::uppercase, total_shellcode.size(), enc_str(" bytes written."));
	}

	// free memory and cleanup
	void release_createmove() {
		// free memory
		mango::Shellcode::free(sdk::globals::process, create_move_shellcode_addr);
		sdk::globals::process.free_virt_mem(trace_filter_addr);
	}
} // namespace hooks