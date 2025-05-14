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

    class Data 
    {
    public:
        double latitude;
        double longitude;
        double hae;
        double circularError;
        double linearError;

        Data(double latitude = std::numeric_limits<double>::quiet_NaN(),
            double longitude = std::numeric_limits<double>::quiet_NaN(),
            double hae = std::numeric_limits<double>::quiet_NaN(),
            double circularError = std::numeric_limits<double>::quiet_NaN(),
            double linearError = std::numeric_limits<double>::quiet_NaN())
            : latitude(latitude), longitude(longitude), hae(hae),
            circularError(circularError), linearError(linearError) {
        }

        bool IsValid(std::string* errorMsg = nullptr) const 
        {
            if (std::isnan(latitude) || std::isnan(longitude) ||
                std::isnan(hae) || std::isnan(circularError) || std::isnan(linearError)) 
            {
                if (errorMsg) *errorMsg = "Point is missing required fields";
                return false;
            }
            return true;
        }

        bool operator==(const Data& other) const 
        {
            constexpr double EPSILON = 1e-6;
            auto equal = [](double a, double b) 
                {
                    return (std::isnan(a) && std::isnan(b)) || std::abs(a - b) < EPSILON;
                };

            return equal(latitude, other.latitude) && equal(longitude, other.longitude) &&
                equal(hae, other.hae) && equal(circularError, other.circularError) &&
                equal(linearError, other.linearError);
        }

        bool operator!=(const Data& other) const 
        { 
            return !(*this == other); 
        }

        std::string ToXml() const 
        {
            if (!IsValid()) return "<point/>";
            std::ostringstream oss;
            oss << "<point lat=\"" << std::fixed << std::setprecision(6) << latitude
                << "\" lon=\"" << longitude << "\" hae=\"" << hae
                << "\" ce=\"" << circularError << "\" le=\"" << linearError << "\"/>";
            return oss.str();
        }

        static Data FromXml(const pugi::xml_node& node) 
        {
            Data point;
            try {
                if (auto attr = node.attribute("lat")) point.latitude = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("lon")) point.longitude = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("hae")) point.hae = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("ce")) point.circularError = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("le")) point.linearError = attr.as_double(std::numeric_limits<double>::quiet_NaN());
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing point attributes: " << e.what() << std::endl;
            }
            return point;
        }

        friend std::ostream& operator<<(std::ostream& os, const Data& point) 
        {
            os << "Point: ";
            if (!point.IsValid()) os << " -NOT VALID- ";
            os << "\n\tLatitude: " << (std::isnan(point.latitude) ? "NaN" : std::to_string(point.latitude))
                << "\n\tLongitude: " << (std::isnan(point.longitude) ? "NaN" : std::to_string(point.longitude))
                << "\n\tHAE: " << (std::isnan(point.hae) ? "NaN" : std::to_string(point.hae))
                << "\n\tCE: " << (std::isnan(point.circularError) ? "NaN" : std::to_string(point.circularError))
                << "\n\tLE: " << (std::isnan(point.linearError) ? "NaN" : std::to_string(point.linearError)) << "\n";
            return os;
        }
    };
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

/// @brief A COT Message subschema class for Date data 
class Date 
{
public:
    unsigned year;
    unsigned month;
    unsigned day;

    Date(unsigned year = 0, unsigned month = 0, unsigned day = 0)
        : year(year), month(month), day(day) {}

    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (year < 1970) 
        {
            if (errorMsg) *errorMsg = "Year must be >= 1970";
            return false;
        }
        if (month < 1 || month > 12) 
        {
            if (errorMsg) *errorMsg = "Month must be between 1 and 12";
            return false;
        }
        unsigned maxDay = DaysInMonth();
        if (day < 1 || day > maxDay) 
        {
            if (errorMsg) *errorMsg = "Day must be between 1 and " + std::to_string(maxDay);
            return false;
        }
        return true;
    }

    bool operator==(const Date& other) const 
    {
        return year == other.year && month == other.month && day == other.day;
    }

    bool operator!=(const Date& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const Date& date) 
    {
        os << std::setfill('0') << std::setw(4) << date.year << "-"
            << std::setfill('0') << std::setw(2) << date.month << "-"
            << std::setfill('0') << std::setw(2) << date.day;
        return os;
    }

private:
    unsigned DaysInMonth() const 
    {
        static const unsigned days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && IsLeapYear()) return 29;
        return days[month - 1];
    }

    bool IsLeapYear() const 
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
};

/// @brief A COT Message subschema class for Time data 
class Time 
{
public:
    unsigned hour;
    unsigned minute;
    double second;

    Time(unsigned hour = 0, unsigned minute = 0, double second = -1)
        : hour(hour), minute(minute), second(second) {}

    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (hour >= 24) 
        {
            if (errorMsg) *errorMsg = "Hour must be < 24";
            return false;
        }
        if (minute >= 60) 
        {
            if (errorMsg) *errorMsg = "Minute must be < 60";
            return false;
        }
        if (second < 0 || second >= 60.0) 
        {
            if (errorMsg) *errorMsg = "Second must be between 0 and 59.999...";
            return false;
        }
        return true;
    }

    bool operator==(const Time& other) const 
    {
        constexpr double EPSILON = 1e-6;
        return hour == other.hour && minute == other.minute &&
            (std::isnan(second) && std::isnan(other.second) || std::abs(second - other.second) < EPSILON);
    }

    bool operator!=(const Time& other) const 
    { 
        return !(*this == other); 
    }

    friend std::ostream& operator<<(std::ostream& os, const Time& time) 
    {
        os << std::setfill('0') << std::setw(2) << time.hour << ":"
            << std::setfill('0') << std::setw(2) << time.minute << ":"
            << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << time.second;
        return os;
    }
};

/// @brief A COT Message subschema class for DateTime data 
class DateTime : public Date, public Time 
{
public:
    DateTime(unsigned year = 0, unsigned month = 0, unsigned day = 0,
             unsigned hour = 0, unsigned minute = 0, double second = -1)
        : Date(year, month, day), Time(hour, minute, second) {}

