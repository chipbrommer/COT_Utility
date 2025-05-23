#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot_info.h
// @brief           An information header file for COT structures and enums.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <algorithm>
#include <climits>                      // MAX and MIN
#include <cmath>                        // isnan
#include <fstream>                      // fstream
#include <functional>                   // std::function
#include <iomanip>                      // setw
#include <iostream>
#include <limits>      
#include <map>
#include <pugixml.hpp>                  // pugi xml
#include <sstream>                      // sstream
#include <string>
#include <unordered_map>                // maps
#include <vector>                       // links vector
//
/////////////////////////////////////////////////////////////////////////////////

namespace Root
{
    enum class Type : int
    {
        a,
        b,
        t,
        r,
        c,
        res,
        Error
    };

    static const std::unordered_map<Type, std::string> TypeToString = {
        {Type::a, "Atoms"},
        {Type::b, "Bits"},
        {Type::t, "Tasking"},
        {Type::r, "Reply"},
        {Type::c, "Capability"},
        {Type::res, "Reservation"},
        {Type::Error, "Error"}
    };

    static const std::unordered_map<std::string, Type> StringToType = {
        {"Atoms", Type::a}, 
        {"Bits", Type::b}, 
        {"Tasking", Type::t},
        {"Reply", Type::r}, 
        {"Capability", Type::c}, 
        {"Reservation", Type::res}
    };

    /// @brief Converts a string into a RootType enumeration value
    /// @param root string to be converted.
    /// @return RootType enum conversion
    static const Type CharToType(std::string& root)
    {
             if (root == "a")   return Root::Type::a;
        else if (root == "b")   return Root::Type::b;
        else if (root == "t")   return Root::Type::t;
        else if (root == "r")   return Root::Type::r;
        else if (root == "c")   return Root::Type::c;
        else if (root == "res") return Root::Type::res;
        else return Root::Type::Error;
    }
};

namespace Point
{
    enum class Type : int
    {
        p,
        u,
        a,
        f,
        n,
        s,
        h,
        j,
        k,
        o,
        x,
        Error
    };

    static const std::unordered_map<Type, std::string> TypeToString = {
        {Point::Type::p, "Pending"},
        {Point::Type::u, "Unknown"},
        {Point::Type::a, "Assumed Friend"},
        {Point::Type::f, "Friend"},
        {Point::Type::n, "Neutral"},
        {Point::Type::s, "Suspect"},
        {Point::Type::h, "Hostile"},
        {Point::Type::j, "Joker"},
        {Point::Type::k, "Faker"},
        {Point::Type::o, "None Specified"},
        {Point::Type::x, "Other"},
        {Point::Type::Error, "Error"}
    };

    static const std::unordered_map<std::string, Type> StringToType = {
        {"Pending", Type::p}, 
        {"Unknown", Type::u}, 
        {"Assumed Friend", Type::a},
        {"Friend", Type::f}, 
        {"Neutral", Type::n}, 
        {"Suspect", Type::s},
        {"Hostile", Type::h}, 
        {"Joker", Type::j}, 
        {"Faker", Type::k},
        {"None Specified", Type::o}, 
        {"Other", Type::x}
    };

    /// @brief Converts a string into a Type enumeration value
    /// @param type string to be converted.
    /// @return Type enum conversion
    static const Type CharToType(std::string& type)
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

};

namespace Location
{
    enum class Type : int
    {
        P,
        A,
        G,
        S,
        U,
        X,
        Error
    };

    static const std::unordered_map<Type, std::string> TypeToString
    {
        {Type::P, "Space"},
        {Type::A, "Air"},
        {Type::G, "Ground"},
        {Type::S, "Sea Surface"},
        {Type::U, "Sea Subsurface"},
        {Type::X, "Other"},
        {Type::Error, "Error"}
    };

