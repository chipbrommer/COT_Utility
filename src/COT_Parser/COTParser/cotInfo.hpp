///////////////////////////////////////////////////////////////////////////////
//!
//! @file		cotInfo.hpp
//! 
//! @brief		An information header file for COT structures and enums. 
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
#include <iostream>                     // out stream friend functions
#include <iomanip>                      // For manipulating output with setw
//
///////////////////////////////////////////////////////////////////////////////

//! @brief Enum for COT Root Type.
enum RootType
{
    ROOT_a = 0,
    ROOT_b,
    ROOT_t,
    ROOT_r,
    ROOT_c,
    ROOT_res,
    ROOT_ERROR,
};

//! @brief Translate 'RootType' Enum to string. 
static const char* enumRootTypeToString[] = {
    "Atoms",
    "Bits",
    "Tasking",
    "Reply",
    "Capability",
    "Reservation",
    "ERROR",
};

//! @brief Enum for COT Point Type.
enum PointType
{
    POINT_p = 0,
    POINT_u,
    POINT_a,
    POINT_f,
    POINT_n,
    POINT_s,
    POINT_h,
    POINT_j,
    POINT_k,
    POINT_o,
    POINT_x,
    POINT_ERROR,
};

//! @brief Translate 'PointType' Enum to string. 
static const char* enumPointTypeToString[] = {
    "Pending",
    "Unknown",
    "Assumed Friend",
    "Friend",
    "Neutral",
    "Suspect",
    "Hostile",
    "Joker",
    "Faker",
    "None Specified",
    "Other",
    "ERROR",
};

//! @brief Enum for COT Locations Types.
enum LocationType
{
    LOCATION_P = 0,
    LOCATION_A,
    LOCATION_G,
    LOCATION_S,
    LOCATION_U,
    LOCATION_X,
    LOCATION_ERROR,
};

//! @brief Translate 'LocationType' Enum to string. 
static const char* enumLocationTypeToString[] = {
    "Space",
    "Air",
    "Ground",
    "Sea Surface",
    "Sea Subsurface",
    "Other",
    "ERROR",
};

//! @brief Enum for How Entry Point was generated.
enum HowEntryType
{
    ENTRY_h = 0,
    ENTRY_m,
    ENTRY_ERROR,
};

//! @brief Translate 'HowEntryType' Enum to string.
static const char* enumHowEntryTypeToString[] = {
    "Human",
    "Machine",
    "ERROR"
};

//! @brief Enum for How Human supplied data was generated.
enum HowDataType
{
    DATA_e = 0,
    DATA_cal,
    DATA_t,
    DATA_paste,
    DATA_i,
    DATA_g,
    DATA_m,
    DATA_s,
    DATA_f,
    DATA_con,
    DATA_pred,
    DATA_r,
    DATA_ERROR,
};

//! @brief Translate 'HowMachineDataType' Enum to string.
static const char* enumHowDataTypeToString[] = {
    "Estimated",
    "Calculated",
    "Transcribed",
    "Cut and Paste",
    "Mensurated",
    "Derived From GPS",
    "Magnetic",
    "Simulated",
    "Fused",
    "Configured",
    "Predicted",
    "Relayed",
    "ERROR"
};

//! @brief Define Date Class
class Date
{
public:
    unsigned year;
    unsigned month;
    unsigned day;

    // constructor initializes everything
    Date(const unsigned year = 0,
        const unsigned month = 0,
        const unsigned day = 0) :
        year(year), month(month), day(day)
    {}

    //! @brief Does struct have valid data ? 
    bool Valid(void) const
    {
        return (year > 0) && (month > 0) && (day > 0);
    }

    // Standard format output yyyy-mm-dd
    friend std::ostream& operator<<(std::ostream& os, const Date& date)
    {
        os << std::setfill('0') << std::setw(4) << date.year << "-"
            << std::setfill('0') << std::setw(2) << date.month << "-"
            << std::setfill('0') << std::setw(2) << date.day;

        return os;
    }
};

