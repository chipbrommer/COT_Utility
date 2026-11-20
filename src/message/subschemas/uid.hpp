#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            uid.hpp
// @brief           A CoT Message subschema class for uid data.
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
	/// @brief A CoT Message subschema class for uid data
	class uid {
	public:
		/// @brief Constructor - Initializes everything
		explicit uid(std::string droid = INVALID_STR,
			std::string nett = INVALID_STR,
			bool alertable = false,
			bool has_alertable = false) noexcept
			: droid(std::move(droid)), nett(std::move(nett)),
			alertable(alertable), has_alertable(has_alertable) {
		}

		/// @brief Equality comparison
		bool operator==(const uid& other) const noexcept {
			return droid == other.droid && nett == other.nett &&
				alertable == other.alertable && has_alertable == other.has_alertable;
		}

		/// @brief Inequality comparison
		bool operator!=(const uid& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (droid.empty() && nett.empty() && !has_alertable) {
				auto res = result{ result::error_code::InvalidUid, "Empty uid attributes" };
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
				auto res = result{ result::error_code::InvalidUid, "Invalid uid data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("uid");
			if (has_alertable) node.append_attribute("alertable") = alertable;
			if (!droid.empty()) node.append_attribute("Droid") = droid.c_str();
			if (!nett.empty()) node.append_attribute("nett") = nett.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized uid
		static uid from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			uid u;
			if (node.name() != std::string("uid")) {
				auto res = result{ result::error_code::InvalidUid, "Expected <uid> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return u;
			}
			if (auto attr = node.attribute("alertable")) {
				u.has_alertable = true;
				std::string val = attr.as_string();
				u.alertable = (val == "true" || val == "1");
			}
			auto droid_attr = node.attribute("Droid");
			if (!droid_attr) droid_attr = node.attribute("droid");
			if (droid_attr) u.droid = droid_attr.as_string();
			if (auto attr = node.attribute("nett")) u.nett = attr.as_string();
			if (!u.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidUid,
								  "Invalid uid data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return u;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const uid& u) {
			os << "uid: ";
			if (!u.is_valid()) os << " -NOT VALID- ";
			os << "\n\talertable: " << (u.has_alertable ? (u.alertable ? "true" : "false") : "None") << "\n"
				<< "\tdroid: " << (u.droid.empty() ? "None" : u.droid) << "\n"
				<< "\tnett: " << (u.nett.empty() ? "None" : u.nett) << "\n";
			return os;
		}

		std::string droid{ INVALID_STR };
		std::string nett{ INVALID_STR };
		bool alertable{ false };
		bool has_alertable{ false };

	private:
		inline static constexpr const char* INVALID_STR = "";
	};
}