    /// @brief Check if the DateTime object is valid
    /// @param errorMsg Optional error message for invalid reason
    /// @return true if valid, else false
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        std::string dateError, timeError;
        bool dateValid = Date::IsValid(&dateError);
        bool timeValid = Time::IsValid(&timeError);
        if (!dateValid || !timeValid) 
        {
            if (errorMsg) 
            {
                *errorMsg = "";
                if (!dateValid) *errorMsg += "Date invalid: " + dateError + "; ";
                if (!timeValid) *errorMsg += "Time invalid: " + timeError;
            }
            return false;
        }
        return true;
    }

    /// @brief Convert the DateTime object into a CoT Timestamp string
    /// @return string containing the converted DateTime object
    std::string ToCotTimestamp() const 
    {
        if (!IsValid()) return "";
        std::stringstream timestamp;
        timestamp << std::setfill('0') << std::setw(4) << year << "-"
                  << std::setfill('0') << std::setw(2) << month << "-"
                  << std::setfill('0') << std::setw(2) << day << "T"
                  << std::setfill('0') << std::setw(2) << hour << ":"
                  << std::setfill('0') << std::setw(2) << minute << ":"
                  << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << second << "Z";
        return timestamp.str();
    }

    /// @brief Parse a CoT Timestamp string into a DateTime object
    /// @param str sring containing the value of for the timestamp (e.g. 2025-05-11T19:05:24.000Z)
    /// @param errorMsg Optional error message if converting into DateTime object fails
    /// @return DateTime object with parse time, or default if invalid
    static DateTime FromString(const std::string& str, std::string* errorMsg = nullptr) 
    {
        if (str.empty()) 
        {
            if (errorMsg) *errorMsg = "Empty timestamp string";
            return DateTime();
        }

        // Split on 'T' to separate date and time
        std::vector<std::string> parts;
        std::stringstream stream(str);
        std::string part;
        while (std::getline(stream, part, 'T')) 
        {
            parts.push_back(part);
        }

        if (parts.size() != 2) 
        {
            if (errorMsg) *errorMsg = "Invalid timestamp format: missing 'T' separator";
            return DateTime();
        }

        // Parse date (YYYY-MM-DD)
        std::vector<std::string> dateParts;
        std::stringstream dateStream(parts[0]);
        while (std::getline(dateStream, part, '-')) 
        {
            dateParts.push_back(part);
        }

        if (dateParts.size() != 3) 
        {
            if (errorMsg) *errorMsg = "Invalid date format: expected YYYY-MM-DD";
            return DateTime();
        }

        // Parse time (hh:mm:ss.sssZ)
        std::string timeStr = parts[1];
        if (timeStr.empty() || timeStr.back() != 'Z') 
        {
            if (errorMsg) *errorMsg = "Invalid time format: missing 'Z' suffix";
            return DateTime();
        }

        timeStr.pop_back(); // Remove 'Z'
        std::vector<std::string> timeParts;
        std::stringstream timeStream(timeStr);
        while (std::getline(timeStream, part, ':')) 
        {
            timeParts.push_back(part);
        }

        if (timeParts.size() != 3) 
        {
            if (errorMsg) *errorMsg = "Invalid time format: expected hh:mm:ss.sss";
            return DateTime();
        }

        try 
        {
            unsigned year = std::stoul(dateParts[0]);
            unsigned month = std::stoul(dateParts[1]);
            unsigned day = std::stoul(dateParts[2]);
            unsigned hour = std::stoul(timeParts[0]);
            unsigned minute = std::stoul(timeParts[1]);
            double second = std::stod(timeParts[2]);

            DateTime dt(year, month, day, hour, minute, second);
            if (!dt.IsValid(errorMsg)) 
            {
                return DateTime();
            }
            return dt;
        }
        catch (const std::exception& e) 
        {
            if (errorMsg) *errorMsg = "Parsing error: " + std::string(e.what());
            return DateTime();
        }
    }

    /// @brief Creates a DateTime object from the current system clock (UTC)
    /// @param errorMsg Optional error message if system time retrieval fails
    /// @return DateTime object with current UTC time, or default if invalid
    static DateTime FromSystemClock(std::string* errorMsg = nullptr)
    {
        // Get current time as epoch seconds
        std::time_t now = std::time(nullptr);
        if (now == static_cast<std::time_t>(-1))
        {
            if (errorMsg) *errorMsg = "Failed to retrieve system time";
            return DateTime();
        }

        // Convert to UTC using gmtime (portable across Windows, Linux, macOS)
        std::tm* utcTime = std::gmtime(&now);
        if (!utcTime)
        {
            if (errorMsg) *errorMsg = "Failed to convert system time to UTC";
            return DateTime();
        }

        // Populate DateTime fields
        // tm_year is years since 1900, so add 1900
        // tm_mon is 0-11, so add 1
        unsigned year = utcTime->tm_year + 1900;
        unsigned month = utcTime->tm_mon + 1;
        unsigned day = utcTime->tm_mday;
        unsigned hour = utcTime->tm_hour;
        unsigned minute = utcTime->tm_min;
        double second = utcTime->tm_sec; // No sub-second precision

        DateTime dt(year, month, day, hour, minute, second);
        if (!dt.IsValid(errorMsg))
        {
            return DateTime();
        }
        return dt;
    }

    /// @brief Sets DateTime fields with user-provided values
    /// @param year Year (>= 1970), default 0 (no change)
    /// @param month Month (1-12), default 0 (no change)
    /// @param day Day (1-31, depending on month), default 0 (no change)
    /// @param hour Hour (0-23), default UINT_MAX (no change)
    /// @param minute Minute (0-59), default UINT_MAX (no change)
    /// @param second Second (0-59.999...), default -1 (no change)
    /// @param errorMsg Optional error message if validation fails
    /// @return True if set successfully, false if invalid
    bool Set(unsigned year = 0, unsigned month = 0, unsigned day = 0,
             unsigned hour = UINT_MAX, unsigned minute = UINT_MAX, double second = -1,
             std::string* errorMsg = nullptr)
    {
        // Store current values
        DateTime temp = *this;

        // Update only provided fields
        if (year != 0) temp.year = year;
        if (month != 0) temp.month = month;
        if (day != 0) temp.day = day;
        if (hour != UINT_MAX) temp.hour = hour;
        if (minute != UINT_MAX) temp.minute = minute;
        if (second != -1) temp.second = second;

        // Validate
        if (!temp.IsValid(errorMsg))
        {
            return false;
        }

        // Update fields if valid
        *this = temp;
        return true;
    }

    /// @brief Set the current DateTime object from the current system clock (UTC)
    void SetFromSystemClock()
    {
        *this = FromSystemClock();
    }

    /// @brief Equality comparison operator
    bool operator==(const DateTime& other) const 
    {
        return static_cast<const Date&>(*this) == static_cast<const Date&>(other) &&
               static_cast<const Time&>(*this) == static_cast<const Time&>(other);
    }

    /// @brief Inequality comparison operator
    bool operator!=(const DateTime& other) const 
    { 
        return !(*this == other); 
    }

    /// @brief Friend class for convenient printing
    friend std::ostream& operator<<(std::ostream& os, const DateTime& dt) 
    {
        os << static_cast<const Date&>(dt) << "T" << static_cast<const Time&>(dt) << "Z";
        return os;
    }
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

    std::string ToXml() const 
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
            << " how=\"" << how << "\">";
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

/// @brief A COT Message subschema class for Takv data
class Takv
{
public:
    std::string version;  /// Version
    std::string device;   /// Device type
    std::string os;       /// Operating system
    std::string platform; /// TAK platform (ATAK-CIV, WINTAK, ATAK-MIL, etc)

    /// @brief Constructor - Initializes Everything
    Takv(const std::string& version = INVALID_VALUE,
        const std::string& device = INVALID_VALUE,
        const std::string& os = INVALID_VALUE,
        const std::string& platform = INVALID_VALUE) :
        version(version), device(device), os(os), platform(platform) {}

