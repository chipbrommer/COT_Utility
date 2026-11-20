#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            group.hpp
// @brief           A CoT Message subschema class for group data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <utility>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for group data
    class group {
    public:
        /// @brief Constructor - Initializes everything
        explicit group(std::string role = INVALID_VALUE,
            std::string name = INVALID_VALUE)
            : role(std::move(role)), name(std::move(name)) {
        }

        /// @brief Equality comparison
        bool operator==(const group& other) const noexcept {
            return role == other.role && name == other.name;
        }

        /// @brief Inequality comparison
        bool operator!=(const group& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidGroup, "Empty name field" };
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
                auto res = result{ result::error_code::InvalidGroup, "Invalid group data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("__group");
            if (!role.empty()) node.append_attribute("role") = role.c_str();
            node.append_attribute("name") = name.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized group
        static group from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            group g;
            if (node.name() != std::string("__group")) {
                auto res = result{ result::error_code::InvalidGroup, "Expected <__group> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return g;
            }
            if (auto attr = node.attribute("role")) g.role = attr.as_string();
            if (auto attr = node.attribute("name")) g.name = attr.as_string();
            if (!g.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidGroup,
                                  "Invalid group data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return g;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const group& g) {
            os << "group: ";
            if (!g.is_valid()) os << " -NOT VALID- ";
            os << "\n\trole: " << (g.role.empty() ? "None" : g.role) << "\n"
                << "\tname: " << (g.name.empty() ? "None" : g.name) << "\n";
            return os;
        }

        std::string role{ INVALID_VALUE };
        std::string name{ INVALID_VALUE };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}