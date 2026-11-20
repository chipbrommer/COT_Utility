#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            fillcolor.hpp
// @brief           A CoT Message subschema class for fillcolor data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <exception>
#include <limits>
#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for fillcolor data
    class fillcolor {
    public:
        /// @brief Constructor - Initializes everything
        explicit fillcolor(int value = INVALID_VALUE) noexcept : value(value) {}

        /// @brief Equality comparison
        bool operator==(const fillcolor& other) const noexcept { return value == other.value; }

        /// @brief Inequality comparison
        bool operator!=(const fillcolor& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                auto res = result{ result::error_code::InvalidFillColor, "fillcolor value is unset" };
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
                auto res = result{ result::error_code::InvalidFillColor, "Invalid fillcolor data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("fillcolor");
            node.append_attribute("value") = value;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized fillcolor
        static fillcolor from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            fillcolor fc;
            if (node.name() != std::string("fillcolor")) {
                auto res = result{ result::error_code::InvalidFillColor, "Expected <fillcolor> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return fc;
            }
            if (auto attr = node.attribute("value")) {
                try {
                    fc.value = attr.as_int(INVALID_VALUE);
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidFillColor,
                                      "Error parsing fillcolor value: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    fc.value = INVALID_VALUE;
                }
            }
            if (!fc.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidFillColor,
                                  "Invalid fillcolor data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return fc;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const fillcolor& fc) {
            os << "fillcolor: ";
            if (!fc.is_valid()) os << " -NOT VALID- (Unset)";
            else os << "\n\tvalue: " << fc.value;
            os << "\n";
            return os;
        }

        int value{ INVALID_VALUE };

    private:
        inline static constexpr int INVALID_VALUE = std::numeric_limits<int>::min();
    };
}