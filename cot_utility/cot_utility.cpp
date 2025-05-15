
/////////////////////////////////////////////////////////////////////////////////
// @file            CoT_Utility.cpp
// @brief           Implementation of CoT Utility
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <sstream>                      // Stringstream
#include <vector>                       // vectors
//
#include "cot_utility.h"                // COT Parser header.
//
///////////////////////////////////////////////////////////////////////////////

Result CoT_Utility::VerifyXML(std::string& buffer)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(buffer.c_str());

    if (!result)
    {
        Result(Result::Code::InvalidXml, result.description());
    }

    return Result(Result::Code::Success, "Valid XML");
}

Result CoT_Utility::GenerateXMLCOTMessage(CoT_Schema& cot, std::string& xml)
{
    std::stringstream msg;

    // XML declaration
    msg << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";

    // Event start
    msg << "<event version=\"2.0\" uid=\"" << cot.event.uid 
        << "\" type=\"" << cot.event.type 
        << "\" time=\"" << cot.event.time.ToCotTimestamp()
        << "\" start=\"" << cot.event.time.ToCotTimestamp()
        << "\" stale=\"" << cot.event.stale.ToCotTimestamp()
        << "\" how=\"" << cot.event.how 
        << "\">";

    // Point data
    msg << "<point lat=\"" << cot.point.latitude << "\" lon=\"" << cot.point.longitude << "\" hae=\"" << cot.point.hae << "\" ce=\"" << cot.point.circularError << "\" le=\"" << cot.point.linearError << "\"/>";

    // Detail section
    msg << "<detail>";

    if (!cot.detail.contact.callsign.empty())
    {
        msg << "<contact callsign=\"" << cot.detail.contact.callsign << "\" endpoint=\"" << cot.detail.contact.endpoint << "\" xmppUsername=\"" << cot.detail.contact.xmppUsername << "\"/>";
    }

    msg << "<uid Droid=\"" << cot.detail.uid.droid << "\"/>";
    msg << "<__group name=\"" << cot.detail.group.name << "\" role=\"" << cot.detail.group.role << "\"/>";
    msg << "<status battery=\"" << cot.detail.status.battery << "\"/>";
    msg << "<track course=\"" << cot.detail.track.course << "\" speed=\"" << cot.detail.track.speed << "\"/>"; // corrected line
    msg << "</detail></event>";

    // Create XML document to load in the msg for propper xml formating 
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(msg.str().c_str());

    if (result)
    {
        std::stringstream newMsg;
        doc.save(newMsg);
        xml = newMsg.str();
        return Result(Result::Code::Success);
    }

    // else just send unformatted string
    xml = "";
    return Result(Result::Code::ProcessingError);
}

Result CoT_Utility::UpdateReceivedCOTMessage(std::string& receivedMessage, CoT_Schema& cot, std::string& modifiedMessage, bool acknowledgment)
{
    // Create XML document to load in the receivedMessage
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(receivedMessage.c_str());

    if (result) 
    {
        // Modified flag
        bool modified = false;

        // Find the 'point' node
        pugi::xml_node pointNode = doc.select_node("//point").node();

        // If we found point...
        if (pointNode) 
        {
            // Set the new latitude value
            pointNode.attribute("lat").set_value(cot.point.latitude);
            modified = true;
        }

        // Find the 'status' node
        pugi::xml_node statusNode = doc.select_node("//status").node();
        
        if (statusNode)
        {
            // Check if acknowledgment flag is true and acknowledgment attribute doesn't exist
            if (acknowledgment && !statusNode.attribute("acknowledgment"))
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
            modifiedMessage = modifiedXmlStream.str();
            return Result(Result::Code::Success);
        }

        // No modification made
        return Result(Result::Code::NoModificationMade);
    }

    return Result(Result::Code::ProcessingError, result.description());
}

Result CoT_Utility::AcknowledgeReceivedCOTMessage(std::string& receivedMessage, std::string& responseMessage)
{
    // Create XML document to load in the receivedMessage
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(receivedMessage.c_str());

    if (result)
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
            return Result(Result::Code::Success);
        }

        // No modification made
        return Result(Result::Code::NoModificationMade);
    }

    return Result(Result::Code::ProcessingError, result.description());
}

