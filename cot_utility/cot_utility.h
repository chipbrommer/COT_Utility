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
#include <unordered_map>                    // maps
//
#include "cot_info.h"                       // schemas
#include <pugixml.hpp>                      // pugi xml
// 
/////////////////////////////////////////////////////////////////////////////////

class CoT_Utility
{
public:

    /// @brief Default Construtor
    CoT_Utility() = default;

    /// @brief Default Deconstructor
    ~CoT_Utility() = default;

    /// @brief Verify a string buffer is XML
    /// @param buffer - [in] - string buffer to be verified
    /// @return - true if good XML, false if not.
    bool VerifyXML(std::string& buffer);

    /// @brief Create an XML COT message from a schema
    /// @param cot - [in] - COT Schema to be created into a message
    /// @return std::string containing the xml message. 
    std::string GenerateXMLCOTMessage(CoT_Schema& cot);

    /// @brief Update fields withing a received message to preserve its original content
    /// @param receivedMessage - the original received message
    /// @param cot - the CoT_Schema to update the fields with
    /// @param modifiedMessage - the resulting modified message if any edits were made
    /// @param acknowledgment - [in/opt] - A flag to indicate the message as an acknowledgement
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult UpdateReceivedCOTMessage(std::string& receivedMessage, CoT_Schema& cot, std::string& modifiedMessage, bool acknowledgment = false);

    /// @brief Add an acknowledgement to a received message for response
    /// @param receivedMessage - the message received
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult AcknowledgeReceivedCOTMessage(std::string& receivedMessage, std::string& responseMessage);

    /// @brief Parse a COT Message from std::string
    /// @param buffer  - [in]  - String buffer containing the XML data to be parsed.
    /// @param cot     - [out] - COT Structures to store the parsed data into. 
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseCOT(std::string& buffer, CoT_Schema& cot);

    /// @brief Overloaded - Parse a COT Message from uint8_t buffer
    /// @param buffer  - [in]  - char buffer containing the XML data to be parsed.
    /// @param Targets - [out] - Vector of COT Structures to store the parsed data into. 
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseCOT(const char* buffer, CoT_Schema& cot);

    /// @brief Parse a COT Message
    /// @param Buffer  - [in]  - char buffer containing the XML data to be parsed.
    /// @return A COT Structures containing the parsed data, use "CoT_Schema.IsValid()" function for verify validity. 
    CoT_Schema ParseBufferToCOT(const char* buffer);

    /// @brief Parse the Track element from a Cursor on Target message
    /// @param buffer - [in] - char buffer containing the XML data to be parsed.
    /// @param track - [in] - reference to a Track instance to parse data into.
    /// @return Track instance with parsed data. 
    CoT_UtilityResult ParseTrackFromCoT(const char* buffer, Track& track);

    /// @brief Utility function to convert CoT_UtilityResult to string
    std::string UtilityResultToString(CoT_UtilityResult error);

    /// @brief Get a string containing the current version information
    std::string GetVersion() const;

    /// @brief Get the last XML related error description, set when a CoT_UtilityResult::ProcessingError occurs
    /// @return string containing the last description from the XML parsing. 
    std::string GetLastXmlError() const;

protected:
private:

    /// @brief Parse a string "type" attriubute
    /// @param type - [in]  - Type string to be parsed
    /// @param ind  - [out] - enumeration value for the PointType parsed from string.
    /// @param loc  - [out] - enumeration value for the LocationType parsed from string.
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseTypeAttribute(std::string& type, Point::Type& ind, Location::Type& loc);

    /// @brief Parse a string "how" attriubute
    /// @param type - [in]  - "How" string to be parsed
    /// @param how  - [out] - enumeration value for the HowEntryType parsed from string.
    /// @param data - [out] - enumeration value for the HowDataType parsed from string.
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseHowAttribute(std::string& type, How::Entry::Type& how, How::Data::Type& data);

    /// @brief Parse a string "time" attriubute
    /// @param type - [in]  - Time string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseTimeAttribute(std::string& type, DateTime& dt);

    /// @brief Parse a string date stamp
    /// @param type - [in]  - Date string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseDateStamp(std::string& type, DateTime& dt);

    /// @brief Parse a string time stamp
    /// @param type - [in]  - Time string to be parsed
    /// @param dt   - [out] - DateTime struct to store the parsed data into.
    /// @return CoT_UtilityResult enumeration, CoT_UtilityResult::Success or other, use CoT_UtilityResultToString to find out more info
    CoT_UtilityResult ParseTimeStamp(std::string& type, DateTime& dt);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Root::Type RootTypeCharToEnum(std::string& root);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Point::Type PointTypeCharToEnum(std::string& type);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    Location::Type LocationTypeCharToEnum(std::string& loc);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root - [in] - string to be converted.
    /// @return RootType enum conversion
    How::Entry::Type HowEntryTypeCharToEnum(std::string& entry);

    /// @brief Converts a string into a RootType enumeration value
    /// @param root  - [in] - string to be converted.
    /// @param entry - [in] - Entry Type to correspond to proper data type. 
    /// @return RootType enum conversion
    How::Data::Type HowDataTypeCharToEnum(std::string& data, How::Entry::Type entry);

    const int MAJOR = 0;
    const int MINOR = 3;
    const int BUILD = 4;

    std::string m_lastPugiResult;
};
