#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            message.hpp
// @brief           A root XML CoT message schema class.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cstddef>
#include <exception>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"
#include "subschemas/event.hpp"

namespace cot
{
	/// @brief A root XML CoT message schema class
	class message {
	public:
		/// @brief Constructor - Initializes everything
		explicit message(subschemas::event e = subschemas::event(),
			std::string xml_version = "1.0",
			std::string xml_encoding = "utf-8",
			std::string xml_standalone = "yes") noexcept
			: event_(std::move(e)),
			xml_version(std::move(xml_version)),
			xml_encoding(std::move(xml_encoding)),
			xml_standalone(std::move(xml_standalone)) {
		}

		/// @brief Equality comparison
		bool operator==(const message& other) const noexcept {
			return event_ == other.event_ &&
				xml_version == other.xml_version &&
				xml_encoding == other.xml_encoding &&
				xml_standalone == other.xml_standalone;
		}

		/// @brief Inequality comparison
		bool operator!=(const message& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (xml_version != "1.0") {
				auto res = result{ result::error_code::InvalidXml, "XML version must be 1.0" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!equals_ignore_case(xml_encoding, "utf-8")) {
				auto res = result{ result::error_code::InvalidXml, "XML encoding must be utf-8" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!equals_ignore_case(xml_standalone, "yes") && !equals_ignore_case(xml_standalone, "no")) {
				auto res = result{ result::error_code::InvalidXml, "XML standalone must be yes or no" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			result ev_res;
			if (!event_.is_valid(&ev_res)) {
				auto res = result{ result::error_code::InvalidEvent,
								  ev_res.description().empty() ? std::string("Invalid event") : ev_res.description() };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			return true;
		}

		/// @brief Serializes to XML
		/// @param parent The parent XML node (typically document)
		/// @return Result indicating success or failure
		result to_xml(pugi::xml_node& parent) const {
			result val_res;
			if (!is_valid(&val_res)) {
				auto res = result{ result::error_code::InvalidEvent,
								  val_res.description().empty() ? std::string("Invalid message data") : val_res.description() };
				if (error_handler) error_handler(res);
				return res;
			}
			auto decl = parent.append_child(pugi::node_declaration);
			decl.append_attribute("version") = xml_version.c_str();
			decl.append_attribute("encoding") = xml_encoding.c_str();
			decl.append_attribute("standalone") = xml_standalone.c_str();
			return event_.to_xml(parent);
		}

		/// @brief Serializes to XML string
		/// @param rslt Optional result for error details
		/// @return XML string or empty if invalid
		std::string to_xml_string(result* rslt = nullptr) const {
			pugi::xml_document doc;
			auto res = to_xml(doc);
			if (!res.is_success()) {
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return "";
			}
			return serialize_document(doc, true, rslt);
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML document node
		/// @param rslt Optional result for error details
		/// @return Deserialized message
		static message from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			message m;
			try {
				for (pugi::xml_node child : node.children()) {
					if (child.type() == pugi::node_declaration) {
						if (auto v = child.attribute("version")) {
							m.xml_version = v.as_string("1.0");
						}
						if (auto enc = child.attribute("encoding")) {
							m.xml_encoding = enc.as_string("utf-8");
						}
						if (auto sa = child.attribute("standalone")) {
							m.xml_standalone = sa.as_string("yes");
						}
						break;
					}
				}

				pugi::xml_node event_node = (node.type() == pugi::node_element && node.name() == std::string_view("event"))
					? node
					: node.child("event");

				if (!event_node) {
					auto res = result{ result::error_code::InvalidEvent, "No <event> node found" };
					if (rslt) *rslt = res;
					if (error_handler) error_handler(res);
					return m;
				}

				result ev_res;
				m.event_ = subschemas::event::from_xml(event_node, &ev_res);
				if (ev_res.is_failed()) {
					if (rslt) *rslt = ev_res;
					return m;
				}

				result val_res;
				if (!m.is_valid(&val_res)) {
					if (rslt) *rslt = val_res;
				}
			}
			catch (const std::exception& ex) {
				auto res = result{ result::error_code::InvalidEvent,
								  "Error parsing message: " + std::string(ex.what()) };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return m;
		}

		/// @brief Deserializes from an XML string
		/// @param xml The XML string to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized message
		static message from_xml_string(std::string_view xml, result* rslt = nullptr) {
			if (xml.empty()) {
				auto res = result{ result::error_code::InvalidInput, "Empty XML string" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return message();
			}
			pugi::xml_document doc;
			pugi::xml_parse_result parse_result = doc.load_buffer(xml.data(), xml.size());
			if (!parse_result) {
				auto res = result{ result::error_code::InvalidXml,
								  std::string(parse_result.description()) + " at offset " +
								  std::to_string(parse_result.offset) };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return message();
			}
			return from_xml(doc, rslt);
		}

		/// @brief Clears the contained event
		void clear_event() noexcept { event_ = subschemas::event(); }

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const message& m) {
			os << "message: ";
			if (!m.is_valid()) os << " -NOT VALID- ";
			os << "\n\txml_version: " << (m.xml_version.empty() ? "None" : m.xml_version)
				<< "\n\txml_encoding: " << (m.xml_encoding.empty() ? "None" : m.xml_encoding)
				<< "\n\txml_standalone: " << (m.xml_standalone.empty() ? "None" : m.xml_standalone)
				<< "\n" << m.event_;
			return os;
		}

		subschemas::event event_;
		std::string xml_version{ "1.0" };
		std::string xml_encoding{ "utf-8" };
		std::string xml_standalone{ "yes" };

	private:
		/// @brief Serializes a pugixml document to a string
		static std::string serialize_document(const pugi::xml_document& doc,
			bool pretty,
			result* rslt = nullptr) {
			try {
				std::ostringstream oss;
				const unsigned int flags = pretty
					? (pugi::format_default | pugi::format_indent)
					: pugi::format_raw;
				doc.save(oss, "  ", flags);
				return oss.str();
			}
			catch (const std::exception& ex) {
				auto res = result{ result::error_code::ProcessingError,
								  "Failed to serialize XML document: " + std::string(ex.what()) };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return "";
			}
		}

		/// @brief Compares two strings for equality without regard to ASCII case
		/// @param a First string view to compare
		/// @param b Second string view to compare
		/// @return True if both views have the same length and matching case-insensitive characters
		static bool equals_ignore_case(std::string_view a, std::string_view b) noexcept {
			if (a.size() != b.size()) return false;
			for (std::size_t i = 0; i < a.size(); ++i) {
				if (std::tolower(static_cast<unsigned char>(a[i])) !=
					std::tolower(static_cast<unsigned char>(b[i]))) {
					return false;
				}
			}
			return true;
		}
	};
}