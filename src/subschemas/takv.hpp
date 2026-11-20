#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            takv.hpp
// @brief           A CoT Message subschema class for takv data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <utility>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for takv data
    class takv {
    public:
        /// @brief Constructor - Initializes everything
        explicit takv(std::string version = INVALID_VALUE,
            std::string device = INVALID_VALUE,
            std::string os = INVALID_VALUE,
            std::string platform = INVALID_VALUE)
            : version(std::move(version)), device(std::move(device)),
            os(std::move(os)), platform(std::move(platform)) {
        }

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
            if (!t.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidTakv,
                                  "Invalid takv data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
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

        std::string version{ INVALID_VALUE };
        std::string device{ INVALID_VALUE };
        std::string os{ INVALID_VALUE };
        std::string platform{ INVALID_VALUE };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}