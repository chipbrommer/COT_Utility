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
#include <algorithm>
#include <climits>                          // MAX and MIN
#include <cmath>                            // isnan
#include <functional>                       // function
#include <iostream>                         // ostream
#include <iomanip>                          // setw
#include <map>                              // ordered map
#include <optional>
#include <sstream>                          // stringstream
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
            Success,                ///< Operation completed successfully
            InvalidEvent,           ///< XML is missing or has invalid <event> tag
            InvalidPoint,           ///< XML is missing or has invalid <point> tag
            InvalidDetail,          ///< XML has invalid <detail> tag
            InvalidColor,           ///< Invalid color data (strokecolor, fillcolor, color)
            InvalidUserIcon,        ///< Invalid usericon data
            InvalidModel,           ///< Invalid model data
            InvalidTrack,           ///< Invalid track data
            InvalidLink,            ///< Invalid link data
            InvalidCustomDetail,    ///< Invalid custom detail data
            InvalidTakv,            ///< Invalid takv data
            InvalidContact,         ///< Invalid contact data
            InvalidUid,             ///< Invalid uid data
            InvalidPrecisionLocation, ///< Invalid precisionlocation data
            InvalidGroup,           ///< Invalid group data
            InvalidStatus,          ///< Invalid status data
            InvalidDate,            ///< Invalid date; must have Year, Month, Day
            InvalidTime,            ///< Invalid time; must have Hour, Minute, Seconds
            InvalidHow,             ///< Invalid how attribute
            InvalidType,            ///< Invalid type attribute
            InvalidXml,             ///< String is not valid XML
            InvalidInput,           ///< Input is empty or malformed
            InsufficientData,       ///< Not enough data provided
            ProcessingError,        ///< Generic processing failure
            NoModificationMade,     ///< No modification was made
            StructureDataInvalid,   ///< Structure has invalid data (restored)
            InvalidData             ///< Invalid data (restored)
        };

        /// @brief Constructs a successful result
        result() noexcept : m_code(error_code::Success) {}

        /// @brief Constructs a result with an error code
        result(error_code code) noexcept : m_code(code) {}

        /// @brief Constructs a result with an error code and description
        result(error_code code, std::string desc) noexcept
            : m_code(code), m_description(std::move(desc)) {}

        /// @brief Copy constructor
        result(const result& other) noexcept = default;

        /// @brief Copy assignment operator
        result& operator=(const result& other) noexcept = default;

        /// @brief Move constructor
        result(result&& other) noexcept = default;

        /// @brief Move assignment operator
        result& operator=(result&& other) noexcept = default;

        /// @brief Checks if the operation was successful
        [[nodiscard]] bool is_success() const noexcept { return m_code == error_code::Success; }

        /// @brief Checks if the operation failed
        [[nodiscard]] bool is_failed() const noexcept { return !is_success(); }

        /// @brief Converts to string for debugging or logging
        [[nodiscard]] std::string to_string() const {
            std::string result = "Result: ";
            auto code_str = to_string(m_code);
            result += code_str;
            if (!m_description.empty() && m_description != code_str) {
                result += "; Description: " + m_description;
            }
            return result;
        }

        /// @brief Implicit conversion to string
        operator std::string() const { return to_string(); }

        /// @brief Gets the error description
        [[nodiscard]] const std::string& description() const noexcept { return m_description; }

    private:
        /// @brief Converts error code to string
        [[nodiscard]] static constexpr std::string_view to_string(error_code code) noexcept {
            switch (code) {
            case error_code::Success: return "Success";
            case error_code::InvalidEvent: return "XML has invalid Event tag";
            case error_code::InvalidPoint: return "XML has invalid Point tag";
            case error_code::InvalidDetail: return "XML has invalid Detail tag";
            case error_code::InvalidColor: return "Invalid color data";
            case error_code::InvalidUserIcon: return "Invalid usericon data";
            case error_code::InvalidModel: return "Invalid model data";
            case error_code::InvalidTrack: return "Invalid track data";
            case error_code::InvalidLink: return "Invalid link data";
            case error_code::InvalidCustomDetail: return "Invalid custom detail data";
            case error_code::InvalidTakv: return "Invalid takv data";
            case error_code::InvalidContact: return "Invalid contact data";
            case error_code::InvalidUid: return "Invalid uid data";
            case error_code::InvalidPrecisionLocation: return "Invalid precisionlocation data";
            case error_code::InvalidGroup: return "Invalid group data";
            case error_code::InvalidStatus: return "Invalid status data";
            case error_code::InvalidDate: return "Invalid date; must have Year, Month, Day";
            case error_code::InvalidTime: return "Invalid time; must have Hour, Minute, Seconds";
            case error_code::InvalidHow: return "Invalid how attribute";
            case error_code::InvalidType: return "Invalid type attribute";
            case error_code::InvalidXml: return "Invalid XML input";
            case error_code::InvalidInput: return "Invalid or empty input";
            case error_code::InsufficientData: return "Insufficient data";
            case error_code::ProcessingError: return "Processing error";
            case error_code::NoModificationMade: return "No modification made";
            case error_code::StructureDataInvalid: return "Structure has invalid data";
            case error_code::InvalidData: return "Invalid data";
            default: return "Unknown error";
            }
        }

        error_code m_code;          ///< Specific error code
        std::string m_description;  ///< Detailed error message
    };

    /// @brief Error handler for custom error logging
    using ErrorHandler = std::function<void(const result&)>;
    inline ErrorHandler error_handler;

    /// @brief Sets the error handler
    inline void set_error_handler(ErrorHandler handler) noexcept {
        error_handler = std::move(handler);
    }

#pragma endregion

