#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for military icon data
    class milicon {
    public:
        /// @brief Constructor - Initializes everything
        explicit milicon(std::string id = INVALID_VALUE)
            : id(std::move(id)) {
        }

        /// @brief Equality comparison
        bool operator==(const milicon& other) const noexcept {
            return id == other.id;
        }

        /// @brief Inequality comparison
        bool operator!=(const milicon& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (id.empty()) {
                auto res = result{ result::error_code::InvalidMilIcon, "Empty id field" };
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
                auto res = result{ result::error_code::InvalidMilIcon, "Invalid milicon data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("__milicon");
            node.append_attribute("id") = id.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized milicon
        static milicon from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            milicon mi;
            if (node.name() != std::string("__milicon")) {
                auto res = result{ result::error_code::InvalidMilIcon, "Expected <__milicon> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return mi;
            }
            if (auto attr = node.attribute("id")) mi.id = attr.as_string();
            if (!mi.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidMilIcon, "Invalid milicon data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return mi;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const milicon& mi) {
            os << "milicon: ";
            if (!mi.is_valid()) os << " -NOT VALID- ";
            os << "\n\tid: " << (mi.id.empty() ? "None" : mi.id) << "\n";
            return os;
        }

        std::string id{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}