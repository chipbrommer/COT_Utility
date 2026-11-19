#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for creator data
    class creator {
    public:
        /// @brief Constructor - Initializes everything
        explicit creator(std::string callsign = INVALID_VALUE,
            std::string time = INVALID_VALUE,
            std::string type = INVALID_VALUE,
            std::string uid = INVALID_VALUE)
            : callsign(std::move(callsign)), time(std::move(time)),
            type(std::move(type)), uid(std::move(uid)) {
        }

        /// @brief Equality comparison
        bool operator==(const creator& other) const noexcept {
            return callsign == other.callsign && time == other.time &&
                type == other.type && uid == other.uid;
        }

        /// @brief Inequality comparison
        bool operator!=(const creator& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (type.empty()) {
                auto res = result{ result::error_code::InvalidCreator, "Empty type field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (uid.empty()) {
                auto res = result{ result::error_code::InvalidCreator, "Empty uid field" };
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
                auto res = result{ result::error_code::InvalidCreator, "Invalid creator data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("creator");
            if (!callsign.empty()) node.append_attribute("callsign") = callsign.c_str();
            if (!time.empty()) node.append_attribute("time") = time.c_str();
            node.append_attribute("type") = type.c_str();
            node.append_attribute("uid") = uid.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized creator
        static creator from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            creator cr;
            if (node.name() != std::string("creator")) {
                auto res = result{ result::error_code::InvalidCreator, "Expected <creator> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return cr;
            }
            if (auto attr = node.attribute("callsign")) cr.callsign = attr.as_string();
            if (auto attr = node.attribute("time")) cr.time = attr.as_string();
            if (auto attr = node.attribute("type")) cr.type = attr.as_string();
            if (auto attr = node.attribute("uid")) cr.uid = attr.as_string();
            if (!cr.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidCreator, "Invalid creator data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return cr;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const creator& cr) {
            os << "creator: ";
            if (!cr.is_valid()) os << " -NOT VALID- ";
            os << "\n\tcallsign: " << (cr.callsign.empty() ? "None" : cr.callsign) << "\n"
                << "\ttime: " << (cr.time.empty() ? "None" : cr.time) << "\n"
                << "\ttype: " << (cr.type.empty() ? "None" : cr.type) << "\n"
                << "\tuid: " << (cr.uid.empty() ? "None" : cr.uid) << "\n";
            return os;
        }

        std::string callsign{ "" };
        std::string time{ "" };
        std::string type{ "" };
        std::string uid{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}