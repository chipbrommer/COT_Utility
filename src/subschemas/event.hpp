#pragma once 

#include "datetime.hpp"
#include "detail.hpp"
#include "point.hpp"

namespace cot::subschemas
{
    /// @brief A CoT Message subschema class for event data
    class event {
    public:
        /// @brief Constructor - Initializes everything
        explicit event(double version = INVALID_VERSION,
            std::string uid = INVALID_STRING,
            std::string type = INVALID_STRING,
            datetime time = datetime(),
            datetime start = datetime(),
            datetime stale = datetime(),
            std::string how = INVALID_STRING,
            point p = point(),
            detail d = detail(),
            std::string access = INVALID_STRING,
            std::string opex = INVALID_STRING,
            std::string qos = INVALID_STRING)
            : version(version),
            uid(std::move(uid)),
            type(std::move(type)),
            time(std::move(time)),
            start(std::move(start)),
            stale(std::move(stale)),
            how(std::move(how)),
            access(std::move(access)),
            opex(std::move(opex)),
            qos(std::move(qos)),
            rootType(Root::Type::Unknown),
            indicator(Point::Type::Unknown),
            location(Location::Type::Unknown),
            howEntry(How::Entry::Type::Unknown),
            howData(How::Data::Type::Unknown),
            point_(std::move(p)),
            detail_(std::move(d)) {
            sync_how_enums();
        }

        /// @brief Equality comparison
        bool operator==(const event& other) const noexcept {
            constexpr double EPSILON = 1e-6;
            bool versionEqual = (std::isnan(version) && std::isnan(other.version)) ||
                std::abs(version - other.version) < EPSILON;
            return versionEqual &&
                uid == other.uid &&
                type == other.type &&
                time == other.time &&
                start == other.start &&
                stale == other.stale &&
                how == other.how &&
                access == other.access &&
                opex == other.opex &&
                qos == other.qos &&
                rootType == other.rootType &&
                indicator == other.indicator &&
                location == other.location &&
                howEntry == other.howEntry &&
                howData == other.howData &&
                point_ == other.point_ &&
                detail_ == other.detail_;
        }

        /// @brief Inequality comparison
        bool operator!=(const event& other) const noexcept { return !(*this == other); }

