///////////////////////////////////////////////////////////////////////////////
//!
//! @file		cotParser.cpp
//! 
//! @brief		Implementation for the COTParser class
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
#include "cotParser.hpp"                // COT Parser header.
//
///////////////////////////////////////////////////////////////////////////////

COTParser::COTParser()
{

}

COTParser::~COTParser()
{

}

bool COTParser::VerifyXML(std::string& buffer)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(buffer.c_str());

    if (!result)
    {
        std::cout << "Error: " << result.description() << "\n";
        return false;
    }

    return true;
}

int COTParser::ParseCOT(std::string& buffer, COTSchema& cot)
{
    // Remove any trash that may come in before the "<?xml" tag.
    size_t position = buffer.find("<?xml");
    buffer.erase(0, position);

    // Verify buffer is good XML data first. 
    if (!VerifyXML(buffer))
    {
        return -1;
    }

    // Create a parsed xml document.
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(buffer.c_str());

    // Set up nodes for ease and easier reading later
    pugi::xml_node root = doc.root();
    pugi::xml_node events = doc.child("event");
    pugi::xml_node points = doc.child("event").child("point");
    pugi::xml_node details = doc.child("event").child("detail");
    int eventsSize = (int)doc.root().select_nodes("event").size();
    int pointsSize = (int)events.select_nodes("point").size();
    int detailsSize = (int)events.select_nodes("details").size();

    // Catch empty data:
    //      <event> and <point> are required data for COT message. 
    //      We dont check <detail> because it is optional data. 
    if (eventsSize == 0)
    {
        return 0;
    }
    else if (pointsSize == 0)
    {
        return 0;
    }

    // Parse <event> tag and gather data. 
    for (auto&& event : root.children("event"))
    {
        std::string time, start, stale;
        // Read attribute value
        pugi::xml_attribute attr;
        (attr = event.attribute("version")) ? cot.event.version = attr.as_double() : cot.event.version = 0;

        // Parse Type attribute into data points.
        (attr = event.attribute("type")) ? cot.event.type = attr.as_string() : cot.event.type = "";
        ParseTypeAttribute(cot.event.type, cot.event.indicator, cot.event.location);

        // Parse UID
        (attr = event.attribute("uid")) ? cot.event.uid = attr.as_string() : cot.event.uid = "";

        // Parse times into data points in COT structure
        (attr = event.attribute("time")) ? time = attr.as_string() : time = "";
        ParseTimeAttribute(time, cot.event.time);
        (attr = event.attribute("start")) ? start = attr.as_string() : start = "";
        ParseTimeAttribute(start, cot.event.start);
        (attr = event.attribute("stale")) ? stale = attr.as_string() : stale = "";
        ParseTimeAttribute(stale, cot.event.stale);

        // Parse How attribute into data points.
        (attr = event.attribute("how")) ? cot.event.how = attr.as_string() : cot.event.how = "";
        ParseHowAttribute(cot.event.how, cot.event.howEntry, cot.event.howData);

        // Parse <event><point> tag and gather data. 
        for (auto&& point : event.children("point"))
        {
            // Read attribute value
            pugi::xml_attribute attr1;
            (attr1 = point.attribute("lat")) ? cot.point.latitude = attr1.as_double() : cot.point.latitude = 0;
            (attr1 = point.attribute("lon")) ? cot.point.longitutde = attr1.as_double() : cot.point.longitutde = 0;
            (attr1 = point.attribute("hae")) ? cot.point.hae = attr1.as_double() : cot.point.hae = 0;
            (attr1 = point.attribute("ce")) ? cot.point.circularError = attr1.as_double() : cot.point.circularError = 0;
            (attr1 = point.attribute("le")) ? cot.point.linearError = attr1.as_double() : cot.point.linearError = 0;
        }

        // Parse <event><detail> tag and gather data. 
        //  NOTE: The only data we gather here currently is the subschema <track>
        for (auto&& detail : events.children("detail"))
        {
            pugi::xml_attribute attr1;
            pugi::xml_node track = detail.child("track");
            if (track)
            {
                (attr1 = track.attribute("course")) ? cot.detail.track.course = attr1.as_double() : cot.detail.track.course = 0;
                (attr1 = track.attribute("speed")) ? cot.detail.track.speed = attr1.as_double() : cot.detail.track.speed = 0;
            }
        }
    }

    // return number of points read
    return pointsSize;
}

