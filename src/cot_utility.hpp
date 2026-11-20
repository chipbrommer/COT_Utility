#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot_utility.hpp
// @brief           A utility for parsing, validating, and generating CoT messages.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <functional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <pugixml.hpp>

#include "version.hpp"
#include "result/result.hpp"
#include "handlers/error_handler.hpp"
#include "message/message.hpp"

namespace cot
{
    namespace detail_impl
    {
        inline result report_error(result res, result* rslt = nullptr) {
            if (rslt) *rslt = res;
            if (error_handler) error_handler(res);
            return res;
        }

        inline pugi::xml_node find_first_element_by_name(pugi::xml_node root, const char* target_name) noexcept {
            if (!target_name) return pugi::xml_node{};
            for (pugi::xml_node child : root.children()) {
                if (child.type() == pugi::node_element) {
                    if (child.name() == std::string_view(target_name)) {
                        return child;
                    }
                    if (pugi::xml_node found = find_first_element_by_name(child, target_name)) {
                        return found;
                    }
                }
            }
            return pugi::xml_node{};
        }

        inline std::size_t count_top_level_elements(const pugi::xml_document& doc, const char* target_name) noexcept {
            if (!target_name) return 0;
            std::size_t count = 0;
            for (pugi::xml_node child : doc.children()) {
                if (child.type() == pugi::node_element && child.name() == std::string_view(target_name)) {
                    ++count;
                }
            }
            return count;
        }

        inline result trim_leading_noise(std::string& xml, result* rslt = nullptr) {
            if (xml.empty()) {
                return report_error(
                    result{ result::error_code::InvalidInput, "Empty input buffer" }, rslt);
            }

            const std::size_t tag_start = xml.find('<');
            if (tag_start == std::string::npos) {
                return report_error(
                    result{ result::error_code::InvalidXml, "No XML markup found in buffer" }, rslt);
            }
            if (tag_start > 0) {
                xml.erase(0, tag_start);
            }
            return result{ result::error_code::Success };
        }
    } // namespace detail_impl

    /// @brief Get the current version information
    /// @return String containing version (e.g., "cot::utility v0.5.0")
    [[nodiscard]] inline std::string get_version() {
        std::ostringstream oss;
        oss << "cot::utility v" << COT_UTILITY_MAJOR << '.' << COT_UTILITY_MINOR << '.' << COT_UTILITY_BUILD;
        return oss.str();
    }