        /// @brief Checks if the class has valid data
        /// @param rslt Optional result structure for invalid reason
        /// @return True if valid, false otherwise
        bool is_valid(result* rslt = nullptr) const {
            if (std::isnan(version) || !std::isfinite(version) || version < 1.0 || version > 2.0) {
                auto res = result{ result::error_code::InvalidEvent, "Invalid version" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (uid.empty()) {
                auto res = result{ result::error_code::InvalidEvent, "Empty UID" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (type.empty()) {
                auto res = result{ result::error_code::InvalidType, "Empty type" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!time.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidTime, "Invalid time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!start.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidTime, "Invalid start time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!stale.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidTime, "Invalid stale time" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (how.empty()) {
                auto res = result{ result::error_code::InvalidHow, "Empty how" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (howEntry == How::Entry::Type::Error || howData == How::Data::Type::Error) {
                auto res = result{ result::error_code::InvalidHow, "Invalid how format or code: " + how };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!point_.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidPoint, "Invalid point" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return false;
            }
            if (!detail_.is_valid(rslt)) {
                auto res = result{ result::error_code::InvalidDetail, "Invalid detail" };
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
                auto res = result{ result::error_code::InvalidEvent, val_res.description() };
                if (error_handler) error_handler(res);
                return res;
            }
            auto node = parent.append_child("event");
            node.append_attribute("version") = version;
            node.append_attribute("uid") = uid.c_str();
            node.append_attribute("type") = type.c_str();
            node.append_attribute("time") = time.to_cot_timestamp().c_str();
            node.append_attribute("start") = start.to_cot_timestamp().c_str();
            node.append_attribute("stale") = stale.to_cot_timestamp().c_str();
            node.append_attribute("how") = how.c_str();
            if (!access.empty()) node.append_attribute("access") = access.c_str();
            if (!opex.empty()) node.append_attribute("opex") = opex.c_str();
            if (!qos.empty()) node.append_attribute("qos") = qos.c_str();

            auto res = point_.to_xml(node);
            if (!res.is_success()) return res;
            res = detail_.to_xml(node);
            if (!res.is_success()) return res;
            return result{};
        }

        /// @brief Serializes to XML string
        /// @param rslt Optional result for error details
        /// @return XML string or empty if invalid
        std::string to_xml_string(result* rslt = nullptr) const {
            pugi::xml_document doc;
            auto res = to_xml(doc);
            if (!res.is_success()) {
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return "";
            }
            return xml_node_to_string(doc.child("event"), rslt);
        }

        /// @brief Deserializes from an XML node
        /// @param node The XML node to parse
        /// @param rslt Optional result for error details
        /// @return Deserialized event
        static event from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
            event e;
            if (node.name() != std::string("event")) {
                auto res = result{ result::error_code::InvalidEvent, "Expected <event> node" };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
                return e;
            }
            try {
                if (auto attr = node.attribute("version")) {
                    std::string raw = attr.as_string();
                    if (!raw.empty()) {
                        std::size_t idx = 0;
                        double parsed = std::stod(raw, &idx);
                        e.version = (idx == raw.size()) ? parsed : INVALID_VERSION;
                    }
                }
                e.uid = node.attribute("uid").as_string();
                e.type = node.attribute("type").as_string();
                e.time = datetime::from_cot_timestamp(node.attribute("time").as_string(), rslt);
                e.start = datetime::from_cot_timestamp(node.attribute("start").as_string(), rslt);
                e.stale = datetime::from_cot_timestamp(node.attribute("stale").as_string(), rslt);
                e.how = node.attribute("how").as_string();
                e.access = node.attribute("access").as_string();
                e.opex = node.attribute("opex").as_string();
                e.qos = node.attribute("qos").as_string();
                e.sync_how_enums();

                for (const auto& child : node.children()) {
                    std::string name = child.name();
                    if (name == "point") e.point_ = point::from_xml(child, rslt);
                    else if (name == "detail") e.detail_ = detail::from_xml(child, rslt);
                }
                if (!e.is_valid(rslt) && rslt && rslt->is_success()) {
                    *rslt = result{ result::error_code::InvalidEvent, "Invalid event data after parsing" };
                    if (error_handler) error_handler(*rslt);
                }
            }
            catch (const std::exception& ex) {
                auto res = result{ result::error_code::InvalidEvent,
                                  "Error parsing event: " + std::string(ex.what()) };
                if (rslt) *rslt = res;
                if (error_handler) error_handler(res);
            }
            return e;
        }

        /// @brief Stream output for printing
        friend std::ostream& operator<<(std::ostream& os, const event& e) {
            auto lookup_entry = [](How::Entry::Type t) -> const std::string& {
                auto it = How::Entry::TypeToString.find(t);
                static const std::string fallback = "Unknown";
                return (it != How::Entry::TypeToString.end()) ? it->second : fallback;
                };
            auto lookup_data = [](How::Data::Type t) -> const std::string& {
                auto it = How::Data::TypeToString.find(t);
                static const std::string fallback = "Unknown";
                return (it != How::Data::TypeToString.end()) ? it->second : fallback;
                };

            os << "event: ";
            if (!e.is_valid()) os << " -NOT VALID- ";
            os << "\n\tversion: " << (std::isnan(e.version) ? "NaN" : std::to_string(e.version))
                << "\n\ttype: " << (e.type.empty() ? "None" : e.type)
                << "\n\trootType: " << static_cast<int>(e.rootType) << " - "
                << Root::TypeToString.at(e.rootType)
                << "\n\tindicator: " << static_cast<int>(e.indicator) << " - "
                << Point::TypeToString.at(e.indicator)
                << "\n\tlocation: " << static_cast<int>(e.location) << " - "
                << Location::TypeToString.at(e.location)
                << "\n\tuid: " << (e.uid.empty() ? "None" : e.uid)
                << "\n\ttime: " << e.time
                << "\n\tstart: " << e.start
                << "\n\tstale: " << e.stale
                << "\n\thow: " << (e.how.empty() ? "None" : e.how)
                << "\n\thowEntry: " << static_cast<int>(e.howEntry) << " - "
                << lookup_entry(e.howEntry)
                << "\n\thowData: " << static_cast<int>(e.howData) << " - "
                << lookup_data(e.howData)
                << "\n\taccess: " << (e.access.empty() ? "None" : e.access)
                << "\n\topex: " << (e.opex.empty() ? "None" : e.opex)
                << "\n\tqos: " << (e.qos.empty() ? "None" : e.qos)
                << "\n" << e.point_ << e.detail_;
            return os;
        }

        /// @brief Getter for point
        [[nodiscard]] const point& get_point() const noexcept { return point_; }

        /// @brief Getter for detail
        [[nodiscard]] const detail& get_detail() const noexcept { return detail_; }

        /// @brief Setter for point
        result set_point(point newPoint) {
            if (!newPoint.is_valid()) {
                auto res = result{ result::error_code::InvalidPoint, "Cannot set invalid point" };
                if (error_handler) error_handler(res);
                return res;
            }
            point_ = std::move(newPoint);
            return result{};
        }

        /// @brief Setter for detail
        result set_detail(detail newDetail) {
            if (!newDetail.is_valid()) {
                auto res = result{ result::error_code::InvalidDetail, "Cannot set invalid detail" };
                if (error_handler) error_handler(res);
                return res;
            }
            detail_ = std::move(newDetail);
            return result{};
        }

        /// @brief Clears point
        void clear_point() noexcept { point_ = point(); }

        /// @brief Clears detail
        void clear_detail() noexcept { detail_ = detail(); }

        double version{ INVALID_VERSION };
        std::string uid{ INVALID_STRING };
        std::string type{ INVALID_STRING };
        datetime time;
        datetime start;
        datetime stale;
        std::string how{ INVALID_STRING };
        std::string access{ INVALID_STRING };
        std::string opex{ INVALID_STRING };
        std::string qos{ INVALID_STRING };
        Root::Type rootType{ Root::Type::Unknown };
        Point::Type indicator{ Point::Type::Unknown };
        Location::Type location{ Location::Type::Unknown };
        How::Entry::Type howEntry{ How::Entry::Type::Unknown };
        How::Data::Type howData{ How::Data::Type::Unknown };
        point point_;
        detail detail_;

    private:
        /// @brief Synchronizes howEntry and howData enums from the raw `how` string (e.g., "m-g")
        void sync_how_enums() noexcept {
            if (how.empty()) {
                howEntry = How::Entry::Type::Unknown;
                howData = How::Data::Type::Unknown;
                return;
            }
            auto dash = how.find('-');
            if (dash == std::string::npos) {
                howEntry = How::Entry::Type::Error;
                howData = How::Data::Type::Error;
                return;
            }
            std::string entry_part = how.substr(0, dash);
            std::string data_part = how.substr(dash + 1);
            howEntry = How::Entry::CharToType(entry_part);
            if (howEntry == How::Entry::Type::Error) {
                howData = How::Data::Type::Error;
                return;
            }
            howData = How::Data::CharToType(data_part, howEntry);
        }

        inline static constexpr double INVALID_VERSION = std::numeric_limits<double>::quiet_NaN();
        inline static constexpr const char* INVALID_STRING = "";
    };
}