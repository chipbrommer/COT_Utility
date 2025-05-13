
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

bool CoT_Utility::VerifyXML(std::string& buffer)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(buffer.c_str());

    if (!result)
    {
        std::cout << "ERROR: " << result.description() << "\n";
        return false;
    }

    return true;
}

std::string CoT_Utility::GenerateXMLCOTMessage(CoT_Schema& cot) 
{
    std::stringstream msg;

    // XML declaration
    msg << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>";

    // Event start
    msg << "<event version=\"2.0\" uid=\"" << cot.event.uid 
        << "\" type=\"" << cot.event.type 
        << "\" time=\"" << cot.event.time.ToCOTTimestamp() 
        << "\" start=\"" << cot.event.time.ToCOTTimestamp() 
        << "\" stale=\"" << cot.event.stale.ToCOTTimestamp() 
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
        return newMsg.str();
    }

    // else just send unformatted string
    return msg.str();
}

CoT_UtilityResult CoT_Utility::UpdateReceivedCOTMessage(std::string& receivedMessage, CoT_Schema& cot, std::string& modifiedMessage, bool acknowledgment)
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
            return CoT_UtilityResult::Success;
        }

        // No modification made
        return CoT_UtilityResult::NoModificationMade;
    }
    else 
    {
        m_lastPugiResult = result.description();
        return CoT_UtilityResult::ProcessingError;
    }
}

CoT_UtilityResult CoT_Utility::AcknowledgeReceivedCOTMessage(std::string& receivedMessage, std::string& responseMessage)
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
            return CoT_UtilityResult::Success;
        }

        // No modification made
        return CoT_UtilityResult::NoModificationMade;
    }
    else
    {
        m_lastPugiResult = result.description();
        return CoT_UtilityResult::ProcessingError;
    }
}

