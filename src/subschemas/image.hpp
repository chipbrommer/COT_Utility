#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for image data
    class image {
    public:
        /// @brief Constructor - Initializes everything
        explicit image(std::string mime = INVALID_VALUE,
            std::string text = INVALID_VALUE)
            : mime(std::move(mime)), text(std::move(text)) {
        }

        /// @brief Equality comparison
        bool operator==(const image& other) const noexcept {
            return mime == other.mime && text == other.text;
        }

        /// @brief Inequality comparison
        bool operator!=(const image& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (text.empty()) {
                auto res = result{ result::error_code::InvalidImage, "Empty image data" };
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
                auto res = result{ result::error_code::InvalidImage, "Invalid image data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("image");
            if (!mime.empty()) node.append_attribute("mime") = mime.c_str();
            node.text().set(text.c_str());
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized image
        static image from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            image img;
            if (node.name() != std::string("image")) {
                auto res = result{ result::error_code::InvalidImage, "Expected <image> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return img;
            }
            if (auto attr = node.attribute("mime")) img.mime = attr.as_string();
            if (auto txt = node.text()) img.text = txt.as_string();
            if (!img.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidImage, "Invalid image data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return img;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const image& img) {
            os << "image: ";
            if (!img.is_valid()) os << " -NOT VALID- ";
            os << "\n\tmime: " << (img.mime.empty() ? "None" : img.mime) << "\n"
                << "\tdata_length: " << img.text.size() << "\n";
            return os;
        }

        std::string mime{ "" };
        std::string text{ "" };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };
}