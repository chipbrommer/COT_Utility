///////////////////////////////////////////////////////////////////////////////
//!
//! @file		cotParser.hpp
//! 
//! @brief		A class to parse COT Messages.
//! 
//! @author		Chip Brommer
//! 
//! @date		< 12 / 19 / 2022 > Initial Start Date
//!
/*****************************************************************************/
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include "cotInfo.hpp"                  // Contains COT Structures
#include "../../PugiXML/pugixml.hpp"    // For parsing XML
//
#include <vector>                       // Vectors
#include <sstream>                      // Stream for parsing strings
#include <string>                       // Strings
//
///////////////////////////////////////////////////////////////////////////////

class COTParser
{
public:

    //! @brief Default Construtor
    COTParser();

    //! @brief Default Deconstructor
    ~COTParser();

    //! @brief Verify a string buffer is XML
    //! @param buffer - [IN] - string buffer to be verified
    //! @return - true if good XML, false if not.
    bool VerifyXML(std::string& buffer);

    //! @brief Parse a COT Message from std::string
    //! @param buffer  - [IN]  - String buffer containing the XML data to be parsed.
    //! @param cot     - [OUT] - Vector of COT Structures to store the parsed data into. 
    //! @return -1 on error, 1 on good parse.
    int ParseCOT(std::string& buffer, COTSchema& cot);

    //! @brief Overloaded - Parse a COT Message from uint8_t buffer
    //! @param buffer  - [IN]  - uint8_t buffer containing the XML data to be parsed.
    //! @param Targets - [OUT] - Vector of COT Structures to store the parsed data into. 
    //! @return -1 on error, 1 on good parse.
    int ParseCOT(const uint8_t* buffer, COTSchema& cot);

    //! @brief Parse a COT Message
    //! @param Buffer  - [IN]  - uint8_t buffer containing the XML data to be parsed.
    //! @return A COT Structures containing the parsed data, use "COTSchema.Valid()" function for verify validity. 
    COTSchema ParseBufferToCOT(const uint8_t* buffer);

protected:
private:

    //! @brief Parse a string "type" attriubute
    //! @param type - [IN]  - Type string to be parsed
    //! @param ind  - [OUT] - enumeration value for the PointType parsed from string.
    //! @param loc  - [OUT] - enumeration value for the LocationType parsed from string.
    //! @return true if parsed, false if not
    bool ParseTypeAttribute(std::string& type, PointType& ind, LocationType& loc);

    //! @brief Parse a string "how" attriubute
    //! @param type - [IN]  - "How" string to be parsed
    //! @param how  - [OUT] - enumeration value for the HowEntryType parsed from string.
    //! @param data - [OUT] - enumeration value for the HowDataType parsed from string.
    //! @return true if parsed, false if not
    bool ParseHowAttribute(std::string& type, HowEntryType& how, HowDataType& data);

    //! @brief Parse a string "time" attriubute
    //! @param type - [IN]  - Time string to be parsed
    //! @param dt   - [OUT] - DateTime struct to store the parsed data into.
    //! @return true if parsed, false if not
    bool ParseTimeAttribute(std::string& type, DateTime& dt);

    //! @brief Parse a string date stamp
    //! @param type - [IN]  - Date string to be parsed
    //! @param dt   - [OUT] - DateTime struct to store the parsed data into.
    //! @return true if parsed, false if not
    bool ParseDateStamp(std::string& type, DateTime& dt);

    //! @brief Parse a string time stamp
    //! @param type - [IN]  - Time string to be parsed
    //! @param dt   - [OUT] - DateTime struct to store the parsed data into.
    //! @return true if parsed, false if not
    bool ParseTimeStamp(std::string& type, DateTime& dt);

    //! @brief Converts a string into a RootType enumeration value
    //! @param root - [IN] - string to be converted.
    //! @return RootType enum conversion
    RootType RootTypeCharToEnum(std::string& root);

    //! @brief Converts a string into a RootType enumeration value
    //! @param root - [IN] - string to be converted.
    //! @return RootType enum conversion
    PointType PointTypeCharToEnum(std::string& type);

    //! @brief Converts a string into a RootType enumeration value
    //! @param root - [IN] - string to be converted.
    //! @return RootType enum conversion
    LocationType LocationTypeCharToEnum(std::string& loc);

    //! @brief Converts a string into a RootType enumeration value
    //! @param root - [IN] - string to be converted.
    //! @return RootType enum conversion
    HowEntryType HowEntryTypeCharToEnum(std::string& entry);

    //! @brief Converts a string into a RootType enumeration value
    //! @param root  - [IN] - string to be converted.
    //! @param entry - [IN] - Entry Type to correspond to proper data type. 
    //! @return RootType enum conversion
    HowDataType HowDataTypeCharToEnum(std::string& data, HowEntryType entry);
};

