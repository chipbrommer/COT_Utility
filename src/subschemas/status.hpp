#pragma once
namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for status data
    class status {
    public:
        /// @brief Constructor - Initializes everything
        explicit status(double battery = INVALID_VALUE) : battery(battery) {}

        /// @brief Equality comparison
        bool operator==(const status& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            return (std::isnan(battery) && std::isnan(other.battery)) ||
                std::abs(battery - other.battery) < EPSILON;
        }

        /// @brief Inequality comparison
        bool operator!=(const status& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(battery)) {
                auto res = result{ result::error_code::InvalidStatus, "Battery is unset (NaN)" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (battery < 0.0 || battery > 100.0) {
                auto res = result{ result::error_code::InvalidStatus, "Battery out of range (0-100)" };
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
                auto res = result{ result::error_code::InvalidStatus, "Invalid status data" };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("status");
            node.append_attribute("battery") = battery;
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized status
        static status from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            status s;
            if (node.name() != std::string("status")) {
                auto res = result{ result::error_code::InvalidStatus, "Expected <status> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return s;
            }
            if (auto attr = node.attribute("battery")) {
                try {
                    s.battery = attr.as_double(INVALID_VALUE);
                }
                catch (const std::exception& e) {
                    auto res = result{ result::error_code::InvalidStatus,
                                      "Error parsing battery: " + std::string(e.what()) };
                    if (rslt) *rslt = res;
                    if (error_handler) error_handler(res);
                    s.battery = INVALID_VALUE;
                }
            }
            if (!s.is_valid(rslt) && rslt && rslt->is_success()) {
                *rslt = result{ result::error_code::InvalidStatus, "Invalid status data after parsing" };
                if (error_handler) error_handler(*rslt);
            }
            return s;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const status& s) {
            os << "status: ";
            if (!s.is_valid()) os << " -NOT VALID- ";
            os << "\n\tbattery: ";
            if (std::isnan(s.battery)) os << "NaN";
            else os << std::fixed << std::setprecision(2) << s.battery;
            os << "\n";
            return os;
        }

        double battery{ 0.0 };

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
    };

}