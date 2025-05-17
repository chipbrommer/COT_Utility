#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot.hpp
// @brief           a single include api for handling CoT messages
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                            reason included
//          --------------------            ------------------------------------
#include <iostream>                         // ostream
#include <iomanip>                          // setw
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>                    // maps
//
#include <pugixml.hpp>                      // pugi xml
// 
/////////////////////////////////////////////////////////////////////////////////

namespace cot
{
    constexpr auto COT_UTILITY_MAJOR = 0;
    constexpr auto COT_UTILITY_MINOR = 5;
    constexpr auto COT_UTILITY_BUILD = 0;

#pragma region error_content

    /// @brief Result structure for CoT operations
    struct result {
        /// @brief Error code indicating the operation outcome
        enum class error_code : int {
            Success,                // No error
            StructureDataInvalid,
            InvalidData,        
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
            NoModificationMade,
            GeneralError,
        };

        /// @brief Check if the operation was successful
        bool is_success() const { return m_code == error_code::Success; }

        /// @brief Check if the operation failed
        bool is_failed() const { return !is_success(); }

        /// @brief Constructor for success case
        result() : m_code(error_code::Success), m_description("") {}

        /// @brief Constructor for error case
        result(error_code e, std::string desc) : m_code(e), m_description(std::move(desc)) {}

        /// @brief Convert to string for debugging or logging
        std::string to_string() const {
            std::string result = "Result: ";
            result += std::to_string(m_code);
            if (!m_description.empty()) {
                result += "; Description: " + m_description;
            }
            return result;
        }

        std::string description() { return this->m_description; }
    private:
        static std::string to_string(error_code error) {
            switch (error)
            {
            case error_code::Success:
                return "Success";
            case error_code::StructureDataInvalid:
                return "Structure has invalid data";
            case error_code::InvalidEvent:
                return "XML has invalid Event tag";
            case error_code::InvalidPoint:
                return "XML has invalid Point tag";
            case error_code::InvalidDate:
                return "XML has invalid Date tag; Date string must have minimum 3 type identifiers (Year, Month, Day)";
            case error_code::InvalidTime:
                return "XML has invalid Time tag; Time must have minimum 3 type identifiers (Hour, Minute, Secs)";
            case error_code::InvalidHow:
                return "XML has invalid How tag";
            case error_code::InvalidType:
                return "XML has invalid Type tag";
            case error_code::InvalidXml:
                return "Invalid XML input";
            case error_code::InvalidInput:
                return "Invalid or empty input";
            case error_code::InvalidTimeSubSchema:
                return "Invalid Time sub-schema";
            case error_code::InsufficientData:
                return "Insufficient Data";
            case error_code::ProcessingError:
                return "Processing error";
            case error_code::NoModificationMade:
                return "No modification made";
            case error_code::GeneralError:
                return "General Error";
            default:
                return "Unknown error";
            }
        }

        error_code m_code;          /// Specific error code
        std::string m_description;  /// Detailed error message
    };

    /// @brief Error handler for custom error logging
    using ErrorHandler = std::function<void(const result&)>;
    inline ErrorHandler error_handler;
    void set_error_handler(ErrorHandler handler) {
        error_handler = std::move(handler);
    }

#pragma endregion

#pragma region message_classes

    /// @brief A COT Message subschema class for uid data
    class uid {
    public:
        std::string droid; /// User

        /// @brief Constructor - Initializes Everything
        uid(const std::string& droid = INVALID_VALUE) : droid(droid) {}

