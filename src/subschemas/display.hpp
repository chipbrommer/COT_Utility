#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            display.hpp
// @brief           A CoT Message subschema class for display settings data.
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
    /// @brief A CoT Message subschema class for display settings data
    class display {
    public:
        /// @brief Constructor - Initializes everything
        explicit display(double maxzoom = INVALID_NUM,
            double minzoom = INVALID_NUM,
            bool rotate = false,
            bool has_rotate = false)
            : maxzoom(maxzoom), minzoom(minzoom), rotate(rotate), has_rotate(has_rotate) {
        }

        /// @brief Equality comparison
        bool operator==(const display& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            bool max_eq = (std::isnan(maxzoom) && std::isnan(other.maxzoom)) ||
                (!std::isnan(maxzoom) && !std::isnan(other.maxzoom) &&
                    std::abs(maxzoom - other.maxzoom) < EPSILON);
            bool min_eq = (std::isnan(minzoom) && std::isnan(other.minzoom)) ||
                (!std::isnan(minzoom) && !std::isnan(other.minzoom) &&
                    std::abs(minzoom - other.minzoom) < EPSILON);
            return max_eq && min_eq && rotate == other.rotate && has_rotate == other.has_rotate;
        }

        /// @brief Inequality comparison
        bool operator!=(const display& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!std::isnan(maxzoom) && maxzoom < 0.0) {
                auto res = result{ result::error_code::InvalidDisplay,
                                  "Invalid maxzoom: must be 0 or greater" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(minzoom) && minzoom < 0.0) {
                auto res = result{ result::error_code::InvalidDisplay,
                                  "Invalid minzoom: must be 0 or greater" };
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
                auto res = result{ result::error_code::InvalidDisplay, "Invalid display data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("display");
            if (!std::isnan(maxzoom)) node.append_attribute("maxzoom") = maxzoom;
            if (!std::isnan(minzoom)) node.append_attribute("minzoom") = minzoom;
            if (has_rotate) node.append_attribute("rotate") = rotate;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized display
        static display from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            display d;
            if (node.name() != std::string("display")) {
                auto res = result{ result::error_code::InvalidDisplay, "Expected <display> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return d;
            }
            if (auto attr = node.attribute("maxzoom")) {
                try {
                    d.maxzoom = std::stod(attr.as_string());
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidDisplay,
                                      "Invalid maxzoom format: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    d.maxzoom = INVALID_NUM;
                }
            }
            if (auto attr = node.attribute("minzoom")) {
                try {
                    d.minzoom = std::stod(attr.as_string());
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidDisplay,
                                      "Invalid minzoom format: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    d.minzoom = INVALID_NUM;
                }
            }
            if (auto attr = node.attribute("rotate")) {
                d.has_rotate = true;
                std::string val = attr.as_string();
                d.rotate = (val == "true" || val == "1");
            }
            if (!d.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidDisplay,
                                  "Invalid display data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return d;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const display& d) {
            os << "display: ";
            if (!d.is_valid()) os << " -NOT VALID- ";
            os << "\n\tmaxzoom: ";
            if (std::isnan(d.maxzoom)) os << "None";
            else os << d.maxzoom;
            os << "\n\tminzoom: ";
            if (std::isnan(d.minzoom)) os << "None";
            else os << d.minzoom;
            os << "\n\trotate: " << (d.has_rotate ? (d.rotate ? "true" : "false") : "None") << "\n";
            return os;
        }

        double maxzoom{ INVALID_NUM };
        double minzoom{ INVALID_NUM };
        bool rotate{ false };
        bool has_rotate{ false };

    private:
        inline static constexpr double INVALID_NUM = std::numeric_limits<double>::quiet_NaN();
    };
}