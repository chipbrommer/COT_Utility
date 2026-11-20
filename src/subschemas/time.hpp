#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            time.hpp
// @brief           A CoT Message subschema class for time data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for time data
    class time {
    public:
        /// @brief Constructor - Initializes everything
        time(unsigned hour = 0, unsigned minute = 0, double second = 0.0) noexcept
            : hour(hour), minute(minute), second(second) {
        }

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
            if (std::isnan(second) || !std::isfinite(second) || second < 0.0 || second >= 60.0) {
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
            const bool sec_eq = (std::isnan(second) && std::isnan(other.second)) ||
                (!std::isnan(second) && !std::isnan(other.second) &&
                    std::abs(second - other.second) < EPSILON);
            return hour == other.hour && minute == other.minute && sec_eq;
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

        unsigned int hour{ 0 };
        unsigned int minute{ 0 };
        double second{ 0.0 };
    };
}