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
    /// @brief Default Constructor
    CoT_Utility() = default;

    /// @brief Default Destructor
    ~CoT_Utility() = default;

    /// @brief Verify a string buffer is XML
    /// @param buffer - [in] - string buffer to be verified
    /// @return Result indicating success or failure with description
    Result VerifyXML(std::string& buffer);

    /// @brief Create an XML COT message from a schema
    /// @param cot - [in] - COT Schema to be created into a message
    /// @return Result containing the XML message or error description
    Result GenerateXMLCOTMessage(CoT_Schema& cot);

    /// @brief Update fields within a received message to preserve its original content
    /// @param receivedMessage - [in] - the original received message
    /// @param cot - [in] - the CoT_Schema to update the fields with
    /// @param modifiedMessage - [out] - the resulting modified message if any edits were made
    /// @param acknowledgment - [in/opt] - A flag to indicate the message as an acknowledgement
    /// @return Result indicating success or failure with description
    Result UpdateReceivedCOTMessage(std::string& receivedMessage, CoT_Schema& cot,
        std::string& modifiedMessage, bool acknowledgment = false);

    /// @brief Add an acknowledgement to a received message for response
    /// @param receivedMessage - [in] - the message received
    /// @param responseMessage - [out] - the acknowledgement message
    /// @return Result indicating success or failure with description
    Result AcknowledgeReceivedCOTMessage(std::string& receivedMessage, std::string& responseMessage);

    /// @brief Parse a COT Message from std::string
    /// @param buffer - [in] - String buffer containing the XML data to be parsed
    /// @param cot - [out] - COT Structures to store the parsed data into
    /// @return Result indicating success or failure with description
    Result ParseCOT(std::string& buffer, CoT_Schema& cot);

    /// @brief Parse a COT Message from uint8_t buffer
    /// @param buffer - [in] - char buffer containing the XML data to be parsed
    /// @param cot - [out] - COT Structures to store the parsed data into
    /// @return Result indicating success or failure with description
    Result ParseCOT(const char* buffer, CoT_Schema& cot);

    /// @brief Parse a COT Message
    /// @param buffer - [in] - char buffer containing the XML data to be parsed
    /// @return Result containing the parsed CoT_Schema or error description
    Result ParseBufferToCOT(const char* buffer);

    /// @brief Parse the Track element from a Cursor on Target message
    /// @param buffer - [in] - char buffer containing the XML data to be parsed
    /// @param track - [out] - reference to a Track instance to parse data into
    /// @return Result indicating success or failure with description
    Result ParseTrackFromCoT(const char* buffer, Track& track);

    /// @brief Get a string containing the current version information
    std::string GetVersion() const;

protected:
private:
    /// @brief Parse a string "type" attribute
    /// @param type - [in] - Type string to be parsed
    /// @param ind - [out] - enumeration value for the PointType parsed from string
    /// @param loc - [out] - enumeration value for the LocationType parsed from string
    /// @return Result indicating success or failure with description
    Result ParseTypeAttribute(std::string& type, Point::Type& ind, Location::Type& loc);

    /// @brief Parse a string "how" attribute
    /// @param type - [in] - "How" string to be parsed
    /// @param how - [out] - enumeration value for the HowEntryType parsed from string
    /// @param data - [out] - enumeration value for the HowDataType parsed from string
    /// @return Result indicating success or failure with description
    Result ParseHowAttribute(std::string& type, How::Entry::Type& how, How::Data::Type& data);

    /// @brief Parse a string "time" attribute
    /// @param type - [in] - Time string to be parsed
    /// @param dt - [out] - DateTime struct to store the parsed data into
    /// @return Result indicating success or failure with description
    Result ParseTimeAttribute(std::string& type, DateTime& dt);

    /// @brief Parse a string date stamp
    /// @param type - [in] - Date string to be parsed
    /// @param dt - [out] - DateTime struct to store the parsed data into
    /// @return Result indicating success or failure with description
    Result ParseDateStamp(std::string& type, DateTime& dt);

    /// @brief Parse a string time stamp
    /// @param type - [in] - Time string to be parsed
    /// @param dt - [out] - DateTime struct to store the parsed data into
    /// @return Result indicating success or failure with description
    Result ParseTimeStamp(std::string& type, DateTime& dt);

    const int MAJOR = 0;
    const int MINOR = 4;
    const int BUILD = 1;
};