#pragma once
#include <cmath>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <pugixml.hpp>

namespace cot::subschemas
{
    /// @brief Movement direction options for route link attributes
    enum class link_direction {
        Unknown,
        Infil,
        Exfil,
        Invalid
    };

    /// @brief Travel/movement method options for route link attributes
    enum class link_method {
        Unknown,
        Driving,
        Walking,
        Flying,
        Swimming,
        Watercraft,
        Invalid
    };

    /// @brief Ordering options for route waypoints/checkpoints
    enum class link_order {
        Unknown,
        Ascending,
        Descending,
        AscendingCheckPoints,
        DescendingCheckPoints,
        Invalid
    };

    /// @brief Route type options for route link attributes
    enum class link_route_type {
        Unknown,
        Primary,
        Secondary,
        Invalid
    };

    /// @brief Converts a link_direction enum to its CoT XML string representation
    [[nodiscard]] constexpr std::string_view to_string(link_direction dir) noexcept {
        switch (dir) {
        case link_direction::Infil:   return "Infil";
        case link_direction::Exfil:   return "Exfil";
        case link_direction::Invalid: return "Invalid";
        case link_direction::Unknown:
        default:                      return "";
        }
    }

    /// @brief Parses a string into a link_direction enum
    [[nodiscard]] inline link_direction parse_link_direction(std::string_view value) noexcept {
        if (value.empty()) return link_direction::Unknown;
        if (value == "Infil") return link_direction::Infil;
        if (value == "Exfil") return link_direction::Exfil;
        return link_direction::Invalid;
    }

    /// @brief Converts a link_method enum to its CoT XML string representation
    [[nodiscard]] constexpr std::string_view to_string(link_method m) noexcept {
        switch (m) {
        case link_method::Driving:    return "Driving";
        case link_method::Walking:    return "Walking";
        case link_method::Flying:     return "Flying";
        case link_method::Swimming:   return "Swimming";
        case link_method::Watercraft: return "Watercraft";
        case link_method::Invalid:    return "Invalid";
        case link_method::Unknown:
        default:                      return "";
        }
    }

    /// @brief Parses a string into a link_method enum
    [[nodiscard]] inline link_method parse_link_method(std::string_view value) noexcept {
        if (value.empty()) return link_method::Unknown;
        if (value == "Driving") return link_method::Driving;
        if (value == "Walking") return link_method::Walking;
        if (value == "Flying") return link_method::Flying;
        if (value == "Swimming") return link_method::Swimming;
        if (value == "Watercraft") return link_method::Watercraft;
        return link_method::Invalid;
    }

    /// @brief Converts a link_order enum to its CoT XML string representation
    [[nodiscard]] constexpr std::string_view to_string(link_order ord) noexcept {
        switch (ord) {
        case link_order::Ascending:             return "Ascending";
        case link_order::Descending:            return "Descending";
        case link_order::AscendingCheckPoints:  return "Ascending Check Points";
        case link_order::DescendingCheckPoints: return "Descending Check Points";
        case link_order::Invalid:               return "Invalid";
        case link_order::Unknown:
        default:                                return "";
        }
    }

    /// @brief Parses a string into a link_order enum
    [[nodiscard]] inline link_order parse_link_order(std::string_view value) noexcept {
        if (value.empty()) return link_order::Unknown;
        if (value == "Ascending") return link_order::Ascending;
        if (value == "Descending") return link_order::Descending;
        if (value == "Ascending Check Points") return link_order::AscendingCheckPoints;
        if (value == "Descending Check Points") return link_order::DescendingCheckPoints;
        return link_order::Invalid;
    }

    /// @brief Converts a link_route_type enum to its CoT XML string representation
    [[nodiscard]] constexpr std::string_view to_string(link_route_type rt) noexcept {
        switch (rt) {
        case link_route_type::Primary:   return "Primary";
        case link_route_type::Secondary: return "Secondary";
        case link_route_type::Invalid:   return "Invalid";
        case link_route_type::Unknown:
        default:                         return "";
        }
    }

    /// @brief Parses a string into a link_route_type enum
    [[nodiscard]] inline link_route_type parse_link_route_type(std::string_view value) noexcept {
        if (value.empty()) return link_route_type::Unknown;
        if (value == "Primary") return link_route_type::Primary;
        if (value == "Secondary") return link_route_type::Secondary;
        return link_route_type::Invalid;
    }