//! @brief Define Time Class
class Time
{
public:
    unsigned hour;
    unsigned minute;
    double   second;

    // Constructor
    Time(const unsigned hour = 0,
        const unsigned minute = 0,
        const double second = NAN) :
        hour(hour), minute(minute), second(second)
    {}

    bool Valid(void) const
    {
        return !isnan(second);
    }

    // standard time format hh:mm:ss.ss
    friend std::ostream& operator<<(std::ostream& os, const Time& time)
    {
        os << std::setfill('0') << std::setw(2) << time.hour << ":"
            << std::setfill('0') << std::setw(2) << time.minute << ":"
            << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << time.second;

        return os;
    }
};

//! @brief Define a Derived Date and Time Class 
struct DateTime : public Date, Time
{
    //! @brief Constructor - Initializes with data
    DateTime(const unsigned year = 0,
        const unsigned month = 0,
        const unsigned day = 0,
        const unsigned hour = 0,
        const unsigned minute = 0,
        const double second = NAN) :
        Date(year, month, day),
        Time(hour, minute, second)
    {}

    //! @brief Constructor - Initializes with instances. 
    DateTime(const Date& date,
        const Time& time) :
        Date(date),
        Time(time)
    {}

    //! @brief Date setter
    void SetDate(const unsigned year,
        const unsigned month,
        const unsigned day)
    {
        this->year = year;
        this->month = month;
        this->day = day;
    }

    //! @brief Time setter
    void SetTime(const unsigned hour,
        const unsigned minute,
        const double second)
    {
        this->hour = hour;
        this->minute = minute;
        this->second = second;
    }

    //! @brief Equal comparison operator
    bool operator==(const DateTime& dt) const
    {
        return  (year == dt.year) &&
            (month == dt.month) &&
            (day == dt.day) &&
            (hour == dt.hour) &&
            (minute == dt.minute) &&
            (second == dt.second);
    }

    //! @brief Equal comparison operator
    bool operator!=(const DateTime& dt) const
    {
        return !(*this == dt);
    }

    //! @brief Instance Valid ? 
    bool Valid(void) const
    {
        return !isnan(second) && (second < 60.0) &&
            (year != 0) && (month > 0) && (day > 0);
    }

    //! @brief Time Valid ? 
    bool TimeValid(void) const
    {
        return !isnan(second);
    }

    //! @brief Print Date and Time.
    //! Standard format yyyy-mm-dd hh:mm:ss.ss
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

//! @brief Define a COT Message subschema class for Point data 
class Point
{
public:
    double  latitude;               //! Latitude in Meters
    double  longitutde;             //! Longitude in Meters
    double  hae;                    //! Height Above Ellipsoid in Meters
    double  circularError;          //! Circular Error in Meters
    double  linearError;            //! Linear Error in Meters

    //! @brief Constructor - Initializes Everything
    Point(const double latitude = NAN,
        const double longitutde = NAN,
        const double hae = NAN,
        const double circularError = NAN,
        const double linearError = NAN) :
        latitude(latitude), longitutde(longitutde), hae(hae),
        circularError(circularError), linearError(linearError)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Point& point) const
    {
        return  (latitude == point.latitude) &&
            (longitutde == point.longitutde) &&
            (hae == point.hae) &&
            (circularError == point.circularError) &&
            (linearError == point.linearError);
    }

    //! @brief Equal comparison operator
    bool operator != (const Point& point) const
    {
        return !(*this == point);
    }

