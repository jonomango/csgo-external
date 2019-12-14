#pragma once

#include "../common.h"

#include <misc/vector.h>
#include <epic/read_write_variable.h>


// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/studio.h
namespace sdk {
	// describes a sequence
	struct mstudioseqdesc_t {
		int					baseptr;
		int					szlabelindex;
		int					szactivitynameindex;
		int					flags;		// looping/non-looping flags
		int					activity;	// initialized at loadtime to game DLL values
		int					actweight;
		int					numevents;
		int					eventindex;
		mango::Vec3f		bbmin;		// per sequence bounding box
		mango::Vec3f		bbmax;
		int					numblends;
		int					animindexindex;
		int					movementindex;	// [blend] float array for blended movement
		int					groupsize[2];
		int					paramindex[2];	// X, Y, Z, XR, YR, ZR
		float				paramstart[2];	// local (0..1) starting value
		float				paramend[2];	// local (0..1) ending value
		int					paramparent;
		float				fadeintime;		// ideal cross fade in time (0.2 default)
		float				fadeouttime;	// ideal cross fade out time (0.2 default)
		int					localentrynode;		// transition node at entry
		int					localexitnode;		// transition node at exit
		int					nodeflags;		// transition rules
		float				entryphase;		// used to match entry gait
		float				exitphase;		// used to match exit gait
		float				lastframe;		// frame that should generation EndOfSequence
		int					nextseq;		// auto advancing sequences
		int					pose;			// index of delta animation between end and nextseq
		int					numikrules;
		int					numautolayers;	//
		int					autolayerindex;
		int					weightlistindex;
		int					posekeyindex;
		int					numiklocks;
		int					iklockindex;
		int					keyvalueindex;
		int					keyvaluesize;
		int					cycleposeindex;		// index of pose parameter to use as cycle index
		int					activitymodifierindex;
		int					numactivitymodifiers;
		int					unused[5];		// remove/add as appropriate (grow back to 8 ints on version change!)
	};

	// describes a hitbox
	struct mstudiobbox_t {
		int				bone;
		int				group;
		mango::Vec3f	bbmin;
		mango::Vec3f	bbmax;
		int				szhitboxnameindex;
		int				m_iPad01[3];
		float			m_flRadius;
		int				m_iPad02[4];
	};

	class mstudiohitboxset_t {
	public:
		// returns a mstudiobbox_t*
		static mango::RWVariable<mstudiobbox_t> get_hitbox(const mango::RWVariable<mstudiohitboxset_t> boxset, const int i);

	public:
		int				sznameindex;
		int				numhitboxes;
		int				hitboxindex;
	};

	// the main model structure
	class studiohdr_t {
	public:
		// returns a mstudiohitboxset_t*
		static mango::RWVariable<mstudiohitboxset_t> get_hitbox_set(const mango::RWVariable<studiohdr_t> studiohdr, const int i);

		// returns a mstudioseqdesc_t*
		static mango::RWVariable<mstudioseqdesc_t> get_local_sequence_desc(const mango::RWVariable<studiohdr_t> studiohdr, const int i);

		// returns a mstudioseqdesc_t*
		static mango::RWVariable<mstudioseqdesc_t> get_sequence_desc(const mango::RWVariable<studiohdr_t> studiohdr, const int i);

		// how many available sequences
		static int get_num_sequences(const mango::RWVariable<studiohdr_t> studiohdr);

