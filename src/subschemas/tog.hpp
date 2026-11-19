#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for tog data
    class tog {
    public:
        /// @brief Constructor - Initializes everything
        explicit tog(std::string enabled = INVALID_VALUE)
            : enabled(std::move(enabled)) {
        }

        /// @brief Equality comparison
        bool operator==(const tog& other) const noexcept { return enabled == other.enabled; }

        /// @brief Inequality comparison
        bool operator!=(const tog& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (enabled.empty()) {
                auto res = result{ result::error_code::InvalidTog, "Empty tog enabled attribute" };
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
                auto res = result{ result::error_code::InvalidTog, "Invalid tog data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("tog");
            node.append_attribute("enabled") = enabled.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized tog
        static tog from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            tog t;
            if (node.name() != std::string("tog")) {
                auto res = result{ result::error_code::InvalidTog, "Expected <tog> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return t;
            }
            if (auto attr = node.attribute("enabled")) t.enabled = attr.as_string();
            if (!t.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidTog, "Invalid tog data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return t;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const tog& t) {
            os << "tog: ";
            if (!t.is_valid()) os << " -NOT VALID- ";
            os << "\n\tenabled: " << (t.enabled.empty() ? "None" : t.enabled) << "\n";
            return os;
        }

        std::string enabled{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}