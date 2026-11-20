#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            spatial.hpp
// @brief           A CoT Message subschema class for <spatial> orientation and spin data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
    /// @brief Represents 3D attitude or rotational spin attributes inside <spatial>
    struct spatial_vector {
        inline static constexpr double INVALID_NUMBER = std::numeric_limits<double>::quiet_NaN();

        double pitch{ INVALID_NUMBER }; ///< Pitch angle (deg) or pitch rate (deg/s)
        double roll{ INVALID_NUMBER };  ///< Roll angle (deg) or roll rate (deg/s)
        double yaw{ INVALID_NUMBER };   ///< Optional yaw angle (deg) or yaw rate (deg/s)
        double ePitch{ INVALID_NUMBER };///< Optional pitch estimation error
        double eRoll{ INVALID_NUMBER }; ///< Optional roll estimation error
        double eYaw{ INVALID_NUMBER };  ///< Optional yaw estimation error

        /// @brief Equality comparison using a small floating-point tolerance
        bool operator==(const spatial_vector& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            auto near_or_nan = [EPSILON](double a, double b) noexcept {
                return (std::isnan(a) && std::isnan(b)) ||
                    (std::isfinite(a) && std::isfinite(b) && std::abs(a - b) < EPSILON);
                };
            return near_or_nan(pitch, other.pitch) &&
                near_or_nan(roll, other.roll) &&
                near_or_nan(yaw, other.yaw) &&
                near_or_nan(ePitch, other.ePitch) &&
                near_or_nan(eRoll, other.eRoll) &&
                near_or_nan(eYaw, other.eYaw);
        }

        /// @brief Inequality comparison
        bool operator!=(const spatial_vector& other) const noexcept { return !(*this == other); }

        /// @brief Checks whether any vector attributes are populated
        [[nodiscard]] bool has_data() const noexcept {
            return !std::isnan(pitch) || !std::isnan(roll) || !std::isnan(yaw) ||
                !std::isnan(ePitch) || !std::isnan(eRoll) || !std::isnan(eYaw);
        }

        /// @brief Validates required and optional numeric fields
        /// @param element_name Tag name used in error messages
        /// @param rslt Optional result structure for error details
        /// @return True if valid, false otherwise
        bool is_valid(std::string_view element_name, result* rslt = nullptr) const {
            if (!has_data()) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Empty <" + std::string(element_name) + "> data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(pitch) || !std::isfinite(pitch)) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Missing or non-finite pitch in <" + std::string(element_name) + ">" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (std::isnan(roll) || !std::isfinite(roll)) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Missing or non-finite roll in <" + std::string(element_name) + ">" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            auto check_optional = [&](double val, std::string_view attr_name) -> bool {
                if (!std::isnan(val) && !std::isfinite(val)) {
                    auto res = result{ result::error_code::InvalidSpatial,
                                      "Non-finite " + std::string(attr_name) + " in <" +
                                      std::string(element_name) + ">" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                return true;
                };
            return check_optional(yaw, "yaw") &&
                check_optional(ePitch, "ePitch") &&
                check_optional(eRoll, "eRoll") &&
                check_optional(eYaw, "eYaw");
        }

        /// @brief Serializes this vector into a child XML element under parent
        result to_xml(pugi::xml_node& parent, std::string_view tag_name) const {
            result val_res;
            if (!is_valid(tag_name, &val_res)) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  val_res.description().empty()
                                      ? "Invalid <" + std::string(tag_name) + "> data"
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            std::string tag_str(tag_name);
            auto node = parent.append_child(tag_str.c_str());
            node.append_attribute("pitch") = pitch;
            node.append_attribute("roll") = roll;
            if (!std::isnan(yaw)) node.append_attribute("yaw") = yaw;
            if (!std::isnan(ePitch)) node.append_attribute("ePitch") = ePitch;
            if (!std::isnan(eRoll)) node.append_attribute("eRoll") = eRoll;
            if (!std::isnan(eYaw)) node.append_attribute("eYaw") = eYaw;
            return result{};
        }

        /// @brief Parses a numeric attribute from an XML node
        static bool parse_number_attr(const pugi::xml_node& node,
            std::string_view attr_name,
            double& out_val,
            result* rslt) {
            std::string attr_str(attr_name);
            auto attr = node.attribute(attr_str.c_str());
            if (!attr) return true;

            std::string raw = attr.as_string();
            if (raw.empty()) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Empty numeric attribute '" + attr_str + "'" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }

            try {
                std::size_t idx = 0;
                double parsed = std::stod(raw, &idx);
                if (idx != raw.size() || !std::isfinite(parsed)) {
                    auto res = result{ result::error_code::InvalidSpatial,
                                      "Invalid numeric attribute '" + attr_str + "': " + raw };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                out_val = parsed;
                return true;
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Failed to parse '" + attr_str + "': " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
        }

        /// @brief Deserializes from an XML element
        static spatial_vector from_xml(const pugi::xml_node& node,
            std::string_view expected_tag,
            result* rslt = nullptr) {
            spatial_vector vec;
            if (node.name() != expected_tag) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  "Expected <" + std::string(expected_tag) + "> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return vec;
            }

            if (!parse_number_attr(node, "pitch", vec.pitch, rslt)) return vec;
            if (!parse_number_attr(node, "roll", vec.roll, rslt)) return vec;
            if (!parse_number_attr(node, "yaw", vec.yaw, rslt)) return vec;
            if (!parse_number_attr(node, "ePitch", vec.ePitch, rslt)) return vec;
            if (!parse_number_attr(node, "eRoll", vec.eRoll, rslt)) return vec;
            if (!parse_number_attr(node, "eYaw", vec.eYaw, rslt)) return vec;

            result val_res;
            if (!vec.is_valid(expected_tag, &val_res)) {
                if (rslt && rslt->is_success()) *rslt = val_res;
            }
            return vec;
        }
    };

    /// @brief A CoT Message subschema class for <spatial> orientation and spin data
    class spatial {
    public:
        /// @brief Constructor - Initializes everything
        explicit spatial(double version = INVALID_NUMBER,
            spatial_vector attitude = spatial_vector{},
            spatial_vector spin = spatial_vector{}) noexcept
            : version(version),
            attitude(std::move(attitude)),
            spin(std::move(spin)) {
        }

        /// @brief Equality comparison
        bool operator==(const spatial& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            const bool ver_eq = (std::isnan(version) && std::isnan(other.version)) ||
                (std::isfinite(version) && std::isfinite(other.version) &&
                    std::abs(version - other.version) < EPSILON);
            return ver_eq &&
                attitude == other.attitude &&
                spin == other.spin;
        }

        /// @brief Inequality comparison
        bool operator!=(const spatial& other) const noexcept { return !(*this == other); }

        /// @brief Checks whether any spatial fields or child elements are populated
        [[nodiscard]] bool has_data() const noexcept {
            return !std::isnan(version) || attitude.has_data() || spin.has_data();
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!has_data()) {
                auto res = result{ result::error_code::InvalidSpatial, "Empty spatial data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(version) && !std::isfinite(version)) {
                auto res = result{ result::error_code::InvalidSpatial, "Non-finite version attribute" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (attitude.has_data() && !attitude.is_valid("attitude", rslt)) {
                return false;
            }
            if (spin.has_data() && !spin.is_valid("spin", rslt)) {
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            result val_res;
            if (!is_valid(&val_res)) {
                auto res = result{ result::error_code::InvalidSpatial,
                                  val_res.description().empty()
                                      ? std::string("Invalid spatial data")
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            auto node = parent.append_child("spatial");
            if (!std::isnan(version)) {
                node.append_attribute("version") = version;
            }
            if (attitude.has_data()) {
                auto res = attitude.to_xml(node, "attitude");
                if (!res.is_success()) return res;
            }
            if (spin.has_data()) {
                auto res = spin.to_xml(node, "spin");
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized spatial
        static spatial from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            spatial sp;
            if (node.name() != std::string_view("spatial")) {
                auto res = result{ result::error_code::InvalidSpatial, "Expected <spatial> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return sp;
            }

            if (!spatial_vector::parse_number_attr(node, "version", sp.version, rslt)) {
                return sp;
            }

            for (auto child : node.children()) {
                const std::string_view child_name = child.name();
                if (child_name == "attitude") {
                    result child_res;
                    sp.attitude = spatial_vector::from_xml(child, "attitude", &child_res);
                    if (child_res.is_failed() && rslt && rslt->is_success()) {
                        *rslt = child_res;
                    }
                }
                else if (child_name == "spin") {
                    result child_res;
                    sp.spin = spatial_vector::from_xml(child, "spin", &child_res);
                    if (child_res.is_failed() && rslt && rslt->is_success()) {
                        *rslt = child_res;
                    }
                }
            }

            result val_res;
            if (!sp.is_valid(&val_res) && rslt && rslt->is_success()) {
                *rslt = val_res;
            }
            return sp;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const spatial& s) {
            auto fmt_num = [](double v) -> std::string {
                if (std::isnan(v)) return "None";
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(4) << v;
                return oss.str();
                };

            os << "spatial: ";
            if (!s.is_valid()) os << " -NOT VALID- ";
            os << "\n\tversion: " << fmt_num(s.version)
                << "\n\tattitude: pitch=" << fmt_num(s.attitude.pitch)
                << ", roll=" << fmt_num(s.attitude.roll)
                << ", yaw=" << fmt_num(s.attitude.yaw)
                << ", ePitch=" << fmt_num(s.attitude.ePitch)
                << ", eRoll=" << fmt_num(s.attitude.eRoll)
                << ", eYaw=" << fmt_num(s.attitude.eYaw)
                << "\n\tspin: pitch=" << fmt_num(s.spin.pitch)
                << ", roll=" << fmt_num(s.spin.roll)
                << ", yaw=" << fmt_num(s.spin.yaw)
                << ", ePitch=" << fmt_num(s.spin.ePitch)
                << ", eRoll=" << fmt_num(s.spin.eRoll)
                << ", eYaw=" << fmt_num(s.spin.eYaw)
                << "\n";
            return os;
        }

        double version{ INVALID_NUMBER };
        spatial_vector attitude{};
        spatial_vector spin{};

    private:
        inline static constexpr double INVALID_NUMBER = std::numeric_limits<double>::quiet_NaN();
    };
}