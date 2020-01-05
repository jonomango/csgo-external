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
	using ModelInstanceHandle_t = uint16_t;

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

	// entity effects
	enum {
		EF_BONEMERGE = 0x001,			// Performs bone merge on client side
		EF_BRIGHTLIGHT = 0x002,			// DLIGHT centered at entity origin
		EF_DIMLIGHT = 0x004,			// player flashlight
		EF_NOINTERP = 0x008,			// don't interpolate the next frame
		EF_NOSHADOW = 0x010,			// Don't cast no shadow
		EF_NODRAW = 0x020,				// don't draw entity
		EF_NORECEIVESHADOW = 0x040,		// Don't receive no shadow
		EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
		EF_ITEM_BLINK = 0x100,			// blink an item so that the user notices it.
		EF_PARENT_ANIMATES = 0x200,		// always assume that the parent entity is animating
		EF_MAX_BITS = 10
	};

	// entity flags, CBaseEntity::m_iEFlags
	enum {
		EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
		EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
		EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
		EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
		EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

		EFL_HAS_PLAYER_CHILD = (1 << 4),	// One of the child entities is a player.

		EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
		EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

		// The default behavior in ShouldTransmit is to not send an entity if it doesn't
		// have a model. Certain entities want to be sent anyway because all the drawing logic
		// is in the client DLL. They can set this flag and the engine will transmit them even
		// if they don't have a model.
		EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

		EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
		EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
		EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

		// Some dirty bits with respect to abs computations
		EFL_DIRTY_ABSTRANSFORM = (1 << 11),
		EFL_DIRTY_ABSVELOCITY = (1 << 12),
		EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
		EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
		EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),

		EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
		// This forces it to pass the "in PVS" for transmission.
		EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
		EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating

		// FIXME: Not really sure where I should add this...
		EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
		EFL_NO_ROTORWASH_PUSH = (1 << 21),		// I shouldn't be pushed by the rotorwash
		EFL_NO_THINK_FUNCTION = (1 << 22),
		EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),

		EFL_CHECK_UNTOUCH = (1 << 24),
		EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
		EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
		EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
		EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
		EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
		EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
		EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
	};

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
		uint32_t m_concommandbase_vtable;
	public:
		uint32_t m_pNext;			// ConCommandBase*
		bool m_bRegistered;
		uint32_t m_pszName;			// const char*
		uint32_t m_pszHelpString;	// const char*
		int m_nFlags;
	};

	struct ConCommand : public ConCommandBase {
		uint32_t m_fnCommandCallback;
		uint32_t m_fnCompletionCallback;
		bool m_bHasCompletionCallback : 1;
		bool m_bUsingNewCommandCallback : 1;
		bool m_bUsingCommandCallbackInterface : 1;
	};

	struct ConVar : public ConCommandBase {
	private:
		uint32_t m_convar_vtable;
	public:
		uint32_t m_pParent;			// ConVar*
		uint32_t m_pszDefaultValue; // const char*
		uint32_t m_pszString;		// const char*
		int m_StringLength;
		float m_fValue;
		int m_nValue;

		// small wrapper to "decrypt" values
		template <typename T>
		T get_value(const uint32_t key) const {
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) {
				return this->m_nValue ^ key;
			} else if constexpr (std::is_same_v<T, float>) {
				const auto decrypted = *reinterpret_cast<const uint32_t*>(&this->m_fValue) ^ key;
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
	struct CBoneAccessor {
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