#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            CoT_Utility.h
// @brief           A class for generating and parsing CoT messages
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

    /// @brief Result structure for CoT operations
    struct result {
        /// @brief Error code indicating the operation outcome
        enum class error_code : int {
            Success,                // No error
            InvalidCotSchema,
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
        static std::string to_string(error_code error)
        {
            switch (error)
            {
            case error_code::Success:
                return "Success";
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

#pragma region message_classes

    /// @brief A COT Message subschema class for Date data 
    class date
    {
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

    /// @brief A COT Message subschema class for Time data 
    class time
    {
    public:
        unsigned hour;
        unsigned minute;
        double second;

        time(unsigned hour = 0, unsigned minute = 0, double second = -1)
            : hour(hour), minute(minute), second(second) {
        }

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

        bool operator==(const time& other) const {
            constexpr double EPSILON = 1e-6;
            return hour == other.hour && minute == other.minute &&
                (std::isnan(second) && std::isnan(other.second) || std::abs(second - other.second) < EPSILON);
        }

        bool operator!=(const time& other) const {
            return !(*this == other);
        }

        std::string to_string() const {
            std::string str;
            os << std::setfill('0') << std::setw(2) << hour << ":"
                << std::setfill('0') << std::setw(2) << minute << ":"
                << std::setfill('0') << std::setw(5) << std::fixed << std::setprecision(2) << second;
            return str;
        }

        friend std::ostream& operator<<(std::ostream& os, const time& time) {
            os << time.to_string();
            return os;
        }
    };

    /// @brief A COT Message subschema class for datetime data 
    class datetime : public date, public time
    {
    public:
        datetime(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = 0, unsigned minute = 0, double second = -1)
            : date(year, month, day), time(hour, minute, second) {
        }

        /// @brief Check if the DateTime object is valid
        /// @param errorMsg Optional error message for invalid reason
        /// @return true if valid, else false
        bool is_valid(std::string* errorMsg = nullptr) const
        {
            std::string dateError, timeError;
            bool dateValid = date::is_valid(&dateError);
            bool timeValid = time::is_valid(&timeError);
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
        static datetime FromString(const std::string& str, std::string* errorMsg = nullptr)
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
        static datetime FromSystemClock(std::string* errorMsg = nullptr)
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

        bool is_valid(std::string* errorMsg = nullptr) const
        {
            if (std::isnan(latitude) || std::isnan(longitude) ||
                std::isnan(hae) || std::isnan(circularError) || std::isnan(linearError))
            {
                if (errorMsg) *errorMsg = "Point is missing required fields";
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
            if (!IsValid()) return "<point/>";
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
                if (errorMsg && !errors.empty()) *errorMsg += "Event invalid: " + errors + "; ";
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
        bool is_valid(std::string* errorMsg = nullptr) const
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

        // Use Track::FromXml to parse the track node
        event = Event::FromXml(eventNode);

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

        // Use Point::FromXml to parse the track node
        point = Point::FromXml(pointNode);

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

        // Use Detail::FromXml to parse the track node
        detail = Detail::FromXml(detailNode);

        // Success
        return result(result::error_code::Success);
    }

#pragma endregion
}