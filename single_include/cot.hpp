#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            CoT_Utility.h
// @brief           A class for generating and parsing CoT messages
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                            reason included
//          --------------------            ------------------------------------
#include <iostream>                         // ostream
#include <iomanip>                          // setw
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>                    // maps
//
#include <pugixml.hpp>                      // pugi xml
// 
/////////////////////////////////////////////////////////////////////////////////

namespace cot
{
    constexpr auto COT_UTILITY_MAJOR = 0;
    constexpr auto COT_UTILITY_MINOR = 5;
    constexpr auto COT_UTILITY_BUILD = 0;

    /// @brief Result structure for CoT operations
    struct result {
        /// @brief Error code indicating the operation outcome
        enum class error_code : int {
            Success,                // No error
            InvalidEvent,           // XML is missing Event tag
            InvalidPoint,           // XML is missing Point tag
            InvalidDetail,          // XML has invalid Detail tag
            InvalidDate,
            InvalidTime,
            InvalidHow,
            InvalidType,
            InvalidXml,             // String is not valid XML
            InvalidInput,           // Input string is empty or malformed
            InvalidTimeSubSchema,
            InsufficientData,
            ProcessingError,        // Generic processing failure
            NoModificationMade
        };

        /// @brief Check if the operation was successful
        bool is_success() const { return m_code == error_code::Success; }

        /// @brief Check if the operation failed
        bool is_failed() const { return !is_success(); }

        /// @brief Constructor for success case
        result() : m_code(error_code::Success), m_description("") {}

        /// @brief Constructor for error case
        result(error_code e, std::string desc) : m_code(e), m_description(std::move(desc)) {}

        /// @brief Convert to string for debugging or logging
        std::string to_string() const {
            std::string result = "Result: ";
            result += std::to_string(m_code);
            if (!m_description.empty()) {
                result += "; Description: " + m_description;
            }
            return result;
        }

        std::string description() { return this->m_description; }
    private:
        static std::string to_string(error_code error)
        {
            switch (error)
            {
            case error_code::Success:
                return "Success";
            case error_code::InvalidEvent:
                return "XML has invalid Event tag";
            case error_code::InvalidPoint:
                return "XML has invalid Point tag";
            case error_code::InvalidDate:
                return "XML has invalid Date tag; Date string must have minimum 3 type identifiers (Year, Month, Day)";
            case error_code::InvalidTime:
                return "XML has invalid Time tag; Time must have minimum 3 type identifiers (Hour, Minute, Secs)";
            case error_code::InvalidHow:
                return "XML has invalid How tag";
            case error_code::InvalidType:
                return "XML has invalid Type tag";
            case error_code::InvalidXml:
                return "Invalid XML input";
            case error_code::InvalidInput:
                return "Invalid or empty input";
            case error_code::InvalidTimeSubSchema:
                return "Invalid Time sub-schema";
            case error_code::InsufficientData:
                return "Insufficient Data";
            case error_code::ProcessingError:
                return "Processing error";
            case error_code::NoModificationMade:
                return "No modification made";
            default:
                return "Unknown error";
            }
        }

        error_code m_code;          /// Specific error code
        std::string m_description;  /// Detailed error message
    };

    /// @brief Error handler for custom error logging
    using ErrorHandler = std::function<void(const result&)>;
    inline ErrorHandler error_handler;
    void set_error_handler(ErrorHandler handler) {
        error_handler = std::move(handler);
    }