bool COTParser::ParseTypeAttribute(std::string& type, PointType& ind, LocationType& loc)
{
    // Read the data from the file as String Vector
    std::vector<std::string> values;
    values.clear();
    std::string word;
    char delimiter = '-';
    std::stringstream stream(type);

    // read the row of data and store it in a string variable, 'word'
    while (std::getline(stream, word, delimiter))
    {
        // Remove any spaces that may be in the stream.
        word.erase(
            remove_if(word.begin(), word.end(), static_cast<int(*)(int)>(isspace)),
            word.end());

        // Push word onto the vector
        values.push_back(word);
    }

    // Type string must have minimum 3 type identifiers to give us the data we need. 
    //      Must also start with an "a" as its the onlt identifier we currently support. 
    if ((values.size() < 2) || (values[0] != "a"))
    {
        return false;
    }
    else
    {
        ind = PointTypeCharToEnum(values[1]);
        loc = LocationTypeCharToEnum(values[2]);
    }

    return true;
}

bool COTParser::ParseHowAttribute(std::string& type, HowEntryType& how, HowDataType& data)
{
    // Read the data from the file as String Vector
    std::vector<std::string> values;
    values.clear();
    std::string word;
    char delimiter = '-';
    std::stringstream stream(type);

    // read the row of data and store it in a string variable, 'word'
    while (std::getline(stream, word, delimiter))
    {
        // Remove any spaces that may be in the stream.
        word.erase(
            remove_if(word.begin(), word.end(), static_cast<int(*)(int)>(isspace)),
            word.end());

        // Push word onto the vector
        values.push_back(word);
    }

    // Type string must have minimum 2 type identifiers to give us the data we need. 
    if (values.size() < 1)
    {
        return false;
    }
    else
    {
        how = HowEntryTypeCharToEnum(values[0]);
        data = HowDataTypeCharToEnum(values[1], how);
    }

    return true;
}

bool COTParser::ParseTimeAttribute(std::string& type, DateTime& dt)
{
    // Read the data from the file as String Vector
    std::vector<std::string> values;
    values.clear();
    std::string word;
    char delimiter = 'T';
    std::stringstream stream(type);

    // read the row of data and store it in a string variable, 'word'
    while (std::getline(stream, word, delimiter))
    {
        // Remove any spaces that may be in the stream.
        word.erase(
            remove_if(word.begin(), word.end(), static_cast<int(*)(int)>(isspace)),
            word.end());

        // Push word onto the vector
        values.push_back(word);
    }

    // Time string must have minimum 2 type identifiers (Date and Time) to give us the data we need. 
    if (values.size() < 2)
    {
        std::cerr << "Failed to parse time attribute!\n";
        return false;
    }
    else if (!ParseDateStamp(values[0], dt))
    {
        std::cerr << "Failed to parse date stamp!\n";
        return false;
    }
    else if (!ParseTimeStamp(values[1], dt))
    {
        std::cerr << "Failed to parse time stamp!\n";
        return false;
    }
    return true;
}

bool COTParser::ParseDateStamp(std::string& type, DateTime& dt)
{
    // Read the data from the file as String Vector
    std::vector<std::string> values;
    values.clear();
    std::string word;
    char delimiter = '-';
    std::stringstream stream(type);

    // read the row of data and store it in a string variable, 'word'
    while (std::getline(stream, word, delimiter))
    {
        // Push word onto the vector
        values.push_back(word);
    }

    // Time string must have minimum 3 type identifiers (Year, Month, Day) to give us the data we need. 
    if (values.size() < 3)
    {
        return false;
    }
    else
    {
        dt.year = std::stoi(values[0]);
        dt.month = std::stoi(values[1]);
        dt.day = std::stoi(values[2]);
    }

    return true;
}

