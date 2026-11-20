#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            point.hpp
// @brief           A CoT Message subschema class for point data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstddef>
#include <exception>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../handlers/error_handler.hpp"
#include "../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for point data
	class point {
	public:
		/// @brief Constructor - Initializes everything
		explicit point(double latitude = INVALID_VALUE,
			double longitude = INVALID_VALUE,
			double hae = INVALID_VALUE,
			double circularError = INVALID_VALUE,
			double linearError = INVALID_VALUE) noexcept
			: latitude(latitude), longitude(longitude), hae(hae),
			circularError(circularError), linearError(linearError) {
		}

		/// @brief Equality comparison
		bool operator==(const point& other) const noexcept {
			constexpr double EPSILON = 1e-6;
			auto equal = [](double a, double b) noexcept {
				return (std::isnan(a) && std::isnan(b)) ||
					(!std::isnan(a) && !std::isnan(b) && std::abs(a - b) < EPSILON);
				};
			return equal(latitude, other.latitude) &&
				equal(longitude, other.longitude) &&
				equal(hae, other.hae) &&
				equal(circularError, other.circularError) &&
				equal(linearError, other.linearError);
		}

		/// @brief Inequality comparison
		bool operator!=(const point& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (!std::isfinite(latitude) || latitude < LAT_MIN || latitude > LAT_MAX) {
				auto res = result{ result::error_code::InvalidPoint,
								  "Invalid latitude: must be a finite number between -90 and 90" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isfinite(longitude) || longitude < LON_MIN || longitude > LON_MAX) {
				auto res = result{ result::error_code::InvalidPoint,
								  "Invalid longitude: must be a finite number between -180 and 180" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(hae) && !std::isfinite(hae)) {
				auto res = result{ result::error_code::InvalidPoint,
								  "HAE must be finite when set" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(circularError) && (!std::isfinite(circularError) || circularError < 0.0)) {
				auto res = result{ result::error_code::InvalidPoint,
								  "Circular error must be non-negative and finite" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(linearError) && (!std::isfinite(linearError) || linearError < 0.0)) {
				auto res = result{ result::error_code::InvalidPoint,
								  "Linear error must be non-negative and finite" };
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
				auto res = result{ result::error_code::InvalidPoint, val_res.description() };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("point");
			node.append_attribute("lat") = latitude;
			node.append_attribute("lon") = longitude;
			if (!std::isnan(hae)) node.append_attribute("hae") = hae;
			if (!std::isnan(circularError)) node.append_attribute("ce") = circularError;
			if (!std::isnan(linearError)) node.append_attribute("le") = linearError;
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized point
		static point from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			point p;
			if (node.name() != std::string("point")) {
				auto res = result{ result::error_code::InvalidPoint, "Expected <point> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return p;
			}

			auto parse_attr = [&rslt](const char* attr_name, double& target) -> bool {
				auto attr = node.attribute(attr_name);
				if (!attr) return true;
				std::string raw = attr.as_string();
				if (raw.empty()) return true;
				try {
					std::size_t idx = 0;
					double parsed = std::stod(raw, &idx);
					if (idx != raw.size()) {
						auto res = result{ result::error_code::InvalidPoint,
										  std::string("Trailing characters in point attribute '") + attr_name + "'" };
						if (rslt) *rslt = res;
						if (error_handler) error_handler(res);
						target = INVALID_VALUE;
						return false;
					}
					target = parsed;
					return true;
				}
				catch (const std::exception& e) {
					auto res = result{ result::error_code::InvalidPoint,
									  std::string("Error parsing point attribute '") + attr_name + "': " + e.what() };
					if (rslt) *rslt = res;
					if (error_handler) error_handler(res);
					target = INVALID_VALUE;
					return false;
				}
				};

			bool ok = true;
			ok = parse_attr("lat", p.latitude) && ok;
			ok = parse_attr("lon", p.longitude) && ok;
			ok = parse_attr("hae", p.hae) && ok;
			ok = parse_attr("ce", p.circularError) && ok;
			ok = parse_attr("le", p.linearError) && ok;

			if (ok && !p.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidPoint,
								  "Invalid point data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return p;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const point& p) {
			std::ios::fmtflags old_flags = os.flags();
			std::streamsize old_precision = os.precision();

			os << "point: ";
			if (!p.is_valid()) os << " -NOT VALID- ";
			os << "\n\tlatitude: ";
			if (std::isnan(p.latitude)) os << "NaN";
			else os << std::fixed << std::setprecision(6) << p.latitude;
			os << "\n\tlongitude: ";
			if (std::isnan(p.longitude)) os << "NaN";
			else os << std::fixed << std::setprecision(6) << p.longitude;

			os.flags(old_flags);
			os.precision(old_precision);

			os << "\n\thae: ";
			if (std::isnan(p.hae)) os << "NaN"; else os << p.hae;
			os << "\n\tce: ";
			if (std::isnan(p.circularError)) os << "NaN"; else os << p.circularError;
			os << "\n\tle: ";
			if (std::isnan(p.linearError)) os << "NaN"; else os << p.linearError;
			os << "\n";
			return os;
		}

		double latitude{ INVALID_VALUE };
		double longitude{ INVALID_VALUE };
		double hae{ INVALID_VALUE };
		double circularError{ INVALID_VALUE };
		double linearError{ INVALID_VALUE };

	private:
		inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
		inline static constexpr double LAT_MIN = -90.0;
		inline static constexpr double LAT_MAX = 90.0;
		inline static constexpr double LON_MIN = -180.0;
		inline static constexpr double LON_MAX = 180.0;
	};
}