        /// @brief Equality comparison operator
        bool operator==(const uid& other) const {
            return droid == other.droid;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const uid& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            bool valid = !droid.empty();
            if (!valid && rslt) {
                *rslt = result(result::error_code::StructureDataInvalid, "Status battery is unset (NaN)");
            }
            return valid;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<uid/>"; // Empty tag for invalid Uid
            }
            std::ostringstream oss;
            oss << "<uid Droid=\"" << droid << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static uid from_xml(const pugi::xml_node& node) {
            uid u;
            pugi::xml_attribute attr;
            if (attr = node.attribute("Droid")) u.droid = attr.as_string();
            return u;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const uid& u) {
            os << "uid: ";
            if (!u.is_valid()) { os << " -NOT VALID- "; }
            os << "\n\tdroid: " << (u.droid.empty() ? "None" : u.droid) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for date data 
    class date {
    public:
        unsigned year;  /// year
        unsigned month; /// month
        unsigned day;   /// day

        /// @brief Constructor - Initializes Everything
        date(unsigned year = 0, unsigned month = 0, unsigned day = 0)
            : year(year), month(month), day(day) {
        }

        /// @brief Check if the instance is valid
        /// @param errorMsg pointer to string to place any possible error message
        /// @return true if valid, else false
        bool is_valid(std::string* errorMsg = nullptr) const {
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
            unsigned maxDay = days_in_month();
            if (day < 1 || day > maxDay)
            {
                if (errorMsg) *errorMsg = "Day must be between 1 and " + std::to_string(maxDay);
                return false;
            }
            return true;
        }

        /// @brief Equality comparison operator
        bool operator==(const date& other) const {
            return year == other.year && month == other.month && day == other.day;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const date& other) const { return !(*this == other); }

        /// @brief Get the date in string format YYYY-MM-DD
        /// @return string containing the date
        std::string to_string() const {
            std::string str;
            str << std::setfill('0') << std::setw(4) << year << "-"
                << std::setfill('0') << std::setw(2) << month << "-"
                << std::setfill('0') << std::setw(2) << day;
            return str;
        }

        /// @brief output the date in string format YYYY-MM-DD
        /// @param os output stream
        /// @param date date instance to be output
        /// @return stream
        friend std::ostream& operator<<(std::ostream& os, const date& date) {
            os << date.to_string();
            return os;
        }

    private:
        /// @brief function to get the number of days in the month
        /// @return number of days in the month
        unsigned days_in_month() const {
            static const unsigned days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            if (month == 2 && is_leap_year()) return 29;
            return days[month - 1];
        }

        /// @brief Check if the current year is a leap year
        /// @return true if leap year, else false
        bool is_leap_year() const {
            return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        }
    };

    /// @brief A COT Message subschema class for time data 
    class time {
    public:
        unsigned hour;      /// hours
        unsigned minute;    /// minutes
        double second;      /// seconds

        /// @brief Constructor - Initializes Everything
        time(unsigned hour = 0, unsigned minute = 0, double second = -1)
            : hour(hour), minute(minute), second(second) {
        }

        /// @brief Check if the instance is valid
        /// @param errorMsg pointer to string to place any possible error message
        /// @return true if valid, else false
        bool is_valid(std::string* errorMsg = nullptr) const {
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

        /// @brief Equality comparison operator
        bool operator==(const time& other) const {
            constexpr double EPSILON = 1e-6;
            return hour == other.hour && minute == other.minute &&
                (std::isnan(second) && std::isnan(other.second) || std::abs(second - other.second) < EPSILON);
        }

        /// @brief Inequality comparison operator
        bool operator!=(const time& other) const {
            return !(*this == other);
        }

        /// @brief Get the date in string format HH:MM:SS.sss
        /// @return string containing the date
        std::string to_string() const {
            std::string str;
            os << std::setfill('0') << std::setw(2) << hour << ":"
                << std::setfill('0') << std::setw(2) << minute << ":"
                << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << second;
            return str;
        }

        /// @brief output the date in string format YYYY-MM-DD
        /// @param os output stream
        /// @param date date instance to be output
        /// @return stream
        friend std::ostream& operator<<(std::ostream& os, const time& time) {
            os << time.to_string();
            return os;
        }
    };

    /// @brief A COT Message subschema class for date and time data 
    class datetime : public date, public time {
    public:
        /// @brief Constructor - Initializes everything
        datetime(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = 0, unsigned minute = 0, double second = -1)
            : date(year, month, day), time(hour, minute, second) {
        }

        /// @brief Check if the datetime object is valid
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const
        {
            std::string dateError, timeError;
            bool dateValid = date::is_valid(&dateError);
            bool timeValid = time::is_valid(&timeError);
            if (!dateValid || !timeValid)
            {
                if (rslt)
                {
                    std::string errorMsg = "";
                    result::error_code code = result::error_code::Success;
                    if (!dateValid){ code = result::error_code::InvalidDate; errorMsg += "date invalid: " + dateError + "; "; }
                    if (!timeValid){ code = result::error_code::InvalidTime; errorMsg += "time invalid: " + timeError;        }
                    *rslt = result(result::error_code::InvalidDate, errorMsg);
                }
                return false;
            }

            if (rslt) { *rslt = result(result::error_code::Success); }
            return true;
        }

        /// @brief Convert the datetime object into a CoT Timestamp string
        /// @return string containing the converted datetime object
        std::string to_cot_timestamp() const
        {
            if (!is_valid()) return "";
            std::stringstream timestamp;
            timestamp << std::setfill('0') << std::setw(4) << year << "-"
                << std::setfill('0') << std::setw(2) << month << "-"
                << std::setfill('0') << std::setw(2) << day << "T"
                << std::setfill('0') << std::setw(2) << hour << ":"
                << std::setfill('0') << std::setw(2) << minute << ":"
                << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << second << "Z";
            return timestamp.str();
        }

        /// @brief Parse a CoT Timestamp string into a datetime object
        /// @param str sring containing the value of for the timestamp (e.g. 2025-05-11T19:05:24.000Z)
        /// @param rslt Optional result structure for invalid reason if converting into datetime object fails
        /// @return datetime object with parse time, or default if invalid
        static datetime from_cot_timestamp(const std::string& str, result* rslt = nullptr)
        {
            if (str.empty())
            {
                if (rslt) { *rslt = result(result::error_code::InsufficientData, "Empty timestamp string"); }
                return datetime();
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
                if (rslt) { *rslt = result(result::error_code::InvalidTime, "Invalid timestamp format: missing 'T' separator"); } 
                return datetime();
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
                if (rslt) { *rslt = result(result::error_code::InvalidDate, "Invalid date format: expected YYYY-MM-DD"); }
                return datetime();
            }

            // Parse time (hh:mm:ss.sssZ)
            std::string timeStr = parts[1];
            if (timeStr.empty() || timeStr.back() != 'Z')
            {
                if (rslt) { *rslt = result(result::error_code::InvalidTime, "Invalid time format: missing 'Z' suffix"); }
                return datetime();
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
                if (rslt) { *rslt = result(result::error_code::InvalidTime, "Invalid time format: expected hh:mm:ss.sss"); }
                return datetime();
            }

            try
            {
                unsigned year = std::stoul(dateParts[0]);
                unsigned month = std::stoul(dateParts[1]);
                unsigned day = std::stoul(dateParts[2]);
                unsigned hour = std::stoul(timeParts[0]);
                unsigned minute = std::stoul(timeParts[1]);
                double second = std::stod(timeParts[2]);

                datetime dt(year, month, day, hour, minute, second);
                if (!dt.is_valid(rslt))
                {
                    return datetime();
                }
                return dt;
            }
            catch (const std::exception& e)
            {
                if (rslt) { *rslt = result(result::error_code::ProcessingError, "Parsing error: " + std::string(e.what())); }
                return datetime();
            }
        }

        /// @brief Creates a datetime object from the current system clock (UTC)
        /// @param rslt Optional result structure for invalid reason if system time retrieval fails
        /// @return datetime object with current UTC time, or default if invalid
        static datetime from_system_clock(result* rslt = nullptr)
        {
            // Get current time as epoch seconds
            std::time_t now = std::time(nullptr);
            if (now == static_cast<std::time_t>(-1))
            {
                if (rslt) { *rslt = result(result::error_code::GeneralError, "Failed to retrieve system time"); }
                return datetime();
            }

            // Convert to UTC using gmtime (portable across Windows, Linux, macOS)
            std::tm* utcTime = std::gmtime(&now);
            if (!utcTime)
            {
                if (rslt) { *rslt = result(result::error_code::GeneralError, "Failed to convert system time to UTC"); }
                return datetime();
            }

            // Populate datetime fields
            // tm_year is years since 1900, so add 1900
            // tm_mon is 0-11, so add 1
            unsigned year = utcTime->tm_year + 1900;
            unsigned month = utcTime->tm_mon + 1;
            unsigned day = utcTime->tm_mday;
            unsigned hour = utcTime->tm_hour;
            unsigned minute = utcTime->tm_min;
            double second = utcTime->tm_sec; // No sub-second precision

            datetime dt(year, month, day, hour, minute, second);
            if (!dt.is_valid(rslt))
            {
                return datetime();
            }
            return dt;
        }

        /// @brief Sets datetime fields with user-provided values
        /// @param year Year (>= 1970), default 0 (no change)
        /// @param month Month (1-12), default 0 (no change)
        /// @param day Day (1-31, depending on month), default 0 (no change)
        /// @param hour Hour (0-23), default UINT_MAX (no change)
        /// @param minute Minute (0-59), default UINT_MAX (no change)
        /// @param second Second (0-59.999...), default -1 (no change)
        /// @param rslt Optional result structure for invalid reason if validation fails
        /// @return True if set successfully, false if invalid
        bool set(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = UINT_MAX, unsigned minute = UINT_MAX, double second = -1,
            result* rslt = nullptr)
        {
            // Store current values
            datetime temp = *this;

            // Update only provided fields
            if (year != 0) temp.year = year;
            if (month != 0) temp.month = month;
            if (day != 0) temp.day = day;
            if (hour != UINT_MAX) temp.hour = hour;
            if (minute != UINT_MAX) temp.minute = minute;
            if (second != -1) temp.second = second;

            // Validate
            if (!temp.is_valid(rslt))
            {
                return false;
            }

            // Update fields if valid
            *this = temp;
            return true;
        }

        /// @brief Set the current datetime object from the current system clock (UTC)
        /// @return result structure with pass or invalid reason if set fails
        result set_from_system_clock()
        {
            result tmp{};
            *this = from_system_clock(tmp);
            return tmp;
        }

        /// @brief Equality comparison operator
        bool operator==(const datetime& other) const
        {
            return static_cast<const Date&>(*this) == static_cast<const Date&>(other) &&
                static_cast<const Time&>(*this) == static_cast<const Time&>(other);
        }

        /// @brief Inequality comparison operator
        bool operator!=(const datetime& other) const
        {
            return !(*this == other);
        }

        /// @brief Friend class for convenient printing
        friend std::ostream& operator<<(std::ostream& os, const datetime& dt)
        {
            os << dt.to_cot_timestamp();
            return os;
        }
    };

    /// @brief A COT Message subschema class for precisionlocation data
    class precisionlocation {
    public:
        std::string altsrc;      /// Altitude source
        std::string geopointsrc; /// Geopoint source

        /// @brief Constructor - Initializes Everything
        precisionlocation(const std::string& altsrc = INVALID_VALUE,
            const std::string& geopointsrc = INVALID_VALUE) :
            altsrc(altsrc), geopointsrc(geopointsrc) {
        }

        /// @brief Equality comparison operator
        bool operator==(const precisionlocation& other) const {
            return altsrc == other.altsrc &&
                geopointsrc == other.geopointsrc;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const precisionlocation& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result *rslt = nullptr) const {
            bool valid = !altsrc.empty() || !geopointsrc.empty();
            if (!valid && rslt) {
                *rslt = result(result::error_code::StructureDataInvalid, "precision_location has no non-empty fields");
            }
            return valid;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
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
        static precisionlocation from_xml(const pugi::xml_node& node) {
            precisionlocation preloc;
            pugi::xml_attribute attr;
            if (attr = node.attribute("altsrc")) preloc.altsrc = attr.as_string();
            if (attr = node.attribute("geopointsrc")) preloc.geopointsrc = attr.as_string();
            return preloc;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const precisionlocation& preloc) {
            os << "precisionlocation: ";
            if (!preloc.is_valid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\talt Source: " << (preloc.altsrc.empty() ? "None" : preloc.altsrc) << "\n"
                << "\tgeopoint Source: " << (preloc.geopointsrc.empty() ? "None" : preloc.geopointsrc) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for takv data
    class takv {
    public:
        std::string version;  /// Version
        std::string device;   /// Device type
        std::string os;       /// Operating system
        std::string platform; /// TAK platform (ATAK-CIV, WINTAK, ATAK-MIL, etc)

        /// @brief Constructor - Initializes Everything
        takv(const std::string& version = INVALID_VALUE,
            const std::string& device = INVALID_VALUE,
            const std::string& os = INVALID_VALUE,
            const std::string& platform = INVALID_VALUE) :
            version(version), device(device), os(os), platform(platform) {
        }

        /// @brief Equality comparison operator
        bool operator==(const takv& other) const {
            return version == other.version &&
                device == other.device &&
                os == other.os &&
                platform == other.platform;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const takv& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            bool valid = !version.empty() || !device.empty() || !os.empty() || !platform.empty();
            if (!valid && rslt) {
                *rslt = result(result::error_code::StructureDataInvalid, "takv has no non-empty fields");
            }
            return valid;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
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
        static takv from_xml(const pugi::xml_node& node) {
            takv t;
            pugi::xml_attribute attr;
            if (attr = node.attribute("version")) t.version = attr.as_string();
            if (attr = node.attribute("device")) t.device = attr.as_string();
            if (attr = node.attribute("os")) t.os = attr.as_string();
            if (attr = node.attribute("platform")) t.platform = attr.as_string();
            return t;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const takv& t) {
            os << "takv: ";
            if (!t.is_valid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\tversion: " << (t.version.empty() ? "None" : t.version) << "\n"
                << "\tdevice: " << (t.device.empty() ? "None" : t.device) << "\n"
                << "\tos: " << (t.os.empty() ? "None" : t.os) << "\n"
                << "\tplatform: " << (t.platform.empty() ? "None" : t.platform) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for contact data
    class contact
    {
    public:
        std::string endpoint;     /// Endpoint of the unit, usually a TCP address
        std::string callsign;     /// Callsign (name) of the item
        std::string xmppUsername; /// XMPP Username

        /// @brief Constructor - Initializes Everything
        contact(const std::string& endpoint = INVALID_VALUE,
            const std::string& callsign = INVALID_VALUE,
            const std::string& xmppUsername = INVALID_VALUE) :
            endpoint(endpoint), callsign(callsign), xmppUsername(xmppUsername) {
        }

        /// @brief Equality comparison operator
        bool operator==(const contact& other) const {
            return endpoint == other.endpoint &&
                callsign == other.callsign &&
                xmppUsername == other.xmppUsername;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const contact& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            bool valid = !endpoint.empty() || !callsign.empty() || !xmppUsername.empty();
            if (!valid && rslt) {
                *rslt = result(result::error_code::StructureDataInvalid, "contact has no non-empty fields");
            }
            return valid;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
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
        static contact from_xml(const pugi::xml_node& node) {
            contact c;
            pugi::xml_attribute attr;
            if (attr = node.attribute("endpoint")) c.endpoint = attr.as_string();
            if (attr = node.attribute("callsign")) c.callsign = attr.as_string();
            if (attr = node.attribute("xmppUsername")) c.xmppUsername = attr.as_string();
            return c;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const contact& c) {
            os << "contact: ";
            if (!c.is_valid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\tendpoint: " << (c.endpoint.empty() ? "None" : c.endpoint) << "\n"
                << "\tcallsign: " << (c.callsign.empty() ? "None" : c.callsign) << "\n"
                << "\txmpp username: " << (c.xmppUsername.empty() ? "None" : c.xmppUsername) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for group data
    class group
    {
    public:
        std::string role; /// Role of the group member sending
        std::string name; /// Group name

        /// @brief Constructor - Initializes Everything
        group(const std::string& role = INVALID_VALUE,
            const std::string& name = INVALID_VALUE) :
            role(role), name(name) {
        }

        /// @brief Equality comparison operator
        bool operator==(const group& other) const {
            return role == other.role &&
                name == other.name;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const group& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            bool valid = !role.empty() || !name.empty();
            if (!valid && rslt) {
                *rslt = result(result::error_code::StructureDataInvalid, "group has no non-empty fields");
            }
            return valid;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
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
        static group from_xml(const pugi::xml_node& node) {
            group g;
            pugi::xml_attribute attr;
            if (attr = node.attribute("role")) g.role = attr.as_string();
            if (attr = node.attribute("name")) g.name = attr.as_string();
            return g;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const group& g) {
            os << "group: ";
            if (!g.is_valid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\trole: " << (g.role.empty() ? "None" : g.role) << "\n"
                << "\tname: " << (g.name.empty() ? "None" : g.name) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for status data
    class status {
    public:
        double battery; /// Battery percentage

        /// @brief Constructor - Initializes Everything
        status(double battery = INVALID_VALUE) : battery(battery) {}

        /// @brief Equality comparison operator
        bool operator==(const status& other) const {
            constexpr double EPSILON = 1e-6;
            return (std::isnan(battery) && std::isnan(other.battery)) ||
                std::abs(battery - other.battery) < EPSILON;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const status& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(battery)) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Status battery is unset (NaN)"); }
                return false;
            }

            if (battery < 0 || battery > 100) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Status battery out of range (0-100)"); }
                return false;
            }

            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<status/>"; // Empty tag for invalid Status
            }
            std::ostringstream oss;
            oss << "<status battery=\"" << std::fixed << std::setprecision(2) << battery << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static status from_xml(const pugi::xml_node& node) {
            status s;
            pugi::xml_attribute attr;
            if (attr = node.attribute("battery")) {
                try {
                    s.battery = attr.as_double(INVALID_VALUE);
                } catch (const std::exception& e) {
                    s.battery = INVALID_VALUE;
                }
            }
            return s;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const status& s) {
            os << "status: ";
            if (!s.is_valid()) { os << " -NOT VALID- "; }
            os << "\n\tbattery: ";
            if (std::isnan(s.battery)) { os << "NaN"; }
            else { os << std::fixed << std::setprecision(2) << s.battery; }
            os << "\n";
            return os;
        }

    private:
        static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN(); /// Sentinel value for invalid double
    };

    /// @brief A COT Message subschema class for strokecolor data
    class strokecolor
    {
    public:
        int value; /// Color value (ARGB as signed 32-bit integer)

        /// @brief Constructor - Initializes Everything
        strokecolor(int value = INVALID_VALUE) : value(value) {}

        /// @brief Equality comparison operator
        bool operator==(const strokecolor& other) const {
            return value == other.value;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const strokecolor& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "strokecolor value is unset"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<strokeColor/>"; // Empty tag for unset color
            }
            std::ostringstream oss;
            oss << "<strokeColor value=\"" << value << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static strokecolor from_xml(const pugi::xml_node& node) {
            strokecolor color;
            pugi::xml_attribute attr;
            if (attr = node.attribute("value")) {
                try {
                    color.value = attr.as_int(INVALID_VALUE);
                } catch (const std::exception& e) {
                    color.value = INVALID_VALUE;
                }
            }
            return color;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const strokecolor& color) {
            os << "strokecolor: ";
            if (!color.is_valid()) { os << " -NOT VALID- (Unset)"; }
            else { os << "\n\tvalue: " << color.value; }
            os << "\n";
            return os;
        }

    private:
        static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
    };

    /// @brief A COT Message subschema class for fillcolor data
    class fillcolor
    {
    public:
        int value; /// Color value (ARGB as signed 32-bit integer)

        /// @brief Constructor - Initializes Everything
        fillcolor(int value = INVALID_VALUE) : value(value) {}

        /// @brief Equality comparison operator
        bool operator==(const fillcolor& other) const {
            return value == other.value;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const fillcolor& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "fillcolor value is unset"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<fillColor/>"; // Empty tag for unset color
            }
            std::ostringstream oss;
            oss << "<fillColor value=\"" << value << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static fillcolor from_xml(const pugi::xml_node& node) {
            fillcolor color;
            pugi::xml_attribute attr;
            if (attr = node.attribute("value")) {
                try {
                    color.value = attr.as_int(INVALID_VALUE);
                } catch (const std::exception& e) {
                    color.value = INVALID_VALUE;
                }
            }
            return color;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const fillcolor& color) {
            os << "fillcolor: ";
            if (!color.is_valid()) { os << " -NOT VALID- (Unset)"; }
            else { os << "\n\tvalue: " << color.value; }
            os << "\n";
            return os;
        }

    private:
        static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
    };

    /// @brief A COT Message subschema class for color data
    class color {
    public:
        int argb; /// Color value (ARGB as signed 32-bit integer)

        /// @brief Constructor - Initializes Everything
        color(int argb = INVALID_VALUE) : argb(argb) {}

        /// @brief Equality comparison operator
        bool operator==(const color& other) const {
            return argb == other.argb;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const color& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (argb == INVALID_VALUE) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "argb is unset"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (!is_valid()) {
                return "<color/>"; // Empty tag for unset color
            }
            std::ostringstream oss;
            oss << "<color argb=\"" << argb << "\"/>";
            return oss.str();
        }

        /// @brief Deserialize from XML node
        static color from_xml(const pugi::xml_node& node)
        {
            color c;
            pugi::xml_attribute attr;
            if (attr = node.attribute("argb")) {
                try {
                    c.argb = attr.as_int(INVALID_VALUE);
                } catch (const std::exception& e) {
                    c.argb = INVALID_VALUE;
                }
            }
            return c;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const color& c) {
            os << "color: ";
            if (!c.is_valid()) {
                os << " -NOT VALID- (Unset)";
            } else {
                os << "\n\targb: " << c.argb;
            }
            os << "\n";
            return os;
        }

    private:
        static constexpr int INVALID_VALUE = std::numeric_limits<int>::min(); /// Sentinel value for invalid color
    };

    /// @brief A COT Message subschema class for usericon data
    class usericon
    {
    public:
        std::string iconsetpath; /// Icon set path

        /// @brief Constructor - Initializes Everything
        usericon(const std::string& iconpath = INVALID_VALUE) : iconsetpath(iconpath) {}

        /// @brief Equality comparison operator
        bool operator==(const usericon& other) const
        {
            return iconsetpath == other.iconsetpath;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const usericon& other) const
        {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (iconsetpath.empty()) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "iconsetpath is unset"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (is_valid()) {
                std::ostringstream oss;
                oss << "<usericon iconsetpath=\"" << iconsetpath << "\"/>";
                return oss.str();
            }
            return "";
        }

        /// @brief Deserialize from XML node
        static usericon from_xml(const pugi::xml_node& node) {
            usericon ui;
            pugi::xml_attribute attr;
            if (attr = node.attribute("iconsetpath")) {
                ui.iconsetpath = attr.as_string();
            }
            return ui;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const usericon& ui) {
            os << "usericon: ";
            if (!ui.is_valid()) { os << " -NOT VALID- "; }
            os << "\n\ticonsetpath: " << (ui.is_valid() ? "None" : ui.iconsetpath) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for model data
    class model
    {
    public:
        std::string value; /// Model value

        /// @brief Constructor - Initializes Everything
        model(const std::string& val = INVALID_VALUE) : value(val) {}

        /// @brief Equality comparison operator
        bool operator==(const model& other) const {
            return value == other.value;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const model& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "value is empty"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
            if (is_valid())
            {
                std::ostringstream oss;
                oss << "<model value=\"" << value << "\"/>";
                return oss.str();
            }
            return "";
        }

        /// @brief Deserialize from XML node
        static model from_xml(const pugi::xml_node& node) {
            model m;
            pugi::xml_attribute attr;
            if (attr = node.attribute("value"))
            {
                m.value = attr.as_string();
            }
            return m;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const model& m) {
            os << "model: ";
            if (!m.is_valid()) { os << " -NOT VALID- "; }
            os << "\n\tValue: " << (m.value.empty() ? "None" : m.value) << "\n";
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for track data
    class track
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
        track(const double course = INVALID_VALUE,
            const double speed = INVALID_VALUE,
            const double slope = INVALID_VALUE,
            const double eCourse = INVALID_VALUE,
            const double eSpeed = INVALID_VALUE,
            const double eSlope = INVALID_VALUE,
            const double version = INVALID_VALUE) :
            course(course), speed(speed), slope(slope),
            eCourse(eCourse), eSpeed(eSpeed), eSlope(eSlope),
            version(version) {
        }

        /// @brief Equal comparison operator
        bool operator==(const track& t) const {
            const double EPSILON = 1e-6;

            auto equal = [](double a, double b, double epsilon) {
                return std::isnan(a) && std::isnan(b) || std::abs(a - b) < epsilon;
                };

            return equal(course, t.course, EPSILON) &&
                equal(speed, t.speed, EPSILON) &&
                equal(slope, t.slope, EPSILON) &&
                equal(eCourse, t.eCourse, EPSILON) &&
                equal(eSpeed, t.eSpeed, EPSILON) &&
                equal(eSlope, t.eSlope, EPSILON) &&
                equal(version, t.version, EPSILON);
        }

        /// @brief Not-equal comparison operator
        bool operator!=(const track& t) const {
            return !(*this == t);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(course)) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Course is NaN"); }
                return false;
            }
            if (course < COURSE_MIN || course > COURSE_MAX) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Course out of range [0, 360]"); }
                return false;
            }
            if (std::isnan(speed)) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Speed is NaN"); }
                return false;
            }
            if (speed < SPEED_MIN) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Speed is negative"); }
                return false;
            }
            if (!std::isnan(slope) && (slope < SLOPE_MIN || slope > SLOPE_MAX)) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Slope out of range [-90, 90]"); }
                return false;
            }
            if (!std::isnan(eCourse) && eCourse < 0) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "eCourse is negative"); }
                return false;
            }
            if (!std::isnan(eSpeed) && eSpeed < 0) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "eSpeed is negative"); }
                return false;
            }
            if (!std::isnan(eSlope) && eSlope < 0) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "eSlope is negative"); }
                return false;
            }
            if (!std::isnan(version) && version <= 0) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Version is non-positive"); }
                return false;
            }
            return true;
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const track& t) {
            os << "track: ";  if (!t.is_valid()) { os << " -NOT VALID- "; }
            os << "\n" << std::fixed << std::setprecision(6)
                << "\tcourse:          " << t.course << "\n"
                << "\tspeed:           " << t.speed << "\n";
            if (!std::isnan(t.slope))   os << "\tslope:           " << t.slope << "\n";
            if (!std::isnan(t.eCourse)) os << "\teCourse:         " << t.eCourse << "\n";
            if (!std::isnan(t.eSpeed))  os << "\teSpeed:          " << t.eSpeed << "\n";
            if (!std::isnan(t.eSlope))  os << "\teSlope:          " << t.eSlope << "\n";
            if (!std::isnan(t.version)) os << "\tversion:         " << t.version << "\n";
            os << std::defaultfloat << "\n";
            return os;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
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
        static track from_xml(const pugi::xml_node& node) {
            track t;
            pugi::xml_attribute attr;
            (attr = node.attribute("course")) ? t.course = attr.as_double() : t.course = INVALID_VALUE;
            (attr = node.attribute("speed")) ? t.speed = attr.as_double() : t.speed = INVALID_VALUE;
            (attr = node.attribute("slope")) ? t.slope = attr.as_double() : t.slope = INVALID_VALUE;
            (attr = node.attribute("eCourse")) ? t.eCourse = attr.as_double() : t.eCourse = INVALID_VALUE;
            (attr = node.attribute("eSpeed")) ? t.eSpeed = attr.as_double() : t.eSpeed = INVALID_VALUE;
            (attr = node.attribute("eSlope")) ? t.eSlope = attr.as_double() : t.eSlope = INVALID_VALUE;
            (attr = node.attribute("version")) ? t.version = attr.as_double() : t.version = INVALID_VALUE;
            return t;
        }

    private:
        static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        static constexpr double COURSE_MIN = 0.0;    /// Minimum course in degrees
        static constexpr double COURSE_MAX = 360.0;  /// Maximum course in degrees
        static constexpr double SPEED_MIN = 0.0;     /// Minimum speed in meters per second
        static constexpr double SLOPE_MIN = -90.0;   /// Minimum slope in degrees
        static constexpr double SLOPE_MAX = 90.0;    /// Maximum slope in degrees
    };

    /// @brief A COT Message subschema class for link data
    class link
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
        link(const std::string& uid = INVALID_STRING,
            const std::string& remarks = INVALID_STRING,
            const std::string& relation = INVALID_STRING,
            const std::string& callsign = INVALID_STRING,
            const std::string& type = INVALID_STRING,
            const std::string& point = INVALID_STRING,
            double lat = INVALID_VALUE,
            double lon = INVALID_VALUE) :
            uid(uid), remarks(remarks), relation(relation),
            callsign(callsign), type(type), point(point),
            latitude(lat), longitude(lon) {
        }

        /// @brief Equality comparison operator
        bool operator==(const link& other) const {
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
        bool operator!=(const link& other) const {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(latitude) || std::isnan(longitude) || uid.empty() || type.empty())
            {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "Invalid latitude or longitude"); }
                return false;
            }
            // String fields are optional, so empty is valid
            return true;
        }

        /// @brief Serialize to XML string
        std::string to_xml() const {
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
        static link from_xml(const pugi::xml_node& node) {
            link l;
            pugi::xml_attribute attr;
            if (attr = node.attribute("uid")) l.uid = attr.as_string();
            if (attr = node.attribute("remarks")) l.remarks = attr.as_string();
            if (attr = node.attribute("relation")) l.relation = attr.as_string();
            if (attr = node.attribute("callsign")) l.callsign = attr.as_string();
            if (attr = node.attribute("type")) l.type = attr.as_string();
            if (attr = node.attribute("point")) l.point = attr.as_string();
            try
            {
                auto [lat, lon] = l.get_lat_lon_from_point();
                l.latitude = lat;
                l.longitude = lon;
            }
            catch (const std::exception& e)
            {
                l.latitude = INVALID_VALUE;
                l.longitude = INVALID_VALUE;
            }
            return l;
        }

        /// @brief Parse point into latitude and longitude
        std::pair<double, double> get_lat_lon_from_point() const {
            if (point.empty()) {
                return { INVALID_VALUE, INVALID_VALUE };
            }
            size_t commaPos = point.find(',');
            if (commaPos == std::string::npos) {
                return { INVALID_VALUE, INVALID_VALUE };
            }
            try {
                double lat = std::stod(point.substr(0, commaPos));
                double lon = std::stod(point.substr(commaPos + 1));
                return { lat, lon };
            } catch (const std::exception& e) {
                return { INVALID_VALUE, INVALID_VALUE };
            }
        }

        /// @brief Print the class
        friend std::ostream& operator<<(std::ostream& os, const link& l) {
            os << "link: ";
            if (!l.is_valid()) { os << " -NOT VALID- "; }
            os << "\n"
                << "\tuid: " << (l.uid.empty() ? "None" : l.uid) << "\n"
                << "\tremarks: " << (l.remarks.empty() ? "None" : l.remarks) << "\n"
                << "\trelation: " << (l.relation.empty() ? "None" : l.relation) << "\n"
                << "\tcallsign: " << (l.callsign.empty() ? "None" : l.callsign) << "\n"
                << "\ttype: " << (l.type.empty() ? "None" : l.type) << "\n"
                << "\tpoint: " << (l.point.empty() ? "None" : l.point) << "\n"
                << "\tlatitude: ";
            if (std::isnan(l.latitude)) {
                os << "NaN";
            } else {
                os << std::fixed << std::setprecision(6) << l.latitude;
            }
            os << "\n\tlongitude: ";
            if (std::isnan(l.longitude)) {
                os << "NaN";
            } else {
                os << std::fixed << std::setprecision(6) << l.longitude;
            }
            os << "\n";
            return os;
        }

    private:
        static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        static constexpr const char* INVALID_STRING = "";
    };

    /// @brief A struct to store custom detail elements in a CoT message
    class customdetail
    {
    public:
        std::string name;                               /// XML tag name (e.g., "customTag")
        std::map<std::string, std::string> attributes;  /// XML attributes as key-value pairs
        std::string content;                            /// Inner text or CDATA

        /// @brief Constructor
        customdetail(const std::string& name = INVALID_VALUE,
            const std::map<std::string, std::string>& attributes = {},
            const std::string& content = INVALID_VALUE) :
            name(name), attributes(attributes), content(content) {
        }

        /// @brief Equality comparison
        bool operator==(const customdetail& other) const {
            return name == other.name &&
                attributes == other.attributes &&
                content == other.content;
        }

        /// @brief Inequality comparison
        bool operator!=(const customdetail& other) const {
            return !(*this == other);
        }

        /// @brief Validate the custom detail
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const {
            if (name.empty() || (attributes.length() == 0 && content.empty())) {
                if (rslt) { *rslt = result(result::error_code::StructureDataInvalid, "name is empty or missing data"); }
                return false;
            }
            return true;
        }

        /// @brief Serialize to XML
        std::string to_xml() const {
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
        static customdetail from_xml(const pugi::xml_node& node) {
            customdetail detail;
            detail.name = node.name();
            for (pugi::xml_attribute attr : node.attributes()) {
                detail.attributes[attr.name()] = attr.as_string();
            }
            detail.content = node.child_value();
            return detail;
        }

        /// @brief Print the custom detail
        friend std::ostream& operator<<(std::ostream& os, const customdetail& detail) {
            os << "\tcustomdetail: " << detail.name;
            if (!detail.is_valid()) { os << " -NOT VALID-"; }
            os << "\n";
            for (const auto& [key, value] : detail.attributes)
            {
                os << "\t\t" << key << ": " << value << "\n";
            }
            if (!detail.content.empty())
            {
                os << "\t\tcontent: " << detail.content << "\n";
            }
            return os;
        }

    private:
        static constexpr const char* INVALID_VALUE = ""; /// Sentinel value for invalid string
    };

    /// @brief A COT Message subschema class for point data 
    class point
    {
    public:
        double latitude;
        double longitude;
        double hae;
        double circularError;
        double linearError;

        point(double latitude = std::numeric_limits<double>::quiet_NaN(),
            double longitude = std::numeric_limits<double>::quiet_NaN(),
            double hae = std::numeric_limits<double>::quiet_NaN(),
            double circularError = std::numeric_limits<double>::quiet_NaN(),
            double linearError = std::numeric_limits<double>::quiet_NaN())
            : latitude(latitude), longitude(longitude), hae(hae),
            circularError(circularError), linearError(linearError) {
        }

        bool is_valid(result* rslt = nullptr) const
        {
            if (std::isnan(latitude) || std::isnan(longitude) ||
                std::isnan(hae) || std::isnan(circularError) || std::isnan(linearError))
            {
                if (rslt) { *rslt = result(result::error_code::InvalidPoint, "Point is missing required fields"); }
                return false;
            }
            return true;
        }

        bool operator==(const point& other) const
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

        bool operator!=(const point& other) const
        {
            return !(*this == other);
        }

        std::string to_xml() const
        {
            if (!is_valid()) return "<point/>";
            std::ostringstream oss;
            oss << "<point lat=\"" << std::fixed << std::setprecision(6) << latitude
                << "\" lon=\"" << longitude << "\" hae=\"" << hae
                << "\" ce=\"" << circularError << "\" le=\"" << linearError << "\"/>";
            return oss.str();
        }

        static result from_xml(const pugi::xml_node& node, point& p)
        {
            point p;
            try {
                if (auto attr = node.attribute("lat")) p.latitude = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("lon")) p.longitude = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("hae")) p.hae = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("ce")) p.circularError = attr.as_double(std::numeric_limits<double>::quiet_NaN());
                if (auto attr = node.attribute("le")) p.linearError = attr.as_double(std::numeric_limits<double>::quiet_NaN());
            }
            catch (const std::exception& e) {
                result(result::error_code::InvalidPoint, "Error parsing point attributes: " + e.what());
            }
            return point;
        }

