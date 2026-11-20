#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            date.hpp
// @brief           A CoT Message subschema class for date data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for date data
    class date {
    public:
        /// @brief Constructor - Initializes everything
        date(unsigned year = 0, unsigned month = 0, unsigned day = 0)
            : year(year), month(month), day(day) {
        }

        /// @brief Checks if the date is valid
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (year < 1970) {
                auto res = result{ result::error_code::InvalidDate, "Year must be 1970 or later" };
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

        unsigned int year{ 0 };
        unsigned int month{ 0 };
        unsigned int day{ 0 };

    private:
        unsigned days_in_month() const noexcept {
            static constexpr unsigned days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            if (month < 1 || month > 12) return 0;
            if (month == 2 && is_leap_year()) return 29;
            return days[month - 1];
        }

        bool is_leap_year() const noexcept {
            return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        }
    };
}