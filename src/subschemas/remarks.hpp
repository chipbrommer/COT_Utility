#pragma once
#include <optional>
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for remarks data
    class remarks {
    public:
        /// @brief Constructor - Initializes everything
        explicit remarks(std::string text = INVALID_VALUE,
            std::string source = INVALID_VALUE,
            std::string to = INVALID_VALUE,
            std::optional<datetime> time = std::nullopt)
            : text(std::move(text)), source(std::move(source)),
            to(std::move(to)), time(std::move(time)) {
        }

        /// @brief Equality comparison
        bool operator==(const remarks& other) const noexcept {
            return text == other.text && source == other.source &&
                to == other.to && time == other.time;
        }

        /// @brief Inequality comparison
        bool operator!=(const remarks& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (time.has_value() && !time->is_valid(rslt)) {
                return false;
            }
            return true;
        }

        /// @brief Serializes to XML
        /// @param parent The parent XML node to append to
        /// @return Result indicating success or failure
        result to_xml(pugi::xml_node& parent) const {
            if (!is_valid()) {
                auto res = result{ result::error_code::InvalidRemarks, "Invalid remarks data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("remarks");
            if (!source.empty()) node.append_attribute("source") = source.c_str();
            if (!to.empty()) node.append_attribute("to") = to.c_str();
            if (time.has_value()) {
                node.append_attribute("time") = time->to_cot_timestamp().c_str();
            }
            if (!text.empty()) node.text().set(text.c_str());
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized remarks
        static remarks from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            remarks r;
            if (node.name() != std::string("remarks")) {
                auto res = result{ result::error_code::InvalidRemarks, "Expected <remarks> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return r;
            }
            if (auto attr = node.attribute("source")) r.source = attr.as_string();
            if (auto attr = node.attribute("to")) r.to = attr.as_string();
            if (auto attr = node.attribute("time")) {
                std::string ts = attr.as_string();
                if (!ts.empty()) {
                    result ts_res;
                    datetime parsed = datetime::from_cot_timestamp(ts, &ts_res);
                    if (!ts_res.is_success()) {
                        auto res = result{ result::error_code::InvalidRemarks,
                                          "Invalid remarks time: " + ts_res.description };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        return r;
                    }
                    r.time = parsed;
                }
            }
            if (auto txt = node.text()) r.text = txt.as_string();
            if (!r.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidRemarks, "Invalid remarks data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return r;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const remarks& r) {
            os << "remarks: ";
            if (!r.is_valid()) os << " -NOT VALID- ";
            os << "\n\ttext: " << (r.text.empty() ? "None" : r.text) << "\n"
                << "\tsource: " << (r.source.empty() ? "None" : r.source) << "\n"
                << "\tto: " << (r.to.empty() ? "None" : r.to) << "\n"
                << "\ttime: " << (r.time.has_value() ? r.time->to_cot_timestamp() : std::string("None")) << "\n";
            return os;
        }

        std::string text{ "" };
        std::string source{ "" };
        std::string to{ "" };
        std::optional<datetime> time{ std::nullopt };

    private:
        inline static constexpr const char* INVALID_VALUE = "";
    };

}