        friend std::ostream& operator<<(std::ostream& os, const Data& point)
        {
            os << "Point: ";
            if (!point.is_valid()) os << " -NOT VALID- ";
            os << "\n\tLatitude: " << (std::isnan(point.latitude) ? "NaN" : std::to_string(point.latitude))
                << "\n\tLongitude: " << (std::isnan(point.longitude) ? "NaN" : std::to_string(point.longitude))
                << "\n\tHAE: " << (std::isnan(point.hae) ? "NaN" : std::to_string(point.hae))
                << "\n\tCE: " << (std::isnan(point.circularError) ? "NaN" : std::to_string(point.circularError))
                << "\n\tLE: " << (std::isnan(point.linearError) ? "NaN" : std::to_string(point.linearError)) << "\n";
            return os;
        }
    };

    /// @brief A COT Message subschema class for event data 
    class event {
    public:
        /// @brief Constructor - Initializes everything
        event() : {}

        /// @brief Checks if the class has valid data
        bool is_valid(std::string* errorMsg = nullptr) const
        {
            bool valid = true;
            std::string errors;

            // Verify member items are valid
            if (!p.is_valid(&errors))
            {
                valid = false;
                if (errorMsg) *errorMsg += "Event invalid: " + errors + "; ";
            }

            return valid;
        }

