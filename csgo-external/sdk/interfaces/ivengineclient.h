#pragma once

#include <stdint.h>
#include <misc/vector.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L556
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L194
	class IVEngineClient {
	public:
		IVEngineClient() noexcept : m_address(0) {}
		explicit IVEngineClient(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		uint32_t ivengineclient() const noexcept { return this->m_address; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L243
		int get_local_player() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L282
		bool is_in_game() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L263
		mango::Vec3f get_view_angles() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/cdll_int.h#L265
		void set_view_angles(const mango::Vec3f& angles) const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk