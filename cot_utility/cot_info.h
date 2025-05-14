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
#include <cmath>                        // isnan
#include <iomanip>                      // setw
#include <limits>                     
#include <map>
#include <pugixml.hpp>                  // pugi xml
#include <sstream>                      // sstream
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

    static const std::unordered_map<Type, std::string> TypeToString
    {
        {Type::a, "Atoms"},
        {Type::b, "Bits"},
        {Type::t, "Tasking"},
        {Type::r, "Reply"},
        {Type::c, "Capability"},
        {Type::res, "Reservation"},
        {Type::Error, "Error"}
    };
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

    static const std::unordered_map<Type, std::string> TypeToString
    {
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

    class Data 
    {
    public:
        double latitude;
        double longitude;
        double hae;
        double circularError;
        double linearError;

        Data(double latitude = NAN, double longitude = NAN, double hae = NAN,
            double circularError = NAN, double linearError = NAN)
            : latitude(latitude), longitude(longitude), hae(hae),
            circularError(circularError), linearError(linearError) {}

        bool IsValid() const 
        {
            return !std::isnan(latitude) && !std::isnan(longitude) &&
                !std::isnan(hae) && !std::isnan(circularError) && !std::isnan(linearError);
        }

        bool operator==(const Data& other) const 
        {
            return latitude == other.latitude &&
                longitude == other.longitude &&
                hae == other.hae &&
                circularError == other.circularError &&
                linearError == other.linearError;
        }

        bool operator!=(const Data& other) const 
        {
            return !(*this == other);
        }

        friend std::ostream& operator<<(std::ostream& os, const Data& point) 
        {
            os << "Point: ";
            if (!point.IsValid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\tLatitude:        " << point.latitude << "\n"
                << "\tLongitude:       " << point.longitude << "\n"
                << "\tHAE:             " << point.hae << "\n"
                << "\tCircular Error:  " << point.circularError << "\n"
                << "\tLinear Error:    " << point.linearError << "\n"
                << "\n";
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

        static const std::unordered_map<Type, std::string> TypeToString
        {
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

    bool IsValid() const 
    {
        return year > 0 && month > 0 && day > 0;
    }

    bool operator==(const Date& other) const 
    {
        return year == other.year && month == other.month && day == other.day;
    }

    bool operator!=(const Date& other) const 
    {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Date& date) 
    {
        os << std::setfill('0') << std::setw(4) << date.year << "-"
            << std::setfill('0') << std::setw(2) << date.month << "-"
            << std::setfill('0') << std::setw(2) << date.day;
        return os;
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

    bool IsValid() const 
    {
        return second >= 0 && second < 60.0;
    }

    bool operator==(const Time& other) const 
    {
        return hour == other.hour && minute == other.minute && second == other.second;
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

    bool IsValid() const 
    {
        return Date::IsValid() && Time::IsValid();
    }

    std::string ToCOTTimestamp() const 
    {
        std::stringstream timestamp;
        timestamp << std::setfill('0') << std::setw(4) << year << "-"
            << std::setfill('0') << std::setw(2) << month << "-"
            << std::setfill('0') << std::setw(2) << day << "T"
            << std::setfill('0') << std::setw(2) << hour << ":"
            << std::setfill('0') << std::setw(2) << minute << ":"
            << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << second << "Z";
        return timestamp.str();
    }

    bool operator==(const DateTime& other) const 
    {
        return static_cast<const Date&>(*this) == static_cast<const Date&>(other) &&
            static_cast<const Time&>(*this) == static_cast<const Time&>(other);
    }

    bool operator!=(const DateTime& other) const 
    {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const DateTime& dt) 
    {
        os << std::setfill('0') << std::setw(4) << dt.year << "-"
            << std::setfill('0') << std::setw(2) << dt.month << "-"
            << std::setfill('0') << std::setw(2) << dt.day << " "
            << std::setfill('0') << std::setw(2) << dt.hour << ":"
            << std::setfill('0') << std::setw(2) << dt.minute << ":"
            << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << dt.second;
        return os;
    }
};

/// @brief A COT Message subschema class for Event data 
class Event
{
public:
    double              version;           /// Decimal Schema version of this event instance
    std::string         type;              /// String Hierarchically organized hint about his event
    Point::Type         indicator;         /// Type enum of the indicator type from the parse 'type' string
    Location::Type      location;          /// Type enum of the indicator type from the parse 'location' string
    std::string         uid;               /// Globally unique name for this information on this event
    DateTime            time;              /// Time stamp: when the event was generated
    DateTime            start;             /// Starting time when an event should be considered valid
    DateTime            stale;             /// Ending time when an event  should no longer be considered valid
    std::string         how;               /// Gives a hint about how the coordinates were generated
    How::Entry::Type    howEntry;          /// Grab how the entry point was created from how string.
    How::Data::Type     howData;           /// Grab the data for the entry type from how string. 

    /// @brief Constructor - Initializes Everything
    Event(const double          version = 0,
        const std::string       type = "",
        const Point::Type       indicator = Point::Type::Error,
        const Location::Type    location = Location::Type::Error,
        const std::string       uid = "",
        const DateTime          time = DateTime(),
        const DateTime          start = DateTime(),
        const DateTime          stale = DateTime(),
        const std::string       how = "",
        const How::Entry::Type    howEntry = How::Entry::Type::Error,
        const How::Data::Type     howData = How::Data::Type::Error) :
        version(version), type(type), indicator(indicator), location(location),
        uid(uid), time(time), start(start), stale(stale), how(how),
        howEntry(howEntry), howData(howData)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Event& event) const
    {
        return  (version == event.version) &&
                (type == event.type) &&
                (indicator == event.indicator) &&
                (location == event.location) &&
                (uid == event.uid) &&
                (time == event.time) &&
                (start == event.start) &&
                (stale == event.stale) &&
                (how == event.how) &&
                (howEntry == event.howEntry) &&
                (howData == event.howData);
    }

    /// @brief Equal comparison operator
    bool operator != (const Event& event) const
    {
        return !(*this == event);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return  (version > 0) && (!type.empty()) && (!uid.empty()) && (time.IsValid()) &&
            (start.IsValid()) && (stale.IsValid()) && (!how.empty());
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Event& event)
    {
        os << "Event: ";  if (!event.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tVersion:\t" << event.version << "\n"
            << "\tType:\t" << event.type << "\n"
            << "\tIndicator:\t" << static_cast<int>(event.indicator) << " - " << Point::TypeToString.at(event.indicator) << "\n"
            << "\tLocation:\t" << static_cast<int>(event.location) << " - " << Location::TypeToString.at(event.location) << "\n"
            << "\tUID:\t" << event.uid << "\n"
            << "\tTime:\t" << event.time << "\n"
            << "\tStart:\t" << event.start << "\n"
            << "\tStale:\t" << event.stale << "\n"
            << "\tHow:\t" << event.how << "\n"
            << "\tHow Entry:\t" << static_cast<int>(event.howEntry) << " - " << How::Entry::TypeToString.at(event.howEntry) << "\n"
            << "\tHow Data:\t" << static_cast<int>(event.howData) << " - " << How::Data::TypeToString.at(event.howData) << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Takv data 
class Takv
{
public:
    std::string version;            /// version
    std::string device;             /// Device type
    std::string os;                 /// Operating system
    std::string platform;           /// TAK platform (ATAK-CIV, WINTAK, ATAK-MIL, etc)

    /// @brief Constructor - Initializes Everything
    Takv(const std::string version = "",
        const std::string device = "",
        const std::string os = "",
        const std::string platform = "") :
        version(version), device(device), os(os), platform(platform)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Takv& takv) const
    {
        return  (version == takv.version) &&
            (device == takv.device) &&
            (os == takv.os) &&
            (platform == takv.platform);
    }

    /// @brief Equal comparison operator
    bool operator != (const Takv& takv) const
    {
        return !(*this == takv);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !version.empty() || !device.empty() || !os.empty() || !platform.empty();
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Takv& takv)
    {
        os << "TAKV: ";  if (!takv.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tVersion:\t" << takv.version << "\n"
            << "\tDevice:\t" << takv.device << "\n"
            << "\tOS:\t" << takv.os << "\n"
            << "\tPlatform:\t" << takv.platform << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Contact data 
class Contact
{
public:
    std::string endpoint;           /// Endpoint of the unit. usually a tcp address. 
    std::string callsign;           /// Callsign (name) of the item.
    std::string xmppUsername;       /// XMP Username

    /// @brief Constructor - Initializes Everything
    Contact(const std::string endpoint = "",
        const std::string callsign = "",
        const std::string xmppUsername = "") :
        endpoint(endpoint), callsign(callsign), xmppUsername(xmppUsername)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Contact& contact) const
    {
        return  (endpoint == contact.endpoint) &&
            (callsign == contact.callsign) &&
            (xmppUsername == contact.xmppUsername);
    }

    /// @brief Equal comparison operator
    bool operator != (const Contact& contact) const
    {
        return !(*this == contact);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !endpoint.empty() || !callsign.empty() || !xmppUsername.empty();
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Contact& contact)
    {
        os << "Contact: ";  if (!contact.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tEndpoint:\t" << contact.endpoint << "\n"
            << "\tCallsign:\t" << contact.callsign << "\n"
            << "\tXMP Username:\t" << contact.xmppUsername << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Uid data 
class Uid
{
public:
    std::string droid;               /// User

    /// @brief Constructor - Initializes Everything
    Uid(const std::string droid = "") :
        droid(droid)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Uid& uid) const
    {
        return  (droid == uid.droid);
    }

    /// @brief Equal comparison operator
    bool operator != (const Uid& uid) const
    {
        return !(*this == uid);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !droid.empty();
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Uid& uid)
    {
        os << "UID: ";  if (!uid.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tDroid:\t" << uid.droid << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for PrecisionLocation data 
class PrecisionLocation
{
public:
    std::string altsrc;                 /// Role of the group member sending
    std::string geopointsrc;            /// Group name

    /// @brief Constructor - Initializes Everything
    PrecisionLocation(const std::string altsrc = "",
        const std::string geopointsrc = "") :
        altsrc(altsrc), geopointsrc(geopointsrc)
    {}

    /// @brief Equal comparison operator
    bool operator == (const PrecisionLocation& preloc) const
    {
        return  (altsrc == preloc.altsrc) &&
            (geopointsrc == preloc.geopointsrc);
    }

    /// @brief Equal comparison operator
    bool operator != (const PrecisionLocation& preloc) const
    {
        return !(*this == preloc);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !altsrc.empty() || !geopointsrc.empty();
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const PrecisionLocation& preloc)
    {
        os << "Precision Location: ";  if (!preloc.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tAlt Source:\t" << preloc.altsrc << "\n"
            << "\tGeopoint Source:\t" << preloc.geopointsrc << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Group data 
class Group
{
public:
    std::string role;               /// Role of the group member sending
    std::string name;               /// Group name

    /// @brief Constructor - Initializes Everything
    Group(const std::string role = "",
        const std::string name = "") :
        role(role), name(name)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Group& group) const
    {
        return  (role == group.role) &&
            (name == group.name);
    }

    /// @brief Equal comparison operator
    bool operator != (const Group& group) const
    {
        return !(*this == group);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !role.empty() || !name.empty();
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Group& group)
    {
        os << "Group: ";  if (!group.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tRole:\t" << group.role << "\n"
            << "\tName:\t" << group.name << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Status data 
class Status
{
public:
    double battery;           /// Battery percentage

    /// @brief Constructor - Initializes Everything
    Status(const double battery = NAN) :
        battery(battery)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Status& status) const
    {
        return  (battery == status.battery);
    }

    /// @brief Equal comparison operator
    bool operator != (const Status& status) const
    {
        return !(*this == status);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !std::isnan(battery);
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Status& status)
    {
        os << "Status: ";  if (!status.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tBattery:\n" << status.battery << "\n"
            << "\n";

        return os;
    }
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
        // Empty path is not valid
        return !iconSetPath.empty();
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
        // Empty path is not valid
        return !value.empty();
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
    bool isValid() const 
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
        if (!detail.isValid()) { os << " -NOT VALID-"; }
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
    Takv takv;                              /// TAKV Sub-Schema
    Contact contact;                        /// Contact Sub-Schema
    Uid uid;                                /// UID Sub-Schema
    Model model;                            /// Model Sub-Schema
    PrecisionLocation precisionLocation;    /// PrecisionLocation Sub-Schema
    Group group;                            /// Group Sub-Schema
    Status status;                          /// Status Sub-Schema
    Track track;                            /// Track Sub-Schema
    StrokeColor strokeColor;                /// StrokeColor Sub-Schema
    FillColor fillColor;                    /// FillColor Sub-Schema
    Color color;                            /// Color Sub-Schema
    UserIcon userIcon;                      /// UserIcon Sub-Schema
    std::string remarks;                    /// Remarks information
    std::vector<Link> links;                /// Links Sub-Schema
    std::vector<CustomDetail> customDetails; /// Custom detail elements

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
    {}

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
    bool operator!=(const Detail& other) const {
        return !(*this == other);
    }

    /// @brief Checks if the class has valid data
    bool IsValid(std::string* errorMsg = nullptr) const {
        // Only track is required; others are optional
        if (!track.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Track is invalid";
            }
            return false;
        }
        // Optional sub-schemas: validate if they have data
        if (takv.IsValid() && !takv.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Takv is invalid";
            }
            return false;
        }
        if (contact.IsValid() && !contact.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Contact is invalid";
            }
            return false;
        }
        if (uid.IsValid() && !uid.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Uid is invalid";
            }
            return false;
        }
        if (model.IsValid() && !model.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Model is invalid";
            }
            return false;
        }
        if (precisionLocation.IsValid() && !precisionLocation.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "PrecisionLocation is invalid";
            }
            return false;
        }
        if (group.IsValid() && !group.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Group is invalid";
            }
            return false;
        }
        if (status.IsValid() && !status.IsValid(errorMsg)) {
            if (errorMsg && errorMsg->empty()) {
                *errorMsg = "Status is invalid";
            }
            return false;
        }
        // Custom details: validate if present
        for (const auto& custom : customDetails) {
            if (!custom.isValid()) {
                if (errorMsg) {
                    *errorMsg = "Custom detail '" + custom.name + "' is invalid";
                }
                return false;
            }
        }
        return true;
    }

    /// @brief Serialize to XML string
    std::string ToXml() const {
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
    static Detail FromXml(const pugi::xml_node& node) {
        Detail detail;
        pugi::xml_attribute attr;

        // Parse known sub-schemas
        for (pugi::xml_node child : node.children()) 
        {
            std::string name = child.name();
            if (name == "takv") 
            {
                // Placeholder: use constructor until Takv::fromXml exists
                if (attr = child.attribute("version")) detail.takv.version = attr.as_string();
                if (attr = child.attribute("platform")) detail.takv.platform = attr.as_string();
                if (attr = child.attribute("os")) detail.takv.os = attr.as_string();
                if (attr = child.attribute("device")) detail.takv.device = attr.as_string();
            }
            else if (name == "contact") 
            {
                if (attr = child.attribute("endpoint")) detail.contact.endpoint = attr.as_string();
                if (attr = child.attribute("callsign")) detail.contact.callsign = attr.as_string();
                if (attr = child.attribute("xmppUsername")) detail.contact.xmppUsername = attr.as_string();
            }
            else if (name == "uid") 
            {
                if (attr = child.attribute("Droid")) detail.uid.droid = attr.as_string();
            }
            else if (name == "model") 
            {
                if (attr = child.attribute("value")) detail.model.value = attr.as_string();
            }
            else if (name == "precisionlocation") 
            {
                if (attr = child.attribute("altsrc")) detail.precisionLocation.altsrc = attr.as_string();
                if (attr = child.attribute("geopointsrc")) detail.precisionLocation.geopointsrc = attr.as_string();
            }
            else if (name == "__group") 
            {
                if (attr = child.attribute("role")) detail.group.role = attr.as_string();
                if (attr = child.attribute("name")) detail.group.name = attr.as_string();
            }
            else if (name == "status") 
            {
                if (attr = child.attribute("battery")) detail.status.battery = attr.as_double();
            }
            else if (name == "track") 
            {
                detail.track = Track::FromXml(child);
            }
            else if (name == "strokeColor") 
            {
                if (attr = child.attribute("value")) detail.strokeColor.value = attr.as_int();
            }
            else if (name == "fillColor") 
            {
                if (attr = child.attribute("value")) detail.fillColor.value = attr.as_int();
            }
            else if (name == "color") 
            {
                if (attr = child.attribute("argb")) detail.color.argb = attr.as_int();
            }
            else if (name == "usericon") 
            {
                if (attr = child.attribute("iconsetpath")) detail.userIcon.iconSetPath = attr.as_string();
            }
            else if (name == "remarks") 
            {
                detail.remarks = child.child_value();
            }
            else if (name == "link") 
            {
                Link link;
                if (attr = child.attribute("uid")) link.uid = attr.as_string();
                if (attr = child.attribute("remarks")) link.remarks = attr.as_string();
                if (attr = child.attribute("relation")) link.relation = attr.as_string();
                if (attr = child.attribute("callsign")) link.callsign = attr.as_string();
                if (attr = child.attribute("type")) link.type = attr.as_string();
                if (attr = child.attribute("point")) link.point = attr.as_string();
                auto [latitude, longitude] = link.GetLatLonFromPoint();
                link.latitude = latitude;
                link.longitude = longitude;
                detail.links.push_back(link);
            }
            else 
            {
                // Store unknown nodes as custom details
                detail.customDetails.push_back(CustomDetail::fromXml(child));
            }
        }

        return detail;
    }

    /// @brief Add a new custom detail
    /// @param name XML tag name (required, non-empty)
    /// @param attributes XML attributes as key-value pairs (optional)
    /// @param content Inner text or CDATA (optional)
    /// @return true if added successfully, false if invalid (e.g., empty name)
    bool AddCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& attributes = {},
        const std::string& content = "") 
    {
        if (name.empty()) 
        {
            return false; // Invalid tag name
        }
        customDetails.emplace_back(name, attributes, content);
        return true;
    }

    /// @brief Remove a custom detail by name, with optional attributes and content matching
    /// @param name XML tag name to match (required, non-empty)
    /// @param attributes Optional attributes to match exactly (empty means match any)
    /// @param content Optional content to match exactly (empty means match any)
    /// @return true if a matching custom detail was removed, false if not found or invalid
    bool removeCustomDetail(const std::string& name,
        const std::map<std::string, std::string>& attributes = {},
        const std::string& content = "") 
    {
        if (name.empty()) 
        {
            return false; // Invalid tag name
        }

        auto it = std::find_if(customDetails.begin(), customDetails.end(),
            [&](const CustomDetail& detail) {
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
            for (const auto& link : detail.links) {

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

/// @brief A Root COT Message schema class for entire message data. 
class CoT_Schema
{
public:
    Event event;                    /// Holds Event Sub-schema
    Point::Data point;              /// Holds Point Sub-schema
    Detail detail;                  /// Holds Detail Sub-schema

    /// @brief Constructor - Initializes Everything
    CoT_Schema(const Event& event = Event(),
        const Point::Data& point = Point::Data(),
        const Detail& detail = Detail()) :
        event(event), point(point), detail(detail)
    {}

    /// @brief Equal comparison operator
    bool operator == (const CoT_Schema& cot) const
    {
        return  (event == cot.event) &&
            (point == cot.point) &&
            (detail == cot.detail);
    }

    /// @brief Equal comparison operator
    bool operator != (const CoT_Schema& cot) const
    {
        return !(*this == cot);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return  (event.IsValid()) || (point.IsValid()) || (detail.IsValid());
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const CoT_Schema& cot)
    {
        os << "COT Data: ";  if (!cot.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << cot.event
            << cot.point
            << cot.detail
            << "\n";

        return os;
    }
};

/// @brief Error codes for parsing operations
enum class CoT_UtilityResult : int
{
    Success,                // No error
    InvalidEvent,           // XML is Missing Event tag
    InvalidPoint,           // XML is Missing Point tag
    InvalidDate,
    InvalidTime,
    InvalidHow,
    InvalidType,
    InvalidXml,             // String is not valid XML
    InvalidInput,           // Input string is empty or malformed
    InvalidTimeSubSchema,
    InsufficientData,       
    ProcessingError,         // Generic processing failure (e.g., internal logic error)
    NoModificationMade,
};