    //! @brief Does class have valid latitude and longitude ? 
    bool Valid(void) const
    {
        return  (!isnan(latitude)) && (!isnan(longitutde)) && (!isnan(hae)) &&
                (!isnan(circularError)) && (!isnan(linearError));
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Point& point)
    {
        os << "Point:\n"
            << "\tLatitude:        " << point.latitude << "\n"
            << "\tLongitutde:      " << point.longitutde << "\n"
            << "\tHAE:             " << point.hae << "\n"
            << "\tCircular Error:  " << point.circularError << "\n"
            << "\tLinear Error:    " << point.linearError << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Event data 
class Event
{
public:
    double       version;           //! Decimal Schema version of this event instance
    std::string  type;              //! String Hierarchically organized hint about his event
    PointType    indicator;         //! Type enum of the indicator type from the parse 'type' string
    LocationType location;          //! Type enum of the indicator type from the parse 'location' string
    std::string  uid;               //! Globally unique name for this information on this event
    DateTime     time;              //! Time stamp: when the event was generated
    DateTime     start;             //! Starting time when an event should be considered valid
    DateTime     stale;             //! Ending time when an event  should no longer be considered valid
    std::string  how;               //! Gives a hint about how the coordinates were generated
    HowEntryType howEntry;          //! Grab how the entry point was created from how string.
    HowDataType  howData;           //! Grab the data for the entry type from how string. 

    //! @brief Constructor - Initializes Everything
    Event(const double        version = 0,
        const std::string   type = "",
        const PointType     indicator = PointType::POINT_ERROR,
        const LocationType  location = LocationType::LOCATION_ERROR,
        const std::string   uid = "",
        const DateTime      time = DateTime(),
        const DateTime      start = DateTime(),
        const DateTime      stale = DateTime(),
        const std::string   how = "",
        const HowEntryType  howEntry = HowEntryType::ENTRY_ERROR,
        const HowDataType   howData = HowDataType::DATA_ERROR) :
        version(version), type(type), indicator(indicator), location(location),
        uid(uid), time(time), start(start), stale(stale), how(how),
        howEntry(howEntry), howData(howData)
    {}

    //! @brief Equal comparison operator
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

    //! @brief Equal comparison operator
    bool operator != (const Event& event) const
    {
        return !(*this == event);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return  (version > 0) && (!type.empty()) && (!uid.empty()) && (time.Valid()) &&
            (start.Valid()) && (stale.Valid()) && (!how.empty());
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Event& event)
    {
        os << "Event:\n"
            << "\tVersion:         " << event.version << "\n"
            << "\tType:            " << event.type << "\n"
            << "\tIndicator:       " << event.indicator << " - " << enumPointTypeToString[event.indicator] << "\n"
            << "\tLocation:        " << event.location << " - " << enumLocationTypeToString[event.location] << "\n"
            << "\tUID:             " << event.uid << "\n"
            << "\tTime:            " << event.time << "\n"
            << "\tStart:           " << event.start << "\n"
            << "\tStale:           " << event.stale << "\n"
            << "\tHow:             " << event.how << "\n"
            << "\tHow Entry:       " << event.howEntry << " - " << enumHowEntryTypeToString[event.howEntry] << "\n"
            << "\tHow Data:        " << event.howData << " - " << enumHowDataTypeToString[event.howData] << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Takv data 
class Takv
{
public:
    std::string version;            //! Role of the group member sending
    std::string device;             //! Device type
    std::string os;                 //! Operating system
    std::string platform;           //! TAK platform (ATAK-CIV, WINTAK, ATAK-MIL, etc)

    //! @brief Constructor - Initializes Everything
    Takv(   const std::string version = "",
            const std::string device = "",
            const std::string os = "",
            const std::string platform = "") :
        version(version), device(device), os(os), platform(platform)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Takv& takv) const
    {
        return  (version == takv.version) &&
                (device == takv.device)&&
                (os == takv.os)&&
                (platform == takv.platform);
    }

    //! @brief Equal comparison operator
    bool operator != (const Takv& takv) const
    {
        return !(*this == takv);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !version.empty() || !device.empty() || !os.empty() || !platform.empty();
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Takv& takv)
    {
        os << "TAKV:\n"
            << "\tVersion:         " << takv.version << "\n"
            << "\tDevice:          " << takv.device << "\n"
            << "\tOS:              " << takv.os << "\n"
            << "\tPlatform:        " << takv.platform << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Contact data 
class Contact
{
public:
    std::string endpoint;           //! Endpoint of the unit. usually a tcp address. 
    std::string callsign;           //! Callsign (name) of the item.
    std::string xmppUsername;       //! XMP Username

    //! @brief Constructor - Initializes Everything
    Contact(const std::string endpoint = "",
            const std::string callsign = "",
            const std::string xmppUsername = "") :
            endpoint(endpoint), callsign(callsign), xmppUsername(xmppUsername)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Contact& contact) const
    {
        return  (endpoint == contact.endpoint) &&
                (callsign == contact.callsign) &&
                (xmppUsername == contact.xmppUsername);
    }

    //! @brief Equal comparison operator
    bool operator != (const Contact& contact) const
    {
        return !(*this == contact);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !endpoint.empty() || !callsign.empty() || !xmppUsername.empty();
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Contact& contact)
    {
        os << "Contact:\n"
            << "\tEndpoint:        " << contact.endpoint << "\n"
            << "\tCallsign:        " << contact.callsign << "\n"
            << "\tXMP Username:    " << contact.xmppUsername << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Uid data 
class Uid
{
public:
    std::string droid;               //! User

    //! @brief Constructor - Initializes Everything
    Uid(const std::string droid = "") :
        droid(droid)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Uid& uid) const
    {
        return  (droid == uid.droid);
    }

    //! @brief Equal comparison operator
    bool operator != (const Uid& uid) const
    {
        return !(*this == uid);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !droid.empty();
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Uid& uid)
    {
        os << "UID:\n"
            << "\tDroid:           " << uid.droid << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for PrecisionLocation data 
class PrecisionLocation
{
public:
    std::string altsrc;                 //! Role of the group member sending
    std::string geopointsrc;            //! Group name

    //! @brief Constructor - Initializes Everything
    PrecisionLocation(  const std::string altsrc = "",
                        const std::string geopointsrc = "") :
                        altsrc(altsrc), geopointsrc(geopointsrc)
    {}

    //! @brief Equal comparison operator
    bool operator == (const PrecisionLocation& preloc) const
    {
        return  (altsrc == preloc.altsrc) &&
                (geopointsrc == preloc.geopointsrc);
    }

    //! @brief Equal comparison operator
    bool operator != (const PrecisionLocation& preloc) const
    {
        return !(*this == preloc);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !altsrc.empty() || !geopointsrc.empty();
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const PrecisionLocation& preloc)
    {
        os << "Precision Location:\n"
            << "\tAlt Source:      " << preloc.altsrc << "\n"
            << "\tGeopoint Source: " << preloc.geopointsrc << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Group data 
class Group
{
public:
    std::string role;               //! Role of the group member sending
    std::string name;               //! Group name

    //! @brief Constructor - Initializes Everything
    Group(const std::string role = "",
          const std::string name = "") :
            role(role), name(name)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Group& group) const
    {
        return  (role == group.role) &&
                (name == group.name);
    }

    //! @brief Equal comparison operator
    bool operator != (const Group& group) const
    {
        return !(*this == group);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !role.empty() || !name.empty();
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Group& group)
    {
        os << "Group:\n"
            << "\tRole:            " << group.role << "\n"
            << "\tName:            " << group.name << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Status data 
class Status
{
public:
    double battery;           //! Battery percentage

    //! @brief Constructor - Initializes Everything
    Status(const double battery = NAN) :
        battery(battery)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Status& status) const
    {
        return  (battery == status.battery);
    }

    //! @brief Equal comparison operator
    bool operator != (const Status& status) const
    {
        return !(*this == status);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !isnan(battery);
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Status& status)
    {
         os << "Status:\n"
            << "\tBattery:         " << status.battery << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Track data 
class Track
{
public:
    double course;                  //! Direction of motion with respect to true north
    double speed;                   //! Magnitude of motion measured in meters per second

    //! @brief Constructor - Initializes Everything
    Track(  const double course = NAN,
            const double speed = NAN) :
            course(course), speed(speed)
    {}

    //! @brief Equal comparison operator
    bool operator == (const Track& track) const
    {
        return  (course == track.course) &&
                (speed == track.speed);
    }

    //! @brief Equal comparison operator
    bool operator != (const Track& track) const
    {
        return !(*this == track);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return !isnan(course) && !isnan(speed);
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Track& track)
    {
        os << "Track:\n"
            << "\tCourse:          " << track.course << "\n"
            << "\tSpeed:           " << track.speed << "\n"
            << "\n";

        return os;
    }
};

//! @brief Define a COT Message subschema class for Detail data 
class Detail
{
public:
    Takv takv;                              //! TAKV Sub-Schema
    Contact contact;                        //! Contact Sub-Schema
    Uid uid;                                //! UID Sub-Schema
    PrecisionLocation precisionLocation;    //! PrecisionLocation Sub-Schema
    Group group;                            //! Group Sub-Schema
    Status status;                          //! Status Sub-Schema
    Track track;                            //! Track Sub-Schema

    //! @brief Constructor - Initializes Everything
    Detail( const Takv takv = Takv(),
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

    //! @brief Equal comparison operator
    bool operator == (const Detail& detail) const
    {
        return  (takv == detail.takv) &&
                (contact == detail.contact) &&
                (uid == detail.uid) &&
                (precisionLocation == detail.precisionLocation) &&
                (group == detail.group) &&
                (status == detail.status) &&
                (track == detail.track);
    }

    //! @brief Equal comparison operator
    bool operator != (const Detail& detail) const
    {
        return !(*this == detail);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return (takv.Valid() &&
                contact.Valid() && 
                uid.Valid() &&
                precisionLocation.Valid() && 
                group.Valid() && 
                status.Valid() && 
                track.Valid());
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const Detail& detail)
    {
        /*
        *  NOTE: This will print only the valid sub-schemas. 
         
            os << "Detail:\n";
            if (detail.takv.Valid()) { os << detail.takv; }
            if (detail.contact.Valid()) { os << detail.contact; }
            if (detail.uid.Valid()) { os << detail.uid; }
            if (detail.precisionLocation.Valid()) { os << detail.precisionLocation; }
            if (detail.group.Valid()) { os << detail.group; }
            if (detail.status.Valid()) { os << detail.status; }
            if (detail.track.Valid()) { os << detail.track; }
            os << "\n";

        */

        os << "Detail:\n"
        << detail.takv
        << detail.contact
        << detail.uid
        << detail.precisionLocation
        << detail.group
        << detail.status
        << detail.track
        << "\n";

        return os;
    }
};

//! @brief Define a Root COT Message schema class for entire message data. 
class COTSchema
{
public:
    Event event;                    //! Holds Event Sub-schema
    Point point;                    //! Holds Point Sub-schema
    Detail detail;                  //! Holds Detail Sub-schema

    //! @brief Constructor - Initializes Everything
    COTSchema(const Event& event = Event(),
        const Point& point = Point(),
        const Detail& detail = Detail()) :
        event(event), point(point), detail(detail)
    {}

    //! @brief Equal comparison operator
    bool operator == (const COTSchema& cot) const
    {
        return  (event == cot.event) &&
                (point == cot.point) &&
                (detail == cot.detail);
    }

    //! @brief Equal comparison operator
    bool operator != (const COTSchema& cot) const
    {
        return !(*this == cot);
    }

    //! @brief Does class have valid data ? 
    bool Valid(void) const
    {
        return  (event.Valid()) || (point.Valid()) || (detail.Valid());
    }

    //! @brief Print the class
    friend std::ostream& operator<<(std::ostream& os, const COTSchema& cot)
    {
        os << "COT Data:\n"
            << cot.event
            << cot.point
            << cot.detail
            << "\n";

        return os;
    }
};