        /// @brief Getter for Point sub-schema
        const point& get_point() const { return p; }

        /// @brief Setter for Point sub-schema with validation
        result set_point(const point& newPoint)
        {
            if (!newPoint.is_valid())
            {
                result(result::error_code::InvalidPoint, "Cannot set invalid Point");
                return false;
            }
            p = newPoint;
            return true;
        }

        void clear_point()
        {
            p = point();
        }

    private:
        double version;
        std::string uid;
        std::string type;
        datetime time;
        datetime start;
        datetime stale;
        std::string how;
        point p;
    };

    /// @brief A root XML CoT Message schema class for entire xml message data
    class message {
    public:
        Event event;                    /// Holds Event Sub-schema
        std::string xml_version;        /// Holds version from XML tag
        std::string xml_encoding;       /// Holds hold encoding from XML tag
        std::string xml_standalone;     /// Holds holds standalone bool from XML tag

        /// @brief Constructor - Initializes everything
        message(const Event& event = Event()) :
            event(event) {
        }

        /// @brief Equality comparison operator
        bool operator==(const message& other) const
        {
            return event == other.event &&
                xml_version == other.xml_version &&
                xml_encoding == other.xml_encoding &&
                xml_standalone == other.xml_standalone;
        }

        /// @brief Inequality comparison operator
        bool operator!=(const message& other) const
        {
            return !(*this == other);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return true if valid, else false
        bool is_valid(result* rslt = nullptr) const
        {
            bool valid = true;
            std::string errors;

            // Event and Point are required in CoT
            if (!event.is_valid(&errors))
            {
                valid = false;
                if (errorMsg && !errors.empty()) *errorMsg += "Event invalid: " + errors + "; ";
            }

            return valid;
        }
 
    };

#pragma endregion

#pragma region utility

