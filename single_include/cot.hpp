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
            InvalidCotSchema,
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

#pragma region message_classes

    class event {
    public:
        /// @brief Constructor - Initializes everything
        event() : {}

        /// @brief Checks if the class has valid data
        bool is_valid(std::string* errorMsg = nullptr) const
        {
            bool valid = true;
            std::string errors;

            // Event and Point are required in CoT
            if (!event.is_valid(&errors))
            {
                valid = false;
                if (errorMsg && !errors.empty()) *errorMsg += "Event invalid: " + errors + "; ";
            }

            return valid;
        }
    private:
    };

    /// @brief A root XML CoT Message schema class for entire xml message data
    class message {
    public:
        Event event;                    /// Holds Event Sub-schema
        std::string xml_version;        /// Holds version from XML tag
        std::string xml_encoding;       /// Holds hold encoding from XML tag
        std::string xml_standalone;     /// Holds holds standalone bool from XML tag

        /// @brief Constructor - Initializes everything
        message(const Event& event = Event()) :
            event(event) {
        }

        /// @brief Equality comparison operator
        bool operator==(const message& other) const
        {
            return event == other.event &&
                xml_version == other.xml_version &&
                xml_encoding == other.xml_encoding &&
                xml_standalone == other.xml_standalone;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const message& other) const
        {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        bool is_valid(std::string* errorMsg = nullptr) const
        {
            bool valid = true;
            std::string errors;

            // Event and Point are required in CoT
            if (!event.is_valid(&errors))
            {
                valid = false;
                if (errorMsg && !errors.empty()) *errorMsg += "Event invalid: " + errors + "; ";
            }

            return valid;
        }
 
    };

#pragma endregion

#pragma region utility
    /// @brief Get the current version information
    /// @return String containing version (e.g., "1.0.0")
    [[nodiscard]] std::string get_version()
    {
        std::ostringstream oss;
        oss << "cot::utility v" << COT_UTILITY_MAJOR << '.' << COT_UTILITY_MINOR << '.' << COT_UTILITY_BUILD;
        return oss.str();
    }

    /// @brief Verify a string buffer is valid XML
    /// @param buffer Input buffer to verify
    /// @return result indicating success or failure with description
    result verify_xml(std::string buffer) {
        if (buffer.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" }
        }

        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml, parse_result.description() };
        }

        return result{ result::error_code::Success, "Successfully verified XML" };
    }

    /// @brief prep an xml buffer to be parsed, will clear content before the starting xml tag
    /// @param xml string view containtaining the xml
    /// @return result indicating success or failure with description
    result prep_message(std::string& xml)
    {
        if (xml.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" }
        }

        size_t position = xml.find("<?xml");
        if (position == std::string::npos)
        {
            return result(result::error_code::InvalidXml);
        }
        xml.erase(0, position);

        // Verify XML structure
        result rslt = verify_xml(xml);
        if (rslt.is_failed())
        {
            return rslt;
        }
        return result::error_code::Success);
    }

    /// @brief Create an XML CoT message from a schema
    /// @param cot Schema to convert to XML
    /// @param xml std::string contianing xml text, xml is empty if error occurs
    /// @return result indicating success or failure with description
    [[nodiscard]] result generate_xml_cot(const schema& cot, std::string& xml) {
        // Validate schema
        std::string error;
        auto validation_result = cot.is_valid(&error);
        if (!validation_result) {
            return result{ result::error_code::InvalidCotSchema, "Invalid cot schema: " + error };
        }

        // Create XML document
        pugi::xml_document doc;

        // Add XML declaration
        pugi::xml_node decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "utf-8";
        decl.append_attribute("standalone") = "yes";

        // Generate XML for event, point, and detail
        std::string temp = cot.to_xml();

        // Verify generated XML
        pugi::xml_document verify_doc;
        pugi::xml_parse_result parse_result = verify_doc.load_string(temp.data(), temp.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml,
                                        std::string(parse_result.description()) + " at offset " +
                                        std::to_string(parse_result.offset) };
        }

        xml = temp;
        return result{ result::error_code::Success, "Successfully generated XML CoT message" };
    }

    /// @brief Add an acknowledgment to a received CoT message
    /// @param received Received message
    /// @param response Output acknowledgment message
    /// @return Result indicating success or failure with description
    [[nodiscard]] result acknowledge_received_cot_message(std::string_view received,
        std::string& response)
    {
        // Create XML document to load in the receivedMessage
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_string(receivedMessage.c_str());

        if (res)
        {
            // Modified flag
            bool modified = false;

            // Find the 'status' node
            pugi::xml_node statusNode = doc.select_node("//status").node();

            if (statusNode)
            {
                // Check if acknowledgment attribute doesn't exist
                if (!statusNode.attribute("acknowledgment"))
                {
                    // Add acknowledgment attribute with value "ack"
                    statusNode.append_attribute("acknowledgment").set_value("ack");
                    modified = true;
                }
            }

            // If modified, save the modified XML string
            if (modified)
            {
                std::stringstream modifiedXmlStream;
                doc.save(modifiedXmlStream);
                responseMessage = modifiedXmlStream.str();
                return result(result::error_code::Success);
            }

            // No modification made
            return result(result::error_code::NoModificationMade);
        }

        return result(result::error_code::ProcessingError, result.description());
    }

    /// @brief Parse a CoT message from a string buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(std::string_view buffer, Schema& cot)
    {
        // Convert input buffer to string and remove garbage before <?xml
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

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
    [[nodiscard]] result parse_cot(const char* buffer, Schema& cot)
    {
        return parse_cot(std::string_view(buffer), cot);
    }

    /// @brief Parse the Event element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param event Output event instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_event_from_cot(std::string_view buffer, Event& event)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");

        // Use Track::FromXml to parse the track node
        event = Event::FromXml(eventNode);

        // Success
        return result(result::error_code::Success);
    }

    /// @brief Parse the Point element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param point Output point instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_point_from_cot(std::string_view buffer, Point& point)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node pointNode = eventNode.child("point");
        if (!pointNode)
        {
            return result(result::error_code::InsufficientData);
        }

        // Use Point::FromXml to parse the track node
        point = Point::FromXml(pointNode);

        // Success
        return result(result::error_code::Success);
    }

    /// @brief Parse the Detail element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param detail parsed detail instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_detail_from_cot(std::string_view buffer, Detail& detail)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node detailNode = eventNode.child("detail");
        if (!detailNode)
        {
            return result(result::error_code::InsufficientData);
        }

        // Use Detail::FromXml to parse the track node
        detail = Detail::FromXml(detailNode);

        // Success
        return result(result::error_code::Success);
    }

#pragma endregion
}