#pragma region message_classes

    /// @brief A CoT Message subschema class for uid data
    class uid {
    public:
        /// @brief Constructor - Initializes everything
        explicit uid(std::string droid = INVALID_VALUE) : droid(std::move(droid)) {}

        /// @brief Equality comparison
        bool operator==(const uid& other) const noexcept { return droid == other.droid; }

        /// @brief Inequality comparison
        bool operator!=(const uid& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (droid.empty()) {
                auto res = result{ result::error_code::InvalidUid, "Empty droid field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidUid, "Invalid uid data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("uid");
            node.append_attribute("droid") = droid.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized uid
        static uid from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            uid u;
            if (node.name() != std::string("uid")) {
                auto res = result{ result::error_code::InvalidUid, "Expected <uid> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return u;
            }
            if (auto attr = node.attribute("droid")) {
                u.droid = attr.as_string();
            }
            if (!u.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidUid, "Invalid uid data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return u;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const uid& u) {
            os << "uid: ";
            if (!u.is_valid()) os << " -NOT VALID- ";
            os << "\n\tdroid: " << (u.droid.empty() ? "None" : u.droid) << "\n";
            return os;
        }

        std::string droid;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for date data
    class date {
    public:
        /// @brief Constructor - Initializes everything
        date(unsigned year = 0, unsigned month = 0, unsigned day = 0)
            : year(year), month(month), day(day) {}

        /// @brief Checks if the date is valid
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (year < 1970) {
                auto res = result{ result::error_code::InvalidDate, "Year must be >= 1970" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (month < 1 || month > 12) {
                auto res = result{ result::error_code::InvalidDate, "Month must be between 1 and 12" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            unsigned maxDay = days_in_month();
            if (day < 1 || day > maxDay) {
                auto res = result{ result::error_code::InvalidDate,
                                  "Day must be between 1 and " + std::to_string(maxDay) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Equality comparison
        bool operator==(const date& other) const noexcept {
            return year == other.year && month == other.month && day == other.day;
        }

        /// @brief Inequality comparison
        bool operator!=(const date& other) const noexcept { return !(*this == other); }

        /// @brief Converts to string in YYYY-MM-DD format
        /// @return String representation of the date
        std::string to_string() const {
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(4) << year << "-"
                << std::setfill('0') << std::setw(2) << month << "-"
                << std::setfill('0') << std::setw(2) << day;
            return oss.str();
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const date& d) {
            os << d.to_string();
            return os;
        }

        unsigned year;
        unsigned month;
        unsigned day;

    private:
        unsigned days_in_month() const {
            static constexpr unsigned days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            if (month == 2 && is_leap_year()) return 29;
            return days[month - 1];
        }

        bool is_leap_year() const noexcept {
            return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        }
    };

    /// @brief A CoT Message subschema class for time data
    class time {
    public:
        /// @brief Constructor - Initializes everything
        time(unsigned hour = 0, unsigned minute = 0, double second = 0.0)
            : hour(hour), minute(minute), second(second) {}

        /// @brief Checks if the time is valid
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (hour >= 24) {
                auto res = result{ result::error_code::InvalidTime, "Hour must be < 24" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (minute >= 60) {
                auto res = result{ result::error_code::InvalidTime, "Minute must be < 60" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (second < 0.0 || second >= 60.0) {
                auto res = result{ result::error_code::InvalidTime, "Second must be between 0 and 59.999..." };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Equality comparison
        bool operator==(const time& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return hour == other.hour && minute == other.minute &&
                (std::isnan(second) && std::isnan(other.second) ||
                    std::abs(second - other.second) < EPSILON);
        }

        /// @brief Inequality comparison
        bool operator!=(const time& other) const noexcept { return !(*this == other); }

        /// @brief Converts to string in HH:MM:SS.sss format
        /// @return String representation of the time
        std::string to_string() const {
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hour << ":"
                << std::setfill('0') << std::setw(2) << minute << ":"
                << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << second;
            return oss.str();
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const time& t) {
            os << t.to_string();
            return os;
        }

        unsigned hour;
        unsigned minute;
        double second;
    };

    /// @brief A CoT Message subschema class for date and time data
    class datetime : public date, public time {
    public:
        /// @brief Constructor - Initializes everything
        datetime(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = 0, unsigned minute = 0, double second = 0.0)
            : date(year, month, day), time(hour, minute, second) {}

        /// @brief Checks if the datetime is valid
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            result date_result, time_result;
            bool date_valid = date::is_valid(&date_result);
            bool time_valid = time::is_valid(&time_result);
            if (!date_valid || !time_valid) {
                std::string error = "";
                result::error_code code = result::error_code::InvalidDate;
                if (!date_valid) error += "Date invalid: " + date_result.description + "; ";
                if (!time_valid) {
                    error += "Time invalid: " + time_result.description;
                    code = result::error_code::InvalidTime;
                }
                auto res = result{ code, error };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Converts to CoT timestamp string (YYYY-MM-DDThh:mm:ss.sssZ)
        /// @return String representation of the datetime
        std::string to_cot_timestamp() const {
            if (!is_valid()) return "";
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(4) << year << "-"
                << std::setfill('0') << std::setw(2) << month << "-"
                << std::setfill('0') << std::setw(2) << day << "T"
                << std::setfill('0') << std::setw(2) << hour << ":"
                << std::setfill('0') << std::setw(2) << minute << ":"
                << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << second << "Z";
            return oss.str();
        }

        /// @brief Parses a CoT timestamp string into a datetime
        /// @param str Timestamp string (e.g., 2025-05-11T19:05:24.000Z)
        /// @param rslt Optional result for error details
        /// @return Parsed datetime, or default if invalid
        static datetime from_cot_timestamp(std::string_view str, result* rslt = nullptr) {
            if (str.empty()) {
                auto res = result{ result::error_code::InvalidInput, "Empty timestamp string" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            // Split on 'T'
            size_t t_pos = str.find('T');
            if (t_pos == std::string_view::npos) {
                auto res = result{ result::error_code::InvalidTime, "Missing 'T' separator" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            std::string_view date_str = str.substr(0, t_pos);
            std::string_view time_str = str.substr(t_pos + 1);

            // Parse date (YYYY-MM-DD)
            std::vector<std::string> date_parts;
            size_t start = 0;
            for (size_t i = 0; i <= date_str.size(); ++i) {
                if (i == date_str.size() || date_str[i] == '-') {
                    date_parts.push_back(std::string(date_str.substr(start, i - start)));
                    start = i + 1;
                }
            }
            if (date_parts.size() != 3) {
                auto res = result{ result::error_code::InvalidDate, "Invalid date format: expected YYYY-MM-DD" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            // Parse time (hh:mm:ss.sssZ)
            if (time_str.empty() || time_str.back() != 'Z') {
                auto res = result{ result::error_code::InvalidTime, "Missing 'Z' suffix" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }
            time_str.remove_suffix(1); // Remove 'Z'
            std::vector<std::string> time_parts;
            start = 0;
            for (size_t i = 0; i <= time_str.size(); ++i) {
                if (i == time_str.size() || time_str[i] == ':') {
                    time_parts.push_back(std::string(time_str.substr(start, i - start)));
                    start = i + 1;
                }
            }
            if (time_parts.size() != 3) {
                auto res = result{ result::error_code::InvalidTime, "Invalid time format: expected hh:mm:ss.sss" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            try {
                unsigned year = std::stoul(date_parts[0]);
                unsigned month = std::stoul(date_parts[1]);
                unsigned day = std::stoul(date_parts[2]);
                unsigned hour = std::stoul(time_parts[0]);
                unsigned minute = std::stoul(time_parts[1]);
                double second = std::stod(time_parts[2]);

                datetime dt(year, month, day, hour, minute, second);
                if (!dt.is_valid(rslt)) {
                    return datetime();
                }
                return dt;
            }
            catch (const std::exception& e) {
                auto res = result{ result::error_code::ProcessingError, "Parsing error: " + std::string(e.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }
        }

        /// @brief Creates a datetime from the current system clock (UTC)
        /// @param rslt Optional result for error details
        /// @return Datetime with current UTC time, or default if invalid
        static datetime from_system_clock(result* rslt = nullptr) {
            std::time_t now = std::time(nullptr);
            if (now == static_cast<std::time_t>(-1)) {
                auto res = result{ result::error_code::ProcessingError, "Failed to retrieve system time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            std::tm* utc_time = std::gmtime(&now);
            if (!utc_time) {
                auto res = result{ result::error_code::ProcessingError, "Failed to convert to UTC" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return datetime();
            }

            unsigned year = utc_time->tm_year + 1900;
            unsigned month = utc_time->tm_mon + 1;
            unsigned day = utc_time->tm_mday;
            unsigned hour = utc_time->tm_hour;
            unsigned minute = utc_time->tm_min;
            double second = utc_time->tm_sec;

            datetime dt(year, month, day, hour, minute, second);
            if (!dt.is_valid(rslt)) {
                return datetime();
            }
            return dt;
        }

        /// @brief Sets datetime fields with provided values
        /// @param year Year (>= 1970), default 0 (no change)
        /// @param month Month (1-12), default 0 (no change)
        /// @param day Day (1-31), default 0 (no change)
        /// @param hour Hour (0-23), default UINT_MAX (no change)
        /// @param minute Minute (0-59), default UINT_MAX (no change)
        /// @param second Second (0-59.999...), default -1 (no change)
        /// @param rslt Optional result for error details
        /// @return True if set successfully, false otherwise
        bool set(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = UINT_MAX, unsigned minute = UINT_MAX, double second = -1.0,
            result* rslt = nullptr) {
            datetime temp = *this;
            if (year != 0) temp.year = year;
            if (month != 0) temp.month = month;
            if (day != 0) temp.day = day;
            if (hour != UINT_MAX) temp.hour = hour;
            if (minute != UINT_MAX) temp.minute = minute;
            if (second >= 0.0) temp.second = second;

            if (!temp.is_valid(rslt)) {
                return false;
            }

            *this = temp;
            return true;
        }

        /// @brief Sets from the current system clock (UTC)
        /// @return Result indicating success or failure
        result set_from_system_clock() {
            result rslt;
            *this = from_system_clock(&rslt);
            return rslt;
        }

        /// @brief Equality comparison
        bool operator==(const datetime& other) const noexcept {
            return static_cast<const date&>(*this) == static_cast<const date&>(other) &&
                static_cast<const time&>(*this) == static_cast<const time&>(other);
        }

        /// @brief Inequality comparison
        bool operator!=(const datetime& other) const noexcept { return !(*this == other); }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const datetime& dt) {
            os << dt.to_cot_timestamp();
            return os;
        }
    };

    /// @brief A CoT Message subschema class for precisionlocation data
    class precisionlocation {
    public:
        /// @brief Constructor - Initializes everything
        explicit precisionlocation(std::string altsrc = INVALID_VALUE,
            std::string geopointsrc = INVALID_VALUE)
            : altsrc(std::move(altsrc)), geopointsrc(std::move(geopointsrc)) {}

        /// @brief Equality comparison
        bool operator==(const precisionlocation& other) const noexcept {
            return altsrc == other.altsrc && geopointsrc == other.geopointsrc;
        }

        /// @brief Inequality comparison
        bool operator!=(const precisionlocation& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (altsrc.empty() && geopointsrc.empty()) {
                auto res = result{ result::error_code::InvalidPrecisionLocation,
                                  "Both altsrc and geopointsrc are empty" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidPrecisionLocation, "Invalid precisionlocation data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("precisionlocation");
            if (!altsrc.empty()) node.append_attribute("altsrc") = altsrc.c_str();
            if (!geopointsrc.empty()) node.append_attribute("geopointsrc") = geopointsrc.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized precisionlocation
        static precisionlocation from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            precisionlocation pl;
            if (node.name() != std::string("precisionlocation")) {
                auto res = result{ result::error_code::InvalidPrecisionLocation, "Expected <precisionlocation> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return pl;
            }
            if (auto attr = node.attribute("altsrc")) pl.altsrc = attr.as_string();
            if (auto attr = node.attribute("geopointsrc")) pl.geopointsrc = attr.as_string();
            if (!pl.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidPrecisionLocation,
                               "Invalid precisionlocation data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return pl;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const precisionlocation& pl) {
            os << "precisionlocation: ";
            if (!pl.is_valid()) os << " -NOT VALID- ";
            os << "\n\taltsrc: " << (pl.altsrc.empty() ? "None" : pl.altsrc) << "\n"
                << "\tgeopointsrc: " << (pl.geopointsrc.empty() ? "None" : pl.geopointsrc) << "\n";
            return os;
        }

        std::string altsrc;
        std::string geopointsrc;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for takv data
    class takv {
    public:
        /// @brief Constructor - Initializes everything
        explicit takv(std::string version = INVALID_VALUE,
            std::string device = INVALID_VALUE,
            std::string os = INVALID_VALUE,
            std::string platform = INVALID_VALUE)
            : version(std::move(version)), device(std::move(device)),
            os(std::move(os)), platform(std::move(platform)) {}

        /// @brief Equality comparison
        bool operator==(const takv& other) const noexcept {
            return version == other.version && device == other.device &&
                os == other.os && platform == other.platform;
        }

        /// @brief Inequality comparison
        bool operator!=(const takv& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (version.empty() && device.empty() && os.empty() && platform.empty()) {
                auto res = result{ result::error_code::InvalidTakv, "All takv fields are empty" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidTakv, "Invalid takv data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("takv");
            if (!version.empty()) node.append_attribute("version") = version.c_str();
            if (!device.empty()) node.append_attribute("device") = device.c_str();
            if (!os.empty()) node.append_attribute("os") = os.c_str();
            if (!platform.empty()) node.append_attribute("platform") = platform.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized takv
        static takv from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            takv t;
            if (node.name() != std::string("takv")) {
                auto res = result{ result::error_code::InvalidTakv, "Expected <takv> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return t;
            }
            if (auto attr = node.attribute("version")) t.version = attr.as_string();
            if (auto attr = node.attribute("device")) t.device = attr.as_string();
            if (auto attr = node.attribute("os")) t.os = attr.as_string();
            if (auto attr = node.attribute("platform")) t.platform = attr.as_string();
            if (!t.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidTakv, "Invalid takv data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return t;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const takv& t) {
            os << "takv: ";
            if (!t.is_valid()) os << " -NOT VALID- ";
            os << "\n\tversion: " << (t.version.empty() ? "None" : t.version) << "\n"
                << "\tdevice: " << (t.device.empty() ? "None" : t.device) << "\n"
                << "\tos: " << (t.os.empty() ? "None" : t.os) << "\n"
                << "\tplatform: " << (t.platform.empty() ? "None" : t.platform) << "\n";
            return os;
        }

        std::string version;
        std::string device;
        std::string os;
        std::string platform;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for contact data
    class contact {
    public:
        /// @brief Constructor - Initializes everything
        explicit contact(std::string endpoint = INVALID_VALUE,
            std::string callsign = INVALID_VALUE,
            std::string xmppUsername = INVALID_VALUE)
            : endpoint(std::move(endpoint)), callsign(std::move(callsign)),
            xmppUsername(std::move(xmppUsername)) {}

        /// @brief Equality comparison
        bool operator==(const contact& other) const noexcept {
            return endpoint == other.endpoint && callsign == other.callsign &&
                xmppUsername == other.xmppUsername;
        }

        /// @brief Inequality comparison
        bool operator!=(const contact& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (callsign.empty()) {
                auto res = result{ result::error_code::InvalidContact, "Empty callsign field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidContact, "Invalid contact data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("contact");
            if (!endpoint.empty()) node.append_attribute("endpoint") = endpoint.c_str();
            node.append_attribute("callsign") = callsign.c_str();
            if (!xmppUsername.empty()) node.append_attribute("xmppUsername") = xmppUsername.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized contact
        static contact from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            contact c;
            if (node.name() != std::string("contact")) {
                auto res = result{ result::error_code::InvalidContact, "Expected <contact> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return c;
            }
            if (auto attr = node.attribute("endpoint")) c.endpoint = attr.as_string();
            if (auto attr = node.attribute("callsign")) c.callsign = attr.as_string();
            if (auto attr = node.attribute("xmppUsername")) c.xmppUsername = attr.as_string();
            if (!c.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidContact, "Invalid contact data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return c;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const contact& c) {
            os << "contact: ";
            if (!c.is_valid()) os << " -NOT VALID- ";
            os << "\n\tendpoint: " << (c.endpoint.empty() ? "None" : c.endpoint) << "\n"
                << "\tcallsign: " << (c.callsign.empty() ? "None" : c.callsign) << "\n"
                << "\txmppUsername: " << (c.xmppUsername.empty() ? "None" : c.xmppUsername) << "\n";
            return os;
        }

        std::string endpoint;
        std::string callsign;
        std::string xmppUsername;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for group data
    class group {
    public:
        /// @brief Constructor - Initializes everything
        explicit group(std::string role = INVALID_VALUE,
            std::string name = INVALID_VALUE)
            : role(std::move(role)), name(std::move(name)) {}

        /// @brief Equality comparison
        bool operator==(const group& other) const noexcept {
            return role == other.role && name == other.name;
        }

        /// @brief Inequality comparison
        bool operator!=(const group& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidGroup, "Empty name field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidGroup, "Invalid group data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("__group");
            if (!role.empty()) node.append_attribute("role") = role.c_str();
            node.append_attribute("name") = name.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized group
        static group from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            group g;
            if (node.name() != std::string("__group")) {
                auto res = result{ result::error_code::InvalidGroup, "Expected <__group> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return g;
            }
            if (auto attr = node.attribute("role")) g.role = attr.as_string();
            if (auto attr = node.attribute("name")) g.name = attr.as_string();
            if (!g.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidGroup, "Invalid group data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return g;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const group& g) {
            os << "group: ";
            if (!g.is_valid()) os << " -NOT VALID- ";
            os << "\n\trole: " << (g.role.empty() ? "None" : g.role) << "\n"
                << "\tname: " << (g.name.empty() ? "None" : g.name) << "\n";
            return os;
        }

        std::string role;
        std::string name;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for status data
    class status {
    public:
        /// @brief Constructor - Initializes everything
        explicit status(double battery = INVALID_VALUE) : battery(battery) {}

        /// @brief Equality comparison
        bool operator==(const status& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return (std::isnan(battery) && std::isnan(other.battery)) ||
                std::abs(battery - other.battery) < EPSILON;
        }

        /// @brief Inequality comparison
        bool operator!=(const status& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(battery)) {
                auto res = result{ result::error_code::InvalidStatus, "Battery is unset (NaN)" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (battery < 0.0 || battery > 100.0) {
                auto res = result{ result::error_code::InvalidStatus, "Battery out of range (0-100)" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidStatus, "Invalid status data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("status");
            node.append_attribute("battery") = battery;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized status
        static status from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            status s;
            if (node.name() != std::string("status")) {
                auto res = result{ result::error_code::InvalidStatus, "Expected <status> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return s;
            }
            if (auto attr = node.attribute("battery")) {
                try {
                    s.battery = attr.as_double(INVALID_VALUE);
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidStatus,
                                      "Error parsing battery: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    s.battery = INVALID_VALUE;
                }
            }
            if (!s.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidStatus, "Invalid status data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return s;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const status& s) {
            os << "status: ";
            if (!s.is_valid()) os << " -NOT VALID- ";
            os << "\n\tbattery: ";
            if (std::isnan(s.battery)) os << "NaN";
            else os << std::fixed << std::setprecision(2) << s.battery;
            os << "\n";
            return os;
        }

        double battery;

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
    };
    
    /*
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
    */
    
    /// @brief A CoT Message subschema class for color data (replaces strokecolor, fillcolor, color)
    class color {
    public:
        /// @brief Constructor - Initializes everything
        explicit color(int argb = INVALID_VALUE) noexcept : argb(argb) {}

        /// @brief Equality comparison
        bool operator==(const color& other) const noexcept { return argb == other.argb; }

        /// @brief Inequality comparison
        bool operator!=(const color& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (argb == INVALID_VALUE) {
                auto res = result{ result::error_code::InvalidColor, "Color argb is unset" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @param tag_name The XML tag name (e.g., "strokecolor", "fillcolor", "color")
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent, std::string_view tag_name = "color") const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidColor, "Invalid color data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child(tag_name.data());
            // Use "value" for strokecolor and fillcolor, "argb" for color
            const char* attr_name = (tag_name == "strokecolor" || tag_name == "fillcolor") ? "value" : "argb";
            node.append_attribute(attr_name) = argb;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @param tag_name Expected XML tag name (e.g., "strokecolor", "fillcolor", "color")
        /// @return Deserialized color
        static color from_xml(const pugi::xml_node& node, result* rslt = nullptr,
            std::string_view tag_name = "color") {
            color c;
            if (node.name() != tag_name) {
                auto res = result{ result::error_code::InvalidColor,
                                  "Expected <" + std::string(tag_name) + "> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return c;
            }
            // Try both "value" and "argb" attributes
            auto attr = node.attribute("value");
            if (!attr) attr = node.attribute("argb");
            if (attr) {
                try {
                    c.argb = attr.as_int(INVALID_VALUE);
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidColor,
                                      "Error parsing color attribute: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    c.argb = INVALID_VALUE;
                }
            }
            if (!c.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidColor, "Invalid color data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return c;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const color& c) {
            os << "color: ";
            if (!c.is_valid()) os << " -NOT VALID- (Unset)";
            else os << "\n\targb: " << c.argb;
            os << "\n";
            return os;
        }

        int argb;

    private:
        inline static constexpr int INVALID_VALUE = std::numeric_limits<int>::min();
    };

    /// @brief A CoT Message subschema class for usericon data
    class usericon {
    public:
        /// @brief Constructor - Initializes everything
        explicit usericon(std::string iconsetpath = INVALID_VALUE)
            : iconsetpath(std::move(iconsetpath)) {}

        /// @brief Equality comparison
        bool operator==(const usericon& other) const noexcept {
            return iconsetpath == other.iconsetpath;
        }

        /// @brief Inequality comparison
        bool operator!=(const usericon& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (iconsetpath.empty()) {
                auto res = result{ result::error_code::InvalidUserIcon, "Empty iconsetpath" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidUserIcon, "Invalid usericon data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("usericon");
            node.append_attribute("iconsetpath") = iconsetpath.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized usericon
        static usericon from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            usericon ui;
            if (node.name() != std::string("usericon")) {
                auto res = result{ result::error_code::InvalidUserIcon, "Expected <usericon> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return ui;
            }
            if (auto attr = node.attribute("iconsetpath")) {
                ui.iconsetpath = attr.as_string();
            }
            if (!ui.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidUserIcon,
                               "Invalid usericon data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return ui;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const usericon& ui) {
            os << "usericon: ";
            if (!ui.is_valid()) os << " -NOT VALID- ";
            os << "\n\ticonsetpath: " << (ui.iconsetpath.empty() ? "None" : ui.iconsetpath) << "\n";
            return os;
        }

        std::string iconsetpath;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for model data
    class model {
    public:
        /// @brief Constructor - Initializes everything
        explicit model(std::string value = INVALID_VALUE) : value(std::move(value)) {}

        /// @brief Equality comparison
        bool operator==(const model& other) const noexcept { return value == other.value; }

        /// @brief Inequality comparison
        bool operator!=(const model& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (value.empty()) {
                auto res = result{ result::error_code::InvalidModel, "Empty model value" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidModel, "Invalid model data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("model");
            node.append_attribute("value") = value.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized model
        static model from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            model m;
            if (node.name() != std::string("model")) {
                auto res = result{ result::error_code::InvalidModel, "Expected <model> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return m;
            }
            if (auto attr = node.attribute("value")) {
                m.value = attr.as_string();
            }
            if (!m.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidModel, "Invalid model data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return m;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const model& m) {
            os << "model: ";
            if (!m.is_valid()) os << " -NOT VALID- ";
            os << "\n\tvalue: " << (m.value.empty() ? "None" : m.value) << "\n";
            return os;
        }

        std::string value;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for track data
    class track {
    public:
        /// @brief Constructor - Initializes everything
        explicit track(double course = INVALID_VALUE,
            double speed = INVALID_VALUE,
            double slope = INVALID_VALUE,
            double eCourse = INVALID_VALUE,
            double eSpeed = INVALID_VALUE,
            double eSlope = INVALID_VALUE,
            double version = INVALID_VALUE)
            : course(course), speed(speed), slope(slope),
            eCourse(eCourse), eSpeed(eSpeed), eSlope(eSlope), version(version) {}

        /// @brief Equality comparison
        bool operator==(const track& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            auto equal = [](double a, double b) {
                return std::isnan(a) && std::isnan(b) ||
                    std::abs(a - b) < EPSILON;
                };
            return equal(course, other.course) &&
                equal(speed, other.speed) &&
                equal(slope, other.slope) &&
                equal(eCourse, other.eCourse) &&
                equal(eSpeed, other.eSpeed) &&
                equal(eSlope, other.eSlope) &&
                equal(version, other.version);
        }

        /// @brief Inequality comparison
        bool operator!=(const track& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(course)) {
                auto res = result{ result::error_code::InvalidTrack, "Course is NaN" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (course < COURSE_MIN || course > COURSE_MAX) {
                auto res = result{ result::error_code::InvalidTrack, "Course out of range [0, 360]" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(speed)) {
                auto res = result{ result::error_code::InvalidTrack, "Speed is NaN" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (speed < SPEED_MIN) {
                auto res = result{ result::error_code::InvalidTrack, "Speed is negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(slope) && (slope < SLOPE_MIN || slope > SLOPE_MAX)) {
                auto res = result{ result::error_code::InvalidTrack, "Slope out of range [-90, 90]" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(eCourse) && eCourse < 0) {
                auto res = result{ result::error_code::InvalidTrack, "eCourse is negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(eSpeed) && eSpeed < 0) {
                auto res = result{ result::error_code::InvalidTrack, "eSpeed is negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(eSlope) && eSlope < 0) {
                auto res = result{ result::error_code::InvalidTrack, "eSlope is negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(version) && version <= 0) {
                auto res = result{ result::error_code::InvalidTrack, "Version is non-positive" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidTrack, "Invalid track data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("track");
            node.append_attribute("course") = course;
            node.append_attribute("speed") = speed;
            if (!std::isnan(slope)) node.append_attribute("slope") = slope;
            if (!std::isnan(eCourse)) node.append_attribute("eCourse") = eCourse;
            if (!std::isnan(eSpeed)) node.append_attribute("eSpeed") = eSpeed;
            if (!std::isnan(eSlope)) node.append_attribute("eSlope") = eSlope;
            if (!std::isnan(version)) node.append_attribute("version") = version;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized track
        static track from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            track t;
            if (node.name() != std::string("track")) {
                auto res = result{ result::error_code::InvalidTrack, "Expected <track> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return t;
            }
            t.course = node.attribute("course").as_double(INVALID_VALUE);
            t.speed = node.attribute("speed").as_double(INVALID_VALUE);
            t.slope = node.attribute("slope").as_double(INVALID_VALUE);
            t.eCourse = node.attribute("eCourse").as_double(INVALID_VALUE);
            t.eSpeed = node.attribute("eSpeed").as_double(INVALID_VALUE);
            t.eSlope = node.attribute("eSlope").as_double(INVALID_VALUE);
            t.version = node.attribute("version").as_double(INVALID_VALUE);
            if (!t.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidTrack, "Invalid track data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return t;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const track& t) {
            os << "track: ";
            if (!t.is_valid()) os << " -NOT VALID- ";
            os << "\n" << std::fixed << std::setprecision(6)
                << "\tcourse: " << t.course << "\n"
                << "\tspeed: " << t.speed << "\n";
            if (!std::isnan(t.slope)) os << "\tslope: " << t.slope << "\n";
            if (!std::isnan(t.eCourse)) os << "\teCourse: " << t.eCourse << "\n";
            if (!std::isnan(t.eSpeed)) os << "\teSpeed: " << t.eSpeed << "\n";
            if (!std::isnan(t.eSlope)) os << "\teSlope: " << t.eSlope << "\n";
            if (!std::isnan(t.version)) os << "\tversion: " << t.version << "\n";
            os << std::defaultfloat;
            return os;
        }

        double course;
        double speed;
        double slope;
        double eCourse;
        double eSpeed;
        double eSlope;
        double version;

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr double COURSE_MIN = 0.0;
        inline static constexpr double COURSE_MAX = 360.0;
        inline static constexpr double SPEED_MIN = 0.0;
        inline static constexpr double SLOPE_MIN = -90.0;
        inline static constexpr double SLOPE_MAX = 90.0;
    };

    /// @brief A CoT Message subschema class for link data
    class link {
    public:
        /// @brief Constructor - Initializes everything
        explicit link(std::string uid = INVALID_STRING,
            std::string remarks = INVALID_STRING,
            std::string relation = INVALID_STRING,
            std::string callsign = INVALID_STRING,
            std::string type = VALID_TYPE,
            double latitude = INVALID_VALUE,
            double longitude = INVALID_VALUE)
            : uid(std::move(uid)), remarks(std::move(remarks)), relation(std::move(relation)),
            callsign(std::move(callsign)), type(std::move(type)),
            latitude(latitude), longitude(longitude) {}

        /// @brief Equality comparison
        bool operator==(const link& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return uid == other.uid &&
                remarks == other.remarks &&
                relation == other.relation &&
                callsign == other.callsign &&
                type == other.type &&
                (std::isnan(latitude) && std::isnan(other.latitude) ||
                    std::abs(latitude - other.latitude) < EPSILON) &&
                (std::isnan(longitude) && std::isnan(other.longitude) ||
                    std::abs(longitude - other.longitude) < EPSILON);
        }

        /// @brief Inequality comparison
        bool operator!=(const link& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (uid.empty()) {
                auto res = result{ result::error_code::InvalidLink, "Empty uid field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (type.empty()) {
                auto res = result{ result::error_code::InvalidLink, "Empty type field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(latitude) || latitude < -90.0 || latitude > 90.0) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Invalid latitude: must be between -90 and 90" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(longitude) || longitude < -180.0 || longitude > 180.0) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Invalid longitude: must be between -180 and 180" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidLink, "Invalid link data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("link");
            node.append_attribute("uid") = uid.c_str();
            if (!remarks.empty()) node.append_attribute("remarks") = remarks.c_str();
            if (!relation.empty()) node.append_attribute("relation") = relation.c_str();
            if (!callsign.empty()) node.append_attribute("callsign") = callsign.c_str();
            node.append_attribute("type") = type.c_str();
            std::string point = std::to_string(latitude) + "," + std::to_string(longitude);
            node.append_attribute("point") = point.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized link
        static link from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            link l;
            if (node.name() != std::string("link")) {
                auto res = result{ result::error_code::InvalidLink, "Expected <link> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return l;
            }
            if (auto attr = node.attribute("uid")) l.uid = attr.as_string();
            if (auto attr = node.attribute("remarks")) l.remarks = attr.as_string();
            if (auto attr = node.attribute("relation")) l.relation = attr.as_string();
            if (auto attr = node.attribute("callsign")) l.callsign = attr.as_string();
            if (auto attr = node.attribute("type")) l.type = attr.as_string();
            if (auto attr = node.attribute("point")) {
                std::string point = attr.as_string();
                size_t comma_pos = point.find(',');
                if (comma_pos != std::string::npos) {
                    try {
                        l.latitude = std::stod(point.substr(0, comma_pos));
                        l.longitude = std::stod(point.substr(comma_pos + 1));
                    }
                    catch (const std::exception& e) {
                        auto res = result{ result::error_code::InvalidLink,
                                          "Invalid point format: " + std::string(e.what()) };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        l.latitude = INVALID_VALUE;
                        l.longitude = INVALID_VALUE;
                    }
                }
            }
            if (!l.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidLink, "Invalid link data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return l;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const link& l) {
            os << "link: ";
            if (!l.is_valid()) os << " -NOT VALID- ";
            os << "\n"
                << "\tuid: " << (l.uid.empty() ? "None" : l.uid) << "\n"
                << "\tremarks: " << (l.remarks.empty() ? "None" : l.remarks) << "\n"
                << "\trelation: " << (l.relation.empty() ? "None" : l.relation) << "\n"
                << "\tcallsign: " << (l.callsign.empty() ? "None" : l.callsign) << "\n"
                << "\ttype: " << (l.type.empty() ? "None" : l.type) << "\n"
                << "\tlatitude: ";
            if (std::isnan(l.latitude)) os << "NaN";
            else os << std::fixed << std::setprecision(6) << l.latitude;
            os << "\n"
                << "\tlongitude: ";
            if (std::isnan(l.longitude)) os << "NaN";
            else os << std::fixed << std::setprecision(6) << l.longitude;
            os << "\n";
            return os;
        }

        std::string uid;
        std::string remarks;
        std::string relation;
        std::string callsign;
        std::string type;
        double latitude;
        double longitude;

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr const char* INVALID_STRING = "";
        inline static constexpr std::string_view VALID_TYPE = "a-f-G";
    };

    /// @brief A CoT Message subschema class for customdetail data
    class customdetail {
    public:
        /// @brief Constructor - Initializes everything
        explicit customdetail(std::string name = INVALID_VALUE,
            std::map<std::string, std::string> attributes = {},
            std::string content = INVALID_VALUE,
            std::vector<customdetail> children = {})
            : name(std::move(name)), attributes(std::move(attributes)),
            content(std::move(content)), children(std::move(children)) {}

        /// @brief Equality comparison
        bool operator==(const customdetail& other) const noexcept {
            return name == other.name &&
                attributes == other.attributes &&
                content == other.content &&
                children == other.children;
        }

        /// @brief Inequality comparison
        bool operator!=(const customdetail& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Empty name field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (attributes.empty() && content.empty() && !children.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail,
                                  "No attributes, content, or children" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            for (const auto& child : children) {
                if (!child.is_valid(rslt)) {
                    auto res = result{ result::error_code::InvalidCustomdetail, "Invalid child element" };
                    if (rslt && rslt->is_success()) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Invalid customdetail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child(name.c_str());
            for (const auto& [key, value] : attributes) {
                node.append_attribute(key.c_str()) = value.c_str();
            }
            if (!content.empty()) {
                node.append_child(pugi::node_pcdata).set_value(content.c_str());
            }
            for (const auto& child : children) {
                auto res = child.to_xml(node);
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized customdetail
        static customdetail from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            customdetail detail;
            detail.name = node.name();
            if (detail.name.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Empty node name" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            for (auto attr : node.attributes()) {
                detail.attributes[attr.name()] = attr.as_string();
            }
            detail.content = node.child_value();
            for (auto child_node : node.children()) {
                if (child_node.type() == pugi::node_element) {
                    detail.children.push_back(from_xml(child_node, rslt));
                }
            }
            if (!detail.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidCustomDetail,
                               "Invalid customdetail data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return detail;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const customdetail& detail) {
            os << "\tcustomdetail: " << detail.name;
            if (!detail.is_valid()) os << " -NOT VALID-";
            os << "\n";
            for (const auto& [key, value] : detail.attributes) {
                os << "\t\t" << key << ": " << value << "\n";
            }
            if (!detail.content.empty()) {
                os << "\t\tcontent: " << detail.content << "\n";
            }
            for (const auto& child : detail.children) {
                os << child;
            }
            return os;
        }

        std::string name;
        std::map<std::string, std::string> attributes;
        std::string content;
        std::vector<customdetail> children;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

    /// @brief A CoT Message subschema class for point data
    class point {
    public:
        /// @brief Constructor - Initializes everything
        explicit point(double latitude = INVALID_VALUE,
            double longitude = INVALID_VALUE,
            double hae = INVALID_VALUE,
            double circularError = INVALID_VALUE,
            double linearError = INVALID_VALUE) noexcept
            : latitude(latitude), longitude(longitude), hae(hae),
            circularError(circularError), linearError(linearError) {}

        /// @brief Equality comparison
        bool operator==(const point& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            auto equal = [](double a, double b) {
                return std::isnan(a) && std::isnan(b) ||
                    std::abs(a - b) < EPSILON;
                };
            return equal(latitude, other.latitude) &&
                equal(longitude, other.longitude) &&
                equal(hae, other.hae) &&
                equal(circularError, other.circularError) &&
                equal(linearError, other.linearError);
        }

        /// @brief Inequality comparison
        bool operator!=(const point& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(latitude) || latitude < LAT_MIN || latitude > LAT_MAX) {
                auto res = result{ result::error_code::InvalidPoint,
                                  "Invalid latitude: must be between -90 and 90" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(longitude) || longitude < LON_MIN || longitude > LON_MAX) {
                auto res = result{ result::error_code::InvalidPoint,
                                  "Invalid longitude: must be between -180 and 180" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(circularError) && circularError < 0) {
                auto res = result{ result::error_code::InvalidPoint,
                                  "Circular error cannot be negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(linearError) && linearError < 0) {
                auto res = result{ result::error_code::InvalidPoint,
                                  "Linear error cannot be negative" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidPoint, "Invalid point data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("point");
            node.append_attribute("lat") = latitude;
            node.append_attribute("lon") = longitude;
            if (!std::isnan(hae)) node.append_attribute("hae") = hae;
            if (!std::isnan(circularError)) node.append_attribute("ce") = circularError;
            if (!std::isnan(linearError)) node.append_attribute("le") = linearError;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized point
        static point from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            point p;
            if (node.name() != std::string("point")) {
                auto res = result{ result::error_code::InvalidPoint, "Expected <point> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return p;
            }
            try {
                p.latitude = node.attribute("lat").as_double(INVALID_VALUE);
                p.longitude = node.attribute("lon").as_double(INVALID_VALUE);
                p.hae = node.attribute("hae").as_double(INVALID_VALUE);
                p.circularError = node.attribute("ce").as_double(INVALID_VALUE);
                p.linearError = node.attribute("le").as_double(INVALID_VALUE);
            }
            catch (const std::exception& e) {
                auto res = result{ result::error_code::InvalidPoint,
                                  "Error parsing point attributes: " + std::string(e.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            if (!p.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidPoint, "Invalid point data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return p;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const point& p) {
            os << "point: ";
            if (!p.is_valid()) os << " -NOT VALID- ";
            os << "\n\tlatitude: ";
            if (std::isnan(p.latitude)) os << "NaN"; else os << std::fixed << std::setprecision(6) << p.latitude;
            os << "\n\tlongitude: ";
            if (std::isnan(p.longitude)) os << "NaN"; else os << std::fixed << std::setprecision(6) << p.longitude;
            os << "\n\thae: ";
            if (std::isnan(p.hae)) os << "NaN"; else os << p.hae;
            os << "\n\tce: ";
            if (std::isnan(p.circularError)) os << "NaN"; else os << p.circularError;
            os << "\n\tle: ";
            if (std::isnan(p.linearError)) os << "NaN"; else os << p.linearError;
            os << "\n";
            return os;
        }

        double latitude;
        double longitude;
        double hae;
        double circularError;
        double linearError;

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr double LAT_MIN = -90.0;
        inline static constexpr double LAT_MAX = 90.0;
        inline static constexpr double LON_MIN = -180.0;
        inline static constexpr double LON_MAX = 180.0;
    };

    /// @brief A CoT Message subschema class for detail data
    class detail {
    public:
        /// @brief Constructor - Initializes everything
        explicit detail(takv takv = takv(),
            contact contact = contact(),
            uid uid = uid(),
            model model = model(),
            precisionlocation precisionlocation = precisionlocation(),
            group group = group(),
            status status = status(),
            track track = track(),
            color strokecolor = color(),
            color fillcolor = color(),
            color color = color(),
            usericon usericon = usericon(),
            std::string remarks = "",
            std::vector<link> links = {},
            std::vector<customdetail> custom_details = {})
            : takv_(std::move(takv)),
            contact_(std::move(contact)),
            uid_(std::move(uid)),
            model_(std::move(model)),
            precisionlocation_(std::move(precisionlocation)),
            group_(std::move(group)),
            status_(std::move(status)),
            track_(std::move(track)),
            strokecolor_(std::move(strokecolor)),
            fillcolor_(std::move(fillcolor)),
            color_(std::move(color)),
            usericon_(std::move(usericon)),
            remarks(std::move(remarks)),
            links(std::move(links)),
            custom_details(std::move(custom_details)) {}

        /// @brief Equality comparison
        bool operator==(const detail& other) const noexcept {
            return takv_ == other.takv_ &&
                contact_ == other.contact_ &&
                uid_ == other.uid_ &&
                model_ == other.model_ &&
                precisionlocation_ == other.precisionlocation_ &&
                group_ == other.group_ &&
                status_ == other.status_ &&
                track_ == other.track_ &&
                strokecolor_ == other.strokecolor_ &&
                fillcolor_ == other.fillcolor_ &&
                color_ == other.color_ &&
                usericon_ == other.usericon_ &&
                remarks == other.remarks &&
                links == other.links &&
                custom_details == other.custom_details;
        }

        /// @brief Inequality comparison
        bool operator!=(const detail& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!contact_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid contact data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!group_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid group data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            for (const auto& link : links) {
                if (!link.is_valid()) {
                    auto res = result{ result::error_code::InvalidDetail, "Invalid link data" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            for (const auto& custom : custom_details) {
                if (!custom.is_valid()) {
                    auto res = result{ result::error_code::InvalidDetail, "Invalid custom detail data" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid detail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("detail");
            if (takv_.is_valid()) {
                auto res = takv_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (contact_.is_valid()) {
                auto res = contact_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (uid_.is_valid()) {
                auto res = uid_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (model_.is_valid()) {
                auto res = model_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (precisionlocation_.is_valid()) {
                auto res = precisionlocation_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (group_.is_valid()) {
                auto res = group_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (status_.is_valid()) {
                auto res = status_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (track_.is_valid()) {
                auto res = track_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (strokecolor_.is_valid()) {
                auto res = strokecolor_.to_xml(node, "strokecolor");
                if (!res.is_success()) return res;
            }
            if (fillcolor_.is_valid()) {
                auto res = fillcolor_.to_xml(node, "fillcolor");
                if (!res.is_success()) return res;
            }
            if (color_.is_valid()) {
                auto res = color_.to_xml(node, "color");
                if (!res.is_success()) return res;
            }
            if (usericon_.is_valid()) {
                auto res = usericon_.to_xml(node);
                if (!res.is_success()) return res;
            }
            if (!remarks.empty()) {
                node.append_child("remarks").append_child(pugi::node_pcdata).set_value(remarks.c_str());
            }
            for (const auto& link : links) {
                auto res = link.to_xml(node);
                if (!res.is_success()) return res;
            }
            for (const auto& custom : custom_details) {
                auto res = custom.to_xml(node);
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized detail
        static detail from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            detail d;
            if (node.name() != std::string("detail")) {
                auto res = result{ result::error_code::InvalidDetail, "Expected <detail> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return d;
            }
            for (auto child : node.children()) {
                std::string name = child.name();
                try {
                    if (name == "takv") d.takv_ = takv::from_xml(child, rslt);
                    else if (name == "contact") d.contact_ = contact::from_xml(child, rslt);
                    else if (name == "uid") d.uid_ = uid::from_xml(child, rslt);
                    else if (name == "model") d.model_ = model::from_xml(child, rslt);
                    else if (name == "precisionlocation") d.precisionlocation_ = precisionlocation::from_xml(child, rslt);
                    else if (name == "__group") d.group_ = group::from_xml(child, rslt);
                    else if (name == "status") d.status_ = status::from_xml(child, rslt);
                    else if (name == "track") d.track_ = track::from_xml(child, rslt);
                    else if (name == "strokecolor") d.strokecolor_ = color::from_xml(child, rslt, "strokecolor");
                    else if (name == "fillcolor") d.fillcolor_ = color::from_xml(child, rslt, "fillcolor");
                    else if (name == "color") d.color_ = color::from_xml(child, rslt, "color");
                    else if (name == "usericon") d.usericon_ = usericon::from_xml(child, rslt);
                    else if (name == "remarks") d.remarks = child.child_value();
                    else if (name == "link") d.links.push_back(link::from_xml(child, rslt));
                    else if (!name.empty()) d.custom_details.push_back(customdetail::from_xml(child, rslt));
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidDetail,
                                      "Error parsing <" + name + ">: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                }
            }
            if (!d.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidDetail, "Invalid detail data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return d;
        }

        /// @brief Adds a new custom detail
        /// @param name XML tag name
        /// @param attributes XML attributes
        /// @param content Inner text or CDATA
        /// @return Result indicating success or failure
        result add_custom_detail(std::string name,
            std::map<std::string, std::string> attributes = {},
            std::string content = "") {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            customdetail cd(std::move(name), std::move(attributes), std::move(content));
            if (!cd.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid custom detail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            if (std::any_of(custom_details.begin(), custom_details.end(),
                [&cd](const auto& d) { return d.name == cd.name; })) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail name already exists" };
                if (error_handler) error_handler(res);
                return res;
            }
            custom_details.push_back(std::move(cd));
            return result{};
        }

        /// @brief Modifies an existing custom detail
        /// @param name XML tag name to match
        /// @param matchAttributes Attributes to match
        /// @param matchContent Content to match
        /// @param newAttributes New attributes
        /// @param newContent New content
        /// @return Result indicating success or failure
        result modify_custom_detail(std::string name,
            const std::map<std::string, std::string>& matchAttributes,
            std::string matchContent,
            std::map<std::string, std::string> newAttributes,
            std::string newContent) {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto it = std::find_if(custom_details.begin(), custom_details.end(),
                [&name, &matchAttributes, &matchContent](const customdetail& detail) {
                    bool nameMatch = detail.name == name;
                    bool attrMatch = matchAttributes.empty() || detail.attributes == matchAttributes;
                    bool contentMatch = matchContent.empty() || detail.content == matchContent;
                    return nameMatch && attrMatch && contentMatch;
                });
            if (it == custom_details.end()) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail not found" };
                if (error_handler) error_handler(res);
                return res;
            }
            it->attributes = std::move(newAttributes);
            it->content = std::move(newContent);
            return result{};
        }

        /// @brief Adds or modifies a custom detail
        /// @param name XML tag name
        /// @param attributes XML attributes
        /// @param content Inner text or CDATA
        /// @param matchAttributes Attributes to match for modification
        /// @param matchContent Content to match for modification
        /// @return Result indicating success or failure
        result add_or_modify_custom_detail(std::string name,
            std::map<std::string, std::string> attributes,
            std::string content,
            const std::map<std::string, std::string>& matchAttributes = {},
            std::string matchContent = "") {
            auto mod_res = modify_custom_detail(name, matchAttributes, matchContent,
                attributes, content);
            if (mod_res.is_success()) return mod_res;
            return add_custom_detail(std::move(name), std::move(attributes), std::move(content));
        }

        /// @brief Removes a custom detail
        /// @param name XML tag name to match
        /// @param attributes Attributes to match
        /// @param content Content to match
        /// @return Result indicating success or failure
        result remove_custom_detail(std::string name,
            const std::map<std::string, std::string>& attributes = {},
            std::string content = "") {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidDetail, "Empty custom detail name" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto it = std::find_if(custom_details.begin(), custom_details.end(),
                [&name, &attributes, &content](const customdetail& detail) {
                    bool nameMatch = detail.name == name;
                    bool attrMatch = attributes.empty() || detail.attributes == attributes;
                    bool contentMatch = content.empty() || detail.content == content;
                    return nameMatch && attrMatch && contentMatch;
                });
            if (it == custom_details.end()) {
                auto res = result{ result::error_code::InvalidDetail, "Custom detail not found" };
                if (error_handler) error_handler(res);
                return res;
            }
            custom_details.erase(it);
            return result{};
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const detail& d) {
            os << "detail: ";
            if (!d.is_valid()) os << " -NOT VALID- ";
            os << "\n"
                << d.takv_
                << d.contact_
                << d.uid_
                << d.model_
                << d.precisionlocation_
                << d.group_
                << d.status_
                << d.track_
                << d.strokecolor_
                << d.fillcolor_
                << d.color_
                << d.usericon_
                << "\tremarks: " << (d.remarks.empty() ? "None" : d.remarks) << "\n"
                << "\tlinks:\n";
            if (d.links.empty()) os << "\t\tNone\n";
            else for (const auto& link : d.links) os << link;
            os << "\tcustom_details:\n";
            if (d.custom_details.empty()) os << "\t\tNone\n";
            else for (const auto& custom : d.custom_details) os << custom;
            return os;
        }

        takv takv_;
        contact contact_;
        uid uid_;
        model model_;
        precisionlocation precisionlocation_;
        group group_;
        status status_;
        track track_;
        color strokecolor_;
        color fillcolor_;
        color color_;
        usericon usericon_;
        std::string remarks;
        std::vector<link> links;
        std::vector<customdetail> custom_details;
    };

    /// @brief A CoT Message subschema class for event data
    class event {
    public:
        /// @brief Constructor - Initializes everything
        explicit event(double version = INVALID_VERSION,
            std::string uid = INVALID_STRING,
            std::string type = INVALID_STRING,
            datetime time = datetime(),
            datetime start = datetime(),
            datetime stale = datetime(),
            std::string how = INVALID_STRING,
            point p = point(),
            detail d = detail()) noexcept
            : version(version),
            uid(std::move(uid)),
            type(std::move(type)),
            time(std::move(time)),
            start(std::move(start)),
            stale(std::move(stale)),
            how(std::move(how)),
            rootType(Root::Type::Unknown),
            indicator(Point::Type::Unknown),
            location(Location::Type::Unknown),
            howEntry(How::Entry::Type::Unknown),
            howData(How::Data::Type::Unknown),
            point_(std::move(p)),
            detail_(std::move(d)) {}

        /// @brief Equality comparison
        bool operator==(const event& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            bool versionEqual = std::isnan(version) && std::isnan(other.version) ||
                std::abs(version - other.version) < EPSILON;
            return versionEqual &&
                uid == other.uid &&
                type == other.type &&
                time == other.time &&
                start == other.start &&
                stale == other.stale &&
                how == other.how &&
                rootType == other.rootType &&
                indicator == other.indicator &&
                location == other.location &&
                howEntry == other.howEntry &&
                howData == other.howData &&
                point_ == other.point_ &&
                detail_ == other.detail_;
        }

        /// @brief Inequality comparison
        bool operator!=(const event& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(version) || version < 1.0 || version > 2.0) {
                auto res = result{ result::error_code::InvalidEvent, "Invalid version" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (uid.empty()) {
                auto res = result{ result::error_code::InvalidEvent, "Empty UID" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (type.empty()) {
                auto res = result{ result::error_code::InvalidType, "Empty type" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!time.is_valid()) {
                auto res = result{ result::error_code::InvalidTime, "Invalid time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!start.is_valid()) {
                auto res = result{ result::error_code::InvalidTime, "Invalid start time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!stale.is_valid()) {
                auto res = result{ result::error_code::InvalidTime, "Invalid stale time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (how.empty()) {
                auto res = result{ result::error_code::InvalidHow, "Empty how" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!point_.is_valid()) {
                auto res = result{ result::error_code::InvalidPoint, "Invalid point" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!detail_.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid detail" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidEvent, "Invalid event data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("event");
            node.append_attribute("version") = version;
            node.append_attribute("uid") = uid.c_str();
            node.append_attribute("type") = type.c_str();
            node.append_attribute("time") = time.to_cot_timestamp().c_str();
            node.append_attribute("start") = start.to_cot_timestamp().c_str();
            node.append_attribute("stale") = stale.to_cot_timestamp().c_str();
            node.append_attribute("how") = how.c_str();
            auto res = point_.to_xml(node);
            if (!res.is_success()) return res;
            res = detail_.to_xml(node);
            if (!res.is_success()) return res;
            return result{};
        }

        /// @brief Serializes to XML string
        /// @param rslt Optional result for error details
        /// @return XML string or empty if invalid
        std::string to_xml_string(result* rslt = nullptr) const {
            pugi::xml_document doc;
            auto res = to_xml(doc);
            if (!res.is_success()) {
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return "";
            }
            return xml_node_to_string(doc.child("event"), rslt);
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized event
        static event from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            event e;
            if (node.name() != std::string("event")) {
                auto res = result{ result::error_code::InvalidEvent, "Expected <event> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return e;
            }
            try {
                e.version = node.attribute("version").as_double(INVALID_VERSION);
                e.uid = node.attribute("uid").as_string();
                e.type = node.attribute("type").as_string();
                e.time = datetime::from_cot_timestamp(node.attribute("time").as_string());
                e.start = datetime::from_cot_timestamp(node.attribute("start").as_string());
                e.stale = datetime::from_cot_timestamp(node.attribute("stale").as_string());
                e.how = node.attribute("how").as_string();
                for (const auto& child : node.children()) {
                    std::string name = child.name();
                    if (name == "point") e.point_ = point::from_xml(child, rslt);
                    else if (name == "detail") e.detail_ = detail::from_xml(child, rslt);
                }
                if (!e.is_valid(rslt) && rslt && rslt->is_success()) {
                    *rslt = result{ result::error_code::InvalidEvent, "Invalid event data after parsing" };
                    if (error_handler) error_handler(*rslt);
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidEvent,
                                  "Error parsing event: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return e;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const event& e) {
            os << "event: ";
            if (!e.is_valid()) os << " -NOT VALID- ";
            os << "\n\tversion: " << (std::isnan(e.version) ? "NaN" : std::to_string(e.version))
                << "\n\ttype: " << (e.type.empty() ? "None" : e.type)
                << "\n\trootType: " << static_cast<int>(e.rootType) << " - "
                << Root::TypeToString.at(e.rootType)
                << "\n\tindicator: " << static_cast<int>(e.indicator) << " - "
                << Point::TypeToString.at(e.indicator)
                << "\n\tlocation: " << static_cast<int>(e.location) << " - "
                << Location::TypeToString.at(e.location)
                << "\n\tuid: " << (e.uid.empty() ? "None" : e.uid)
                << "\n\ttime: " << e.time
                << "\n\tstart: " << e.start
                << "\n\tstale: " << e.stale
                << "\n\thow: " << (e.how.empty() ? "None" : e.how)
                << "\n\thowEntry: " << static_cast<int>(e.howEntry) << " - "
                << How::Entry::TypeToString.at(e.howEntry)
                << "\n\thowData: " << static_cast<int>(e.howData) << " - "
                << How::Data::TypeToString.at(e.howData)
                << "\n" << e.point_ << e.detail_;
            return os;
        }

        /// @brief Getter for point
        [[nodiscard]] const point& get_point() const noexcept { return point_; }

        /// @brief Getter for detail
        [[nodiscard]] const detail& get_detail() const noexcept { return detail_; }

        /// @brief Setter for point
        result set_point(point newPoint) noexcept {
            if (!newPoint.is_valid()) {
                auto res = result{ result::error_code::InvalidPoint, "Cannot set invalid point" };
                if (error_handler) error_handler(res);
                return res;
            }
            point_ = std::move(newPoint);
            return result{};
        }

        /// @brief Setter for detail
        result set_detail(detail newDetail) noexcept {
            if (!newDetail.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Cannot set invalid detail" };
                if (error_handler) error_handler(res);
                return res;
            }
            detail_ = std::move(newDetail);
            return result{};
        }

        /// @brief Clears point
        void clear_point() noexcept { point_ = point(); }

        /// @brief Clears detail
        void clear_detail() noexcept { detail_ = detail(); }

        double version;
        std::string uid;
        std::string type;
        datetime time;
        datetime start;
        datetime stale;
        std::string how;
        Root::Type rootType;
        Point::Type indicator;
        Location::Type location;
        How::Entry::Type howEntry;
        How::Data::Type howData;
        point point_;
        detail detail_;

    private:
        inline static constexpr double INVALID_VERSION = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr const char* INVALID_STRING = "";
    };

    /// @brief A root XML CoT message schema class
    class message {
    public:
        /// @brief Constructor - Initializes everything
        explicit message(event e = event(),
            std::string xml_version = "1.0",
            std::string xml_encoding = "utf-8",
            std::string xml_standalone = "yes") noexcept
            : event_(std::move(e)),
            xml_version(std::move(xml_version)),
            xml_encoding(std::move(xml_encoding)),
            xml_standalone(std::move(xml_standalone)) {}

        /// @brief Equality comparison
        bool operator==(const message& other) const noexcept {
            return event_ == other.event_ &&
                xml_version == other.xml_version &&
                xml_encoding == other.xml_encoding &&
                xml_standalone == other.xml_standalone;
        }

        /// @brief Inequality comparison
        bool operator!=(const message& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (xml_version != "1.0") {
                auto res = result{ result::error_code::InvalidXml, "XML version must be 1.0" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (xml_encoding != "utf-8") {
                auto res = result{ result::error_code::InvalidXml, "XML encoding must be utf-8" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (xml_standalone != "yes" && xml_standalone != "no") {
                auto res = result{ result::error_code::InvalidXml, "XML standalone must be yes or no" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!event_.is_valid()) {
                auto res = result{ result::error_code::InvalidEvent, "Invalid event" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node (typically document)
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidEvent, "Invalid message data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto decl = parent.append_child(pugi::node_declaration);
            decl.append_attribute("version") = xml_version.c_str();
            decl.append_attribute("encoding") = xml_encoding.c_str();
            decl.append_attribute("standalone") = xml_standalone.c_str();
            return event_.to_xml(parent);
        }

        /// @brief Serializes to XML string
        /// @param rslt Optional result for error details
        /// @return XML string or empty if invalid
        std::string to_xml_string(result* rslt = nullptr) const {
            pugi::xml_document doc;
            auto res = to_xml(doc);
            if (!res.is_success()) {
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return "";
            }
            return xml_document_to_string(doc, true, rslt);
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML document node
        /// @param rslt Optional result for error details
        /// @return Deserialized message
        static message from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            message m;
            try {
                if (auto decl = node.child(pugi::node_declaration)) {
                    m.xml_version = decl.attribute("version").as_string("1.0");
                    m.xml_encoding = decl.attribute("encoding").as_string("utf-8");
                    m.xml_standalone = decl.attribute("standalone").as_string("yes");
                }
                if (auto event_node = node.child("event")) {
                    m.event_ = event::from_xml(event_node, rslt);
                }
                else {
                    auto res = result{ result::error_code::InvalidEvent, "No <event> node found" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                }
                if (!m.is_valid(rslt) && rslt && rslt->is_success()) {
                    *rslt = result{ result::error_code::InvalidEvent, "Invalid message data after parsing" };
                    if (error_handler) error_handler(*rslt);
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidEvent,
                                  "Error parsing message: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return m;
        }

        /// @brief Deserializes from an XML string
        /// @param xml The XML string to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized message
        static message from_xml_string(std::string_view xml, result* rslt = nullptr) {
            if (xml.empty()) {
                auto res = result{ result::error_code::InvalidInput, "Empty XML string" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return message();
            }
            pugi::xml_document doc;
            pugi::xml_parse_result parse_result = doc.load_buffer(xml.data(), xml.size());
            if (!parse_result) {
                auto res = result{ result::error_code::InvalidXml,
                                  std::string(parse_result.description()) + " at offset " +
                                  std::to_string(parse_result.offset) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return message();
            }
            return from_xml(doc, rslt);
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const message& m) {
            os << "message: ";
            if (!m.is_valid()) os << " -NOT VALID- ";
            os << "\n\txml_version: " << (m.xml_version.empty() ? "None" : m.xml_version)
                << "\n\txml_encoding: " << (m.xml_encoding.empty() ? "None" : m.xml_encoding)
                << "\n\txml_standalone: " << (m.xml_standalone.empty() ? "None" : m.xml_standalone)
                << "\n" << m.event_;
            return os;
        }

        event event_;
        std::string xml_version;
        std::string xml_encoding;
        std::string xml_standalone;

    private:
        inline static constexpr const char* INVALID_STRING = "";
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
            return result{ result::error_code::InvalidInput, "Empty input buffer" };
        }

        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_buffer(buffer.data(), buffer.size());
        if (!parse_result) {
            return result{ result::error_code::InvalidXml, parse_result.description() };
        }

        return result{ result::error_code::Success, "Successfully verified XML" };
    }

    /// @brief Validate that a buffer is a valid cot structure
    /// @param buffer buffer containing the cot message
    /// @param result optional result structure to indicate reason cot may be invalid
    /// @param document optional document to get the loaded document from buffer.
    /// @return true if valid, else false
    [[nodiscard]] bool validate_cot_structure(std::string_view buffer, result* rslt = nullptr, pugi::xml_document& document = nullptr) {
        // Parse XML
        pugi::xml_document doc;
        pugi::xml_parse_result xmlresult = doc.load_string(buffer);
        if (!xmlresult) {
            if (rslt) { *rslt = result(result::error_code::ProcessingError); }
            return false;
        }

        // Set the document if one is passed in. 
        if (document) { document = doc; }

        // Validate structure
        pugi::xml_node eventNode = doc.child("event");
        if (!eventNode || doc.root().select_nodes("event").size() != 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidEvent); }
            return false;
        }
        if (eventNode.select_nodes("point").size() != 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidPoint); }
            return false;
        }
        if (eventNode.select_nodes("detail").size() > 1) {
            if (rslt) { *rslt = result(result::error_code::InvalidDetail); }
            return false;
        }

        if (rslt) { *rslt = result(result::error_code::Success); }
        return true;
    }

    /// @brief prep an xml buffer to be parsed, will clear content before the starting xml tag
    /// @param xml string view containtaining the xml
    /// @return result indicating success or failure with description
    result prep_xml_message(std::string& xml)
    {
        if (xml.empty()) {
            return result{ result::error_code::InvalidInput, "Empty input buffer" };
        }

        size_t position = xml.find("<?xml");
        if (position == std::string::npos) {
            return result(result::error_code::InvalidXml);
        }
        xml.erase(0, position);

        // Verify XML structure
        result rslt = verify_xml(xml);
        if (rslt.is_failed()) {
            return rslt;
        }

        return result(result::error_code::Success);
    }

    /// @brief Create an XML CoT message from a schema
    /// @param cot Schema to convert to XML
    /// @param xml std::string contianing xml text, xml is empty if error occurs
    /// @return result indicating success or failure with description
    [[nodiscard]] result generate_xml_cot(const message& cot, std::string& xml) {
        // Validate schema
        std::string error;
        result rslt;
        auto validation_result = cot.is_valid(&rslt);
        if (!validation_result) {
            return result{ result::error_code::InvalidData, "Invalid cot schema: " + error };
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
        std::string& response) {
        // Create XML document to load in the received
        pugi::xml_document doc;
        pugi::xml_parse_result res = doc.load_string(received.str());

        if (res) {
            // Modified flag
            bool modified = false;

            // Find the 'status' node
            pugi::xml_node statusNode = doc.select_node("//status").node();

            if (statusNode) {
                // Check if acknowledgment attribute doesn't exist
                if (!statusNode.attribute("acknowledgment")) {
                    // Add acknowledgment attribute with value "ack"
                    statusNode.append_attribute("acknowledgment").set_value("ack");
                    modified = true;
                }
            }

            // If modified, save the modified XML string and return success
            if (modified) {
                std::stringstream modifiedXmlStream;
                doc.save(modifiedXmlStream);
                response = modifiedXmlStream.str();
                return result();
            }

            // No modification made
            return result(result::error_code::NoModificationMade);
        }

        return result(result::error_code::ProcessingError);
    }

    /// @brief Parse a CoT message from a string buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(std::string_view buffer, message& cot) {
        // Convert input buffer to string and remove garbage before <?xml
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML
        pugi::xml_document doc;
        if (!validate_cot_structure(buffer, rslt, doc)) {
            return rslt;
        }

        // Parse event
        cot.event_ = event::from_xml(eventNode);

        // Validate parsed data
        if (!cot.is_valid(&rslt)) {
            return rslt;
        }

        return result(result::error_code::Success);
    }

    /// @brief Parse a CoT message from a character buffer
    /// @param buffer Input buffer containing XML data
    /// @param cot Output message schema to store parsed data
    /// @return Result indicating success or failure with description
    [[nodiscard]] result parse_cot(const char* buffer, message& cot) {
        return parse_cot(std::string_view(buffer), cot);
    }

    /// @brief Parse the Event element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param event Output event instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_event_from_cot(std::string_view buffer, event& e) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event>
        pugi::xml_node eventNode = doc.child("event");
        e = event::from_xml(eventNode);

        // Success
        return result();
    }

    /// @brief Parse the Point element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param point Output point instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_point_from_cot(std::string_view buffer, point& p) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <point>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node pointNode = eventNode.child("point");
        if (!pointNode) {
            return result(result::error_code::InsufficientData);
        }
        p = point::from_xml(pointNode);

        // Success
        return result();
    }

    /// @brief Parse the detail element from a CoT message
    /// @param buffer Input buffer containing XML data
    /// @param detail parsed detail instance
    /// @return result indicating success or failure with description
    [[nodiscard]] result parse_detail_from_cot(std::string_view buffer, detail& detail) {
        // Prep message for parsing
        std::string xmlBuffer(buffer);
        result rslt = prep_xml_message(xmlBuffer);

        if (rslt.is_failed()) {
            return rslt;
        }

        // Parse XML document
        pugi::xml_document doc;
        pugi::xml_parse_result xmlResult = doc.load_string(xmlBuffer.c_str());
        if (!xmlResult) {
            return result(result::error_code::ProcessingError);
        }

        // Check for exactly one <event> node
        int eventsSize = (int)doc.root().select_nodes("event").size();
        if (eventsSize != 1) {
            return result(result::error_code::InvalidEvent);
        }

        // Navigate to <event> and <detail>
        pugi::xml_node eventNode = doc.child("event");
        pugi::xml_node detailNode = eventNode.child("detail");
        if (!detailNode) {
            return result(result::error_code::InsufficientData);
        }
        detail = detail::from_xml(detailNode);

        // Success
        return result();
    }

#pragma endregion
}