#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            precisionlocation.hpp
// @brief           A CoT Message subschema class for precisionlocation data.
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
    /// @brief A CoT Message subschema class for precisionlocation data
    class precisionlocation {
    public:
        /// @brief Constructor - Initializes everything
        explicit precisionlocation(std::string altsrc = INVALID_VALUE,
            std::string geopointsrc = INVALID_VALUE)
            : altsrc(std::move(altsrc)), geopointsrc(std::move(geopointsrc)) {
        }

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
            if (!pl.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidPrecisionLocation,
                               "Invalid precisionlocation data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
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

        std::string altsrc{ INVALID_VALUE };
        std::string geopointsrc{ INVALID_VALUE };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}