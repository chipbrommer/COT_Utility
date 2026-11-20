#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            strokestyle.hpp
// @brief           A CoT Message subschema class for strokestyle data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <utility>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for strokestyle data
	class strokestyle {
	public:
		/// @brief Constructor - Initializes everything
		explicit strokestyle(std::string value = INVALID_VALUE)
			: value(std::move(value)) {
		}

		/// @brief Equality comparison
		bool operator==(const strokestyle& other) const noexcept { return value == other.value; }

		/// @brief Inequality comparison
		bool operator!=(const strokestyle& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (value.empty()) {
				auto res = result{ result::error_code::InvalidStrokeStyle, "Empty strokestyle value" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			return true;
		}

		/// @brief Serializes to XML
		/// @param parent The parent XML node to append to
		/// @return Result indicating success or failure
		result to_xml(pugi::xml_node& parent) const {
			if (!is_valid()) {
				auto res = result{ result::error_code::InvalidStrokeStyle, "Invalid strokestyle data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("strokestyle");
			node.append_attribute("value") = value.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized strokestyle
		static strokestyle from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			strokestyle ss;
			if (node.name() != std::string("strokestyle")) {
				auto res = result{ result::error_code::InvalidStrokeStyle, "Expected <strokestyle> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return ss;
			}
			if (auto attr = node.attribute("value")) ss.value = attr.as_string();
			if (!ss.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidStrokeStyle,
								  "Invalid strokestyle data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return ss;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const strokestyle& ss) {
			os << "strokestyle: ";
			if (!ss.is_valid()) os << " -NOT VALID- ";
			os << "\n\tvalue: " << (ss.value.empty() ? "None" : ss.value) << "\n";
			return os;
		}

		std::string value{ INVALID_VALUE };

	private:
		inline static constexpr const char* INVALID_VALUE = "";
	};
}