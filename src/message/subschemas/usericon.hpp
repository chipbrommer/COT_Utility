#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            usericon.hpp
// @brief           A CoT Message subschema class for usericon data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <utility>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for usericon data
	class usericon {
	public:
		/// @brief Constructor - Initializes everything
		explicit usericon(std::string iconsetpath = INVALID_VALUE)
			: iconsetpath(std::move(iconsetpath)) {
		}

		/// @brief Equality comparison
		bool operator==(const usericon& other) const noexcept {
			return iconsetpath == other.iconsetpath;
		}

		/// @brief Inequality comparison
		bool operator!=(const usericon& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (iconsetpath.empty()) {
				auto res = result{ result::error_code::InvalidUserIcon, "Empty iconsetpath" };
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
				auto res = result{ result::error_code::InvalidUserIcon, "Invalid usericon data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("usericon");
			node.append_attribute("iconsetpath") = iconsetpath.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized usericon
		static usericon from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			usericon ui;
			if (node.name() != std::string("usericon")) {
				auto res = result{ result::error_code::InvalidUserIcon, "Expected <usericon> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return ui;
			}
			if (auto attr = node.attribute("iconsetpath")) {
				ui.iconsetpath = attr.as_string();
			}
			if (!ui.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidUserIcon,
								  "Invalid usericon data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return ui;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const usericon& ui) {
			os << "usericon: ";
			if (!ui.is_valid()) os << " -NOT VALID- ";
			os << "\n\ticonsetpath: " << (ui.iconsetpath.empty() ? "None" : ui.iconsetpath) << "\n";
			return os;
		}

		std::string iconsetpath{ INVALID_VALUE };

	private:
		inline static constexpr const char* INVALID_VALUE = "";
	};
}