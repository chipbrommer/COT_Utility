#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            labels_on.hpp
// @brief           A CoT Message subschema class for labels_on data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for labels_on data
    class labels_on {
    public:
        /// @brief Constructor - Initializes everything
        explicit labels_on(bool value = false, bool has_value = false) noexcept
            : value(value), has_value(has_value) {
        }

        /// @brief Equality comparison
        bool operator==(const labels_on& other) const noexcept {
            return value == other.value && has_value == other.has_value;
        }

        /// @brief Inequality comparison
        bool operator!=(const labels_on& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            (void)rslt;
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidLabelsOn, "Invalid labels_on data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("labels_on");
            if (has_value) node.append_attribute("value") = value;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized labels_on
        static labels_on from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            labels_on lo;
            if (node.name() != std::string("labels_on")) {
                auto res = result{ result::error_code::InvalidLabelsOn, "Expected <labels_on> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return lo;
            }
            if (auto attr = node.attribute("value")) {
                lo.has_value = true;
                std::string val = attr.as_string();
                lo.value = (val == "true" || val == "1");
            }
            if (!lo.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidLabelsOn,
                                  "Invalid labels_on data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return lo;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const labels_on& lo) {
            os << "labels_on: ";
            if (!lo.is_valid()) os << " -NOT VALID- ";
            os << "\n\tvalue: " << (lo.has_value ? (lo.value ? "true" : "false") : "None") << "\n";
            return os;
        }

        bool value{ false };
        bool has_value{ false };
    };
}