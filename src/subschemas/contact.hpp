#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for contact data
    class contact {
    public:
        /// @brief Constructor - Initializes everything
        explicit contact(std::string endpoint = INVALID_VALUE,
            std::string callsign = INVALID_VALUE,
            std::string xmppUsername = INVALID_VALUE)
            : endpoint(std::move(endpoint)), callsign(std::move(callsign)),
            xmppUsername(std::move(xmppUsername)) {
        }

        /// @brief Equality comparison
        bool operator==(const contact& other) const noexcept {
            return endpoint == other.endpoint && callsign == other.callsign &&
                xmppUsername == other.xmppUsername;
        }

        /// @brief Inequality comparison
        bool operator!=(const contact& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (callsign.empty()) {
                auto res = result{ result::error_code::InvalidContact, "Empty callsign field" };
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
                auto res = result{ result::error_code::InvalidContact, "Invalid contact data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("contact");
            if (!endpoint.empty()) node.append_attribute("endpoint") = endpoint.c_str();
            node.append_attribute("callsign") = callsign.c_str();
            if (!xmppUsername.empty()) node.append_attribute("xmppUsername") = xmppUsername.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized contact
        static contact from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            contact c;
            if (node.name() != std::string("contact")) {
                auto res = result{ result::error_code::InvalidContact, "Expected <contact> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return c;
            }
            if (auto attr = node.attribute("endpoint")) c.endpoint = attr.as_string();
            if (auto attr = node.attribute("callsign")) c.callsign = attr.as_string();
            if (auto attr = node.attribute("xmppUsername")) c.xmppUsername = attr.as_string();
            if (!c.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidContact, "Invalid contact data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return c;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const contact& c) {
            os << "contact: ";
            if (!c.is_valid()) os << " -NOT VALID- ";
            os << "\n\tendpoint: " << (c.endpoint.empty() ? "None" : c.endpoint) << "\n"
                << "\tcallsign: " << (c.callsign.empty() ? "None" : c.callsign) << "\n"
                << "\txmppUsername: " << (c.xmppUsername.empty() ? "None" : c.xmppUsername) << "\n";
            return os;
        }

        std::string endpoint{ "" };
        std::string callsign{ "" };
        std::string xmppUsername{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

}