    /// @brief Verify a string buffer is valid XML
    /// @param buffer Input buffer to verify
    /// @param rslt Optional result structure receiving outcome details
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result verify_xml(std::string_view buffer, result* rslt = nullptr) {
        if (buffer.empty()) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidInput, "Empty input buffer" }, rslt);
        }

        pugi::xml_document doc;
        const pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidXml,
                        std::string(parse_result.description()) + " at offset " +
                        std::to_string(parse_result.offset) }, rslt);
        }

        result ok{ result::error_code::Success, "Successfully verified XML" };
        if (rslt) *rslt = ok;
        return ok;
    }

    /// @brief Prep an XML buffer to be parsed by trimming leading non-markup noise before the first '<'
    /// @param xml Mutable XML buffer to clean in-place
    /// @param rslt Optional result structure receiving outcome details
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result prep_xml_message(std::string& xml, result* rslt = nullptr) {
        result trim_res = detail_impl::trim_leading_noise(xml, rslt);
        if (trim_res.is_failed()) {
            return trim_res;
        }
        return verify_xml(xml, rslt);
    }

    /// @brief Validate that a buffer is a valid CoT structure
    /// @param buffer Buffer containing the CoT message
    /// @param rslt Optional result structure indicating reason CoT may be invalid
    /// @param document Optional pointer to receive the loaded XML document
    /// @return true if valid structure, else false
    [[nodiscard]] inline bool validate_cot_structure(std::string_view buffer,
        result* rslt = nullptr,
        pugi::xml_document* document = nullptr) {
        std::string cleaned(buffer);
        result trim_res;
        if (detail_impl::trim_leading_noise(cleaned, &trim_res).is_failed()) {
            if (rslt) *rslt = trim_res;
            return false;
        }

        pugi::xml_document doc;
        const pugi::xml_parse_result xml_result = doc.load_buffer(cleaned.data(), cleaned.size());
        if (!xml_result) {
            detail_impl::report_error(
                result{ result::error_code::InvalidXml,
                        std::string(xml_result.description()) + " at offset " +
                        std::to_string(xml_result.offset) }, rslt);
            return false;
        }

        const std::size_t event_count = detail_impl::count_top_level_elements(doc, "event");
        if (event_count != 1) {
            detail_impl::report_error(
                result{ result::error_code::InvalidEvent,
                        event_count == 0 ? "Missing <event> element" : "Multiple <event> elements found" }, rslt);
            return false;
        }

        pugi::xml_node event_node = doc.child("event");
        std::size_t point_count = 0;
        std::size_t detail_count = 0;
        for (pugi::xml_node child : event_node.children()) {
            if (child.type() == pugi::node_element) {
                if (child.name() == std::string_view("point")) ++point_count;
                else if (child.name() == std::string_view("detail")) ++detail_count;
            }
        }

        if (point_count != 1) {
            detail_impl::report_error(
                result{ result::error_code::InvalidPoint,
                        point_count == 0 ? "Missing <point> element" : "Multiple <point> elements found" }, rslt);
            return false;
        }

        if (detail_count > 1) {
            detail_impl::report_error(
                result{ result::error_code::InvalidDetail, "Multiple <detail> elements found" }, rslt);
            return false;
        }

        if (document) {
            *document = std::move(doc);
        }

        result ok{ result::error_code::Success, "Valid CoT structure" };
        if (rslt) *rslt = ok;
        return true;
    }

    /// @brief Create an XML CoT message from a schema
    /// @param msg Schema to convert to XML
    /// @param xml Output string containing XML text (cleared on error)
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result generate_xml_cot(const message& msg, std::string& xml) {
        xml.clear();

        result val_res;
        if (!msg.is_valid(&val_res)) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidData,
                        "Invalid CoT schema: " + val_res.description() });
        }

        result ser_res;
        std::string temp = msg.to_xml_string(&ser_res);
        if (temp.empty() || ser_res.is_failed()) {
            return detail_impl::report_error(
                result{ result::error_code::ProcessingError,
                        ser_res.description().empty()
                            ? std::string("Failed to serialize CoT message to XML string")
                            : ser_res.description() });
        }

        pugi::xml_document verify_doc;
        const pugi::xml_parse_result parse_result = verify_doc.load_buffer(temp.data(), temp.size());
        if (!parse_result) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidXml,
                        std::string(parse_result.description()) + " at offset " +
                        std::to_string(parse_result.offset) });
        }

        xml = std::move(temp);
        return result{ result::error_code::Success, "Successfully generated XML CoT message" };
    }

    /// @brief Add an acknowledgment attribute to a received CoT message's <status> element
    /// @param received Received XML message buffer
    /// @param response Output acknowledgment XML string
    /// @param rslt Optional result structure for error details
    /// @return Result indicating success or failure with description
    [[nodiscard]] inline result acknowledge_received_cot_message(std::string_view received,
        std::string& response,
        result* rslt = nullptr) {
        response.clear();

        std::string cleaned(received);
        result trim_res;
        if (detail_impl::trim_leading_noise(cleaned, &trim_res).is_failed()) {
            if (rslt) *rslt = trim_res;
            return trim_res;
        }

        pugi::xml_document doc;
        const pugi::xml_parse_result res = doc.load_buffer(cleaned.data(), cleaned.size());
        if (!res) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidXml,
                        std::string(res.description()) + " at offset " +
                        std::to_string(res.offset) }, rslt);
        }

        pugi::xml_node status_node = detail_impl::find_first_element_by_name(doc, "status");
        if (!status_node) {
            return detail_impl::report_error(
                result{ result::error_code::NoModificationMade, "No <status> element found" }, rslt);
        }

        if (status_node.attribute("acknowledgment")) {
            return detail_impl::report_error(
                result{ result::error_code::NoModificationMade,
                        "<status> already has acknowledgment attribute" }, rslt);
        }

        status_node.append_attribute("acknowledgment").set_value("ack");

        std::ostringstream oss;
        doc.save(oss);
        response = oss.str();

        result ok{ result::error_code::Success, "Acknowledgment added" };
        if (rslt) *rslt = ok;
        return ok;
    }

    /// @brief Parse a CoT message from a string_view buffer
    /// @param buffer Input buffer containing XML data
    /// @param msg Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] inline result parse_cot(std::string_view buffer, message& msg) {
        pugi::xml_document doc;
        result struct_res;
        if (!validate_cot_structure(buffer, &struct_res, &doc)) {
            return struct_res;
        }

        pugi::xml_node event_node = doc.child("event");
        result ev_res;
        msg.event_ = subschemas::event::from_xml(event_node, &ev_res);
        if (ev_res.is_failed()) {
            return ev_res;
        }

        result val_res;
        if (!msg.is_valid(&val_res)) {
            return val_res;
        }

        return result{ result::error_code::Success, "Successfully parsed CoT message" };
    }

    /// @brief Parse a CoT message from a null-terminated character buffer
    /// @param buffer Input buffer containing XML data
    /// @param msg Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] inline result parse_cot(const char* buffer, message& msg) {
        if (!buffer) {
            return detail_impl::report_error(
                result{ result::error_code::InvalidInput, "Null input buffer" });
        }
        return parse_cot(std::string_view(buffer), msg);
    }

    /// @brief Parse the <event> element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param e Output event instance
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result parse_event_from_cot(std::string_view buffer, subschemas::event& e) {
        pugi::xml_document doc;
        result struct_res;
        if (!validate_cot_structure(buffer, &struct_res, &doc)) {
            return struct_res;
        }

        result ev_res;
        e = subschemas::event::from_xml(doc.child("event"), &ev_res);
        if (ev_res.is_failed()) {
            return ev_res;
        }

        result val_res;
        if (!e.is_valid(&val_res)) {
            return val_res;
        }

        return result{ result::error_code::Success, "Successfully parsed <event>" };
    }

    /// @brief Parse the <point> element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param p Output point instance
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result parse_point_from_cot(std::string_view buffer, subschemas::point& p) {
        pugi::xml_document doc;
        result struct_res;
        if (!validate_cot_structure(buffer, &struct_res, &doc)) {
            return struct_res;
        }

        pugi::xml_node point_node = doc.child("event").child("point");
        if (!point_node) {
            return detail_impl::report_error(
                result{ result::error_code::InsufficientData, "Missing <point> element" });
        }

        result pt_res;
        p = subschemas::point::from_xml(point_node, &pt_res);
        if (pt_res.is_failed()) {
            return pt_res;
        }

        return result{ result::error_code::Success, "Successfully parsed <point>" };
    }

    /// @brief Parse the <detail> element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param out_detail Output detail instance
    /// @return result indicating success or failure with description
    [[nodiscard]] inline result parse_detail_from_cot(std::string_view buffer, subschemas::detail& out_detail) {
        pugi::xml_document doc;
        result struct_res;
        if (!validate_cot_structure(buffer, &struct_res, &doc)) {
            return struct_res;
        }

        pugi::xml_node detail_node = doc.child("event").child("detail");
        if (!detail_node) {
            return detail_impl::report_error(
                result{ result::error_code::InsufficientData, "Missing <detail> element" });
        }

        result det_res;
        out_detail = subschemas::detail::from_xml(detail_node, &det_res);
        if (det_res.is_failed()) {
            return det_res;
        }

        return result{ result::error_code::Success, "Successfully parsed <detail>" };
    }
} // namespace cot