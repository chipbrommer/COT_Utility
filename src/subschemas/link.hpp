#pragma once
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <pugixml.hpp>

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for <link> elements in <detail>
    class link {
    public:
        /// @brief Constructor - Initializes everything
        explicit link(std::string uid = INVALID_STRING,
            std::string type = INVALID_STRING,
            std::string relation = INVALID_STRING,
            std::string callsign = INVALID_STRING,
            std::string parent_callsign = INVALID_STRING,
            std::string device = INVALID_STRING,
            std::string event_ref = INVALID_STRING,
            std::string mime = INVALID_STRING,
            std::string mission = INVALID_STRING,
            std::string production_time = INVALID_STRING,
            std::string remarks = INVALID_STRING,
            std::string url = INVALID_STRING,
            double latitude = INVALID_VALUE,
            double longitude = INVALID_VALUE) noexcept
            : uid(std::move(uid)),
            type(std::move(type)),
            relation(std::move(relation)),
            callsign(std::move(callsign)),
            parent_callsign(std::move(parent_callsign)),
            device(std::move(device)),
            event_ref(std::move(event_ref)),
            mime(std::move(mime)),
            mission(std::move(mission)),
            production_time(std::move(production_time)),
            remarks(std::move(remarks)),
            url(std::move(url)),
            latitude(latitude),
            longitude(longitude) {
        }

        /// @brief Equality comparison
        bool operator==(const link& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            const bool lat_eq = (std::isnan(latitude) && std::isnan(other.latitude)) ||
                (!std::isnan(latitude) && !std::isnan(other.latitude) &&
                    std::abs(latitude - other.latitude) < EPSILON);
            const bool lon_eq = (std::isnan(longitude) && std::isnan(other.longitude)) ||
                (!std::isnan(longitude) && !std::isnan(other.longitude) &&
                    std::abs(longitude - other.longitude) < EPSILON);

            return uid == other.uid &&
                type == other.type &&
                relation == other.relation &&
                callsign == other.callsign &&
                parent_callsign == other.parent_callsign &&
                device == other.device &&
                event_ref == other.event_ref &&
                mime == other.mime &&
                mission == other.mission &&
                production_time == other.production_time &&
                remarks == other.remarks &&
                url == other.url &&
                lat_eq &&
                lon_eq;
        }

        /// @brief Inequality comparison
        bool operator!=(const link& other) const noexcept { return !(*this == other); }

        /// @brief Checks whether valid latitude and longitude coordinates are set
        [[nodiscard]] bool has_point() const noexcept {
            return !std::isnan(latitude) && !std::isnan(longitude);
        }

        /// @brief Checks whether any link attributes or coordinates are set
        [[nodiscard]] bool has_data() const noexcept {
            return !uid.empty() || !type.empty() || !relation.empty() ||
                !callsign.empty() || !parent_callsign.empty() || !device.empty() ||
                !event_ref.empty() || !mime.empty() || !mission.empty() ||
                !production_time.empty() || !remarks.empty() || !url.empty() ||
                !std::isnan(latitude) || !std::isnan(longitude);
        }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (!has_data()) {
                auto res = result{ result::error_code::InvalidLink, "Empty link data" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }

            const bool has_lat = !std::isnan(latitude);
            const bool has_lon = !std::isnan(longitude);
            if (has_lat != has_lon) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Incomplete point: both latitude and longitude must be provided together" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }

            if (has_lat && (latitude < -90.0 || latitude > 90.0)) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Invalid latitude: must be between -90 and 90" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }

            if (has_lon && (longitude < -180.0 || longitude > 180.0)) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Invalid longitude: must be between -180 and 180" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }

            if (uid.empty() && relation.empty() && mission.empty() && url.empty() && event_ref.empty()) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Link must specify at least uid, relation, mission, event, or url" };
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
                auto res = result{ result::error_code::InvalidLink,
                                  val_res.description().empty()
                                      ? std::string("Invalid link data")
                                      : val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }

            auto node = parent.append_child("link");
            if (!callsign.empty()) node.append_attribute("callsign") = callsign.c_str();
            if (!device.empty()) node.append_attribute("device") = device.c_str();
            if (!event_ref.empty()) node.append_attribute("event") = event_ref.c_str();
            if (!mime.empty()) node.append_attribute("mime") = mime.c_str();
            if (!mission.empty()) node.append_attribute("mission") = mission.c_str();
            if (!parent_callsign.empty()) node.append_attribute("parent_callsign") = parent_callsign.c_str();
            if (has_point()) {
                std::ostringstream oss;
                oss << std::setprecision(10) << latitude << "," << longitude;
                node.append_attribute("point") = oss.str().c_str();
            }
            if (!production_time.empty()) node.append_attribute("production_time") = production_time.c_str();
            if (!relation.empty()) node.append_attribute("relation") = relation.c_str();
            if (!remarks.empty()) node.append_attribute("remarks") = remarks.c_str();
            if (!type.empty()) node.append_attribute("type") = type.c_str();
            if (!uid.empty()) node.append_attribute("uid") = uid.c_str();
            if (!url.empty()) node.append_attribute("url") = url.c_str();
            return result{};
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized link
        static link from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            link l;
            if (node.name() != std::string_view("link")) {
                auto res = result{ result::error_code::InvalidLink, "Expected <link> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return l;
            }

            try {
                if (auto attr = node.attribute("callsign")) l.callsign = attr.as_string();
                if (auto attr = node.attribute("device")) l.device = attr.as_string();
                if (auto attr = node.attribute("event")) l.event_ref = attr.as_string();
                if (auto attr = node.attribute("mime")) l.mime = attr.as_string();
                if (auto attr = node.attribute("mission")) l.mission = attr.as_string();
                if (auto attr = node.attribute("parent_callsign")) l.parent_callsign = attr.as_string();
                if (auto attr = node.attribute("production_time")) l.production_time = attr.as_string();
                if (auto attr = node.attribute("relation")) l.relation = attr.as_string();
                if (auto attr = node.attribute("remarks")) l.remarks = attr.as_string();
                if (auto attr = node.attribute("type")) l.type = attr.as_string();
                if (auto attr = node.attribute("uid")) l.uid = attr.as_string();
                if (auto attr = node.attribute("url")) l.url = attr.as_string();

                if (auto attr = node.attribute("point")) {
                    std::string point_str = attr.as_string();
                    const std::size_t comma_pos = point_str.find(',');
                    if (comma_pos == std::string::npos ||
                        point_str.find(',', comma_pos + 1) != std::string::npos) {
                        auto res = result{ result::error_code::InvalidLink,
                                          "Invalid point attribute format: " + point_str };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        l.latitude = INVALID_VALUE;
                        l.longitude = INVALID_VALUE;
                        return l;
                    }

                    std::size_t lat_idx = 0;
                    std::size_t lon_idx = 0;
                    const std::string lat_part = point_str.substr(0, comma_pos);
                    const std::string lon_part = point_str.substr(comma_pos + 1);
                    const double lat = std::stod(lat_part, &lat_idx);
                    const double lon = std::stod(lon_part, &lon_idx);

                    if (lat_idx != lat_part.size() || lon_idx != lon_part.size() ||
                        !std::isfinite(lat) || !std::isfinite(lon)) {
                        auto res = result{ result::error_code::InvalidLink,
                                          "Invalid numeric coordinates in point attribute: " + point_str };
                        if (rslt) *rslt = res;
                        if (error_handler) error_handler(res);
                        l.latitude = INVALID_VALUE;
                        l.longitude = INVALID_VALUE;
                        return l;
                    }

                    l.latitude = lat;
                    l.longitude = lon;
                }

                result val_res;
                if (!l.is_valid(&val_res) && rslt && rslt->is_success()) {
                    *rslt = val_res;
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidLink,
                                  "Error parsing <link>: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                l.latitude = INVALID_VALUE;
                l.longitude = INVALID_VALUE;
            }

            return l;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const link& l) {
            os << "link: ";
            if (!l.is_valid()) os << " -NOT VALID- ";
            os << "\n\tuid: " << (l.uid.empty() ? "None" : l.uid)
                << "\n\ttype: " << (l.type.empty() ? "None" : l.type)
                << "\n\trelation: " << (l.relation.empty() ? "None" : l.relation)
                << "\n\tcallsign: " << (l.callsign.empty() ? "None" : l.callsign)
                << "\n\tparent_callsign: " << (l.parent_callsign.empty() ? "None" : l.parent_callsign)
                << "\n\tdevice: " << (l.device.empty() ? "None" : l.device)
                << "\n\tevent: " << (l.event_ref.empty() ? "None" : l.event_ref)
                << "\n\tmime: " << (l.mime.empty() ? "None" : l.mime)
                << "\n\tmission: " << (l.mission.empty() ? "None" : l.mission)
                << "\n\tproduction_time: " << (l.production_time.empty() ? "None" : l.production_time)
                << "\n\tremarks: " << (l.remarks.empty() ? "None" : l.remarks)
                << "\n\turl: " << (l.url.empty() ? "None" : l.url)
                << "\n\tlatitude: ";
            if (std::isnan(l.latitude)) os << "None";
            else os << std::fixed << std::setprecision(6) << l.latitude;
            os << "\n\tlongitude: ";
            if (std::isnan(l.longitude)) os << "None";
            else os << std::fixed << std::setprecision(6) << l.longitude;
            os << "\n";
            return os;
        }

        std::string uid{ INVALID_STRING };
        std::string type{ INVALID_STRING };
        std::string relation{ INVALID_STRING };
        std::string callsign{ INVALID_STRING };
        std::string parent_callsign{ INVALID_STRING };
        std::string device{ INVALID_STRING };
        std::string event_ref{ INVALID_STRING };
        std::string mime{ INVALID_STRING };
        std::string mission{ INVALID_STRING };
        std::string production_time{ INVALID_STRING };
        std::string remarks{ INVALID_STRING };
        std::string url{ INVALID_STRING };
        double latitude{ INVALID_VALUE };
        double longitude{ INVALID_VALUE };

    private:
        inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr const char* INVALID_STRING = "";
    };
}