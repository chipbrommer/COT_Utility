#pragma once
#include <string>
#include <ostream>
#include <pugixml.hpp>

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for TakControl data
    class takcontrol {
    public:
        /// @brief Nested subschema for <TakProtocolSupport version="..."/>
        struct protocol_support {
            std::string version{ "" };

            bool operator==(const protocol_support& other) const noexcept {
                return version == other.version;
            }
            bool operator!=(const protocol_support& other) const noexcept {
                return !(*this == other);
            }
            [[nodiscard]] bool has_data() const noexcept {
                return !version.empty();
            }
            bool is_valid(result* rslt = nullptr) const {
                if (version.empty()) {
                    auto res = result{ result::error_code::InvalidTakControl,
                                      "TakProtocolSupport missing version attribute" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                return true;
            }
        };

        /// @brief Nested subschema for <TakRequest version="..."/>
        struct request {
            std::string version{ "" };

            bool operator==(const request& other) const noexcept {
                return version == other.version;
            }
            bool operator!=(const request& other) const noexcept {
                return !(*this == other);
            }
            [[nodiscard]] bool has_data() const noexcept {
                return !version.empty();
            }
            bool is_valid(result* rslt = nullptr) const {
                if (version.empty()) {
                    auto res = result{ result::error_code::InvalidTakControl,
                                      "TakRequest missing version attribute" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                return true;
            }
        };

        /// @brief Nested subschema for <TakResponse status="..."/>
        struct response {
            bool status{ false };
            bool has_status{ false };

            bool operator==(const response& other) const noexcept {
                return status == other.status && has_status == other.has_status;
            }
            bool operator!=(const response& other) const noexcept {
                return !(*this == other);
            }
            [[nodiscard]] bool has_data() const noexcept {
                return has_status;
            }
            bool is_valid(result* rslt = nullptr) const {
                if (!has_status) {
                    auto res = result{ result::error_code::InvalidTakControl,
                                      "TakResponse missing status attribute" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                return true;
            }
        };

        /// @brief Nested subschema for <TakServerVersionInfo serverVersion="..."/>
        struct server_version_info {
            std::string server_version{ "" };

            bool operator==(const server_version_info& other) const noexcept {
                return server_version == other.server_version;
            }
            bool operator!=(const server_version_info& other) const noexcept {
                return !(*this == other);
            }
            [[nodiscard]] bool has_data() const noexcept {
                return !server_version.empty();
            }
            bool is_valid(result* rslt = nullptr) const {
                if (server_version.empty()) {
                    auto res = result{ result::error_code::InvalidTakControl,
                                      "TakServerVersionInfo missing serverVersion attribute" };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
                return true;
            }
        };

        /// @brief Constructor - Initializes everything
        explicit takcontrol(protocol_support protocol_support = protocol_support{},
            request request = request{},
            response response = response{},
            server_version_info server_version_info = server_version_info{}) noexcept
            : protocol_support_(std::move(protocol_support)),
            request_(std::move(request)),
            response_(std::move(response)),
            server_version_info_(std::move(server_version_info)) {
        }

        /// @brief Equality comparison
        bool operator==(const takcontrol& other) const noexcept {
            return protocol_support_ == other.protocol_support_ &&
                request_ == other.request_ &&
                response_ == other.response_ &&
                server_version_info_ == other.server_version_info_;
        }

        /// @brief Inequality comparison
        bool operator!=(const takcontrol& other) const noexcept { return !(*this == other); }

        /// @brief Checks if any control child element is populated
        [[nodiscard]] bool has_data() const noexcept {
            return protocol_support_.has_data() ||
                request_.has_data() ||
                response_.has_data() ||
                server_version_info_.has_data();
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (protocol_support_.has_data() && !protocol_support_.is_valid(rslt)) {
                return false;
            }
            if (request_.has_data() && !request_.is_valid(rslt)) {
                return false;
            }
            if (response_.has_data() && !response_.is_valid(rslt)) {
                return false;
            }
            if (server_version_info_.has_data() && !server_version_info_.is_valid(rslt)) {
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
                auto res = result{ result::error_code::InvalidTakControl, val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            auto node = parent.append_child("TakControl");

            if (protocol_support_.has_data()) {
                auto child = node.append_child("TakProtocolSupport");
                child.append_attribute("version") = protocol_support_.version.c_str();
            }
            if (request_.has_data()) {
                auto child = node.append_child("TakRequest");
                child.append_attribute("version") = request_.version.c_str();
            }
            if (response_.has_data()) {
                auto child = node.append_child("TakResponse");
                child.append_attribute("status") = response_.status ? "true" : "false";
            }
            if (server_version_info_.has_data()) {
                auto child = node.append_child("TakServerVersionInfo");
                child.append_attribute("serverVersion") = server_version_info_.server_version.c_str();
            }

            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized takcontrol
        static takcontrol from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            takcontrol tc;
            if (node.name() != std::string("TakControl")) {
                auto res = result{ result::error_code::InvalidTakControl, "Expected <TakControl> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return tc;
            }

            for (auto child : node.children()) {
                std::string name = child.name();
                if (name == "TakProtocolSupport") {
                    if (auto attr = child.attribute("version")) {
                        tc.protocol_support_.version = attr.as_string();
                    }
                    else {
                        auto res = result{ result::error_code::InvalidTakControl,
                                          "TakProtocolSupport missing version attribute" };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return tc;
                    }
                }
                else if (name == "TakRequest") {
                    if (auto attr = child.attribute("version")) {
                        tc.request_.version = attr.as_string();
                    }
                    else {
                        auto res = result{ result::error_code::InvalidTakControl,
                                          "TakRequest missing version attribute" };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return tc;
                    }
                }
                else if (name == "TakResponse") {
                    if (auto attr = child.attribute("status")) {
                        std::string raw = attr.as_string();
                        if (raw == "true" || raw == "1") {
                            tc.response_.status = true;
                            tc.response_.has_status = true;
                        }
                        else if (raw == "false" || raw == "0") {
                            tc.response_.status = false;
                            tc.response_.has_status = true;
                        }
                        else {
                            auto res = result{ result::error_code::InvalidTakControl,
                                              "Invalid TakResponse status attribute: " + raw };
                            if (rslt) *rslt = res;
                            if (error_handler) error_handler(res);
                            return tc;
                        }
                    }
                    else {
                        auto res = result{ result::error_code::InvalidTakControl,
                                          "TakResponse missing status attribute" };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return tc;
                    }
                }
                else if (name == "TakServerVersionInfo") {
                    if (auto attr = child.attribute("serverVersion")) {
                        tc.server_version_info_.server_version = attr.as_string();
                    }
                    else {
                        auto res = result{ result::error_code::InvalidTakControl,
                                          "TakServerVersionInfo missing serverVersion attribute" };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return tc;
                    }
                }
            }

            if (!tc.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidTakControl,
                                "Invalid TakControl data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return tc;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const takcontrol& tc) {
            os << "TakControl: ";
            if (!tc.is_valid()) os << " -NOT VALID- ";
            os << "\n\tTakProtocolSupport version: "
                << (tc.protocol_support_.version.empty() ? "None" : tc.protocol_support_.version)
                << "\n\tTakRequest version: "
                << (tc.request_.version.empty() ? "None" : tc.request_.version)
                << "\n\tTakResponse status: "
                << (tc.response_.has_status ? (tc.response_.status ? "true" : "false") : "None")
                << "\n\tTakServerVersionInfo serverVersion: "
                << (tc.server_version_info_.server_version.empty()
                    ? "None" : tc.server_version_info_.server_version)
                << "\n";
            return os;
        }

        protocol_support protocol_support_;
        request request_;
        response response_;
        server_version_info server_version_info_;
    };
}