Result CoT_Utility::ParseCOT(std::string& buffer, CoT_Schema& cot)
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
        { return Result(Result::Code::InvalidEvent); }
    if (eventNode.select_nodes("point").size() != 1)
        { return Result(Result::Code::InvalidPoint); }
    if (eventNode.select_nodes("detail").size() > 1)
    { return Result(Result::Code::InvalidXml); }

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

Result CoT_Utility::ParseCOT(const char* buffer, CoT_Schema& cot)
{
    std::string str = buffer;
    return ParseCOT(str, cot);
}

CoT_Schema CoT_Utility::ParseBufferToCOT(const char* buffer)
{
    CoT_Schema cot;
    ParseCOT(buffer, cot);
    return cot;
}

Result CoT_Utility::ParseTrackFromCoT(const char* buffer, Track& track)
{
    // Convert input buffer to string and remove garbage before <?xml
    std::string xmlBuffer(buffer);
    size_t position = xmlBuffer.find("<?xml");
    if (position == std::string::npos)
    {
        return Result(Result::Code::InvalidXml);
    }
    xmlBuffer.erase(0, position);

    // Verify XML structure
    if (!VerifyXML(xmlBuffer).IsSuccess())
    {
        return Result(Result::Code::InvalidXml);
    }

    // Parse XML document
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xmlBuffer.c_str());
    if (!result)
    {
        return Result(Result::Code::ProcessingError);
    }

    // Check for exactly one <event> node
    int eventsSize = (int)doc.root().select_nodes("event").size();
    if (eventsSize != 1)
    {
        return Result(Result::Code::InvalidEvent);
    }

    // Navigate to <event> and <detail>
    pugi::xml_node event = doc.child("event");
    pugi::xml_node detail = event.child("detail");
    if (!detail)
    {
        return Result(Result::Code::InsufficientData);
    }

    // Parse <track>
    pugi::xml_node trackNode = detail.child("track");
    if (!trackNode)
    {
        track = Track();
        return Result(Result::Code::InsufficientData);
    }

    // Use Track::FromXml to parse the track node
    track = Track::FromXml(trackNode);

    // Success
    return Result(Result::Code::Success);
}

std::string CoT_Utility::GetVersion() const
{
    return "COT Utility v" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(BUILD);
}

Result CoT_Utility::ParseTypeAttribute(std::string& type, Point::Type& ind, Location::Type& loc)
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
        return Result(Result::Code::InvalidType);
    }
    else
    {
        ind = Point::CharToType(values[1]);
        loc = Location::CharToType(values[2]);
    }

    return Result(Result::Code::Success);
}

Result CoT_Utility::ParseHowAttribute(std::string& type, How::Entry::Type& how, How::Data::Type& data)
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
        return Result(Result::Code::InvalidHow);
    }
    else
    {
        how = How::Entry::CharToType(values[0]);
        data = How::Data::CharToType(values[1], how);
    }

    return Result(Result::Code::Success);
}

Result CoT_Utility::ParseTimeAttribute(std::string& type, DateTime& dt)
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
        return Result(Result::Code::InvalidTimeSubSchema);
    }

    Result res = ParseDateStamp(values[0], dt);
    if (!res.IsSuccess())
    {
        return res;
    }

    res = ParseTimeStamp(values[1], dt);
    if (!res.IsSuccess())
    {
        return res;
    }

    return Result(Result::Code::Success);
}

Result CoT_Utility::ParseDateStamp(std::string& type, DateTime& dt)
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

    // Date string must have minimum 3 type identifiers (Year, Month, Day) to give us the data we need. 
    if (values.size() < 3)
    {
        return Result(Result::Code::InvalidDate);
    }
    else
    {
        dt.year = std::stoi(values[0]);
        dt.month = std::stoi(values[1]);
        dt.day = std::stoi(values[2]);
    }

    return Result(Result::Code::Success);
}

Result CoT_Utility::ParseTimeStamp(std::string& type, DateTime& dt)
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
        return Result(Result::Code::InvalidTime);
    }
    else
    {
        dt.hour = std::stoi(values[0]);
        dt.minute = std::stoi(values[1]);
        dt.second = std::stoi(values[2]);
    }

    return Result(Result::Code::Success);
}
