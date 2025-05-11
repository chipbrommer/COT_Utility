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
#include <iostream>                     // ostream
#include <iomanip>                      // setw
#include <unordered_map>                // maps
#include <sstream>                      // sstream
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
            << "\tVersion:         " << event.version << "\n"
            << "\tType:            " << event.type << "\n"
            << "\tIndicator:       " << static_cast<int>(event.indicator) << " - " << Point::TypeToString.at(event.indicator) << "\n"
            << "\tLocation:        " << static_cast<int>(event.location) << " - " << Location::TypeToString.at(event.location) << "\n"
            << "\tUID:             " << event.uid << "\n"
            << "\tTime:            " << event.time << "\n"
            << "\tStart:           " << event.start << "\n"
            << "\tStale:           " << event.stale << "\n"
            << "\tHow:             " << event.how << "\n"
            << "\tHow Entry:       " << static_cast<int>(event.howEntry) << " - " << How::Entry::TypeToString.at(event.howEntry) << "\n"
            << "\tHow Data:        " << static_cast<int>(event.howData) << " - " << How::Data::TypeToString.at(event.howData) << "\n"
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
            << "\tVersion:         " << takv.version << "\n"
            << "\tDevice:          " << takv.device << "\n"
            << "\tOS:              " << takv.os << "\n"
            << "\tPlatform:        " << takv.platform << "\n"
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
            << "\tEndpoint:        " << contact.endpoint << "\n"
            << "\tCallsign:        " << contact.callsign << "\n"
            << "\tXMP Username:    " << contact.xmppUsername << "\n"
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
            << "\tDroid:           " << uid.droid << "\n"
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
            << "\tAlt Source:      " << preloc.altsrc << "\n"
            << "\tGeopoint Source: " << preloc.geopointsrc << "\n"
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
            << "\tRole:            " << group.role << "\n"
            << "\tName:            " << group.name << "\n"
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
            << "\tBattery:         " << status.battery << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Track data 
class Track
{
public:
    double course;                  /// Direction of motion with respect to true north
    double speed;                   /// Magnitude of motion measured in meters per second

    /// @brief Constructor - Initializes Everything
    Track(const double course = NAN,
        const double speed = NAN) :
        course(course), speed(speed)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Track& track) const
    {
        return  (course == track.course) &&
            (speed == track.speed);
    }

    /// @brief Equal comparison operator
    bool operator != (const Track& track) const
    {
        return !(*this == track);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return !std::isnan(course) && !std::isnan(speed);
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Track& track)
    {
        os << "Track: ";  if (!track.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << "\tCourse:          " << track.course << "\n"
            << "\tSpeed:           " << track.speed << "\n"
            << "\n";

        return os;
    }
};

/// @brief A COT Message subschema class for Detail data 
class Detail
{
public:
    Takv takv;                              /// TAKV Sub-Schema
    Contact contact;                        /// Contact Sub-Schema
    Uid uid;                                /// UID Sub-Schema
    PrecisionLocation precisionLocation;    /// PrecisionLocation Sub-Schema
    Group group;                            /// Group Sub-Schema
    Status status;                          /// Status Sub-Schema
    Track track;                            /// Track Sub-Schema
    std::string remarks;                    /// Remarks information
    std::string iconSetPath;                /// Icon Set path information

    /// @brief Constructor - Initializes Everything
    Detail(const Takv takv = Takv(),
        const Contact contact = Contact(),
        const Uid uid = Uid(),
        const PrecisionLocation precisionLocation = PrecisionLocation(),
        const Group group = Group(),
        const Status status = Status(),
        const Track track = Track()
    ) :
        takv(takv),
        contact(contact),
        uid(uid),
        precisionLocation(precisionLocation),
        group(group),
        status(status),
        track(track)
    {}

    /// @brief Equal comparison operator
    bool operator == (const Detail& detail) const
    {
        return  (takv == detail.takv) &&
            (contact == detail.contact) &&
            (uid == detail.uid) &&
            (precisionLocation == detail.precisionLocation) &&
            (group == detail.group) &&
            (status == detail.status) &&
            (track == detail.track) && 
            (remarks == detail.remarks) &&
            (iconSetPath == detail.iconSetPath);
    }

    /// @brief Equal comparison operator
    bool operator != (const Detail& detail) const
    {
        return !(*this == detail);
    }

    /// @brief Does class have valid data ? 
    bool IsValid() const
    {
        return (takv.IsValid() &&
            contact.IsValid() &&
            uid.IsValid() &&
            precisionLocation.IsValid() &&
            group.IsValid() &&
            status.IsValid() &&
            track.IsValid());
    }

    /// @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Detail& detail)
    {
        /*
        *  NOTE: This will print only the valid sub-schemas.

            os << "Detail:\n";
            if (detail.takv.IsValid()) { os << detail.takv; }
            if (detail.contact.IsValid()) { os << detail.contact; }
            if (detail.uid.IsValid()) { os << detail.uid; }
            if (detail.precisionLocation.IsValid()) { os << detail.precisionLocation; }
            if (detail.group.IsValid()) { os << detail.group; }
            if (detail.status.IsValid()) { os << detail.status; }
            if (detail.track.IsValid()) { os << detail.track; }
            os << "\n";

        */

        os << "Detail: ";  if (!detail.IsValid()) { os << " -NOT VALID- "; }
        os << "\n"
            << detail.takv
            << detail.contact
            << detail.uid
            << detail.precisionLocation
            << detail.group
            << detail.status
            << detail.track
            << detail.remarks
            << detail.iconSetPath
            << "\n";

        return os;
    }
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
    ProcessingError,         // Generic processing failure (e.g., internal logic error)
    NoModificationMade,
};

