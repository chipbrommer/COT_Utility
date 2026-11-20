#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            strokecolor.hpp
// @brief           A CoT Message subschema class for strokecolor data.
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
    /// @brief A CoT Message subschema class for strokecolor data
    class strokecolor {
    public:
        /// @brief Constructor - Initializes everything
        explicit strokecolor(int value = INVALID_VALUE) noexcept : value(value) {}

        /// @brief Equality comparison
        bool operator==(const strokecolor& other) const noexcept { return value == other.value; }

        /// @brief Inequality comparison
        bool operator!=(const strokecolor& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (value == INVALID_VALUE) {
                auto res = result{ result::error_code::InvalidStrokeColor, "strokecolor value is unset" };
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
                auto res = result{ result::error_code::InvalidStrokeColor, "Invalid strokecolor data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("strokecolor");
            node.append_attribute("value") = value;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized strokecolor
        static strokecolor from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            strokecolor sc;
            if (node.name() != std::string("strokecolor")) {
                auto res = result{ result::error_code::InvalidStrokeColor, "Expected <strokecolor> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return sc;
            }
            if (auto attr = node.attribute("value")) {
                try {
                    sc.value = attr.as_int(INVALID_VALUE);
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidStrokeColor,
                                      "Error parsing strokecolor value: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    sc.value = INVALID_VALUE;
                }
            }
            if (!sc.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidStrokeColor,
                                  "Invalid strokecolor data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return sc;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const strokecolor& sc) {
            os << "strokecolor: ";
            if (!sc.is_valid()) os << " -NOT VALID- (Unset)";
            else os << "\n\tvalue: " << sc.value;
            os << "\n";
            return os;
        }

        int value{ INVALID_VALUE };

    private:
        inline static constexpr int INVALID_VALUE = std::numeric_limits<int>::min();
    };
}