CoT_UtilityResult CoT_Utility::ParseCOT(std::string& buffer, CoT_Schema& cot)
{
    // Remove any trash that may come in before the "<?xml" tag.
    size_t position = buffer.find("<?xml");
    buffer.erase(0, position);

    // Verify buffer is good XML data first. 
    if (!VerifyXML(buffer))
    {
        return CoT_UtilityResult::InvalidXml;
    }

    // Create a parsed xml document.
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(buffer.c_str());

    if (!result)
    {
        return CoT_UtilityResult::ProcessingError;
    }

    // Set up nodes for ease and easier reading later
    pugi::xml_node root = doc.root();
    pugi::xml_node events = doc.child("event");
    pugi::xml_node points = doc.child("event").child("point");
    pugi::xml_node details = doc.child("event").child("detail");
    int eventsSize = (int)doc.root().select_nodes("event").size();
    int pointsSize = (int)events.select_nodes("point").size();
    int detailsSize = (int)events.select_nodes("detail").size();

    // Catch bad data:
    //      <event> and <point> are required data for COT message and there should
    //      be exactly one of each schema. 
    //      We don't check <detail> because it is optional data. 

    if (eventsSize != 1)
    {
        return CoT_UtilityResult::InvalidEvent;
    }
    else if (pointsSize != 1)
    {
        return CoT_UtilityResult::InvalidPoint;
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
            (attr1 = point.attribute("lon")) ? cot.point.longitude = attr1.as_double() : cot.point.longitude = 0;
            (attr1 = point.attribute("hae")) ? cot.point.hae = attr1.as_double() : cot.point.hae = 0;
            (attr1 = point.attribute("ce")) ? cot.point.circularError = attr1.as_double() : cot.point.circularError = 0;
            (attr1 = point.attribute("le")) ? cot.point.linearError = attr1.as_double() : cot.point.linearError = 0;
        }

        // Parse <event><detail> tag and gather data. 
        for (auto&& detail : event.children("detail"))
        {
            pugi::xml_attribute attr1;

            // Parse <takv>
            pugi::xml_node takv = detail.child("takv");
            if (takv)
            {
                (attr1 = takv.attribute("version")) ? cot.detail.takv.version = attr1.as_string() : cot.detail.takv.version = "";
                (attr1 = takv.attribute("device")) ? cot.detail.takv.device = attr1.as_string() : cot.detail.takv.device = "";
                (attr1 = takv.attribute("os")) ? cot.detail.takv.os = attr1.as_string() : cot.detail.takv.os = "";
                (attr1 = takv.attribute("platform")) ? cot.detail.takv.platform = attr1.as_string() : cot.detail.takv.platform = "";
            }
            else
            {
                cot.detail.takv = Takv();
            }

            // Parse <contact>
            pugi::xml_node contact = detail.child("contact");
            if (contact)
            {
                (attr1 = contact.attribute("endpoint")) ? cot.detail.contact.endpoint = attr1.as_string() : cot.detail.contact.endpoint = "";
                (attr1 = contact.attribute("callsign")) ? cot.detail.contact.callsign = attr1.as_string() : cot.detail.contact.callsign = "";
                (attr1 = contact.attribute("xmppUsername")) ? cot.detail.contact.xmppUsername = attr1.as_string() : cot.detail.contact.xmppUsername = "";
            }
            else
            {
                cot.detail.contact = Contact();
            }

            // Parse <uid>
            pugi::xml_node uid = detail.child("uid");
            if (uid)
            {
                (attr1 = uid.attribute("Droid")) ? cot.detail.uid.droid = attr1.as_string() : cot.detail.uid.droid = "";
            }
            else
            {
                cot.detail.uid = Uid();
            }

            // Parse <model>
            pugi::xml_node model = detail.child("model");
            if (model)
            {
                (attr1 = model.attribute("value")) ? cot.detail.model.value = attr1.as_string() : cot.detail.model.value = "";
            }
            else
            {
                cot.detail.model = Model();
            }

            // Parse <precisionlocation>
            pugi::xml_node precision = detail.child("precisionlocation");
            if (precision)
            {
                (attr1 = precision.attribute("altsrc")) ? cot.detail.precisionLocation.altsrc = attr1.as_string() : cot.detail.precisionLocation.altsrc = "";
                (attr1 = precision.attribute("geopointsrc")) ? cot.detail.precisionLocation.geopointsrc = attr1.as_string() : cot.detail.precisionLocation.geopointsrc = "";
            }
            else
            {
                cot.detail.precisionLocation = PrecisionLocation();
            }

            // Parse <__group>
            pugi::xml_node group = detail.child("__group");
            if (group)
            {
                (attr1 = group.attribute("role")) ? cot.detail.group.role = attr1.as_string() : cot.detail.group.role = "";
                (attr1 = group.attribute("name")) ? cot.detail.group.name = attr1.as_string() : cot.detail.group.name = "";
            }
            else
            {
                cot.detail.group = Group();
            }

            // Parse <status>
            pugi::xml_node status = detail.child("status");
            if (status)
            {
                (attr1 = status.attribute("battery")) ? cot.detail.status.battery = attr1.as_double() : cot.detail.status.battery = 0;
            }
            else
            {
                cot.detail.status = Status();
            }

            // Parse <track>
            pugi::xml_node track = detail.child("track");
            if (track)
            {
                (attr1 = track.attribute("course")) ? cot.detail.track.course = attr1.as_double() : cot.detail.track.course = 0;
                (attr1 = track.attribute("speed")) ? cot.detail.track.speed = attr1.as_double() : cot.detail.track.speed = 0;
            }
            else
            {
                cot.detail.track = Track();
            }

            // Parse <remarks> 
            pugi::xml_node remarks = detail.child("remarks");
            cot.detail.remarks = remarks ? remarks.child_value() : "";

            // Parse <strokeColor>
            pugi::xml_node strokeColor = detail.child("strokeColor");
            if (strokeColor) {
                (attr1 = strokeColor.attribute("value")) ? cot.detail.strokeColor.value = attr1.as_int() : cot.detail.strokeColor.value = 0;
            }
            else {
                cot.detail.strokeColor = StrokeColor();
            }

            // Parse <fillColor>
            pugi::xml_node fillColor = detail.child("fillColor");
            if (fillColor) {
                (attr1 = fillColor.attribute("value")) ? cot.detail.fillColor.value = attr1.as_int() : cot.detail.fillColor.value = 0;
            }
            else {
                cot.detail.fillColor = FillColor();
            }

            // Parse <color>
            pugi::xml_node color = detail.child("color");
            if (color)
            {
                (attr1 = color.attribute("argb")) ? cot.detail.color.argb = attr1.as_int() : cot.detail.color.argb = 0;
            }
            else {
                cot.detail.color = Color();
            }

            // Parse <usericon>
            pugi::xml_node usericon = detail.child("usericon");
            if (usericon) {
                (attr1 = usericon.attribute("iconsetpath")) ? cot.detail.userIcon.iconSetPath = attr1.as_string() : cot.detail.userIcon.iconSetPath = "";
            }
            else {
                cot.detail.userIcon = UserIcon();
            }

            // Parse <link> elements
            for (auto&& linkNode : detail.children("link"))
            {
                Link link;
                (attr1 = linkNode.attribute("uid")) ? link.uid = attr1.as_string() : link.uid = "";
                (attr1 = linkNode.attribute("remarks")) ? link.remarks = attr1.as_string() : link.remarks = "";
                (attr1 = linkNode.attribute("relation")) ? link.relation = attr1.as_string() : link.relation = "";
                (attr1 = linkNode.attribute("callsign")) ? link.callsign = attr1.as_string() : link.callsign = "";
                (attr1 = linkNode.attribute("type")) ? link.type = attr1.as_string() : link.type = "";
                (attr1 = linkNode.attribute("point")) ? link.point = attr1.as_string() : link.point = "";

                // Set lat and lon using getLatLon
                auto [lat, lon] = link.GetLatLonFromPoint();
                link.latitude = lat;
                link.longitude = lon;

                cot.detail.links.push_back(link);
            }
        }
    }

    // Success
    return CoT_UtilityResult::Success;
}

