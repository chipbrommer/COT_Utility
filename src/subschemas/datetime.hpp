#pragma once
namespace cot::subschemas
{    
    /// @brief A CoT Message subschema class for date and time data
    class datetime : public date, public time {
    public:
        /// @brief Constructor - Initializes everything
        datetime(unsigned year = 0, unsigned month = 0, unsigned day = 0,
            unsigned hour = 0, unsigned minute = 0, double second = 0.0)
            : date(year, month, day), time(hour, minute, second) {
        }

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
}