    /// @brief A CoT Message subschema class for route link attributes
    class link_attr {
    public:
        /// @brief Constructor - Initializes everything
        explicit link_attr(double color = INVALID_NUMBER,
            std::string method = INVALID_STRING,
            std::string direction = INVALID_STRING,
            std::string routetype = INVALID_STRING,
            std::string order = INVALID_STRING,
            std::string planningmethod = INVALID_STRING,
            std::string prefix = INVALID_STRING,
            std::string stroke = INVALID_STRING,
            std::string style = INVALID_STRING,
            std::string type = INVALID_STRING)
            : color(color),
            method(std::move(method)),
            direction(std::move(direction)),
            routetype(std::move(routetype)),
            order(std::move(order)),
            planningmethod(std::move(planningmethod)),
            prefix(std::move(prefix)),
            stroke(std::move(stroke)),
            style(std::move(style)),
            type(std::move(type)) {
        }

        /// @brief Equality comparison
        bool operator==(const link_attr& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            const bool color_eq = (std::isnan(color) && std::isnan(other.color)) ||
                (std::isfinite(color) && std::isfinite(other.color) && std::abs(color - other.color) < EPSILON);
            return color_eq &&
                method == other.method &&
                direction == other.direction &&
                routetype == other.routetype &&
                order == other.order &&
                planningmethod == other.planningmethod &&
                prefix == other.prefix &&
                stroke == other.stroke &&
                style == other.style &&
                type == other.type;
        }

        /// @brief Inequality comparison
        bool operator!=(const link_attr& other) const noexcept { return !(*this == other); }

        /// @brief Gets direction as an enum
        [[nodiscard]] link_direction get_direction_enum() const noexcept {
            return parse_link_direction(direction);
        }

        /// @brief Sets direction from an enum
        void set_direction_enum(link_direction val) {
            direction = (val == link_direction::Unknown || val == link_direction::Invalid)
                ? std::string{}
            : std::string(to_string(val));
        }

        /// @brief Gets method as an enum
        [[nodiscard]] link_method get_method_enum() const noexcept {
            return parse_link_method(method);
        }

        /// @brief Sets method from an enum
        void set_method_enum(link_method val) {
            method = (val == link_method::Unknown || val == link_method::Invalid)
                ? std::string{}
            : std::string(to_string(val));
        }

        /// @brief Gets order as an enum
        [[nodiscard]] link_order get_order_enum() const noexcept {
            return parse_link_order(order);
        }

        /// @brief Sets order from an enum
        void set_order_enum(link_order val) {
            order = (val == link_order::Unknown || val == link_order::Invalid)
                ? std::string{}
            : std::string(to_string(val));
        }

        /// @brief Gets planningmethod as an enum
        [[nodiscard]] link_direction get_planningmethod_enum() const noexcept {
            return parse_link_direction(planningmethod);
        }

        /// @brief Sets planningmethod from an enum
        void set_planningmethod_enum(link_direction val) {
            planningmethod = (val == link_direction::Unknown || val == link_direction::Invalid)
                ? std::string{}
            : std::string(to_string(val));
        }

        /// @brief Gets routetype as an enum
        [[nodiscard]] link_route_type get_routetype_enum() const noexcept {
            return parse_link_route_type(routetype);
        }

        /// @brief Sets routetype from an enum
        void set_routetype_enum(link_route_type val) {
            routetype = (val == link_route_type::Unknown || val == link_route_type::Invalid)
                ? std::string{}
            : std::string(to_string(val));
        }

