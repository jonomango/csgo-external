#include "studio.h"

#include "../misc/constants.h"


namespace sdk {
	// returns a mstudiobbox_t*
	mango::RWVariable<mstudiobbox_t> mstudiohitboxset_t::get_hitbox(const mango::RWVariable<mstudiohitboxset_t> boxset, const int i) {
		return { globals::process, &boxset + boxset().hitboxindex + (i * sizeof(mstudiobbox_t)) };
	}

	// returns a mstudiohitboxset_t*
	mango::RWVariable<mstudiohitboxset_t> studiohdr_t::get_hitbox_set(const mango::RWVariable<studiohdr_t> studiohdr, const int i) {
		return { globals::process, &studiohdr + studiohdr().hitboxsetindex + (i * sizeof(mstudiohitboxset_t)) };
	}

	// returns a mstudioseqdesc_t*
	mango::RWVariable<mstudioseqdesc_t> studiohdr_t::get_local_sequence_desc(const mango::RWVariable<studiohdr_t> studiohdr, const int i) {
		return { globals::process, &studiohdr + studiohdr().localseqindex + (i * sizeof(mstudioseqdesc_t)) };
	}

	// returns a mstudioseqdesc_t*
	mango::RWVariable<mstudioseqdesc_t> studiohdr_t::get_sequence_desc(const mango::RWVariable<studiohdr_t> studiohdr, const int i) {
		const auto shdr = studiohdr();
		if (shdr.numincludemodels == 0)
			return studiohdr_t::get_local_sequence_desc(studiohdr, i);

		const auto virtualmodel = interfaces::model_info.get_virtual_model(studiohdr);
		if (!virtualmodel)
			return studiohdr_t::get_local_sequence_desc(studiohdr, i);

		const auto vm = virtualmodel();
		if (const auto newstudiohdr = interfaces::mdl_cache.get_studio_hdr(vm.m_group[vm.m_seq[i]().group]().cache))
			return studiohdr_t::get_local_sequence_desc(newstudiohdr, vm.m_seq[i]().index);
		
		// error
		return {};
	}

	// how many available sequences
	int studiohdr_t::get_num_sequences(const mango::RWVariable<studiohdr_t> studiohdr) {
		const auto shdr = studiohdr();
		if (shdr.numincludemodels == 0)
			return shdr.numlocalseq;

		if (const auto virtualmodel = interfaces::model_info.get_virtual_model(studiohdr))
			return virtualmodel().m_seq.m_iSize;
		return 0;
	}
} // namespace sdk