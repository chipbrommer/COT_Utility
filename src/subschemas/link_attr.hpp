#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for route link attributes
    class link_attr {
    public:
        /// @brief Constructor - Initializes everything
        explicit link_attr(std::string color = INVALID_VALUE,
            std::string method = INVALID_VALUE,
            std::string direction = INVALID_VALUE,
            std::string routetype = INVALID_VALUE,
            std::string order = INVALID_VALUE)
            : color(std::move(color)), method(std::move(method)),
            direction(std::move(direction)), routetype(std::move(routetype)),
            order(std::move(order)) {
        }

        /// @brief Equality comparison
        bool operator==(const link_attr& other) const noexcept {
            return color == other.color && method == other.method &&
                direction == other.direction && routetype == other.routetype &&
                order == other.order;
        }

        /// @brief Inequality comparison
        bool operator!=(const link_attr& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            (void)rslt;
            return !color.empty() || !method.empty() || !direction.empty() ||
                !routetype.empty() || !order.empty();
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidLinkAttr, "Invalid link_attr data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("link_attr");
            if (!color.empty()) node.append_attribute("color") = color.c_str();
            if (!method.empty()) node.append_attribute("method") = method.c_str();
            if (!direction.empty()) node.append_attribute("direction") = direction.c_str();
            if (!routetype.empty()) node.append_attribute("routetype") = routetype.c_str();
            if (!order.empty()) node.append_attribute("order") = order.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized link_attr
        static link_attr from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            link_attr attr_obj;
            if (node.name() != std::string("link_attr")) {
                auto res = result{ result::error_code::InvalidLinkAttr, "Expected <link_attr> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return attr_obj;
            }
            if (auto attr = node.attribute("color")) attr_obj.color = attr.as_string();
            if (auto attr = node.attribute("method")) attr_obj.method = attr.as_string();
            if (auto attr = node.attribute("direction")) attr_obj.direction = attr.as_string();
            if (auto attr = node.attribute("routetype")) attr_obj.routetype = attr.as_string();
            if (auto attr = node.attribute("order")) attr_obj.order = attr.as_string();
            if (!attr_obj.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidLinkAttr, "Invalid link_attr data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return attr_obj;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const link_attr& a) {
            os << "link_attr: ";
            if (!a.is_valid()) os << " -NOT VALID- ";
            os << "\n\tcolor: " << (a.color.empty() ? "None" : a.color) << "\n"
                << "\tmethod: " << (a.method.empty() ? "None" : a.method) << "\n"
                << "\tdirection: " << (a.direction.empty() ? "None" : a.direction) << "\n"
                << "\troutetype: " << (a.routetype.empty() ? "None" : a.routetype) << "\n"
                << "\torder: " << (a.order.empty() ? "None" : a.order) << "\n";
            return os;
        }

        std::string color{ "" };
        std::string method{ "" };
        std::string direction{ "" };
        std::string routetype{ "" };
        std::string order{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}