    /// @brief Get the current version information
    /// @return String containing version (e.g., "1.0.0")
    [[nodiscard]] std::string get_version()
    {
        std::ostringstream oss;
        oss << "cot::utility v" << COT_UTILITY_MAJOR << '.' << COT_UTILITY_MINOR << '.' << COT_UTILITY_BUILD;
        return oss.str();
    }

    /// @brief Verify a string buffer is valid XML
    /// @param buffer Input buffer to verify
    /// @return result indicating success or failure with description
    result verify_xml(std::string buffer) {
        if (buffer.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" }
        }

        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml, parse_result.description() };
        }

        return result{ result::error_code::Success, "Successfully verified XML" };
    }

    /// @brief prep an xml buffer to be parsed, will clear content before the starting xml tag
    /// @param xml string view containtaining the xml
    /// @return result indicating success or failure with description
    result prep_message(std::string& xml)
    {
        if (xml.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" }
        }

        size_t position = xml.find("<?xml");
        if (position == std::string::npos)
        {
            return result(result::error_code::InvalidXml);
        }
        xml.erase(0, position);

        // Verify XML structure
        result rslt = verify_xml(xml);
        if (rslt.is_failed())
        {
            return rslt;
        }
        return result::error_code::Success);
    }

    /// @brief Create an XML CoT message from a schema
    /// @param cot Schema to convert to XML
    /// @param xml std::string contianing xml text, xml is empty if error occurs
    /// @return result indicating success or failure with description
    [[nodiscard]] result generate_xml_cot(const schema& cot, std::string& xml) {
        // Validate schema
        std::string error;
        auto validation_result = cot.is_valid(&error);
        if (!validation_result) {
            return result{ result::error_code::InvalidCotSchema, "Invalid cot schema: " + error };
        }

        // Create XML document
        pugi::xml_document doc;

        // Add XML declaration
        pugi::xml_node decl = doc.append_child(pugi::node_declaration);
        decl.append_attribute("version") = "1.0";
        decl.append_attribute("encoding") = "utf-8";
        decl.append_attribute("standalone") = "yes";

        // Generate XML for event, point, and detail
        std::string temp = cot.to_xml();

        // Verify generated XML
        pugi::xml_document verify_doc;
        pugi::xml_parse_result parse_result = verify_doc.load_string(temp.data(), temp.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml,
                                        std::string(parse_result.description()) + " at offset " +
                                        std::to_string(parse_result.offset) };
        }

        xml = temp;
        return result{ result::error_code::Success, "Successfully generated XML CoT message" };
    }

    /// @brief Add an acknowledgment to a received CoT message
    /// @param received Received message
    /// @param response Output acknowledgment message
    /// @return Result indicating success or failure with description
    [[nodiscard]] result acknowledge_received_cot_message(std::string_view received,
        std::string& response)
    {
        // Create XML document to load in the receivedMessage
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_string(receivedMessage.c_str());

        if (res)
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
                return result(result::error_code::Success);
            }

            // No modification made
            return result(result::error_code::NoModificationMade);
        }

        return result(result::error_code::ProcessingError, result.description());
    }

    /// @brief Parse a CoT message from a string buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(std::string_view buffer, Schema& cot)
    {
        // Convert input buffer to string and remove garbage before <?xml
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(buffer.c_str());
        if (!result) { return Result(Result::Code::ProcessingError); }

        // Validate structure
        pugi::xml_node eventNode = doc.child("event");
        if (!eventNode || doc.root().select_nodes("event").size() != 1)
        {
            return Result(result::error_code::InvalidEvent);
        }
        if (eventNode.select_nodes("point").size() != 1)
        {
            return Result(Result::Code::InvalidPoint);
        }
        if (eventNode.select_nodes("detail").size() > 1)
        {
            return Result(Result::Code::InvalidXml);
        }

        // Parse event, point, and detail (if present)
        cot.event = Event::from_xml(eventNode);
        cot.point = Point::Data::from_xml(eventNode.child("point"));
        if (pugi::xml_node detailNode = eventNode.child("detail"))
        {
            cot.detail = Detail::from_xml(detailNode);
        }

        // Validate parsed data
        std::string error;
        if (!cot.event.is_valid(&error))
        {
            return Result(Result::Code::InvalidEvent, std::string("Parsed Event is invalid: " + error));
        }
        if (!cot.point.is_valid(&error))
        {
            return Result(Result::Code::InvalidPoint, std::string("Parsed Point is invalid: " + error));
        }
        if (eventNode.child("detail") && !cot.detail.is_valid(&error))
        {
            return Result(Result::Code::InvalidDetail, std::string("Parsed Detail is invalid: " + error));
        }

        return Result(Result::Code::Success);
    }

    /// @brief Parse a CoT message from a character buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(const char* buffer, Schema& cot)
    {
        return parse_cot(std::string_view(buffer), cot);
    }

    /// @brief Parse the Event element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param event Output event instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_event_from_cot(std::string_view buffer, Event& event)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");

        // Use Track::from_xml to parse the track node
        event = Event::from_xml(eventNode);

        // Success
        return result(result::error_code::Success);
    }

    /// @brief Parse the Point element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param point Output point instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_point_from_cot(std::string_view buffer, Point& point)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node pointNode = eventNode.child("point");
        if (!pointNode)
        {
            return result(result::error_code::InsufficientData);
        }

        // Use Point::from_xml to parse the track node
        point = Point::from_xml(pointNode);

        // Success
        return result(result::error_code::Success);
    }

    /// @brief Parse the Detail element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param detail parsed detail instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_detail_from_cot(std::string_view buffer, Detail& detail)
    {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_message(xmlBuffer);

        if (rslt.is_failed())
        {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult)
        {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1)
        {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node detailNode = eventNode.child("detail");
        if (!detailNode)
        {
            return result(result::error_code::InsufficientData);
        }

        // Use Detail::from_xml to parse the track node
        detail = Detail::from_xml(detailNode);

        // Success
        return result(result::error_code::Success);
    }

#pragma endregion
}