#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            model.hpp
// @brief           A CoT Message subschema class for model data.
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
    /// @brief A CoT Message subschema class for model data
    class model {
    public:
        /// @brief Constructor - Initializes everything
        explicit model(std::string value = INVALID_VALUE) : value(std::move(value)) {}

        /// @brief Equality comparison
        bool operator==(const model& other) const noexcept { return value == other.value; }

        /// @brief Inequality comparison
        bool operator!=(const model& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (value.empty()) {
                auto res = result{ result::error_code::InvalidModel, "Empty model value" };
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
                auto res = result{ result::error_code::InvalidModel, "Invalid model data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("model");
            node.append_attribute("value") = value.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized model
        static model from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            model m;
            if (node.name() != std::string("model")) {
                auto res = result{ result::error_code::InvalidModel, "Expected <model> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return m;
            }
            if (auto attr = node.attribute("value")) {
                m.value = attr.as_string();
            }
            if (!m.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidModel,
                                  "Invalid model data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return m;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const model& m) {
            os << "model: ";
            if (!m.is_valid()) os << " -NOT VALID- ";
            os << "\n\tvalue: " << (m.value.empty() ? "None" : m.value) << "\n";
            return os;
        }

        std::string value{ INVALID_VALUE };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}