    static const std::unordered_map<std::string, Type> StringToType
    {
        {"Space", Type::P}, 
        {"Air", Type::A}, 
        {"Ground", Type::G},
        {"Sea Surface", Type::S}, 
        {"Sea Subsurface", Type::U}, 
        {"Other", Type::X}
    };

    /// @brief Converts a string into a Type enumeration value
    /// @param loc string to be converted.
    /// @return Type enum conversion
    static const Type CharToType(std::string& loc)
    {
             if (loc == "P") return Location::Type::P;
        else if (loc == "A") return Location::Type::A;
        else if (loc == "G") return Location::Type::G;
        else if (loc == "S") return Location::Type::S;
        else if (loc == "U") return Location::Type::U;
        else if (loc == "X") return Location::Type::X;
        else return Location::Type::Error;
    }
};

namespace How
{
    /// @brief Type section of the 'how' data
    namespace Entry
    {
        enum class Type : int
        {
            h,
            m,
            Error
        };

        static const std::unordered_map<Type, std::string> TypeToString
        {
            {Type::h, "Human"},
            {Type::m, "Machine"},
            {Type::Error, "Error"}
        };

        static const std::unordered_map<std::string, Type> StringToType
        {
            {"Human", Type::h}, 
            {"Machine", Type::m}
        };

        /// @brief Converts a string into a Type enumeration value
        /// @param entry string to be converted.
        /// @return Type enum conversion
        static const Type CharToType(std::string& entry)
        {
                 if (entry == "h")   return How::Entry::Type::h;
            else if (entry == "m")   return How::Entry::Type::m;
            else return How::Entry::Type::Error;
        }
    };

    /// @brief Data section of the 'how' data
    namespace Data
    {
        enum class Type : int
        {
            e,
            cal,
            t,
            paste,
            i,
            g,
            m,
            s,
            f,
            con,
            pred,
            r,
            Error
        };

        static const std::unordered_map<Type, std::string> TypeToString = {
            {Type::e, "Estimated"},
            {Type::cal, "Calculated"},
            {Type::t, "Transcribed"},
            {Type::paste, "Cut and Paste"},
            {Type::i, "Mensurated"},
            {Type::g, "Derived From GPS"},
            {Type::m, "Magnetic"},
            {Type::s, "Simulated"},
            {Type::f, "Fused"},
            {Type::con, "Configured"},
            {Type::pred, "Predicted"},
            {Type::r, "Relayed"},
            {Type::Error, "Error"}
        };

        static const std::unordered_map<std::string, Type> StringToType = {
            {"Estimated", Type::e}, 
            {"Calculated", Type::cal}, 
            {"Transcribed", Type::t},
            {"Cut and Paste", Type::paste}, 
            {"Mensurated", Type::i}, 
            {"Derived From GPS", Type::g},
            {"Magnetic", Type::m}, 
            {"Simulated", Type::s}, 
            {"Fused", Type::f},
            {"Configured", Type::con}, 
            {"Predicted", Type::pred}, 
            {"Relayed", Type::r}
        };

        /// @brief Converts a string into a Type enumeration value
        /// @param data string to be converted.
        /// @param entry How Entry Type to correspond to proper data type. 
        /// @return Type enum conversion
        static const Type CharToType(std::string& data, How::Entry::Type entry)
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
    };
};

/// @brief A COT Message subschema class for Event data 
class Event 
{
public:
    double version;
    std::string type;
    Root::Type rootType;
    Point::Type indicator;
    Location::Type location;
    std::string uid;
    DateTime time;
    DateTime start;
    DateTime stale;
    std::string how;
    How::Entry::Type howEntry;
    How::Data::Type howData;

