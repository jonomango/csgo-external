#pragma once

#include <stdint.h>

#include <epic/read_write_variable.h>
#include <epic/process.h>
#include <misc/color.h>
#include <misc/vector.h>


namespace sdk {
	// HACK HACK
	namespace globals {
		extern mango::Process process;
	}

	using MDLHandle_t = uint16_t;
	using MaterialHandle_t = uint16_t;
	using DataCacheHandle_t = uint32_t;

	// How many bits to use to encode an edict.
	static constexpr auto MAX_EDICT_BITS = 11; // # of bits needed to represent max edicts
	static constexpr auto MAX_EDICTS = (1 << MAX_EDICT_BITS);

	// Used for networking ehandles.
	static constexpr auto NUM_ENT_ENTRY_BITS = (MAX_EDICT_BITS + 1);
	static constexpr auto NUM_ENT_ENTRIES = (1 << NUM_ENT_ENTRY_BITS);
	static constexpr auto ENT_ENTRY_MASK = (NUM_ENT_ENTRIES - 1);
	static constexpr auto INVALID_EHANDLE_INDEX = 0xFFFFFFFF;

	static constexpr auto NUM_SERIAL_NUM_BITS = (32 - NUM_ENT_ENTRY_BITS);
	static constexpr auto MDLHANDLE_INVALID = MDLHandle_t(~0);

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/basehandle.h#L25
	class CBaseHandle {
	public:
		constexpr CBaseHandle() noexcept 
			: m_Index(INVALID_EHANDLE_INDEX) {}
		constexpr CBaseHandle(const uint32_t value) noexcept 
			: m_Index(value) {}
		constexpr CBaseHandle(const CBaseHandle& other) noexcept 
			: m_Index(other.m_Index) {}
		constexpr CBaseHandle(const int iEntry, const int iSerialNumber) noexcept 
			: m_Index(iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS)) {}

	public:
		bool is_valid() const noexcept {
			return this->m_Index != INVALID_EHANDLE_INDEX;
		}

		int get_entry_index() const noexcept {
			return m_Index & ENT_ENTRY_MASK;
		}

		int get_serial_number() const noexcept {
			return m_Index >> NUM_ENT_ENTRY_BITS;
		}

		uint32_t get_value() const noexcept {
			return this->m_Index;
		}

	private:
		uint32_t m_Index;
	};

	template <typename T>
	class CUtlVector {
	public:
		mango::RWVariable<T> operator[](const size_t index) const noexcept {
			return { globals::process, this->m_pData + index * sizeof(T) };
		}
	public:
		uint32_t m_pData;			// pointer to an array of elements
		int m_iAllocated;			// size of the allocated array
	private:
		uint8_t _padding[0x4];
	public:
		int m_iSize;				// m_iSize is always < than m_iAllocated
		uint32_t m_pDataCopy;		// not sure what purpose this serves
	};

	struct CUtlMap {
		uint32_t m_pData;
		int16_t m_iAllocated;
	private:
		uint8_t _padding[0xC];
	public:
		int16_t m_iSize;
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

	struct ConCommand : public ConCommandBase {
		uint32_t m_command_callback;
		uint32_t m_completion_callback;
		bool m_has_completion_callback : 1;
		bool m_using_new_command_callback : 1;
		bool m_using_command_callback_interface : 1;
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
	struct CGlobalVarsBase {
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

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/gl_model_private.h#L360
	struct model_t {
		uint32_t fnHandle;		// FileNameHandle_t
		char szName[260];		// MAX_QPATH
		int nLoadFlags;			// mark loaded/not loaded
		int nServerCount;		// marked at load
		modtype_t type;
	private:
		char pad_0x0114[0x24];
	public:
		MDLHandle_t studio;
	};

	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/client/animationlayer.h#L18
	struct C_AnimationLayer {
	private:
		uint8_t m_padding_0[0x14];
	public:
		int m_nOrder;
		int m_nSequence;
		float m_flPrevCycle;
		float m_flWeight;
		float m_flWeightDeltaRate;
		float m_flPlaybackRate;
		float m_flCycle;
		uint32_t m_pEntity;
	private:
		uint8_t m_padding_1[0x4];
	};
} // namespace sdk