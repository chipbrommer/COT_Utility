#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            inclination.hpp
// @brief           A CoT Message subschema class for inclination data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <exception>
#include <limits>
#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for inclination data
    class inclination {
    public:
        /// @brief Constructor - Initializes everything
        explicit inclination(double value = INVALID_NUM) noexcept : value(value) {}

        /// @brief Equality comparison
        bool operator==(const inclination& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return (std::isnan(value) && std::isnan(other.value)) ||
                (!std::isnan(value) && !std::isnan(other.value) &&
                    std::abs(value - other.value) < EPSILON);
        }

        /// @brief Inequality comparison
        bool operator!=(const inclination& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(value)) {
                auto res = result{ result::error_code::InvalidInclination, "Inclination value is unset" };
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
                auto res = result{ result::error_code::InvalidInclination, "Invalid inclination data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("inclination");
            node.append_attribute("value") = value;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized inclination
        static inclination from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            inclination inc;
            if (node.name() != std::string("inclination")) {
                auto res = result{ result::error_code::InvalidInclination, "Expected <inclination> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return inc;
            }
            if (auto attr = node.attribute("value")) {
                try {
                    inc.value = std::stod(attr.as_string());
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidInclination,
                                      "Error parsing inclination value: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    inc.value = INVALID_NUM;
                }
            }
            if (!inc.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidInclination,
                                  "Invalid inclination data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return inc;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const inclination& inc) {
            os << "inclination: ";
            if (!inc.is_valid()) os << " -NOT VALID- (Unset)";
            else os << "\n\tvalue: " << inc.value;
            os << "\n";
            return os;
        }

        double value{ INVALID_NUM };

    private:
        inline static constexpr double INVALID_NUM = std::numeric_limits<double>::quiet_NaN();
    };
}