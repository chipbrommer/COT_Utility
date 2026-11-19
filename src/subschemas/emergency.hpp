#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for emergency alert data
    class emergency {
    public:
        /// @brief Constructor - Initializes everything
        explicit emergency(std::string text = INVALID_STRING,
            std::string type = INVALID_STRING,
            bool cancel = false,
            bool has_cancel = false)
            : text(std::move(text)), type(std::move(type)),
            cancel(cancel), has_cancel(has_cancel) {
        }

        /// @brief Equality comparison
        bool operator==(const emergency& other) const noexcept {
            return text == other.text && type == other.type &&
                cancel == other.cancel && has_cancel == other.has_cancel;
        }

        /// @brief Inequality comparison
        bool operator!=(const emergency& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (text.empty()) {
                auto res = result{ result::error_code::InvalidEmergency, "Empty text field" };
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
                auto res = result{ result::error_code::InvalidEmergency, "Invalid emergency data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("emergency");
            if (has_cancel) node.append_attribute("cancel") = cancel;
            if (!type.empty()) node.append_attribute("type") = type.c_str();
            node.text().set(text.c_str());
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized emergency
        static emergency from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            emergency em;
            if (node.name() != std::string("emergency")) {
                auto res = result{ result::error_code::InvalidEmergency, "Expected <emergency> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return em;
            }
            if (auto attr = node.attribute("cancel")) {
                em.has_cancel = true;
                std::string val = attr.as_string();
                em.cancel = (val == "true" || val == "1");
            }
            if (auto attr = node.attribute("type")) em.type = attr.as_string();
            if (auto txt = node.text()) em.text = txt.as_string();
            if (!em.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidEmergency, "Invalid emergency data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return em;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const emergency& em) {
            os << "emergency: ";
            if (!em.is_valid()) os << " -NOT VALID- ";
            os << "\n\tcancel: " << (em.has_cancel ? (em.cancel ? "true" : "false") : "None") << "\n"
                << "\ttype: " << (em.type.empty() ? "None" : em.type) << "\n"
                << "\ttext: " << (em.text.empty() ? "None" : em.text) << "\n";
            return os;
        }

        std::string text{ "" };
        std::string type{ "" };
        bool cancel{ false };
        bool has_cancel{ false };

    private:
        inline static constexpr const char* INVALID_STRING = "";
    };
}