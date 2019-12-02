#pragma once

#include <misc/vector.h>


// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/studio.h
namespace sdk {
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

	struct mstudiohitboxset_t {
		// returns a mstudiobbox_t*
		uint32_t get_hitbox(const uint32_t address, const int i) const {
			return address + hitboxindex + (i * sizeof(mstudiobbox_t));
		}

		int				sznameindex;
		int				numhitboxes;
		int				hitboxindex;
	};

	struct studiohdr_t {
		// returns a mstudiohitboxset_t*
		uint32_t get_hitbox_set(const uint32_t address, const int i) const {
			return address + hitboxsetindex + (i * sizeof(mstudiohitboxset_t));
		}

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
} // namespace sdk