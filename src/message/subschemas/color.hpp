#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            color.hpp
// @brief           A CoT Message subschema class for color data
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <exception>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for color data (replaces strokecolor, fillcolor, color)
	class color {
	public:
		/// @brief Constructor - Initializes everything
		explicit color(int argb = INVALID_VALUE) noexcept : argb(argb) {}

		/// @brief Equality comparison
		bool operator==(const color& other) const noexcept { return argb == other.argb; }

		/// @brief Inequality comparison
		bool operator!=(const color& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (argb == INVALID_VALUE) {
				auto res = result{ result::error_code::InvalidColor, "Color argb is unset" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			return true;
		}

		/// @brief Serializes to XML
		/// @param parent The parent XML node to append to
		/// @param tag_name The XML tag name (e.g., "strokecolor", "fillcolor", "color")
		/// @return Result indicating success or failure
		result to_xml(pugi::xml_node& parent, std::string_view tag_name = "color") const {
			if (!is_valid()) {
				auto res = result{ result::error_code::InvalidColor, "Invalid color data" };
				if (error_handler) error_handler(res);
				return res;
			}
			const std::string tag_str(tag_name);
			auto node = parent.append_child(tag_str.c_str());
			// Use "value" for strokecolor and fillcolor, "argb" for color
			const char* attr_name = (tag_name == "strokecolor" || tag_name == "fillcolor") ? "value" : "argb";
			node.append_attribute(attr_name) = argb;
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @param tag_name Expected XML tag name (e.g., "strokecolor", "fillcolor", "color")
		/// @return Deserialized color
		static color from_xml(const pugi::xml_node& node, result* rslt = nullptr,
			std::string_view tag_name = "color") {
			color c;
			if (node.name() != tag_name) {
				auto res = result{ result::error_code::InvalidColor,
								  "Expected <" + std::string(tag_name) + "> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return c;
			}
			// Try both "value" and "argb" attributes
			auto attr = node.attribute("value");
			if (!attr) attr = node.attribute("argb");
			if (attr) {
				try {
					c.argb = attr.as_int(INVALID_VALUE);
				}
				catch (const std::exception& e) {
					auto res = result{ result::error_code::InvalidColor,
									  "Error parsing color attribute: " + std::string(e.what()) };
					if (rslt) *rslt = res;
					if (error_handler) error_handler(res);
					c.argb = INVALID_VALUE;
				}
			}
			if (!c.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidColor, "Invalid color data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return c;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const color& c) {
			os << "color: ";
			if (!c.is_valid()) os << " -NOT VALID- (Unset)";
			else os << "\n\targb: " << c.argb;
			os << "\n";
			return os;
		}

		int argb{ INVALID_VALUE };

	private:
		inline static constexpr int INVALID_VALUE = std::numeric_limits<int>::min();
	};
}