    /// @brief Verify a string buffer is valid XML
    /// @param buffer Input buffer to verify
    /// @return bool indicating success or failure
    bool verify_xml(std::string_view buffer) {
        if (buffer.empty()) {
            if (error_handler) {
                error_handler(result{ result::error_code::InvalidInput, "Empty input buffer" });
            }
            return false;
        }
        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            if (error_handler) {
                error_handler(result{ result::error_code::InvalidXml, parse_result.description() });
            }
            return false;
        }
        if (error_handler) {
            error_handler(result{ result::error_code::Success, "Successfully verified XML" });
        }
        return true;
    }

    /// @brief Create an XML CoT message from a schema
    /// @param cot Schema to convert to XML
    /// @return Optional string containing the XML message, or nullopt on error
    [[nodiscard]] std::optional<std::string> generate_xml_cot(const schema& cot) {
        // Validate schema
        std::string error;
        auto validation_result = cot.is_valid(&error);
        if (!validation_result.is_success()) {
            if (error_handler) {
                error_handler(validation_result);
            }
            return std::nullopt;
        }

        // Create XML document
        pugi::xml_document doc;

        // Add XML declaration
        pugi::xml_node decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "utf-8";
        decl.append_attribute("standalone") = "yes";

        // Create root event node
        pugi::xml_node event_node = doc.append_child("event");

        // Generate XML for event, point, and detail
        auto event_result = cot.event.to_xml(event_node);
        if (!event_result.is_success()) {
            if (error_handler) {
                error_handler(event_result);
            }
            return std::nullopt;
        }

        auto point_result = cot.point.to_xml(event_node);
        if (!point_result.is_success()) {
            if (error_handler) {
                error_handler(point_result);
            }
            return std::nullopt;
        }

        auto detail_result = cot.detail.to_xml(event_node);
        if (!detail_result.is_success()) {
            if (error_handler) {
                error_handler(detail_result);
            }
            return std::nullopt;
        }

        // Write XML to string
        std::stringstream xml_stream;
        doc.save(xml_stream);
        std::string xml = xml_stream.str();

        // Verify generated XML
        pugi::xml_document verify_doc;
        pugi::xml_parse_result parse_result = verify_doc.load_buffer(xml.data(), xml.size());
        if (!parse_result) {
            auto error_result = result{ result::error_code::InvalidXml,
                                       std::string(parse_result.description()) + " at offset " +
                                       std::to_string(parse_result.offset) };
            if (error_handler) {
                error_handler(error_result);
            }
            return std::nullopt;
        }

        if (error_handler) {
            error_handler(result{ result::error_code::Success, "Successfully generated XML CoT message" });
        }
        return xml;
    }

    /// @brief Update fields within a received CoT message
    /// @param received Original received message
    /// @param cot Schema with fields to update
    /// @param modified Output modified message
    /// @param acknowledgment Flag to indicate acknowledgment
    /// @return Result indicating success or failure with description
    [[nodiscard]] result update_received_cot_message(std::string_view received,
        const Schema& cot,
        std::string& modified,
        bool acknowledgment = false);

    /// @brief Add an acknowledgment to a received CoT message
    /// @param received Received message
    /// @param response Output acknowledgment message
    /// @return Result indicating success or failure with description
    [[nodiscard]] result acknowledge_received_cot_message(std::string_view received,
        std::string& response);

    /// @brief Parse a CoT message from a string buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(std::string_view buffer, Schema& cot)
    {
        // Remove any trash before "<?xml"
        size_t position = buffer.find("<?xml");
        if (position == std::string::npos) { return Result(Result::Code::InvalidXml); }
        buffer.erase(0, position);

        // Parse XML
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(buffer.c_str());
        if (!result) { return Result(Result::Code::ProcessingError); }

        // Validate structure
        pugi::xml_node eventNode = doc.child("event");
        if (!eventNode || doc.root().select_nodes("event").size() != 1)
        {
            return Result(result::error_code::InvalidEvent);
        }
        if (eventNode.select_nodes("point").size() != 1)
        {
            return Result(Result::Code::InvalidPoint);
        }
        if (eventNode.select_nodes("detail").size() > 1)
        {
            return Result(Result::Code::InvalidXml);
        }

        // Parse event, point, and detail (if present)
        cot.event = Event::FromXml(eventNode);
        cot.point = Point::Data::FromXml(eventNode.child("point"));
        if (pugi::xml_node detailNode = eventNode.child("detail"))
        {
            cot.detail = Detail::FromXml(detailNode);
        }

        // Validate parsed data
        std::string error;
        if (!cot.event.IsValid(&error))
        {
            return Result(Result::Code::InvalidEvent, std::string("Parsed Event is invalid: " + error));
        }
        if (!cot.point.IsValid(&error))
        {
            return Result(Result::Code::InvalidPoint, std::string("Parsed Point is invalid: " + error));
        }
        if (eventNode.child("detail") && !cot.detail.IsValid(&error))
        {
            return Result(Result::Code::InvalidDetail, std::string("Parsed Detail is invalid: " + error));
        }

        return Result(Result::Code::Success);
    }

    /// @brief Parse a CoT message from a character buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(const char* buffer, Schema& cot);

    /// @brief Parse a CoT message into a schema
    /// @param buffer Input buffer containing XML data
    /// @return Result containing parsed schema or error description
    [[nodiscard]] result parse_buffer_to_cot(std::string_view buffer);

    /// @brief Parse the Track element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param track Output track instance
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_track_from_cot(std::string_view buffer, Track& track);

    /// @brief Get the current version information
    /// @return String containing version (e.g., "1.0.0")
    [[nodiscard]] std::string get_version()
    {
        std::ostringstream oss;
        oss << "cot::utility v" << COT_UTILITY_MAJOR << '.' << COT_UTILITY_MINOR << '.' << COT_UTILITY_BUILD;
        return oss.str();
    }
}