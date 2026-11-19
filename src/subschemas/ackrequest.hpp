#pragma once
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <pugixml.hpp>

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for <ackrequest> data
    class ackrequest {
    public:
        /// @brief Constructor - Initializes everything
        explicit ackrequest(bool ackrequested = false,
            std::string tag = INVALID_STRING,
            std::string uid = INVALID_STRING,
            bool has_ackrequested = false) noexcept
            : ackrequested(ackrequested),
            tag(std::move(tag)),
            uid(std::move(uid)),
            has_ackrequested(has_ackrequested) {
        }

        /// @brief Equality comparison
        bool operator==(const ackrequest& other) const noexcept {
            return ackrequested == other.ackrequested &&
                has_ackrequested == other.has_ackrequested &&
                tag == other.tag &&
                uid == other.uid;
        }

        /// @brief Inequality comparison
        bool operator!=(const ackrequest& other) const noexcept { return !(*this == other); }

        /// @brief Checks if any ackrequest fields are populated
        [[nodiscard]] bool has_data() const noexcept {
            return has_ackrequested || !tag.empty() || !uid.empty();
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!has_ackrequested) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  "Missing or unset ackrequested attribute" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (tag.empty()) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  "Empty tag attribute" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (uid.empty()) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  "Empty uid attribute" };
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
            result val_res;
            if (!is_valid(&val_res)) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  val_res.description().empty()
                                      ? std::string("Invalid ackrequest data")
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            auto node = parent.append_child("ackrequest");
            node.append_attribute("ackrequested") = ackrequested ? "true" : "false";
            node.append_attribute("tag") = tag.c_str();
            node.append_attribute("uid") = uid.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized ackrequest
        static ackrequest from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            ackrequest req;
            if (node.name() != std::string_view("ackrequest")) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  "Expected <ackrequest> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return req;
            }

            try {
                if (auto attr = node.attribute("ackrequested")) {
                    std::string raw = attr.as_string();
                    if (raw == "true" || raw == "1") {
                        req.ackrequested = true;
                        req.has_ackrequested = true;
                    }
                    else if (raw == "false" || raw == "0") {
                        req.ackrequested = false;
                        req.has_ackrequested = true;
                    }
                    else {
                        auto res = result{ result::error_code::InvalidAckRequest,
                                          "Invalid ackrequested boolean attribute: " + raw };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return req;
                    }
                }
                else {
                    auto res = result{ result::error_code::InvalidAckRequest,
                                      "Missing ackrequested attribute" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return req;
                }

                if (auto attr = node.attribute("tag")) {
                    req.tag = attr.as_string();
                }
                if (auto attr = node.attribute("uid")) {
                    req.uid = attr.as_string();
                }

                result val_res;
                if (!req.is_valid(&val_res)) {
                    if (rslt) *rslt = val_res;
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidAckRequest,
                                  "Error parsing <ackrequest>: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }

            return req;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const ackrequest& req) {
            os << "ackrequest: ";
            if (!req.is_valid()) os << " -NOT VALID- ";
            os << "\n\tackrequested: "
                << (req.has_ackrequested ? (req.ackrequested ? "true" : "false") : "None")
                << "\n\ttag: " << (req.tag.empty() ? "None" : req.tag)
                << "\n\tuid: " << (req.uid.empty() ? "None" : req.uid)
                << "\n";
            return os;
        }

        bool ackrequested{ false };
        std::string tag{ INVALID_STRING };
        std::string uid{ INVALID_STRING };
        bool has_ackrequested{ false };

    private:
        inline static constexpr const char* INVALID_STRING = "";
    };
}