CoT_UtilityResult CoT_Utility::ParseCOT(const char* buffer, CoT_Schema& cot)
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

std::string CoT_Utility::UtilityResultToString(CoT_UtilityResult error)
{
    switch (error)
    {
    case CoT_UtilityResult::Success:
        return "Success";
    case CoT_UtilityResult::InvalidEvent:
        return "XML has invalid Event tag";
    case CoT_UtilityResult::InvalidPoint:
        return "XML has invalid Point tag";
    case CoT_UtilityResult::InvalidDate:
        return "XML has invalid Date tag; Date string must have minimum 3 type identifiers (Year, Month, Day)";
    case CoT_UtilityResult::InvalidTime:
        return "XML has invalid Time tag; Time must have minimum 3 type identifiers (Hour, Minute, Secs)";
    case CoT_UtilityResult::InvalidHow:
        return "XML has invalid How tag";
    case CoT_UtilityResult::InvalidType:
        return "XML has invalid Type tag";
    case CoT_UtilityResult::InvalidXml:
        return "Invalid XML input";
    case CoT_UtilityResult::InvalidInput:
        return "Invalid or empty input";
    case CoT_UtilityResult::InvalidTimeSubSchema:
        return "Invalid Time sub-schema";
    case CoT_UtilityResult::ProcessingError:
        return "Processing error";
    case CoT_UtilityResult::NoModificationMade:
        return "No modification made";
    default:
        return "Unknown error";
    }
}

std::string CoT_Utility::GetVersion() const
{
    return "COT Utility v" + std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(BUILD);
}

std::string CoT_Utility::GetLastXmlError() const
{
    return m_lastPugiResult;
}

CoT_UtilityResult CoT_Utility::ParseTypeAttribute(std::string& type, Point::Type& ind, Location::Type& loc)
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
        return CoT_UtilityResult::InvalidType;
    }
    else
    {
        ind = PointTypeCharToEnum(values[1]);
        loc = LocationTypeCharToEnum(values[2]);
    }

    return CoT_UtilityResult::Success;
}

CoT_UtilityResult CoT_Utility::ParseHowAttribute(std::string& type, How::Entry::Type& how, How::Data::Type& data)
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
        return CoT_UtilityResult::InvalidHow;
    }
    else
    {
        how = HowEntryTypeCharToEnum(values[0]);
        data = HowDataTypeCharToEnum(values[1], how);
    }

    return CoT_UtilityResult::Success;
}