    Event(double version = INVALID_VERSION,
        const std::string& type = INVALID_STRING,
        Root::Type rootType = Root::Type::Error,
        Point::Type indicator = Point::Type::Error,
        Location::Type location = Location::Type::Error,
        const std::string& uid = INVALID_STRING,
        const DateTime& time = DateTime(),
        const DateTime& start = DateTime(),
        const DateTime& stale = DateTime(),
        const std::string& how = INVALID_STRING,
        How::Entry::Type howEntry = How::Entry::Type::Error,
        How::Data::Type howData = How::Data::Type::Error)
        : version(version), type(type), rootType(rootType), indicator(indicator), location(location),
        uid(uid), time(time), start(start), stale(stale), how(how),
        howEntry(howEntry), howData(howData) {}

    bool operator==(const Event& other) const 
    {
        constexpr double EPSILON = 1e-6;
        bool versionEqual = (std::isnan(version) && std::isnan(other.version)) ||
            std::abs(version - other.version) < EPSILON;
        return versionEqual &&
            type == other.type &&
            rootType == other.rootType &&
            indicator == other.indicator &&
            location == other.location &&
            uid == other.uid &&
            time == other.time &&
            start == other.start &&
            stale == other.stale &&
            how == other.how &&
            howEntry == other.howEntry &&
            howData == other.howData;
    }

    bool operator!=(const Event& other) const 
    { 
        return !(*this == other); 
    }

    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = true;
        std::string errors;

        if (std::isnan(version) || version <= 0) 
        {
            valid = false;
            errors += "Invalid version; ";
        }
        if (type.empty()) 
        {
            valid = false;
            errors += "Empty type; ";
        }
        if (rootType == Root::Type::Error) 
        {
            valid = false;
            errors += "Invalid rootType; ";
        }
        if (indicator == Point::Type::Error) 
        {
            valid = false;
            errors += "Invalid indicator; ";
        }
        if (location == Location::Type::Error) 
        {
            valid = false;
            errors += "Invalid location; ";
        }
        if (uid.empty()) 
        {
            valid = false;
            errors += "Empty uid; ";
        }
        if (!time.IsValid()) 
        {
            valid = false;
            errors += "Invalid time; ";
        }
        if (!start.IsValid()) 
        {
            valid = false;
            errors += "Invalid start; ";
        }
        if (!stale.IsValid()) 
        {
            valid = false;
            errors += "Invalid stale; ";
        }
        if (how.empty()) 
        {
            valid = false;
            errors += "Empty how; ";
        }
        if (howEntry == How::Entry::Type::Error) 
        {
            valid = false;
            errors += "Invalid howEntry; ";
        }
        if (howData == How::Data::Type::Error) 
        {
            valid = false;
            errors += "Invalid howData; ";
        }

