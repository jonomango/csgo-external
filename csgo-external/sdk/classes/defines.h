#pragma once

#include <stdint.h>

#include <misc/color.h>
#include <misc/vector.h>


namespace sdk {
	using MDLHandle = unsigned short;

	struct CUtlVector {
		uint32_t m_pData;
	private:
		uint8_t _padding[0x8];
	public:
		int m_iSize;
	};

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

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/mathlib/vector.h#L374
	class alignas(16) VectorAligned : public mango::Vec3f {
	public:
		VectorAligned& operator=(const mango::Vec3f& other) {
			(*this)[0] = other[0];
			(*this)[1] = other[1];
			(*this)[2] = other[2];
			return *this;
		}

		float w; // unused afaik
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/cmodel.h#L61
	class Ray_t {
	public:
		VectorAligned m_Start;			// starting point, centered within the extents
		VectorAligned m_Delta;			// direction + length of the ray
		VectorAligned m_StartOffset;	// Add this to m_Start to get the actual ray start
		VectorAligned m_Extents;		// Describes an axis aligned box extruded along a ray
	private:
		uint8_t m_padding_0[4];
	public:
		bool m_IsRay;					// are the extents zero?
		bool m_IsSwept;					// is delta != 0?

		void Init(const mango::Vec3f& vecStart, const mango::Vec3f& vecEnd) {
			m_IsRay = true;

			m_Start = vecStart;
			m_Delta = vecEnd - vecStart;
			m_IsSwept = (m_Delta.length() != 0.0); // usually true

			m_Extents.fill(0.f);
			m_StartOffset.fill(0.f);
		}
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cmodel.h#L50
	struct csurface_t {
		uint32_t name;
		int16_t surfaceProps;
		uint16_t flags;
	};

	struct cplane_t {
		mango::Vec3f normal;
		float dist;
		uint8_t type;
		uint8_t signbits;
	private:
		uint8_t m_padding_0[2];
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/gametrace.h#L30
	struct trace_t {
		mango::Vec3f start;
		mango::Vec3f end;
		cplane_t plane;
		float fraction;
		int contents;
		uint16_t dispFlags;
		bool allsolid;
		bool startSolid;
		float fractionLeftSolid;
		csurface_t surface;
		int hitGroup;
		int16_t physicsBone;
		uint32_t entity;
		int hitbox;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/bone_accessor.h
	struct BoneAccessor {
		uint32_t m_pAnimating; // C_BaseAnimating*
		uint32_t m_pBones; // matrix3x4_t*
		int m_ReadableBones;
		int m_WritableBones;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/model_types.h#L39
	enum modtype_t {
		mod_bad = 0,
		mod_brush,
		mod_sprite,
		mod_studio
	};

	// couldn't find much about this, mostly used reclass
	class model_t {
	private:
		char pad_0x0000[0x4]; //0x0000
	public:
		char name[260]; //0x12D5888
	private:
		char pad_0x0108[0x8]; //0x0108
	public:
		modtype_t modtype; //0x0110
	private:
		char pad_0x0114[0x24]; //0x0114
	public:
		MDLHandle mdl_handle; //0x0138 
	};
} // namespace sdk