CoT_UtilityResult CoT_Utility::ParseTimeAttribute(std::string& type, DateTime& dt)
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
        return CoT_UtilityResult::InvalidTimeSubSchema;
    }

    CoT_UtilityResult res = ParseDateStamp(values[0], dt);
    if (res != CoT_UtilityResult::Success)
    {
        return res;
    }

    res = ParseTimeStamp(values[1], dt);
    if (res != CoT_UtilityResult::Success)
    {
        return res;
    }

    return CoT_UtilityResult::Success;
}

CoT_UtilityResult CoT_Utility::ParseDateStamp(std::string& type, DateTime& dt)
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
        return CoT_UtilityResult::InvalidDate;
    }
    else
    {
        dt.year = std::stoi(values[0]);
        dt.month = std::stoi(values[1]);
        dt.day = std::stoi(values[2]);
    }

    return CoT_UtilityResult::Success;
}

CoT_UtilityResult CoT_Utility::ParseTimeStamp(std::string& type, DateTime& dt)
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
        return CoT_UtilityResult::InvalidTime;
    }
    else
    {
        dt.hour = std::stoi(values[0]);
        dt.minute = std::stoi(values[1]);
        dt.second = std::stoi(values[2]);
    }

    return CoT_UtilityResult::Success;
}

Root::Type CoT_Utility::RootTypeCharToEnum(std::string& root)
{
         if (root == "a")   return Root::Type::a;
    else if (root == "b")   return Root::Type::b;
    else if (root == "t")   return Root::Type::t;
    else if (root == "r")   return Root::Type::r;
    else if (root == "c")   return Root::Type::c;
    else if (root == "res") return Root::Type::res;
    else return Root::Type::Error;
}

Point::Type CoT_Utility::PointTypeCharToEnum(std::string& type)
{
         if (type == "p") return Point::Type::p;
    else if (type == "u") return Point::Type::u;
    else if (type == "a") return Point::Type::a;
    else if (type == "f") return Point::Type::f;
    else if (type == "n") return Point::Type::n;
    else if (type == "s") return Point::Type::s;
    else if (type == "h") return Point::Type::h;
    else if (type == "j") return Point::Type::j;
    else if (type == "k") return Point::Type::k;
    else if (type == "o") return Point::Type::o;
    else if (type == "x") return Point::Type::x;
    else return Point::Type::Error;
}

Location::Type CoT_Utility::LocationTypeCharToEnum(std::string& loc)
{
         if (loc == "P") return Location::Type::P;
    else if (loc == "A") return Location::Type::A;
    else if (loc == "G") return Location::Type::G;
    else if (loc == "S") return Location::Type::S;
    else if (loc == "U") return Location::Type::U;
    else if (loc == "X") return Location::Type::X;
    else return Location::Type::Error;
}

How::Entry::Type CoT_Utility::HowEntryTypeCharToEnum(std::string& entry)
{
         if (entry == "h")   return How::Entry::Type::h;
    else if (entry == "m")   return How::Entry::Type::m;
    else return How::Entry::Type::Error;
}

How::Data::Type CoT_Utility::HowDataTypeCharToEnum(std::string& data, How::Entry::Type entry)
{
    if (entry == How::Entry::Type::h)
    {
             if (data == "e")   return How::Data::Type::e;
        else if (data == "c")   return How::Data::Type::cal;
        else if (data == "t")   return How::Data::Type::t;
        else if (data == "p")   return How::Data::Type::paste;
        else return How::Data::Type::Error;
    }
    else if (entry == How::Entry::Type::m)
    {
             if (data == "i")   return How::Data::Type::i;
        else if (data == "g")   return How::Data::Type::g;
        else if (data == "m")   return How::Data::Type::m;
        else if (data == "s")   return How::Data::Type::s;
        else if (data == "f")   return How::Data::Type::f;
        else if (data == "c")   return How::Data::Type::con;
        else if (data == "p")   return How::Data::Type::pred;
        else if (data == "r")   return How::Data::Type::r;
        else return How::Data::Type::Error;
    }
    else
    {
        return How::Data::Type::Error;
    }
}