        if (!valid && errorMsg) 
        {
            *errorMsg = errors.empty() ? "Event is invalid" : errors;
        }
        return valid;
    }

    std::string ToXml(bool add_ending_tag = false) const 
    {
        if (!IsValid()) return "";
        std::ostringstream oss;
        oss << "<event"
            << " version=\"" << std::fixed << std::setprecision(1) << version << "\""
            << " type=\"" << type << "\""
            << " uid=\"" << uid << "\""
            << " time=\"" << time.ToCotTimestamp() << "\""
            << " start=\"" << start.ToCotTimestamp() << "\""
            << " stale=\"" << stale.ToCotTimestamp() << "\""
            << " how=\"" << how << ">";
        if (add_ending_tag)
        {
            oss << "</event>";
        }
        return oss.str();
    }

    static Event FromXml(const pugi::xml_node& node)
    {
        Event event;
        try
        {
            if (auto attr = node.attribute("version"))
                event.version = attr.as_double(INVALID_VERSION);
            if (auto attr = node.attribute("type"))
            {
                event.type = attr.as_string();
                // Parse type: e.g., "a-f-G"
                std::vector<std::string> typeParts;
                std::stringstream typeStream(event.type);
                std::string part;

                while (std::getline(typeStream, part, '-')) 
                {
                    typeParts.push_back(part);
                }
                if (typeParts.size() >= 3)
                {
                    event.rootType = Root::CharToType(typeParts[0]);
                    event.indicator = Point::CharToType(typeParts[1]);
                    event.location = Location::CharToType(typeParts[2]);
                }
            }
            if (auto attr = node.attribute("uid")) event.uid = attr.as_string();
            if (auto attr = node.attribute("time"))
            {
                std::string error;
                event.time = DateTime::FromString(attr.as_string(), &error);
                if (!event.time.IsValid()) std::cerr << "Error parsing time: " << error << std::endl;
            }
            if (auto attr = node.attribute("start"))
            {
                std::string error;
                event.start = DateTime::FromString(attr.as_string(), &error);
                if (!event.start.IsValid()) std::cerr << "Error parsing start: " << error << std::endl;
            }
            if (auto attr = node.attribute("stale"))
            {
                std::string error;
                event.stale = DateTime::FromString(attr.as_string(), &error);
                if (!event.stale.IsValid()) std::cerr << "Error parsing stale: " << error << std::endl;
            }
            if (auto attr = node.attribute("how"))
            {
                event.how = attr.as_string();
                // Parse how: e.g., "m-g"
                std::vector<std::string> howParts;
                std::stringstream howStream(event.how);
                std::string part;
                while (std::getline(howStream, part, '-'))
                {
                    howParts.push_back(part);
                }
                if (howParts.size() >= 2)
                {
                    event.howEntry = How::Entry::CharToType(howParts[0]);
                    event.howData = How::Data::CharToType(howParts[1], event.howEntry);
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing Event attributes: " << e.what() << std::endl;
        }
        return event;
    }

    std::string ToString() const 
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Event& event) 
    {
        os << "Event: ";
        if (!event.IsValid()) os << " -NOT VALID- ";
        os << "\n"
            << "\tVersion: " << (std::isnan(event.version) ? "NaN" : std::to_string(event.version)) << "\n"
            << "\tType: " << (event.type.empty() ? "None" : event.type) << "\n"
            << "\tRoot Type: " << static_cast<int>(event.rootType) << " - "
            << Root::TypeToString.at(event.rootType) << "\n"
            << "\tIndicator: " << static_cast<int>(event.indicator) << " - "
            << Point::TypeToString.at(event.indicator) << "\n"
            << "\tLocation: " << static_cast<int>(event.location) << " - "
            << Location::TypeToString.at(event.location) << "\n"
            << "\tUID: " << (event.uid.empty() ? "None" : event.uid) << "\n"
            << "\tTime: " << event.time << "\n"
            << "\tStart: " << event.start << "\n"
            << "\tStale: " << event.stale << "\n"
            << "\tHow: " << (event.how.empty() ? "None" : event.how) << "\n"
            << "\tHow Entry: " << static_cast<int>(event.howEntry) << " - "
            << How::Entry::TypeToString.at(event.howEntry) << "\n"
            << "\tHow Data: " << static_cast<int>(event.howData) << " - "
            << How::Data::TypeToString.at(event.howData) << "\n";
        return os;
    }

private:
    static constexpr double INVALID_VERSION = std::numeric_limits<double>::quiet_NaN();
    static constexpr const char* INVALID_STRING = "";
};

/// @brief A Root CoT Message schema class for entire message data
class CoT_Schema 
{
public:
    Event event;                    /// Holds Event Sub-schema
    Point::Data point;              /// Holds Point Sub-schema
    Detail detail;                  /// Holds Detail Sub-schema

    /// @brief Constructor - Initializes everything
    CoT_Schema(const Event& event = Event(),
        const Point::Data& point = Point::Data(),
        const Detail& detail = Detail()) :
        event(event), point(point), detail(detail) {}

