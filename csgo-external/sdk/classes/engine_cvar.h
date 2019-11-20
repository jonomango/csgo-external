#pragma once

#include "../misc/interface_base.h"

#include <optional>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier1/convar.h
	struct ConCommandBase {
	private:
		uint32_t m_con_command_base_vtable;
	public:
		uint32_t m_next;
	private:
		uint8_t m_padding_0[0x4];
	public:
		uint32_t m_name;
		uint32_t m_help;
		int m_flags;
	};

	struct ConVar : public ConCommandBase {
	private:
		uint32_t m_con_var_vtable;
		uint8_t m_padding_1[0x8];
	public:
		uint32_t m_value_str;
		int m_value_str_length;
		float m_value_float;
		int m_value_int;

		// small wrapper to decrypt values
		template <typename T>
		T get_value(const uint32_t key) const {
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
				return this->m_value_int ^ key;
			} else if constexpr (std::is_same_v<T, float>) {
				const auto decrypted = *reinterpret_cast<const uint32_t*>(&this->m_value_float) ^ key;
				return *reinterpret_cast<const float*>(&decrypted);
			} else {
				static_assert(false, "Type not supported.");
			}
		}
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/icvar.h
	class EngineCvar : public InterfaceBase<EngineCvar,
		mango::fnv1a<uint64_t>("vstdlib.dll:VEngineCvar")> {
	public:
		
		// return a pointer to the head of the (linked) list of commands
		uint32_t get_command_head() const;

		// get the address of the cvar
		uint32_t find_var(const std::string_view name) const;
	};
} // namespace sdk