	public:
		int             id;
		int             version;
		int             checksum;
		char			name[64];
		int             length;
		mango::Vec3f	eyeposition;
		mango::Vec3f	illumposition;
		mango::Vec3f	hull_min;
		mango::Vec3f	hull_max;
		mango::Vec3f	view_bbmin;
		mango::Vec3f	view_bbmax;
		int             flags;
		int             numbones;
		int             boneindex;
		int             numbonecontrollers;
		int             bonecontrollerindex;
		int             numhitboxsets;
		int             hitboxsetindex;
		int             numlocalanim;
		int             localanimindex;
		int             numlocalseq;
		int             localseqindex;
		int				activitylistversion;
		int				eventsindexed;
		int             numtextures;
		int             textureindex;
		int             numcdtextures;
		int             cdtextureindex;
		int             numskinref;
		int             numskinfamilies;
		int             skinindex;
		int             numbodyparts;
		int             bodypartindex;
		int             numlocalattachments;
		int             localattachmentindex;
		int             numlocalnodes;
		int             localnodeindex;
		int             localnodenameindex;
		int             numflexdesc;
		int             flexdescindex;
		int             numflexcontrollers;
		int             flexcontrollerindex;
		int             numflexrules;
		int             flexruleindex;
		int             numikchains;
		int             ikchainindex;
		int             nummouths;
		int             mouthindex;
		int             numlocalposeparameters;
		int             localposeparamindex;
		int             surfacepropindex;
		int             keyvalueindex;
		int             keyvaluesize;
		int             numlocalikautoplaylocks;
		int             localikautoplaylockindex;
		float			mass;
		int             contents;
		int             numincludemodels;
		int             includemodelindex;
		uint32_t		virtualModel;
		int             szanimblocknameindex;
		int             numanimblocks;
		int             animblockindex;
		uint32_t		animblockModel;
		int             bonetablebynameindex;
		uint32_t		pVertexBase;
		uint32_t		pIndexBase;
		uint8_t			constdirectionallightdot;
		uint8_t			rootLOD;
		uint8_t			numAllowedRootLODs;
		uint8_t			unused[1];
		int             unused4;
		int             numflexcontrollerui;
		int             flexcontrolleruiindex;
		float			flVertAnimFixedPointScale;
		int             unused3[1];
		int             studiohdr2index;
		int             unused2[1];
	};

	// cba adding the other fields...
	struct studiohdr2_t {
	private:
		uint8_t _padding1[0x30];
	public:
		MDLHandle_t m_VirtualModelMDL;
	};

	struct virtualgroup_t {
		MDLHandle_t cache;
		CUtlVector<int> boneMap;				// maps global bone to local bone
		CUtlVector<int> masterBone;				// maps local bone to global bone
		CUtlVector<int> masterSeq;				// maps local sequence to master sequence
		CUtlVector<int> masterAnim;				// maps local animation to master animation
		CUtlVector<int> masterAttachment;		// maps local attachment to global
		CUtlVector<int> masterPose;				// maps local pose parameter to global
		CUtlVector<int> masterNode;				// maps local transition nodes to global
	};

	struct virtualgeneric_t {
		int group;
		int index;
	};

	struct virtualsequence_t {
		int	flags;
		int activity;
		int group;
		int index;
	};

	struct virtualmodel_t {
	private:
		uint8_t pad_0x0000[0x8];
	public:
		CUtlVector<virtualsequence_t>	m_seq;
		CUtlVector<virtualgeneric_t>	m_anim;
		CUtlVector<virtualgeneric_t>	m_attachment;
		CUtlVector<virtualgeneric_t>	m_pose;
		CUtlVector<virtualgroup_t>		m_group;
		CUtlVector<virtualgeneric_t>	m_node;
		CUtlVector<virtualgeneric_t>	m_iklock;
		CUtlVector<uint16_t>			m_autoplaySequences;
	private:
		uint8_t pad_0x00A8[0xC];
	};

	// https://github.com/VSES/SourceEngine2007/blob/master/src_main/datacache/mdlcache.cpp#L75
	struct studiodata_t {
		DataCacheHandle_t m_MDLCache;
	private:
		uint8_t pad_0x0004[0x3C];
	public:
		uint16_t m_nRefCount;
	private:
		uint8_t pad_0x0042[0x2];
	public:
		uint32_t m_pVirtualModel; // virtualmodel_t*
	private:
		uint8_t pad_0x0048[0x40];
	public:
		uint32_t m_pUserData; // studiohdr_t*
	private:
		uint8_t pad_0x008C[0x18];
	};
} // namespace sdk