#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            milsym.hpp
// @brief           A CoT Message subschema class for military symbol data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"
#include "unitmodifier.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for military symbol data
    class milsym {
    public:
        /// @brief Constructor - Initializes everything
        explicit milsym(std::string id = INVALID_VALUE,
            std::vector<unitmodifier> modifiers = {})
            : id(std::move(id)), modifiers(std::move(modifiers)) {
        }

        /// @brief Equality comparison
        bool operator==(const milsym& other) const noexcept {
            return id == other.id && modifiers == other.modifiers;
        }

        /// @brief Inequality comparison
        bool operator!=(const milsym& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (id.empty()) {
                auto res = result{ result::error_code::InvalidMilSym, "Empty id field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            for (const auto& mod : modifiers) {
                if (!mod.is_valid(rslt)) {
                    return false;
                }
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidMilSym, "Invalid milsym data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("__milsym");
            node.append_attribute("id") = id.c_str();
            for (const auto& mod : modifiers) {
                auto res = mod.to_xml(node);
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized milsym
        static milsym from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            milsym ms;
            if (node.name() != std::string("__milsym")) {
                auto res = result{ result::error_code::InvalidMilSym, "Expected <__milsym> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return ms;
            }
            if (auto attr = node.attribute("id")) ms.id = attr.as_string();
            for (auto child : node.children()) {
                if (child.name() == std::string("unitmodifier")) {
                    ms.modifiers.push_back(unitmodifier::from_xml(child, rslt));
                }
            }
            if (!ms.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidMilSym, "Invalid milsym data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return ms;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const milsym& ms) {
            os << "milsym: ";
            if (!ms.is_valid()) os << " -NOT VALID- ";
            os << "\n\tid: " << (ms.id.empty() ? "None" : ms.id) << "\n"
                << "\tmodifiers:\n";
            if (ms.modifiers.empty()) os << "\t\tNone\n";
            else for (const auto& mod : ms.modifiers) os << mod;
            return os;
        }

        std::string id{ "" };
        std::vector<unitmodifier> modifiers;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}