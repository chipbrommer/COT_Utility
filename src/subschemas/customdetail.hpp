#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for customdetail data
    class customdetail {
    public:
        /// @brief Constructor - Initializes everything
        explicit customdetail(std::string name = INVALID_VALUE,
            std::map<std::string, std::string> attributes = {},
            std::string content = INVALID_VALUE,
            std::vector<customdetail> children = {})
            : name(std::move(name)), attributes(std::move(attributes)),
            content(std::move(content)), children(std::move(children)) {
        }

        /// @brief Equality comparison
        bool operator==(const customdetail& other) const noexcept {
            return name == other.name &&
                attributes == other.attributes &&
                content == other.content &&
                children == other.children;
        }

        /// @brief Inequality comparison
        bool operator!=(const customdetail& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (name.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Empty name field" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (attributes.empty() && content.empty() && !children.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail,
                                  "No attributes, content, or children" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            for (const auto& child : children) {
                if (!child.is_valid(rslt)) {
                    auto res = result{ result::error_code::InvalidCustomdetail, "Invalid child element" };
                    if (rslt && rslt->is_success()) *rslt = res;
                    if (error_handler) error_handler(res);
                    return false;
                }
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Invalid customdetail data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child(name.c_str());
            for (const auto& [key, value] : attributes) {
                node.append_attribute(key.c_str()) = value.c_str();
            }
            if (!content.empty()) {
                node.append_child(pugi::node_pcdata).set_value(content.c_str());
            }
            for (const auto& child : children) {
                auto res = child.to_xml(node);
                if (!res.is_success()) return res;
            }
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized customdetail
        static customdetail from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            customdetail detail;
            detail.name = node.name();
            if (detail.name.empty()) {
                auto res = result{ result::error_code::InvalidCustomDetail, "Empty node name" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            for (auto attr : node.attributes()) {
                detail.attributes[attr.name()] = attr.as_string();
            }
            detail.content = node.child_value();
            for (auto child_node : node.children()) {
                if (child_node.type() == pugi::node_element) {
                    detail.children.push_back(from_xml(child_node, rslt));
                }
            }
            if (!detail.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidCustomDetail,
                               "Invalid customdetail data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return detail;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const customdetail& detail) {
            os << "\tcustomdetail: " << detail.name;
            if (!detail.is_valid()) os << " -NOT VALID-";
            os << "\n";
            for (const auto& [key, value] : detail.attributes) {
                os << "\t\t" << key << ": " << value << "\n";
            }
            if (!detail.content.empty()) {
                os << "\t\tcontent: " << detail.content << "\n";
            }
            for (const auto& child : detail.children) {
                os << child;
            }
            return os;
        }

        std::string name{ "" };
        std::map<std::string, std::string> attributes;
        std::string content{ "" };
        std::vector<customdetail> children;

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}