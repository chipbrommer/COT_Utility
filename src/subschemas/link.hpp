#pragma once
namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for link data
	class link {
	public:
		/// @brief Constructor - Initializes everything
		explicit link(std::string uid = INVALID_STRING,
			std::string remarks = INVALID_STRING,
			std::string relation = INVALID_STRING,
			std::string callsign = INVALID_STRING,
			std::string type = VALID_TYPE,
			double latitude = INVALID_VALUE,
			double longitude = INVALID_VALUE)
			: uid(std::move(uid)), remarks(std::move(remarks)), relation(std::move(relation)),
			callsign(std::move(callsign)), type(std::move(type)),
			latitude(latitude), longitude(longitude) {
		}

		/// @brief Equality comparison
		bool operator==(const link& other) const noexcept {
			constexpr double EPSILON = 1e-6;
			return uid == other.uid &&
				remarks == other.remarks &&
				relation == other.relation &&
				callsign == other.callsign &&
				type == other.type &&
				(std::isnan(latitude) && std::isnan(other.latitude) ||
					std::abs(latitude - other.latitude) < EPSILON) &&
				(std::isnan(longitude) && std::isnan(other.longitude) ||
					std::abs(longitude - other.longitude) < EPSILON);
		}

		/// @brief Inequality comparison
		bool operator!=(const link& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (uid.empty()) {
				auto res = result{ result::error_code::InvalidLink, "Empty uid field" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (type.empty()) {
				auto res = result{ result::error_code::InvalidLink, "Empty type field" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (std::isnan(latitude) || latitude < -90.0 || latitude > 90.0) {
				auto res = result{ result::error_code::InvalidLink,
								  "Invalid latitude: must be between -90 and 90" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (std::isnan(longitude) || longitude < -180.0 || longitude > 180.0) {
				auto res = result{ result::error_code::InvalidLink,
								  "Invalid longitude: must be between -180 and 180" };
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
				auto res = result{ result::error_code::InvalidLink, "Invalid link data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("link");
			node.append_attribute("uid") = uid.c_str();
			if (!remarks.empty()) node.append_attribute("remarks") = remarks.c_str();
			if (!relation.empty()) node.append_attribute("relation") = relation.c_str();
			if (!callsign.empty()) node.append_attribute("callsign") = callsign.c_str();
			node.append_attribute("type") = type.c_str();
			std::string point = std::to_string(latitude) + "," + std::to_string(longitude);
			node.append_attribute("point") = point.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized link
		static link from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			link l;
			if (node.name() != std::string("link")) {
				auto res = result{ result::error_code::InvalidLink, "Expected <link> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return l;
			}
			if (auto attr = node.attribute("uid")) l.uid = attr.as_string();
			if (auto attr = node.attribute("remarks")) l.remarks = attr.as_string();
			if (auto attr = node.attribute("relation")) l.relation = attr.as_string();
			if (auto attr = node.attribute("callsign")) l.callsign = attr.as_string();
			if (auto attr = node.attribute("type")) l.type = attr.as_string();
			if (auto attr = node.attribute("point")) {
				std::string point = attr.as_string();
				size_t comma_pos = point.find(',');
				if (comma_pos != std::string::npos) {
					try {
						l.latitude = std::stod(point.substr(0, comma_pos));
						l.longitude = std::stod(point.substr(comma_pos + 1));
					}
					catch (const std::exception& e) {
						auto res = result{ result::error_code::InvalidLink,
										  "Invalid point format: " + std::string(e.what()) };
						if (rslt) *rslt = res;
						if (error_handler) error_handler(res);
						l.latitude = INVALID_VALUE;
						l.longitude = INVALID_VALUE;
					}
				}
			}
			if (!l.is_valid(rslt) && rslt && rslt->is_success()) {
				*rslt = result{ result::error_code::InvalidLink, "Invalid link data after parsing" };
				if (error_handler) error_handler(*rslt);
			}
			return l;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const link& l) {
			os << "link: ";
			if (!l.is_valid()) os << " -NOT VALID- ";
			os << "\n"
				<< "\tuid: " << (l.uid.empty() ? "None" : l.uid) << "\n"
				<< "\tremarks: " << (l.remarks.empty() ? "None" : l.remarks) << "\n"
				<< "\trelation: " << (l.relation.empty() ? "None" : l.relation) << "\n"
				<< "\tcallsign: " << (l.callsign.empty() ? "None" : l.callsign) << "\n"
				<< "\ttype: " << (l.type.empty() ? "None" : l.type) << "\n"
				<< "\tlatitude: ";
			if (std::isnan(l.latitude)) os << "NaN";
			else os << std::fixed << std::setprecision(6) << l.latitude;
			os << "\n"
				<< "\tlongitude: ";
			if (std::isnan(l.longitude)) os << "NaN";
			else os << std::fixed << std::setprecision(6) << l.longitude;
			os << "\n";
			return os;
		}

		std::string uid{ "" };
		std::string remarks{ "" };
		std::string relation{ "" };
		std::string callsign{ "" };
		std::string type{ "" };
		double latitude{ 0.0 };
		double longitude{ 0.0 };

	private:
		inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
		inline static constexpr const char* INVALID_STRING = "";
		inline static constexpr std::string_view VALID_TYPE = "a-f-G";
	};
}