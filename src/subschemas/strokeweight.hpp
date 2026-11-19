#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for strokeweight data
    class strokeweight {
    public:
        /// @brief Constructor - Initializes everything
        explicit strokeweight(double value = INVALID_NUM) noexcept : value(value) {}

        /// @brief Equality comparison
        bool operator==(const strokeweight& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return (std::isnan(value) && std::isnan(other.value)) ||
                std::abs(value - other.value) < EPSILON;
        }

        /// @brief Inequality comparison
        bool operator!=(const strokeweight& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(value)) {
                auto res = result{ result::error_code::InvalidStrokeWeight, "strokeweight value is unset" };
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
                auto res = result{ result::error_code::InvalidStrokeWeight, "Invalid strokeweight data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("strokeweight");
            node.append_attribute("value") = value;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized strokeweight
        static strokeweight from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            strokeweight sw;
            if (node.name() != std::string("strokeweight")) {
                auto res = result{ result::error_code::InvalidStrokeWeight, "Expected <strokeweight> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return sw;
            }
            if (auto attr = node.attribute("value")) {
                try {
                    sw.value = std::stod(attr.as_string());
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidStrokeWeight,
                                      "Error parsing strokeweight value: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    sw.value = INVALID_NUM;
                }
            }
            if (!sw.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidStrokeWeight, "Invalid strokeweight data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return sw;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const strokeweight& sw) {
            os << "strokeweight: ";
            if (!sw.is_valid()) os << " -NOT VALID- (Unset)";
            else os << "\n\tvalue: " << sw.value;
            os << "\n";
            return os;
        }

        double value{ INVALID_NUM };

    private:
        inline static constexpr double INVALID_NUM = std::numeric_limits<double>::quiet_NaN();
    };
}