    /// @brief Equality comparison operator
    bool operator==(const CoT_Schema& other) const 
    {
        return event == other.event &&
            point == other.point &&
            detail == other.detail;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const CoT_Schema& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = true;
        std::string errors;

        // Event and Point are required in CoT
        if (!event.IsValid(&errors)) 
        {
            valid = false;
            if (errorMsg && !errors.empty()) *errorMsg += "Event invalid: " + errors + "; ";
        }
        if (!point.IsValid(&errors)) 
        {
            valid = false;
            if (errorMsg && !errors.empty()) *errorMsg += "Point invalid: " + errors + "; ";
        }
        // Detail is optional but must be valid if present
        if (detail.IsValid(&errors) && !detail.IsValid(&errors)) 
        {
            valid = false;
            if (errorMsg && !errors.empty()) *errorMsg += "Detail invalid: " + errors + "; ";
        }

        if (!valid && errorMsg && errorMsg->empty()) 
        {
            *errorMsg = "CoT_Schema missing required components or contains invalid data";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        std::ostringstream oss;
        if (!event.IsValid()) 
        {
            return oss.str(); // Return empty XML if event is invalid
        }
        oss << event.ToXml();
        if (point.IsValid()) oss << point.ToXml();
        if (detail.IsValid()) oss << detail.ToXml();
        oss << "</event>";
        return oss.str();
    }

    /// @brief Get formatted XML string (with basic indentation)
    std::string ToXmlString() const 
    {
        std::string xml = ToXml();
        std::ostringstream oss;
        int indentLevel = 0;
        const std::string indentStr = "  ";

        for (size_t i = 0; i < xml.length(); ++i) {
            if (xml[i] == '<' && xml[i + 1] != '/') {
                // Opening tag
                oss << std::string(indentLevel * indentStr.length(), ' ') << xml[i];
                indentLevel++;
            }
            else if (xml[i] == '<' && xml[i + 1] == '/') {
                // Closing tag
                indentLevel--;
                oss << std::string(indentLevel * indentStr.length(), ' ') << xml[i];
            }
            else if (xml[i] == '>' && i > 0 && xml[i - 1] == '/') {
                // Self-closing tag
                oss << xml[i];
                indentLevel--;
            }
            else {
                oss << xml[i];
            }
            if (xml[i] == '>' && i + 1 < xml.length() && xml[i + 1] != '<') {
                // Add newline after tag if followed by content
                oss << "\n";
            }
        }
        return oss.str();
    }

    /// @brief Save XML to a file
    bool ToXmlFile(const std::string& filename) const 
    {
        std::ofstream ofs(filename);
        if (!ofs.is_open()) 
        {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
        ofs << ToXmlString();
        ofs.close();
        return true;
    }

    /// @brief Deserialize from XML node
    static CoT_Schema FromXml(const pugi::xml_node& node) 
    {
        CoT_Schema cot;
        if (std::string(node.name()) != "event") {
            std::cerr << "Root node must be <event>, found: " << node.name() << std::endl;
            return cot;
        }

        // Dispatch table for sub-schema parsing
        static const std::map<std::string, std::function<void(pugi::xml_node, CoT_Schema&)>> parsers = {
            {"point", [](pugi::xml_node n, CoT_Schema& c) { c.point = Point::Data::FromXml(n); }},
            {"detail", [](pugi::xml_node n, CoT_Schema& c) { c.detail = Detail::FromXml(n); }}
        };

        // Parse event attributes
        try 
        {
            cot.event = Event::FromXml(node);
        }
        catch (const std::exception& e) 
        {
            std::cerr << "Error parsing event attributes: " << e.what() << std::endl;
        }

        // Parse child nodes
        for (pugi::xml_node child : node.children()) {
            std::string name = child.name();
            try {
                auto it = parsers.find(name);
                if (it != parsers.end()) {
                    it->second(child, cot); // Call the parser function
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing <" << name << ">: " << e.what() << std::endl;
            }
        }

        return cot;
    }

    /// @brief Deserialize from XML string
    static CoT_Schema FromXmlString(const std::string& xmlStr) 
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(xmlStr.c_str());
        if (!result) 
        {
            std::cerr << "Failed to parse XML string: " << result.description() << std::endl;
            return CoT_Schema();
        }
        return FromXml(doc.child("event"));
    }

};