    /// @brief Equality comparison operator
    bool operator==(const Takv& other) const 
    {
        return version == other.version &&
            device == other.device &&
            os == other.os &&
            platform == other.platform;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Takv& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = !version.empty() || !device.empty() || !os.empty() || !platform.empty();
        if (!valid && errorMsg) 
        {
            *errorMsg = "Takv has no non-empty fields";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<takv/>"; // Empty tag for invalid Takv
        }
        std::ostringstream oss;
        oss << "<takv";
        if (!version.empty()) oss << " version=\"" << version << "\"";
        if (!device.empty()) oss << " device=\"" << device << "\"";
        if (!os.empty()) oss << " os=\"" << os << "\"";
        if (!platform.empty()) oss << " platform=\"" << platform << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Takv FromXml(const pugi::xml_node& node) 
    {
        Takv takv;
        pugi::xml_attribute attr;
        if (attr = node.attribute("version")) takv.version = attr.as_string();
        if (attr = node.attribute("device")) takv.device = attr.as_string();
        if (attr = node.attribute("os")) takv.os = attr.as_string();
        if (attr = node.attribute("platform")) takv.platform = attr.as_string();
        return takv;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Takv& takv) 
    {
        os << "Takv: ";
        if (!takv.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tVersion: " << (takv.version.empty() ? "None" : takv.version) << "\n"
            << "\tDevice: " << (takv.device.empty() ? "None" : takv.device) << "\n"
            << "\tOS: " << (takv.os.empty() ? "None" : takv.os) << "\n"
            << "\tPlatform: " << (takv.platform.empty() ? "None" : takv.platform) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Contact data
class Contact
{
public:
    std::string endpoint;     /// Endpoint of the unit, usually a TCP address
    std::string callsign;     /// Callsign (name) of the item
    std::string xmppUsername; /// XMPP Username

    /// @brief Constructor - Initializes Everything
    Contact(const std::string& endpoint = INVALID_VALUE,
        const std::string& callsign = INVALID_VALUE,
        const std::string& xmppUsername = INVALID_VALUE) :
        endpoint(endpoint), callsign(callsign), xmppUsername(xmppUsername) {}

    /// @brief Equality comparison operator
    bool operator==(const Contact& other) const 
    {
        return endpoint == other.endpoint &&
            callsign == other.callsign &&
            xmppUsername == other.xmppUsername;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Contact& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = !endpoint.empty() || !callsign.empty() || !xmppUsername.empty();
        if (!valid && errorMsg) 
        {
            *errorMsg = "Contact has no non-empty fields";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<contact/>"; // Empty tag for invalid Contact
        }
        std::ostringstream oss;
        oss << "<contact";
        if (!endpoint.empty()) oss << " endpoint=\"" << endpoint << "\"";
        if (!callsign.empty()) oss << " callsign=\"" << callsign << "\"";
        if (!xmppUsername.empty()) oss << " xmppUsername=\"" << xmppUsername << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Contact FromXml(const pugi::xml_node& node) 
    {
        Contact contact;
        pugi::xml_attribute attr;
        if (attr = node.attribute("endpoint")) contact.endpoint = attr.as_string();
        if (attr = node.attribute("callsign")) contact.callsign = attr.as_string();
        if (attr = node.attribute("xmppUsername")) contact.xmppUsername = attr.as_string();
        return contact;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Contact& contact) 
    {
        os << "Contact: ";
        if (!contact.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tEndpoint: " << (contact.endpoint.empty() ? "None" : contact.endpoint) << "\n"
            << "\tCallsign: " << (contact.callsign.empty() ? "None" : contact.callsign) << "\n"
            << "\tXMPP Username: " << (contact.xmppUsername.empty() ? "None" : contact.xmppUsername) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Uid data
class Uid
{
public:
    std::string droid; /// User

    /// @brief Constructor - Initializes Everything
    Uid(const std::string& droid = INVALID_VALUE) : droid(droid) {}

    /// @brief Equality comparison operator
    bool operator==(const Uid& other) const 
    {
        return droid == other.droid;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Uid& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = !droid.empty();
        if (!valid && errorMsg) 
        {
            *errorMsg = "Uid droid is empty";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<uid/>"; // Empty tag for invalid Uid
        }
        std::ostringstream oss;
        oss << "<uid Droid=\"" << droid << "\"/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Uid FromXml(const pugi::xml_node& node) 
    {
        Uid uid;
        pugi::xml_attribute attr;
        if (attr = node.attribute("Droid")) uid.droid = attr.as_string();
        return uid;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Uid& uid) 
    {
        os << "Uid: ";
        if (!uid.IsValid()) { os << " -NOT VALID- "; }
        os << "\n\tDroid: " << (uid.droid.empty() ? "None" : uid.droid) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for PrecisionLocation data
class PrecisionLocation
{
public:
    std::string altsrc;      /// Altitude source
    std::string geopointsrc; /// Geopoint source

    /// @brief Constructor - Initializes Everything
    PrecisionLocation(const std::string& altsrc = INVALID_VALUE,
        const std::string& geopointsrc = INVALID_VALUE) :
        altsrc(altsrc), geopointsrc(geopointsrc) {}

    /// @brief Equality comparison operator
    bool operator==(const PrecisionLocation& other) const 
    {
        return altsrc == other.altsrc &&
            geopointsrc == other.geopointsrc;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const PrecisionLocation& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = !altsrc.empty() || !geopointsrc.empty();
        if (!valid && errorMsg) 
        {
            *errorMsg = "PrecisionLocation has no non-empty fields";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<precisionlocation/>"; // Empty tag for invalid PrecisionLocation
        }
        std::ostringstream oss;
        oss << "<precisionlocation";
        if (!altsrc.empty()) oss << " altsrc=\"" << altsrc << "\"";
        if (!geopointsrc.empty()) oss << " geopointsrc=\"" << geopointsrc << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static PrecisionLocation FromXml(const pugi::xml_node& node) 
    {
        PrecisionLocation preloc;
        pugi::xml_attribute attr;
        if (attr = node.attribute("altsrc")) preloc.altsrc = attr.as_string();
        if (attr = node.attribute("geopointsrc")) preloc.geopointsrc = attr.as_string();
        return preloc;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const PrecisionLocation& preloc) 
    {
        os << "PrecisionLocation: ";
        if (!preloc.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tAlt Source: " << (preloc.altsrc.empty() ? "None" : preloc.altsrc) << "\n"
            << "\tGeopoint Source: " << (preloc.geopointsrc.empty() ? "None" : preloc.geopointsrc) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Group data
class Group
{
public:
    std::string role; /// Role of the group member sending
    std::string name; /// Group name

    /// @brief Constructor - Initializes Everything
    Group(const std::string& role = INVALID_VALUE,
        const std::string& name = INVALID_VALUE) :
        role(role), name(name) {}

    /// @brief Equality comparison operator
    bool operator==(const Group& other) const 
    {
        return role == other.role &&
            name == other.name;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Group& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        bool valid = !role.empty() || !name.empty();
        if (!valid && errorMsg) 
        {
            *errorMsg = "Group has no non-empty fields";
        }
        return valid;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<__group/>"; // Empty tag for invalid Group
        }
        std::ostringstream oss;
        oss << "<__group";
        if (!role.empty()) oss << " role=\"" << role << "\"";
        if (!name.empty()) oss << " name=\"" << name << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Group FromXml(const pugi::xml_node& node) 
    {
        Group group;
        pugi::xml_attribute attr;
        if (attr = node.attribute("role")) group.role = attr.as_string();
        if (attr = node.attribute("name")) group.name = attr.as_string();
        return group;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Group& group) 
    {
        os << "Group: ";
        if (!group.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tRole: " << (group.role.empty() ? "None" : group.role) << "\n"
            << "\tName: " << (group.name.empty() ? "None" : group.name) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Status data
class Status
{
public:
    double battery; /// Battery percentage

    /// @brief Constructor - Initializes Everything
    Status(double battery = INVALID_VALUE) : battery(battery) {}

    /// @brief Equality comparison operator
    bool operator==(const Status& other) const 
    {
        constexpr double EPSILON = 1e-6;
        return (std::isnan(battery) && std::isnan(other.battery)) ||
            std::abs(battery - other.battery) < EPSILON;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Status& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (std::isnan(battery)) 
        {
            if (errorMsg) *errorMsg = "Status battery is unset (NaN)";
            return false;
        }

        if (battery < 0 || battery > 100) 
        {
            if (errorMsg) *errorMsg = "Status battery out of range (0-100)";
            return false;
        }

        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<status/>"; // Empty tag for invalid Status
        }
        std::ostringstream oss;
        oss << "<status battery=\"" << std::fixed << std::setprecision(2) << battery << "\"/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Status FromXml(const pugi::xml_node& node) 
    {
        Status status;
        pugi::xml_attribute attr;
        if (attr = node.attribute("battery")) 
        {
            try {
                status.battery = attr.as_double(INVALID_VALUE);
            }
            catch (const std::exception& e) 
            {
                status.battery = INVALID_VALUE;
            }
        }
        return status;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Status& status)
    {
        os << "Status: ";
        if (!status.IsValid()) { os << " -NOT VALID- "; }
        os << "\n\tBattery: ";
        if (std::isnan(status.battery)) 
        {
            os << "NaN";
        }
        else 
        {
            os << std::fixed << std::setprecision(2) << status.battery;
        }
        os << "\n";
        return os;
    }

private:
    static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN(); /// Sentinel value for invalid double
};

/// @brief A COT Message subschema class for Track data
class Track
{
public:
    double course;      /// Direction of motion with respect to true north in degrees (0 to 360). 
    double speed;       /// Magnitude of motion in meters per second (non-negative). 
    double slope;       /// Vertical component of motion in degrees (-90 to 90, negative for downward).
    double eCourse;     /// 1-sigma error for course in degrees (non-negative).
    double eSpeed;      /// 1-sigma error for speed in meters per second (non-negative).
    double eSlope;      /// 1-sigma error for slope in degrees (non-negative).
    double version;     /// Version of the track schema (positive).

    /// @brief Constructor - Initializes everything. course and speed are required, others optional.
    Track(const double course = INVALID_VALUE,
        const double speed = INVALID_VALUE,
        const double slope = INVALID_VALUE,
        const double eCourse = INVALID_VALUE,
        const double eSpeed = INVALID_VALUE,
        const double eSlope = INVALID_VALUE,
        const double version = INVALID_VALUE) :
        course(course), speed(speed), slope(slope),
        eCourse(eCourse), eSpeed(eSpeed), eSlope(eSlope),
        version(version)
    {}

    /// @brief Equal comparison operator
    bool operator==(const Track& track) const
    {
        const double EPSILON = 1e-6;

        auto equal = [](double a, double b, double epsilon) {
                return std::isnan(a) && std::isnan(b) || std::abs(a - b) < epsilon;
            };

        return equal(course, track.course, EPSILON) &&
            equal(speed, track.speed, EPSILON) &&
            equal(slope, track.slope, EPSILON) &&
            equal(eCourse, track.eCourse, EPSILON) &&
            equal(eSpeed, track.eSpeed, EPSILON) &&
            equal(eSlope, track.eSlope, EPSILON) &&
            equal(version, track.version, EPSILON);
    }

    /// @brief Not-equal comparison operator
    bool operator!=(const Track& track) const
    {
        return !(*this == track);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const
    {
        if (std::isnan(course)) {
            if (errorMsg) *errorMsg = "Course is NaN";
            return false;
        }
        if (course < COURSE_MIN || course > COURSE_MAX) {
            if (errorMsg) *errorMsg = "Course out of range [0, 360]";
            return false;
        }
        if (std::isnan(speed)) {
            if (errorMsg) *errorMsg = "Speed is NaN";
            return false;
        }
        if (speed < SPEED_MIN) {
            if (errorMsg) *errorMsg = "Speed is negative";
            return false;
        }
        if (!std::isnan(slope) && (slope < SLOPE_MIN || slope > SLOPE_MAX)) {
            if (errorMsg) *errorMsg = "Slope out of range [-90, 90]";
            return false;
        }
        if (!std::isnan(eCourse) && eCourse < 0) {
            if (errorMsg) *errorMsg = "eCourse is negative";
            return false;
        }
        if (!std::isnan(eSpeed) && eSpeed < 0) {
            if (errorMsg) *errorMsg = "eSpeed is negative";
            return false;
        }
        if (!std::isnan(eSlope) && eSlope < 0) {
            if (errorMsg) *errorMsg = "eSlope is negative";
            return false;
        }
        if (!std::isnan(version) && version <= 0) {
            if (errorMsg) *errorMsg = "Version is non-positive";
            return false;
        }
        return true;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Track& track)
    {
        os << "Track: ";  if (!track.IsValid()) { os << " -NOT VALID- "; }
        os << "\n" << std::fixed << std::setprecision(6)
            << "\tCourse:          " << track.course << "\n"
            << "\tSpeed:           " << track.speed << "\n";
        if (!std::isnan(track.slope))   os << "\tSlope:           " << track.slope << "\n";
        if (!std::isnan(track.eCourse)) os << "\teCourse:         " << track.eCourse << "\n";
        if (!std::isnan(track.eSpeed))  os << "\teSpeed:          " << track.eSpeed << "\n";
        if (!std::isnan(track.eSlope))  os << "\teSlope:          " << track.eSlope << "\n";
        if (!std::isnan(track.version)) os << "\tVersion:         " << track.version << "\n";
        os << std::defaultfloat << "\n";
        return os;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const
    {
        std::ostringstream oss;
        oss << "<track";
        if (!std::isnan(course)) oss << " course=\"" << std::fixed << std::setprecision(6) << course << "\"";
        if (!std::isnan(speed)) oss << " speed=\"" << speed << "\"";
        if (!std::isnan(slope)) oss << " slope=\"" << slope << "\"";
        if (!std::isnan(eCourse)) oss << " eCourse=\"" << eCourse << "\"";
        if (!std::isnan(eSpeed)) oss << " eSpeed=\"" << eSpeed << "\"";
        if (!std::isnan(eSlope)) oss << " eSlope=\"" << eSlope << "\"";
        if (!std::isnan(version)) oss << " version=\"" << version << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Track FromXml(const pugi::xml_node& node)
    {
        Track track;
        pugi::xml_attribute attr;
        (attr = node.attribute("course")) ? track.course = attr.as_double() : track.course = INVALID_VALUE;
        (attr = node.attribute("speed")) ? track.speed = attr.as_double() : track.speed = INVALID_VALUE;
        (attr = node.attribute("slope")) ? track.slope = attr.as_double() : track.slope = INVALID_VALUE;
        (attr = node.attribute("eCourse")) ? track.eCourse = attr.as_double() : track.eCourse = INVALID_VALUE;
        (attr = node.attribute("eSpeed")) ? track.eSpeed = attr.as_double() : track.eSpeed = INVALID_VALUE;
        (attr = node.attribute("eSlope")) ? track.eSlope = attr.as_double() : track.eSlope = INVALID_VALUE;
        (attr = node.attribute("version")) ? track.version = attr.as_double() : track.version = INVALID_VALUE;
        return track;
    }

private:
    static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
    static constexpr double COURSE_MIN = 0.0;    /// Minimum course in degrees
    static constexpr double COURSE_MAX = 360.0;  /// Maximum course in degrees
    static constexpr double SPEED_MIN = 0.0;     /// Minimum speed in meters per second
    static constexpr double SLOPE_MIN = -90.0;   /// Minimum slope in degrees
    static constexpr double SLOPE_MAX = 90.0;    /// Maximum slope in degrees
};

/// @brief A COT Message subschema class for Stroke Color data
class StrokeColor
{
public:
    int value; /// Color value (ARGB as signed 32-bit integer)

    /// @brief Constructor - Initializes Everything
    StrokeColor(int value = INVALID_VALUE) : value(value) {}

    /// @brief Equality comparison operator
    bool operator==(const StrokeColor& other) const 
    {
        return value == other.value;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const StrokeColor& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (value == INVALID_VALUE) 
        {
            if (errorMsg) *errorMsg = "StrokeColor value is unset";
            return false;
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<strokeColor/>"; // Empty tag for unset color
        }
        std::ostringstream oss;
        oss << "<strokeColor value=\"" << value << "\"/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static StrokeColor FromXml(const pugi::xml_node& node) 
    {
        StrokeColor color;
        pugi::xml_attribute attr;
        if (attr = node.attribute("value")) 
        {
            try 
            {
                color.value = attr.as_int(INVALID_VALUE);
            }
            catch (const std::exception& e) 
            {
                color.value = INVALID_VALUE;
            }
        }
        return color;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const StrokeColor& color) 
    {
        os << "StrokeColor: ";
        if (!color.IsValid()) 
        {
            os << " -NOT VALID- (Unset)";
        }
        else 
        {
            os << "\n\tValue: " << color.value;
        }
        os << "\n";
        return os;
    }

private:
    static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
};

/// @brief A COT Message subschema class for Fill Color data
class FillColor
{
public:
    int value; /// Color value (ARGB as signed 32-bit integer)

    /// @brief Constructor - Initializes Everything
    FillColor(int value = INVALID_VALUE) : value(value) {}

    /// @brief Equality comparison operator
    bool operator==(const FillColor& other) const 
    {
        return value == other.value;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const FillColor& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (value == INVALID_VALUE) 
        {
            if (errorMsg) *errorMsg = "FillColor value is unset";
            return false;
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<fillColor/>"; // Empty tag for unset color
        }
        std::ostringstream oss;
        oss << "<fillColor value=\"" << value << "\"/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static FillColor FromXml(const pugi::xml_node& node) 
    {
        FillColor color;
        pugi::xml_attribute attr;
        if (attr = node.attribute("value")) 
        {
            try 
            {
                color.value = attr.as_int(INVALID_VALUE);
            }
            catch (const std::exception& e) 
            {
                color.value = INVALID_VALUE;
            }
        }
        return color;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const FillColor& color) 
    {
        os << "FillColor: ";
        if (!color.IsValid()) 
        {
            os << " -NOT VALID- (Unset)";
        }
        else 
        {
            os << "\n\tValue: " << color.value;
        }
        os << "\n";
        return os;
    }

private:
    static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
};

/// @brief A COT Message subschema class for Color data
class Color
{
public:
    int argb; /// Color value (ARGB as signed 32-bit integer)

    /// @brief Constructor - Initializes Everything
    Color(int argb = INVALID_VALUE) : argb(argb) {}

    /// @brief Equality comparison operator
    bool operator==(const Color& other) const 
    {
        return argb == other.argb;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Color& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (argb == INVALID_VALUE) 
        {
            if (errorMsg) *errorMsg = "Color argb is unset";
            return false;
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (!IsValid()) 
        {
            return "<color/>"; // Empty tag for unset color
        }
        std::ostringstream oss;
        oss << "<color argb=\"" << argb << "\"/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Color FromXml(const pugi::xml_node& node) 
    {
        Color color;
        pugi::xml_attribute attr;
        if (attr = node.attribute("argb")) 
        {
            try 
            {
                color.argb = attr.as_int(INVALID_VALUE);
            }
            catch (const std::exception& e) 
            {
                color.argb = INVALID_VALUE;
            }
        }
        return color;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Color& color) 
    {
        os << "Color: ";
        if (!color.IsValid()) 
        {
            os << " -NOT VALID- (Unset)";
        }
        else 
        {
            os << "\n\tARGB: " << color.argb;
        }
        os << "\n";
        return os;
    }

private:
    static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
};

/// @brief A COT Message subschema class for User Icon data
class UserIcon
{
public:
    std::string iconSetPath; /// Icon set path

    /// @brief Constructor - Initializes Everything
    UserIcon(const std::string& iconpath = INVALID_VALUE) : iconSetPath(iconpath) {}

    /// @brief Equality comparison operator
    bool operator==(const UserIcon& other) const 
    {
        return iconSetPath == other.iconSetPath;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const UserIcon& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const
    {
        if(iconSetPath.empty())
        {
            if (errorMsg) *errorMsg = "iconsetpath is empty";
            return false;
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (IsValid())
        {
            std::ostringstream oss;
            oss << "<usericon iconsetpath=\"" << iconSetPath << "\"/>";
            return oss.str();
        }
        return "";
    }

    /// @brief Deserialize from XML node
    static UserIcon FromXml(const pugi::xml_node& node) 
    {
        UserIcon ui;
        pugi::xml_attribute attr;
        if (attr = node.attribute("iconsetpath")) 
        {
            ui.iconSetPath = attr.as_string();
        }
        return ui;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const UserIcon& ui) {
        os << "UserIcon: ";
        if (!ui.IsValid()) { os << " -NOT VALID- "; }
        os << "\n\tIconSetPath: " << (ui.IsValid() ? "None" : ui.iconSetPath) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Model data
class Model
{
public:
    std::string value; /// Model value

    /// @brief Constructor - Initializes Everything
    Model(const std::string& val = INVALID_VALUE) : value(val) {}

    /// @brief Equality comparison operator
    bool operator==(const Model& other) const 
    {
        return value == other.value;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Model& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (value.empty())
        {
            if (errorMsg) *errorMsg = "value is empty";
            return false;
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        if (IsValid())
        {
            std::ostringstream oss;
            oss << "<model value=\"" << value << "\"/>";
            return oss.str();
        }
        return "";
    }

    /// @brief Deserialize from XML node
    static Model FromXml(const pugi::xml_node& node) 
    {
        Model model;
        pugi::xml_attribute attr;
        if (attr = node.attribute("value")) 
        {
            model.value = attr.as_string();
        }
        return model;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Model& model) 
    {
        os << "Model: ";
        if (!model.IsValid()) { os << " -NOT VALID- "; }
        os << "\n\tValue: " << (model.value.empty() ? "None" : model.value) << "\n";
        return os;
    }

private:
    static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
};

/// @brief A COT Message subschema class for Link data
class Link
{
public:
    std::string uid;          /// Unique identifier
    std::string remarks;      /// Remarks
    std::string relation;     /// Relation
    std::string callsign;     /// Callsign
    std::string type;         /// Type
    std::string point;        /// Point as a string (e.g., "lat,lon")
    double latitude;          /// Latitude portion of the point
    double longitude;         /// Longitude portion of the point

    /// @brief Constructor - Initializes Everything
    Link(const std::string& uid = INVALID_STRING,
        const std::string& remarks = INVALID_STRING,
        const std::string& relation = INVALID_STRING,
        const std::string& callsign = INVALID_STRING,
        const std::string& type = INVALID_STRING,
        const std::string& point = INVALID_STRING,
        double lat = INVALID_VALUE,
        double lon = INVALID_VALUE) :
        uid(uid), remarks(remarks), relation(relation),
        callsign(callsign), type(type), point(point),
        latitude(lat), longitude(lon) {}

    /// @brief Equality comparison operator
    bool operator==(const Link& other) const 
    {
        constexpr double EPSILON = 1e-6;
        return uid == other.uid &&
            remarks == other.remarks &&
            relation == other.relation &&
            callsign == other.callsign &&
            type == other.type &&
            point == other.point &&
            (std::isnan(latitude) && std::isnan(other.latitude) ||
                std::abs(latitude - other.latitude) < EPSILON) &&
            (std::isnan(longitude) && std::isnan(other.longitude) ||
                std::abs(longitude - other.longitude) < EPSILON);
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Link& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        if (std::isnan(latitude) || std::isnan(longitude) || uid.empty() || type.empty()) 
        {
            if (errorMsg) *errorMsg = "Invalid latitude or longitude";
            return false;
        }
        // String fields are optional, so empty is valid
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const 
    {
        std::ostringstream oss;
        oss << "<link";
        if (!uid.empty()) oss << " uid=\"" << uid << "\"";
        if (!remarks.empty()) oss << " remarks=\"" << remarks << "\"";
        if (!relation.empty()) oss << " relation=\"" << relation << "\"";
        if (!callsign.empty()) oss << " callsign=\"" << callsign << "\"";
        if (!type.empty()) oss << " type=\"" << type << "\"";
        if (!point.empty()) oss << " point=\"" << point << "\"";
        oss << "/>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Link FromXml(const pugi::xml_node& node) 
    {
        Link link;
        pugi::xml_attribute attr;
        if (attr = node.attribute("uid")) link.uid = attr.as_string();
        if (attr = node.attribute("remarks")) link.remarks = attr.as_string();
        if (attr = node.attribute("relation")) link.relation = attr.as_string();
        if (attr = node.attribute("callsign")) link.callsign = attr.as_string();
        if (attr = node.attribute("type")) link.type = attr.as_string();
        if (attr = node.attribute("point")) link.point = attr.as_string();
        try 
        {
            auto [lat, lon] = link.GetLatLonFromPoint();
            link.latitude = lat;
            link.longitude = lon;
        }
        catch (const std::exception& e) 
        {
            link.latitude = INVALID_VALUE;
            link.longitude = INVALID_VALUE;
        }
        return link;
    }

    /// @brief Parse point into latitude and longitude
    std::pair<double, double> GetLatLonFromPoint() const 
    {
        if (point.empty()) 
        {
            return { INVALID_VALUE, INVALID_VALUE };
        }
        size_t commaPos = point.find(',');
        if (commaPos == std::string::npos) 
        {
            return { INVALID_VALUE, INVALID_VALUE };
        }
        try 
        {
            double lat = std::stod(point.substr(0, commaPos));
            double lon = std::stod(point.substr(commaPos + 1));
            return { lat, lon };
        }
        catch (const std::exception& e) 
        {
            return { INVALID_VALUE, INVALID_VALUE };
        }
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Link& link) 
    {
        os << "Link: ";
        if (!link.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tUID: " << (link.uid.empty() ? "None" : link.uid) << "\n"
            << "\tRemarks: " << (link.remarks.empty() ? "None" : link.remarks) << "\n"
            << "\tRelation: " << (link.relation.empty() ? "None" : link.relation) << "\n"
            << "\tCallsign: " << (link.callsign.empty() ? "None" : link.callsign) << "\n"
            << "\tType: " << (link.type.empty() ? "None" : link.type) << "\n"
            << "\tPoint: " << (link.point.empty() ? "None" : link.point) << "\n"
            << "\tLatitude: ";
        if (std::isnan(link.latitude)) 
        {
            os << "NaN";
        }
        else 
        {
            os << std::fixed << std::setprecision(6) << link.latitude;
        }
        os << "\n\tLongitude: ";
        if (std::isnan(link.longitude)) 
        {
            os << "NaN";
        }
        else 
        {
            os << std::fixed << std::setprecision(6) << link.longitude;
        }
        os << "\n";
        return os;
    }

private:
    static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
    static constexpr const char* INVALID_STRING = "";
};

/// @brief A struct to store custom detail elements in a CoT message
class CustomDetail 
{
public:
    std::string name;                               /// XML tag name (e.g., "customTag")
    std::map<std::string, std::string> attributes;  /// XML attributes as key-value pairs
    std::string content;                            /// Inner text or CDATA

    /// @brief Constructor
    CustomDetail(const std::string& name = "",
        const std::map<std::string, std::string>& attributes = {},
        const std::string& content = "") :
        name(name), attributes(attributes), content(content) {}

    /// @brief Equality comparison
    bool operator==(const CustomDetail& other) const 
    {
        return name == other.name &&
            attributes == other.attributes &&
            content == other.content;
    }

    /// @brief Inequality comparison
    bool operator!=(const CustomDetail& other) const 
    {
        return !(*this == other);
    }

    /// @brief Validate the custom detail
    bool IsValid() const 
    {
        return !name.empty(); // Basic validation: non-empty tag name
    }

    /// @brief Serialize to XML
    std::string ToXml() const 
    {
        std::ostringstream oss;
        oss << "<" << name;
        for (const auto& [key, value] : attributes) 
        {
            oss << " " << key << "=\"" << value << "\"";
        }
        if (content.empty()) 
        {
            oss << "/>";
        }
        else 
        {
            oss << ">" << content << "</" << name << ">";
        }
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static CustomDetail FromXml(const pugi::xml_node& node) 
    {
        CustomDetail detail;
        detail.name = node.name();
        for (pugi::xml_attribute attr : node.attributes()) 
        {
            detail.attributes[attr.name()] = attr.as_string();
        }
        detail.content = node.child_value();
        return detail;
    }

    /// @brief Print the custom detail
    friend std::ostream& operator<<(std::ostream& os, const CustomDetail& detail) 
    {
        os << "\tCustomDetail: " << detail.name;
        if (!detail.IsValid()) { os << " -NOT VALID-"; }
        os << "\n";
        for (const auto& [key, value] : detail.attributes) 
        {
            os << "\t\t" << key << ": " << value << "\n";
        }
        if (!detail.content.empty()) 
        {
            os << "\t\tContent: " << detail.content << "\n";
        }
        return os;
    }
};

/// @brief A CoT Message subschema class for Detail data
class Detail 
{
public:
    Takv takv;                                  /// TAKV Sub-Schema
    Contact contact;                            /// Contact Sub-Schema
    Uid uid;                                    /// UID Sub-Schema
    Model model;                                /// Model Sub-Schema
    PrecisionLocation precisionLocation;        /// PrecisionLocation Sub-Schema
    Group group;                                /// Group Sub-Schema
    Status status;                              /// Status Sub-Schema
    Track track;                                /// Track Sub-Schema
    StrokeColor strokeColor;                    /// StrokeColor Sub-Schema
    FillColor fillColor;                        /// FillColor Sub-Schema
    Color color;                                /// Color Sub-Schema
    UserIcon userIcon;                          /// UserIcon Sub-Schema
    std::string remarks;                        /// Remarks information
    std::vector<Link> links;                    /// Links Sub-Schema
    std::vector<CustomDetail> customDetails;    /// Custom detail elements

    /// @brief Maps known Detail sub-schemas to their XML tag names
    static const std::map<std::string, std::string> XmlTagNames;

    /// @brief Constructor - Initializes everything
    Detail(const Takv& takv = Takv(),
        const Contact& contact = Contact(),
        const Uid& uid = Uid(),
        const Model& model = Model(),
        const PrecisionLocation& precisionLocation = PrecisionLocation(),
        const Group& group = Group(),
        const Status& status = Status(),
        const Track& track = Track(),
        const StrokeColor& strokeColor = StrokeColor(),
        const FillColor& fillColor = FillColor(),
        const Color& color = Color(),
        const UserIcon& userIcon = UserIcon(),
        const std::string& remarks = "",
        const std::vector<Link>& links = {},
        const std::vector<CustomDetail>& customDetails = {}) :
        takv(takv),
        contact(contact),
        uid(uid),
        model(model),
        precisionLocation(precisionLocation),
        group(group),
        status(status),
        track(track),
        strokeColor(strokeColor),
        fillColor(fillColor),
        color(color),
        userIcon(userIcon),
        remarks(remarks),
        links(links),
        customDetails(customDetails)
    {
    }

    /// @brief Equality comparison operator
    bool operator==(const Detail& other) const {
        return takv == other.takv &&
            contact == other.contact &&
            uid == other.uid &&
            model == other.model &&
            precisionLocation == other.precisionLocation &&
            group == other.group &&
            status == other.status &&
            track == other.track &&
            strokeColor == other.strokeColor &&
            fillColor == other.fillColor &&
            color == other.color &&
            userIcon == other.userIcon &&
            remarks == other.remarks &&
            links == other.links &&
            customDetails == other.customDetails;
    }

    /// @brief Inequality comparison operator
    bool operator!=(const Detail& other) const 
    {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const 
    {
        // Currently permissive; all sub-schemas are optional
        // Future: Add validation for required sub-schemas or custom details
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const
    {
        std::ostringstream oss;
        oss << "<detail>";
        if (takv.IsValid()) oss << takv.ToXml();
        if (contact.IsValid()) oss << contact.ToXml();
        if (uid.IsValid()) oss << uid.ToXml();
        if (model.IsValid()) oss << model.ToXml();
        if (precisionLocation.IsValid()) oss << precisionLocation.ToXml();
        if (group.IsValid()) oss << group.ToXml();
        if (status.IsValid()) oss << status.ToXml();
        if (track.IsValid()) oss << track.ToXml();
        if (strokeColor.IsValid()) oss << strokeColor.ToXml();
        if (fillColor.IsValid()) oss << fillColor.ToXml();
        if (color.IsValid()) oss << color.ToXml();
        if (userIcon.IsValid()) oss << userIcon.ToXml();
        if (!remarks.empty()) oss << "<remarks>" << remarks << "</remarks>";
        for (const auto& link : links) 
        {
            oss << link.ToXml();
        }
        for (const auto& custom : customDetails) 
        {
            oss << custom.ToXml();
        }
        oss << "</detail>";
        return oss.str();
    }

    /// @brief Deserialize from XML node
    static Detail FromXml(const pugi::xml_node& node)
    {
        Detail detail;

        // Dispatch table for sub-schema parsing
        static const std::map<std::string, std::function<void(pugi::xml_node, Detail&)>> parsers = {
            {"takv", [](pugi::xml_node n, Detail& d) { d.takv = Takv::FromXml(n); }},
            {"contact", [](pugi::xml_node n, Detail& d) { d.contact = Contact::FromXml(n); }},
            {"uid", [](pugi::xml_node n, Detail& d) { d.uid = Uid::FromXml(n); }},
            {"model", [](pugi::xml_node n, Detail& d) { d.model = Model::FromXml(n); }},
            {"precisionlocation", [](pugi::xml_node n, Detail& d) { d.precisionLocation = PrecisionLocation::FromXml(n); }},
            {"__group", [](pugi::xml_node n, Detail& d) { d.group = Group::FromXml(n); }},
            {"status", [](pugi::xml_node n, Detail& d) { d.status = Status::FromXml(n); }},
            {"track", [](pugi::xml_node n, Detail& d) { d.track = Track::FromXml(n); }},
            {"strokeColor", [](pugi::xml_node n, Detail& d) { d.strokeColor = StrokeColor::FromXml(n); }},
            {"fillColor", [](pugi::xml_node n, Detail& d) { d.fillColor = FillColor::FromXml(n); }},
            {"color", [](pugi::xml_node n, Detail& d) { d.color = Color::FromXml(n); }},
            {"usericon", [](pugi::xml_node n, Detail& d) { d.userIcon = UserIcon::FromXml(n); }},
            {"remarks", [](pugi::xml_node n, Detail& d) { d.remarks = n.child_value(); }},
            {"link", [](pugi::xml_node n, Detail& d) { d.links.push_back(Link::FromXml(n)); }}
        };

        // Parse child nodes
        for (pugi::xml_node child : node.children()) 
        {
            std::string name = child.name();
            try 
            {
                auto it = parsers.find(name);
                if (it != parsers.end()) 
                {
                    it->second(child, detail); // Call the parser function
                }
                else if (!name.empty()) 
                {
                    // Unknown nodes are custom details
                    detail.customDetails.push_back(CustomDetail::FromXml(child));
                }
            }
            catch (const std::exception& e) 
            {
                std::cerr << "Error parsing <" << name << ">: " << e.what() << std::endl;
            }
        }

        return detail;
    }

    /// @brief Add a new custom detail
    /// @param name XML tag name (required, non-empty)
    /// @param attributes XML attributes as key-value pairs (optional)
    /// @param content Inner text or CDATA (optional)
    /// @return true if added successfully, false if invalid (e.g., empty name) or already exists
    bool AddCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& attributes = {},
        const std::string& content = "")
    {
        if (name.empty()) 
        {
            return false; // Invalid tag name
        }

        // Check if a custom detail with this name already exists
        auto it = std::find_if(customDetails.begin(), customDetails.end(),
            [&name](const CustomDetail& detail) { return detail.name == name; });

        if (it != customDetails.end()) 
        {
            return false; // Duplicate name
        }
        customDetails.emplace_back(name, attributes, content);
        return true;
    }

    /// @brief Modify an existing custom detail
    /// @param name XML tag name to match (required, non-empty)
    /// @param matchAttributes Optional attributes to match exactly (empty means match any)
    /// @param matchContent Optional content to match exactly (empty means match any)
    /// @param newAttributes New attributes to set (replaces existing attributes)
    /// @param newContent New content to set (replaces existing content)
    /// @return true if a matching custom detail was modified, false if not found or invalid
    bool ModifyCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& matchAttributes,
        const std::string& matchContent,
        const std::map<std::string, std::string>& newAttributes,
        const std::string& newContent)
    {
        if (name.empty()) 
        {
            return false; // Invalid tag name
        }

        auto it = std::find_if(customDetails.begin(), customDetails.end(),
            [&name, &matchAttributes, &matchContent](const CustomDetail& detail) {
                bool nameMatch = detail.name == name;
                bool attrMatch = matchAttributes.empty() || detail.attributes == matchAttributes;
                bool contentMatch = matchContent.empty() || detail.content == matchContent;
                return nameMatch && attrMatch && contentMatch;
            });

        if (it != customDetails.end()) 
        {
            it->attributes = newAttributes;
            it->content = newContent;
            return true;
        }
        return false; // No match found
    }

    /// @brief Add or modify a custom detail
    /// @param name XML tag name (required, non-empty)
    /// @param attributes XML attributes as key-value pairs
    /// @param content Inner text or CDATA
    /// @param matchAttributes Optional attributes to match for modification (empty means match any)
    /// @param matchContent Optional content to match for modification (empty means match any)
    /// @return true if added or modified successfully, false if invalid
    bool AddOrModifyCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& attributes,
        const std::string& content,
        const std::map<std::string, std::string>& matchAttributes = {},
        const std::string& matchContent = "")
    {
        // Try to modify first
        if (ModifyCustomDetail(name, matchAttributes, matchContent, attributes, content)) 
        {
            return true; // Modified existing detail
        }
        // If modification fails, try to add
        return AddCustomDetail(name, attributes, content);
    }

    /// @brief Remove a custom detail by name, with optional attributes and content matching
    /// @param name XML tag name to match (required, non-empty)
    /// @param attributes Optional attributes to match exactly (empty means match any)
    /// @param content Optional content to match exactly (empty means match any)
    /// @return true if a matching custom detail was removed, false if not found or invalid
    bool RemoveCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& attributes = {},
        const std::string& content = "")
    {
        if (name.empty()) 
        {
            return false; // Invalid tag name
        }

        auto it = std::find_if(customDetails.begin(), customDetails.end(),
            [&name, &attributes, &content](const CustomDetail& detail) {
                bool nameMatch = detail.name == name;
                bool attrMatch = attributes.empty() || detail.attributes == attributes;
                bool contentMatch = content.empty() || detail.content == content;
                return nameMatch && attrMatch && contentMatch;
            });

        if (it != customDetails.end()) 
        {
            customDetails.erase(it);
            return true;
        }
        return false; // No match found
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Detail& detail)
    {
        os << "Detail: ";
        if (!detail.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << detail.takv
            << detail.contact
            << detail.uid
            << detail.model
            << detail.precisionLocation
            << detail.group
            << detail.status
            << detail.track
            << detail.strokeColor
            << detail.fillColor
            << detail.color
            << detail.userIcon
            << "Remarks: " << (detail.remarks.empty() ? "None" : detail.remarks) << "\n"
            << "Links:\n";
        if (detail.links.empty()) 
        {
            os << "\tNone\n";
        }
        else 
        {
            for (const auto& link : detail.links) 
            {
                os << "\t" << link;
            }
        }
        os << "Custom Details:\n";
        if (detail.customDetails.empty()) 
        {
            os << "\tNone\n";
        }
        else 
        {
            for (const auto& custom : detail.customDetails) 
            {
                os << custom;
            }
        }
        os << "\n";
        return os;
    }

private:
    static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
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
        oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
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

    /// @brief Getter for Event sub-schema
    const Event& GetEvent() const { return event; }

    /// @brief Getter for Point sub-schema
    const Point::Data& GetPoint() const { return point; }

    /// @brief Getter for Detail sub-schema
    const Detail& GetDetail() const { return detail; }

    /// @brief Setter for Event sub-schema with validation
    bool SetEvent(const Event& newEvent) 
    {
        if (!newEvent.IsValid()) 
        {
            std::cerr << "Cannot set invalid Event" << std::endl;
            return false;
        }
        event = newEvent;
        return true;
    }

    /// @brief Setter for Point sub-schema with validation
    bool SetPoint(const Point::Data& newPoint) 
    {
        if (!newPoint.IsValid()) 
        {
            std::cerr << "Cannot set invalid Point" << std::endl;
            return false;
        }
        point = newPoint;
        return true;
    }

    /// @brief Setter for Detail sub-schema with validation
    bool SetDetail(const Detail& newDetail) 
    {
        if (newDetail.IsValid() && !newDetail.IsValid()) 
        {
            std::cerr << "Cannot set invalid Detail" << std::endl;
            return false;
        }
        detail = newDetail;
        return true;
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const CoT_Schema& cot) 
    {
        os << "CoT_Schema: ";
        if (!cot.IsValid()) os << " -NOT VALID- ";
        os << "\n"
            << cot.event
            << cot.point
            << cot.detail
            << "\n";
        return os;
    }
};

/// @brief Result structure for cot operations
struct Result
{
    /// @brief Error code indicating the operation outcome
    enum class Code 
    {
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

    Code code;                  // Error code
    std::string description;    // Detailed error message

    /// @brief Check if the operation was successful
    bool IsSuccess() const { return code == Code::Success; }

    /// @brief Constructor for success case
    Result() : code(Code::Success), description("") {}

    /// @brief Constructor for error case
    Result(Code c, std::string desc) : code(c), description(std::move(desc)) {}

    /// @brief Convert to string for debugging or logging
    std::string ToString() const
    {
        std::string result = "Result: ";
        switch (code) 
        {
        case Code::Success: result += "Success"; break;
        case Code::InvalidEvent: result += "InvalidEvent"; break;
        case Code::InvalidPoint: result += "InvalidPoint"; break;
        case Code::InvalidDetail: result += "InvalidDetail"; break;
        case Code::InvalidDate: result += "InvalidDate"; break;
        case Code::InvalidTime: result += "InvalidTime"; break;
        case Code::InvalidHow: result += "InvalidHow"; break;
        case Code::InvalidType: result += "InvalidType"; break;
        case Code::InvalidXml: result += "InvalidXml"; break;
        case Code::InvalidInput: result += "InvalidInput"; break;
        case Code::InvalidTimeSubSchema: result += "InvalidTimeSubSchema"; break;
        case Code::InsufficientData: result += "InsufficientData"; break;
        case Code::ProcessingError: result += "ProcessingError"; break;
        case Code::NoModificationMade: result += "NoModificationMade"; break;
        }

        if (!description.empty()) 
        {
            result += "; Description: " + description;
        }

        return result;
    }
};