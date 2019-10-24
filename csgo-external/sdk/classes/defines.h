#pragma once

#include <stdint.h>


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
} // namespace sdk