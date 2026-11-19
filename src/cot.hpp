#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot.hpp
// @brief           a single include api for handling CoT messages
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                            reason included
//          --------------------            ------------------------------------
#include <algorithm>
#include <climits>                          // MAX and MIN
#include <cmath>                            // isnan
#include <functional>                       // function
#include <iostream>                         // ostream
#include <iomanip>                          // setw
#include <map>                              // ordered map
#include <optional>
#include <sstream>                          // stringstream
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

#pragma region message_classes

    /*
    /// @brief A COT Message subschema class for strokecolor data
    class strokecolor
    {
    public:
        int value; /// Color value (ARGB as signed 32-bit integer)

        /// @brief Constructor - Initializes Everything
        strokecolor(int value = INVALID_VALUE) : value(value) {}

        /// @brief Equality comparison operator
        bool operator==(const strokecolor& other) const {
            return value == other.value;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const strokecolor& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "strokecolor value is unset"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<strokeColor/>"; // Empty tag for unset color
            }
            std::ostringstream oss;
            oss << "<strokeColor value=\"" << value << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static strokecolor from_xml(const pugi::xml_node& node) {
            strokecolor color;
            pugi::xml_attribute attr;
            if (attr = node.attribute("value")) {
                try {
                    color.value = attr.as_int(INVALID_VALUE);
                } catch (const std::exception& e) {
                    color.value = INVALID_VALUE;
                }
            }
            return color;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const strokecolor& color) {
            os << "strokecolor: ";
            if (!color.is_valid()) { os << " -NOT VALID- (Unset)"; }
            else { os << "\n\tvalue: " << color.value; }
            os << "\n";
            return os;
        }

    private:
        static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
    };
    */
    
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
            return result{ result::error_code::InvalidInput, "Empty input buffer" };
        }

        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml, parse_result.description() };
        }

        return result{ result::error_code::Success, "Successfully verified XML" };
    }

    /// @brief Validate that a buffer is a valid cot structure
    /// @param buffer buffer containing the cot message
    /// @param result optional result structure to indicate reason cot may be invalid
    /// @param document optional document to get the loaded document from buffer.
    /// @return true if valid, else false
    [[nodiscard]] bool validate_cot_structure(std::string_view buffer, result* rslt = nullptr, pugi::xml_document& document = nullptr) {
        // Parse XML
        pugi::xml_document doc;
        pugi::xml_parse_result xmlresult = doc.load_string(buffer);
        if (!xmlresult) {
            if (rslt) { *rslt = result(result::error_code::ProcessingError); }
            return false;
        }

        // Set the document if one is passed in. 
        if (document) { document = doc; }

        // Validate structure
        pugi::xml_node eventNode = doc.child("event");
        if (!eventNode || doc.root().select_nodes("event").size() != 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidEvent); }
            return false;
        }
        if (eventNode.select_nodes("point").size() != 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidPoint); }
            return false;
        }
        if (eventNode.select_nodes("detail").size() > 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidDetail); }
            return false;
        }

        if (rslt) { *rslt = result(result::error_code::Success); }
        return true;
    }

    /// @brief prep an xml buffer to be parsed, will clear content before the starting xml tag
    /// @param xml string view containtaining the xml
    /// @return result indicating success or failure with description
    result prep_xml_message(std::string& xml)
    {
        if (xml.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" };
        }

        size_t position = xml.find("<?xml");
        if (position == std::string::npos) {
            return result(result::error_code::InvalidXml);
        }
        xml.erase(0, position);

        // Verify XML structure
        result rslt = verify_xml(xml);
        if (rslt.is_failed()) {
            return rslt;
        }

        return result(result::error_code::Success);
    }

    /// @brief Create an XML CoT message from a schema
    /// @param cot Schema to convert to XML
    /// @param xml std::string contianing xml text, xml is empty if error occurs
    /// @return result indicating success or failure with description
    [[nodiscard]] result generate_xml_cot(const message& cot, std::string& xml) {
        // Validate schema
        std::string error;
        result rslt;
        auto validation_result = cot.is_valid(&rslt);
        if (!validation_result) {
            return result{ result::error_code::InvalidData, "Invalid cot schema: " + error };
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
        std::string& response) {
        // Create XML document to load in the received
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_string(received.str());

        if (res) {
            // Modified flag
            bool modified = false;

            // Find the 'status' node
            pugi::xml_node statusNode = doc.select_node("//status").node();

            if (statusNode) {
                // Check if acknowledgment attribute doesn't exist
                if (!statusNode.attribute("acknowledgment")) {
                    // Add acknowledgment attribute with value "ack"
                    statusNode.append_attribute("acknowledgment").set_value("ack");
                    modified = true;
                }
            }

            // If modified, save the modified XML string and return success
            if (modified) {
                std::stringstream modifiedXmlStream;
                doc.save(modifiedXmlStream);
                response = modifiedXmlStream.str();
                return result();
            }

            // No modification made
            return result(result::error_code::NoModificationMade);
        }

        return result(result::error_code::ProcessingError);
    }

    /// @brief Parse a CoT message from a string buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(std::string_view buffer, message& cot) {
        // Convert input buffer to string and remove garbage before <?xml
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML
        pugi::xml_document doc;
        if (!validate_cot_structure(buffer, rslt, doc)) {
            return rslt;
        }

        // Parse event
        cot.event_ = event::from_xml(eventNode);

        // Validate parsed data
        if (!cot.is_valid(&rslt)) {
            return rslt;
        }

        return result(result::error_code::Success);
    }

    /// @brief Parse a CoT message from a character buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(const char* buffer, message& cot) {
        return parse_cot(std::string_view(buffer), cot);
    }

    /// @brief Parse the Event element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param event Output event instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_event_from_cot(std::string_view buffer, event& e) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event>
        pugi::xml_node eventNode = doc.child("event");
        e = event::from_xml(eventNode);

        // Success
        return result();
    }

    /// @brief Parse the Point element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param point Output point instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_point_from_cot(std::string_view buffer, point& p) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <point>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node pointNode = eventNode.child("point");
        if (!pointNode) {
            return result(result::error_code::InsufficientData);
        }
        p = point::from_xml(pointNode);

        // Success
        return result();
    }

    /// @brief Parse the detail element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param detail parsed detail instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_detail_from_cot(std::string_view buffer, detail& detail) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node detailNode = eventNode.child("detail");
        if (!detailNode) {
            return result(result::error_code::InsufficientData);
        }
        detail = detail::from_xml(detailNode);

        // Success
        return result();
    }

#pragma endregion
}