        /// @brief Checks if any attribute fields are populated
        [[nodiscard]] bool has_data() const noexcept {
            return !std::isnan(color) || !method.empty() || !direction.empty() ||
                !routetype.empty() || !order.empty() || !planningmethod.empty() ||
                !prefix.empty() || !stroke.empty() || !style.empty() || !type.empty();
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!has_data()) {
                auto res = result{ result::error_code::InvalidLinkAttr, "Empty link_attr data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!std::isnan(color) && !std::isfinite(color)) {
                auto res = result{ result::error_code::InvalidLinkAttr, "Invalid non-finite color value" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (get_direction_enum() == link_direction::Invalid) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Invalid direction value: " + direction };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (get_method_enum() == link_method::Invalid) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Invalid method value: " + method };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (get_order_enum() == link_order::Invalid) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Invalid order value: " + order };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (get_planningmethod_enum() == link_direction::Invalid) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Invalid planningmethod value: " + planningmethod };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (get_routetype_enum() == link_route_type::Invalid) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Invalid routetype value: " + routetype };
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
            result val_res;
            if (!is_valid(&val_res)) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  val_res.description().empty()
                                      ? std::string("Invalid link_attr data")
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            auto node = parent.append_child("link_attr");
            if (!std::isnan(color)) node.append_attribute("color") = color;
            if (!method.empty()) node.append_attribute("method") = method.c_str();
            if (!direction.empty()) node.append_attribute("direction") = direction.c_str();
            if (!routetype.empty()) node.append_attribute("routetype") = routetype.c_str();
            if (!order.empty()) node.append_attribute("order") = order.c_str();
            if (!planningmethod.empty()) node.append_attribute("planningmethod") = planningmethod.c_str();
            if (!prefix.empty()) node.append_attribute("prefix") = prefix.c_str();
            if (!stroke.empty()) node.append_attribute("stroke") = stroke.c_str();
            if (!style.empty()) node.append_attribute("style") = style.c_str();
            if (!type.empty()) node.append_attribute("type") = type.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized link_attr
        static link_attr from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            link_attr attr_obj;
            if (node.name() != std::string_view("link_attr")) {
                auto res = result{ result::error_code::InvalidLinkAttr, "Expected <link_attr> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return attr_obj;
            }

            try {
                if (auto attr = node.attribute("color")) {
                    std::string raw = attr.as_string();
                    if (!raw.empty()) {
                        std::size_t idx = 0;
                        double parsed = std::stod(raw, &idx);
                        if (idx != raw.size() || !std::isfinite(parsed)) {
                            auto res = result{ result::error_code::InvalidLinkAttr,
                                              "Invalid numeric color attribute: " + raw };
                            if (rslt) *rslt = res;
                            if (error_handler) error_handler(res);
                            return attr_obj;
                        }
                        attr_obj.color = parsed;
                    }
                }
                if (auto attr = node.attribute("method")) attr_obj.method = attr.as_string();
                if (auto attr = node.attribute("direction")) attr_obj.direction = attr.as_string();
                if (auto attr = node.attribute("routetype")) attr_obj.routetype = attr.as_string();
                if (auto attr = node.attribute("order")) attr_obj.order = attr.as_string();
                if (auto attr = node.attribute("planningmethod")) attr_obj.planningmethod = attr.as_string();
                if (auto attr = node.attribute("prefix")) attr_obj.prefix = attr.as_string();
                if (auto attr = node.attribute("stroke")) attr_obj.stroke = attr.as_string();
                if (auto attr = node.attribute("style")) attr_obj.style = attr.as_string();
                if (auto attr = node.attribute("type")) attr_obj.type = attr.as_string();

                result val_res;
                if (!attr_obj.is_valid(&val_res) && rslt && rslt->is_success()) {
                    *rslt = val_res;
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidLinkAttr,
                                  "Error parsing <link_attr>: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }

            return attr_obj;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const link_attr& a) {
            os << "link_attr: ";
            if (!a.is_valid()) os << " -NOT VALID- ";
            os << "\n\tcolor: " << (std::isnan(a.color) ? std::string("None") : std::to_string(a.color))
                << "\n\tmethod: " << (a.method.empty() ? "None" : a.method)
                << "\n\tdirection: " << (a.direction.empty() ? "None" : a.direction)
                << "\n\troutetype: " << (a.routetype.empty() ? "None" : a.routetype)
                << "\n\torder: " << (a.order.empty() ? "None" : a.order)
                << "\n\tplanningmethod: " << (a.planningmethod.empty() ? "None" : a.planningmethod)
                << "\n\tprefix: " << (a.prefix.empty() ? "None" : a.prefix)
                << "\n\tstroke: " << (a.stroke.empty() ? "None" : a.stroke)
                << "\n\tstyle: " << (a.style.empty() ? "None" : a.style)
                << "\n\ttype: " << (a.type.empty() ? "None" : a.type)
                << "\n";
            return os;
        }

        double color{ INVALID_NUMBER };
        std::string method{ INVALID_STRING };
        std::string direction{ INVALID_STRING };
        std::string routetype{ INVALID_STRING };
        std::string order{ INVALID_STRING };
        std::string planningmethod{ INVALID_STRING };
        std::string prefix{ INVALID_STRING };
        std::string stroke{ INVALID_STRING };
        std::string style{ INVALID_STRING };
        std::string type{ INVALID_STRING };

    private:
        inline static constexpr double INVALID_NUMBER = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr const char* INVALID_STRING = "";
    };
}