bool COTParser::ParseTimeStamp(std::string& type, DateTime& dt)
{
    // Read the data from the file as String Vector
    std::vector<std::string> values;
    values.clear();
    std::string word;
    char delimiter = ':';
    std::stringstream stream(type);

    // read the row of data and store it in a string variable, 'word'
    while (std::getline(stream, word, delimiter))
    {
        // Remove the T for Time. 
        word.erase(remove(word.begin(), word.end(), 'T'), word.end());

        // Push word onto the vector
        values.push_back(word);
    }

    // Time string must have minimum 3 type identifiers (Hour, Minute, Secs) to give us the data we need. 
    if (values.size() < 3)
    {
        return false;
    }
    else
    {
        dt.hour = std::stoi(values[0]);
        dt.minute = std::stoi(values[1]);
        dt.second = std::stoi(values[2]);
    }

    return true;
}

RootType COTParser::RootTypeCharToEnum(std::string& root)
{
    if (root == "a")   return RootType::ROOT_a;
    else if (root == "b")   return RootType::ROOT_b;
    else if (root == "t")   return RootType::ROOT_t;
    else if (root == "r")   return RootType::ROOT_r;
    else if (root == "c")   return RootType::ROOT_c;
    else if (root == "res") return RootType::ROOT_res;
    else return RootType::ROOT_ERROR;
}

PointType COTParser::PointTypeCharToEnum(std::string& type)
{
    if (type == "p") return PointType::POINT_p;
    else if (type == "u") return PointType::POINT_u;
    else if (type == "a") return PointType::POINT_a;
    else if (type == "f") return PointType::POINT_f;
    else if (type == "n") return PointType::POINT_n;
    else if (type == "s") return PointType::POINT_s;
    else if (type == "h") return PointType::POINT_h;
    else if (type == "j") return PointType::POINT_j;
    else if (type == "k") return PointType::POINT_k;
    else if (type == "o") return PointType::POINT_o;
    else if (type == "x") return PointType::POINT_x;
    else return PointType::POINT_ERROR;
}

LocationType COTParser::LocationTypeCharToEnum(std::string& loc)
{
    if (loc == "P") return LocationType::LOCATION_P;
    else if (loc == "A") return LocationType::LOCATION_A;
    else if (loc == "G") return LocationType::LOCATION_G;
    else if (loc == "S") return LocationType::LOCATION_S;
    else if (loc == "U") return LocationType::LOCATION_U;
    else if (loc == "X") return LocationType::LOCATION_X;
    else return LocationType::LOCATION_ERROR;
}

HowEntryType COTParser::HowEntryTypeCharToEnum(std::string& entry)
{
    if (entry == "h")   return HowEntryType::ENTRY_h;
    else if (entry == "m")   return HowEntryType::ENTRY_m;
    else return HowEntryType::ENTRY_ERROR;
}

HowDataType COTParser::HowDataTypeCharToEnum(std::string& data, HowEntryType entry)
{
    if (entry == 0)
    {
        if (data == "e")   return HowDataType::DATA_e;
        else if (data == "c")   return HowDataType::DATA_cal;
        else if (data == "t")   return HowDataType::DATA_t;
        else if (data == "p")   return HowDataType::DATA_paste;
        else return HowDataType::DATA_ERROR;
    }
    else if (entry == 1)
    {
        if (data == "i")   return HowDataType::DATA_i;
        else if (data == "g")   return HowDataType::DATA_g;
        else if (data == "m")   return HowDataType::DATA_m;
        else if (data == "s")   return HowDataType::DATA_s;
        else if (data == "f")   return HowDataType::DATA_f;
        else if (data == "c")   return HowDataType::DATA_con;
        else if (data == "p")   return HowDataType::DATA_pred;
        else if (data == "r")   return HowDataType::DATA_r;
        else return HowDataType::DATA_ERROR;
    }
    else
    {
        return HowDataType::DATA_ERROR;
    }
}