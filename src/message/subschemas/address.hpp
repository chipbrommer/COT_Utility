#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            address.hpp
// @brief           A CoT Message subschema class for address data.
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
    /// @brief A CoT Message subschema class for address data
    class address {
    public:
        /// @brief Constructor - Initializes everything
        explicit address(std::string text = INVALID_VALUE,
            std::string geocoder = INVALID_VALUE,
            std::string time = INVALID_VALUE)
            : text(std::move(text)), geocoder(std::move(geocoder)),
            time(std::move(time)) {
        }

        /// @brief Equality comparison
        bool operator==(const address& other) const noexcept {
            return text == other.text && geocoder == other.geocoder &&
                time == other.time;
        }

        /// @brief Inequality comparison
        bool operator!=(const address& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (text.empty()) {
                auto res = result{ result::error_code::InvalidAddress, "Empty text field" };
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
                auto res = result{ result::error_code::InvalidAddress, "Invalid address data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("__address");
            node.append_attribute("text") = text.c_str();
            if (!geocoder.empty()) node.append_attribute("geocoder") = geocoder.c_str();
            if (!time.empty()) node.append_attribute("time") = time.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized address
        static address from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            address addr;
            if (node.name() != std::string("__address")) {
                auto res = result{ result::error_code::InvalidAddress, "Expected <__address> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return addr;
            }
            if (auto attr = node.attribute("text")) addr.text = attr.as_string();
            if (auto attr = node.attribute("geocoder")) addr.geocoder = attr.as_string();
            if (auto attr = node.attribute("time")) addr.time = attr.as_string();
            if (!addr.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidAddress, "Invalid address data after parsing" };
                if (rslt && rslt->is_success()) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return addr;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const address& addr) {
            os << "address: ";
            if (!addr.is_valid()) os << " -NOT VALID- ";
            os << "\n\ttext: " << (addr.text.empty() ? "None" : addr.text) << "\n"
                << "\tgeocoder: " << (addr.geocoder.empty() ? "None" : addr.geocoder) << "\n"
                << "\ttime: " << (addr.time.empty() ? "None" : addr.time) << "\n";
            return os;
        }

        std::string text{ "" };
        std::string geocoder{ "" };
        std::string time{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}