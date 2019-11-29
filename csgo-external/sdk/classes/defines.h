#pragma once

#include <stdint.h>

#include <misc/color.h>
#include <misc/vector.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/dt_common.h#L111
	enum SendPropType : uint32_t {
		DPT_Int = 0,
		DPT_Float,
		DPT_Vector,
		DPT_VectorXY, // Only encodes the XY of a vector, ignores Z
		DPT_String,
		DPT_Array,	// An array of the base types (can't be of datatables).
		DPT_DataTable,
		DPT_NUMSendPropTypes
	};

	struct ClientClass {
	private:
		uint8_t _padding[0x8];
	public:
		uint32_t m_name;
		uint32_t m_recv_table; // RecvTable*
		uint32_t m_next;
		int32_t m_class_id;
	};

	struct RecvTable {
	public:
		uint32_t m_props_array; // RecvProp*
		uint32_t m_num_props;
	private:
		uint8_t _padding1[0x4];
	public:
		uint32_t m_name;
	private:
		uint8_t _padding2[0x2];
	};

	struct RecvProp {
	public:
		uint32_t m_name;
		uint32_t m_type;
	private:
		uint8_t _padding1[0x1D];
	public:
		uint32_t m_recv_table; // RecvTable*
		uint32_t m_offset;
	private:
		uint8_t _padding2[0xC];
	};

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

	struct TypeDescription {
		uint32_t m_field_type;
		uint32_t m_field_name;
		uint32_t m_field_offset[2];
	private:
		uint8_t m_padding_1[16];
	public:
		uint32_t m_field_size;
	private:
		uint8_t m_padding_2[24];
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/datamap.h
	struct Datamap {
		uint32_t m_data_fields; // TypeDescription*
		uint32_t m_num_fields;
		uint32_t m_class_name;
		uint32_t m_base_datamap;
	private:
		uint8_t m_padding_1[6];
	};

	struct GlowObject {
		uint32_t m_entity;
		mango::rgbaf m_glow_color;
	private:
		uint8_t m_padding_0[0x10];
	public:
		bool m_render_when_occluded;
		bool m_render_when_unoccluded;
	private:
		uint8_t m_padding_1[0xC];
	public:
		int m_next_free_slot;
	}; // sizeof(GlowObject) == 0x38

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/materialsystem/imaterialvar.h
	struct MaterialVar {
		uint32_t m_vtable;
		uint32_t m_string_value;
		int32_t m_int_value;
		mango::Vec4f m_vec_value;
		uint8_t m_type : 4;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/usercmd.h
	struct CUserCmd {
	private:
		uint8_t m_padding[0x4];
	public:
		int command_number;
		int tick_count;
		mango::Vec3f viewangles;
		mango::Vec3f aimdirection;
		mango::Vec3f movement;
		int buttons;
		uint8_t impulse;
		int weaponselect;
		int weaponsubtype;
		int random_seed;
		short mousedx;
		short mousedy;
		bool hasbeenpredicted;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/globalvars_base.h
	struct GlobalVarsBase {
		float realtime;
		int	framecount;
		float absoluteframetime;
	private:
		uint8_t m_padding[0x4];
	public:
		float curtime;
		float frametime;
		int maxclients;
		int	tickcount;
		float interval_per_tick;
		float interpolation_amount;
	};
} // namespace sdk