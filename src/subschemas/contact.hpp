#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            contact.hpp
// @brief           A CoT Message subschema class for contact data.
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
	/// @brief A CoT Message subschema class for contact data
	class contact {
	public:
		/// @brief Constructor - Initializes everything
		explicit contact(std::string endpoint = INVALID_VALUE,
			std::string callsign = INVALID_VALUE,
			std::string xmppUsername = INVALID_VALUE,
			std::string phone = INVALID_VALUE,
			std::string name = INVALID_VALUE)
			: endpoint(std::move(endpoint)), callsign(std::move(callsign)),
			xmppUsername(std::move(xmppUsername)), phone(std::move(phone)),
			name(std::move(name)) {
		}

		/// @brief Equality comparison
		bool operator==(const contact& other) const noexcept {
			return endpoint == other.endpoint && callsign == other.callsign &&
				xmppUsername == other.xmppUsername && phone == other.phone &&
				name == other.name;
		}

		/// @brief Inequality comparison
		bool operator!=(const contact& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (callsign.empty()) {
				auto res = result{ result::error_code::InvalidContact, "Empty callsign field" };
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
				auto res = result{ result::error_code::InvalidContact, "Invalid contact data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("contact");
			if (!endpoint.empty()) node.append_attribute("endpoint") = endpoint.c_str();
			node.append_attribute("callsign") = callsign.c_str();
			if (!xmppUsername.empty()) node.append_attribute("xmppUsername") = xmppUsername.c_str();
			if (!phone.empty()) node.append_attribute("phone") = phone.c_str();
			if (!name.empty()) node.append_attribute("name") = name.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized contact
		static contact from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			contact c;
			if (node.name() != std::string("contact")) {
				auto res = result{ result::error_code::InvalidContact, "Expected <contact> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return c;
			}
			if (auto attr = node.attribute("endpoint")) c.endpoint = attr.as_string();
			if (auto attr = node.attribute("callsign")) c.callsign = attr.as_string();
			if (auto attr = node.attribute("xmppUsername")) c.xmppUsername = attr.as_string();
			if (auto attr = node.attribute("phone")) c.phone = attr.as_string();
			if (auto attr = node.attribute("name")) c.name = attr.as_string();
			if (!c.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidContact, "Invalid contact data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return c;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const contact& c) {
			os << "contact: ";
			if (!c.is_valid()) os << " -NOT VALID- ";
			os << "\n\tendpoint: " << (c.endpoint.empty() ? "None" : c.endpoint) << "\n"
				<< "\tcallsign: " << (c.callsign.empty() ? "None" : c.callsign) << "\n"
				<< "\txmppUsername: " << (c.xmppUsername.empty() ? "None" : c.xmppUsername) << "\n"
				<< "\tphone: " << (c.phone.empty() ? "None" : c.phone) << "\n"
				<< "\tname: " << (c.name.empty() ? "None" : c.name) << "\n";
			return os;
		}

		std::string endpoint{ "" };
		std::string callsign{ "" };
		std::string xmppUsername{ "" };
		std::string phone{ "" };
		std::string name{ "" };

	private:
		inline static constexpr const char* INVALID_VALUE = "";
	};
}