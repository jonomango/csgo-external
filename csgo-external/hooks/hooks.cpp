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

		// returns the amount of damage and sets position
		// int __fastcall get_damage(void* entity, vec3* position)
		mango::Shellcode getdamage_shellcode(
			mango::Shellcode::ret(8)
		);

		// CreateMove hook
		mango::Shellcode createmove_shellcode(
			mango::Shellcode::prologue<false>(),

			// call the original createmove
			"\xFF\x75\x0C",						// push [ebp + 0xC]
			"\xFF\x75\x08",						// push [ebp + 0x8]
			"\xB8", orig_create_move,			// mov eax, orig_create_move
			"\xFF\xD0",							// call eax

			// stack space for local variables
			// [ebp - 0x04] == local_player (uint32_t)
			// [ebp - 0x08] == most_damage  (int)
			// [ebp - 0x0C] == best_entity  (uint32_t)
			// [ebp - 0x10] == aim_position	(vec3f)
			// [ebp - 0x1C] == position		(vec3f)
			"\x83\xEC", uint8_t(				// sub esp, (size of local variables)
				sizeof(uint32_t) +
				sizeof(int) + 
				sizeof(uint32_t) +
				sizeof(mango::Vec3f) + 
				sizeof(mango::Vec3f)),

			// get the localplayer index
			"\xB9", uint32_t(					// mov ecx, engine_client
				interfaces::engine_client),
			"\x8B\x01",							// mov eax, [ecx]
			"\xFF\x50", uint8_t(				// call [eax + get_local_player offset]
				indices::get_local_player_index * 4), 

			// set most_damage and best_entity to 0
			"\xC7\x45\xFC\x00\x00\x00\x00",		// mov [ebp - 0x8], 0
			"\xC7\x45\xF8\x00\x00\x00\x00",		// mov [ebp - 0xC], 0

			// loop through every entity and call getdamage (above the createmove shellcode)
			// ecx will hold the entity index
			"\xB9\x01\x00\x00\x00",				// mov ecx, 1

			// this is the start of the loop
			"\x51",								// push ecx

			// get the entity using IClientEntityList::GetClientEntity
			"\x51",								// push ecx (entity index)
			"\xB9", uint32_t(					// mov ecx, client_entity_list
				interfaces::client_entity_list),
			"\x8B\x01",							// mov eax, [ecx]
			"\xFF\x50", uint8_t(				// call [eax + (get_client_entity offset)]
				indices::get_client_entity * sizeof(uint32_t)),

			// check if entity is nullptr
			"\x85\xC0",							// test eax, eax
			"\x0F\x84", uint32_t(				// jz (end of loop)
				23),															// FIX OFFSET		

			// check if they're dead
			"\x83\xB8", uint32_t(				// cmp [eax + (health offset)], 0
				offsets::m_iHealth), "\x00",
			"\x0F\x84", uint32_t(				// jz (end of loop)
				10),															// FIX OFFSET		

			// call get_damage(entity, &position)
			"\x8D\x55\x18",						// lea edx, [ebp + 0x1C]
			"\x52",								// push edx (&position)
			"\x50",								// push eax (entity)
			"\xE8", uint32_t(					// call get_damage
				-int32_t(getdamage_shellcode.size() + 5 + 76)),					// FIX OFFSET

			// increment the index and jump to start if < 64
			"\x59",								// pop ecx
			"\x41",								// inc ecx
			"\x83\xF9\x40",						// cmp ecx, 64
			"\x0F\x8C", uint32_t(				// jl (start of loop)
				-(6 /* size of jmp opcode */ + 48)),							// FIX OFFSET

			// return false
			"\x31\xC0",							// xor eax, eax

			mango::Shellcode::epilogue<false>(),
			mango::Shellcode::ret(8)
		);

		// allocate shellcode
		create_move_shellcode_addr = uint32_t(uintptr_t(globals::process.alloc_virt_mem(
			getdamage_shellcode.size() + createmove_shellcode.size(), PAGE_EXECUTE_READWRITE)));

		// write shellcode
		getdamage_shellcode.write(globals::process, create_move_shellcode_addr);
		createmove_shellcode.write(globals::process, create_move_shellcode_addr + getdamage_shellcode.size());

		// hook the function
		client_mode_hook.hook(indices::create_move, create_move_shellcode_addr + getdamage_shellcode.size());
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