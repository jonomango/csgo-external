#pragma once

#include "../classes/imaterial.h"

#include <string_view>
#include <optional>
#include <stdint.h>


namespace sdk {
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L552
	class IMaterialSystem {
	public:
		IMaterialSystem() noexcept : m_address(0) {}
		explicit IMaterialSystem(const uint32_t address) noexcept : m_address(address) {}

		// get the underlying address
		constexpr operator uint32_t() const noexcept { return this->m_address; }

	public:
		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L832
		MaterialHandle_t first_material() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L837
		MaterialHandle_t next_material(MaterialHandle_t current) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L840
		MaterialHandle_t invalid_material() const { return 0xFFFF; }

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L843
		IMaterial get_material(const MaterialHandle_t handle) const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L847
		int get_num_materials() const;

		// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/materialsystem/imaterialsystem.h#L819
		IMaterial find_material(const std::string_view name, const std::optional<std::string_view> group_name = {}) const;

	private:
		// each object is like 16 bytes big i think
		uint32_t get_handle_array() const;

		// dynamically get the handle array offset
		uint32_t get_handle_array_offset() const;

	private:
		uint32_t m_address = 0;
	};
} // namespace sdk