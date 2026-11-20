#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            sensor.hpp
// @brief           A CoT Message subschema class for sensor data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdint>
#include <exception>
#include <limits>
#include <optional>
#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for sensor data
	class sensor {
	public:
		/// @brief Constructor - Initializes everything
		explicit sensor(
			double azimuth = INVALID_VALUE,
			double elevation = INVALID_VALUE,
			double roll = INVALID_VALUE,
			double fov = INVALID_VALUE,
			double vfov = INVALID_VALUE,
			double north = INVALID_VALUE,
			double range = INVALID_VALUE,
			std::optional<int> displayMagneticReference = std::nullopt,
			std::optional<bool> hideFov = std::nullopt,
			std::optional<int> rangeLines = std::nullopt,
			std::optional<std::int64_t> rangeLineStrokeColor = std::nullopt,
			double rangeLineStrokeWeight = INVALID_VALUE,
			std::optional<std::int64_t> strokeColor = std::nullopt,
			double strokeWeight = INVALID_VALUE,
			std::optional<int> fovAlpha = std::nullopt,
			std::optional<int> fovRed = std::nullopt,
			std::optional<int> fovGreen = std::nullopt,
			std::optional<int> fovBlue = std::nullopt,
			std::string model = {},
			std::string type = {},
			std::string version = {}) noexcept
			: azimuth(azimuth)
			, elevation(elevation)
			, roll(roll)
			, fov(fov)
			, vfov(vfov)
			, north(north)
			, range(range)
			, displayMagneticReference(displayMagneticReference)
			, hideFov(hideFov)
			, rangeLines(rangeLines)
			, rangeLineStrokeColor(rangeLineStrokeColor)
			, rangeLineStrokeWeight(rangeLineStrokeWeight)
			, strokeColor(strokeColor)
			, strokeWeight(strokeWeight)
			, fovAlpha(fovAlpha)
			, fovRed(fovRed)
			, fovGreen(fovGreen)
			, fovBlue(fovBlue)
			, model(std::move(model))
			, type(std::move(type))
			, version(std::move(version)) {
		}

		/// @brief Equality comparison
		bool operator==(const sensor& other) const noexcept {
			constexpr double EPSILON = 1e-6;
			auto dbl_eq = [EPSILON](double a, double b) noexcept {
				return (std::isnan(a) && std::isnan(b)) ||
					(!std::isnan(a) && !std::isnan(b) && std::abs(a - b) < EPSILON);
				};

			return dbl_eq(azimuth, other.azimuth) &&
				dbl_eq(elevation, other.elevation) &&
				dbl_eq(roll, other.roll) &&
				dbl_eq(fov, other.fov) &&
				dbl_eq(vfov, other.vfov) &&
				dbl_eq(north, other.north) &&
				dbl_eq(range, other.range) &&
				dbl_eq(rangeLineStrokeWeight, other.rangeLineStrokeWeight) &&
				dbl_eq(strokeWeight, other.strokeWeight) &&
				displayMagneticReference == other.displayMagneticReference &&
				hideFov == other.hideFov &&
				rangeLines == other.rangeLines &&
				rangeLineStrokeColor == other.rangeLineStrokeColor &&
				strokeColor == other.strokeColor &&
				fovAlpha == other.fovAlpha &&
				fovRed == other.fovRed &&
				fovGreen == other.fovGreen &&
				fovBlue == other.fovBlue &&
				model == other.model &&
				type == other.type &&
				version == other.version;
		}

		/// @brief Inequality comparison
		bool operator!=(const sensor& other) const noexcept { return !(*this == other); }

		/// @brief Checks whether any sensor field has been populated
		/// @return True if any data is set, false otherwise
		bool has_data() const noexcept {
			return !std::isnan(azimuth) ||
				!std::isnan(elevation) ||
				!std::isnan(roll) ||
				!std::isnan(fov) ||
				!std::isnan(vfov) ||
				!std::isnan(north) ||
				!std::isnan(range) ||
				!std::isnan(rangeLineStrokeWeight) ||
				!std::isnan(strokeWeight) ||
				displayMagneticReference.has_value() ||
				hideFov.has_value() ||
				rangeLines.has_value() ||
				rangeLineStrokeColor.has_value() ||
				strokeColor.has_value() ||
				fovAlpha.has_value() ||
				fovRed.has_value() ||
				fovGreen.has_value() ||
				fovBlue.has_value() ||
				!model.empty() ||
				!type.empty() ||
				!version.empty();
		}

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			auto fail = [&](const std::string& msg) -> bool {
				auto res = result{ result::error_code::InvalidSensor, msg };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
				};

			auto check_finite_if_set = [&](double val, const char* name) -> bool {
				if (!std::isnan(val) && !std::isfinite(val)) {
					return fail(std::string("sensor ") + name + " is non-finite");
				}
				return true;
				};

			if (!check_finite_if_set(azimuth, "azimuth")) return false;
			if (!check_finite_if_set(elevation, "elevation")) return false;
			if (!check_finite_if_set(roll, "roll")) return false;
			if (!check_finite_if_set(fov, "fov")) return false;
			if (!check_finite_if_set(vfov, "vfov")) return false;
			if (!check_finite_if_set(north, "north")) return false;
			if (!check_finite_if_set(range, "range")) return false;
			if (!check_finite_if_set(rangeLineStrokeWeight, "rangeLineStrokeWeight")) return false;
			if (!check_finite_if_set(strokeWeight, "strokeWeight")) return false;

			if (!std::isnan(range) && range < 0.0) {
				return fail("sensor range is negative");
			}
			if (!std::isnan(rangeLineStrokeWeight) && rangeLineStrokeWeight < 0.0) {
				return fail("sensor rangeLineStrokeWeight is negative");
			}
			if (!std::isnan(strokeWeight) && strokeWeight < 0.0) {
				return fail("sensor strokeWeight is negative");
			}

			return true;
		}

		/// @brief Serializes to XML
		/// @param parent The parent XML node to append to
		/// @return Result indicating success or failure
		result to_xml(pugi::xml_node& parent) const {
			if (!is_valid()) {
				auto res = result{ result::error_code::InvalidSensor, "Invalid sensor data" };
				if (error_handler) error_handler(res);
				return res;
			}
			if (!has_data()) {
				return result{};
			}

			auto node = parent.append_child("sensor");
			auto set_double = [&](const char* attr_name, double val) {
				if (!std::isnan(val)) node.append_attribute(attr_name) = val;
				};

			set_double("azimuth", azimuth);
			set_double("elevation", elevation);
			set_double("roll", roll);
			set_double("fov", fov);
			set_double("vfov", vfov);
			set_double("north", north);
			set_double("range", range);
			set_double("rangeLineStrokeWeight", rangeLineStrokeWeight);
			set_double("strokeWeight", strokeWeight);

			if (displayMagneticReference) node.append_attribute("displayMagneticReference") = *displayMagneticReference;
			if (hideFov) node.append_attribute("hideFov") = *hideFov ? "true" : "false";
			if (rangeLines) node.append_attribute("rangeLines") = *rangeLines;
			if (rangeLineStrokeColor) node.append_attribute("rangeLineStrokeColor") = static_cast<long long>(*rangeLineStrokeColor);
			if (strokeColor) node.append_attribute("strokeColor") = static_cast<long long>(*strokeColor);
			if (fovAlpha) node.append_attribute("fovAlpha") = *fovAlpha;
			if (fovRed) node.append_attribute("fovRed") = *fovRed;
			if (fovGreen) node.append_attribute("fovGreen") = *fovGreen;
			if (fovBlue) node.append_attribute("fovBlue") = *fovBlue;
			if (!model.empty()) node.append_attribute("model") = model.c_str();
			if (!type.empty()) node.append_attribute("type") = type.c_str();
			if (!version.empty()) node.append_attribute("version") = version.c_str();

			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized sensor
		static sensor from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			sensor s;
			if (node.name() != std::string("sensor")) {
				auto res = result{ result::error_code::InvalidSensor, "Expected <sensor> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return s;
			}

			auto parse_double = [&](const char* attr_name, double& target) {
				if (auto attr = node.attribute(attr_name)) {
					try {
						target = std::stod(attr.as_string());
					}
					catch (const std::exception& e) {
						auto res = result{ result::error_code::InvalidSensor,
							std::string("Error parsing sensor ") + attr_name + ": " + e.what() };
						if (rslt) *rslt = res;
						if (error_handler) error_handler(res);
						target = INVALID_VALUE;
					}
				}
				};

			auto parse_int = [&](const char* attr_name, std::optional<int>& target) {
				if (auto attr = node.attribute(attr_name)) {
					try {
						target = std::stoi(attr.as_string());
					}
					catch (const std::exception& e) {
						auto res = result{ result::error_code::InvalidSensor,
							std::string("Error parsing sensor ") + attr_name + ": " + e.what() };
						if (rslt) *rslt = res;
						if (error_handler) error_handler(res);
						target = std::nullopt;
					}
				}
				};

			auto parse_i64 = [&](const char* attr_name, std::optional<std::int64_t>& target) {
				if (auto attr = node.attribute(attr_name)) {
					try {
						target = static_cast<std::int64_t>(std::stoll(attr.as_string()));
					}
					catch (const std::exception& e) {
						auto res = result{ result::error_code::InvalidSensor,
							std::string("Error parsing sensor ") + attr_name + ": " + e.what() };
						if (rslt) *rslt = res;
						if (error_handler) error_handler(res);
						target = std::nullopt;
					}
				}
				};

			parse_double("azimuth", s.azimuth);
			parse_double("elevation", s.elevation);
			parse_double("roll", s.roll);
			parse_double("fov", s.fov);
			parse_double("vfov", s.vfov);
			parse_double("north", s.north);
			parse_double("range", s.range);
			parse_double("rangeLineStrokeWeight", s.rangeLineStrokeWeight);
			parse_double("strokeWeight", s.strokeWeight);

			parse_int("displayMagneticReference", s.displayMagneticReference);
			parse_int("rangeLines", s.rangeLines);
			parse_int("fovAlpha", s.fovAlpha);
			parse_int("fovRed", s.fovRed);
			parse_int("fovGreen", s.fovGreen);
			parse_int("fovBlue", s.fovBlue);

			parse_i64("rangeLineStrokeColor", s.rangeLineStrokeColor);
			parse_i64("strokeColor", s.strokeColor);

			if (auto attr = node.attribute("hideFov")) {
				s.hideFov = attr.as_bool();
			}
			if (auto attr = node.attribute("model")) {
				s.model = attr.as_string();
			}
			if (auto attr = node.attribute("type")) {
				s.type = attr.as_string();
			}
			if (auto attr = node.attribute("version")) {
				s.version = attr.as_string();
			}

			if (!s.is_valid(rslt)) {
				auto res = result{ result::error_code::InvalidSensor,
					"Invalid sensor data after parsing" };
				if (rslt && rslt->is_success()) *rslt = res;
				if (error_handler) error_handler(res);
			}
			return s;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const sensor& s) {
			os << "sensor: ";
			if (!s.is_valid()) os << " -NOT VALID-";
			if (!s.has_data()) {
				os << " (Unset)\n";
				return os;
			}
			if (!std::isnan(s.azimuth)) os << "\n\tazimuth: " << s.azimuth;
			if (!std::isnan(s.elevation)) os << "\n\televation: " << s.elevation;
			if (!std::isnan(s.roll)) os << "\n\troll: " << s.roll;
			if (!std::isnan(s.fov)) os << "\n\tfov: " << s.fov;
			if (!std::isnan(s.vfov)) os << "\n\tvfov: " << s.vfov;
			if (!std::isnan(s.north)) os << "\n\tnorth: " << s.north;
			if (!std::isnan(s.range)) os << "\n\trange: " << s.range;
			if (!std::isnan(s.rangeLineStrokeWeight)) os << "\n\trangeLineStrokeWeight: " << s.rangeLineStrokeWeight;
			if (!std::isnan(s.strokeWeight)) os << "\n\tstrokeWeight: " << s.strokeWeight;
			if (s.displayMagneticReference) os << "\n\tdisplayMagneticReference: " << *s.displayMagneticReference;
			if (s.hideFov) os << "\n\thideFov: " << (*s.hideFov ? "true" : "false");
			if (s.rangeLines) os << "\n\trangeLines: " << *s.rangeLines;
			if (s.rangeLineStrokeColor) os << "\n\trangeLineStrokeColor: " << *s.rangeLineStrokeColor;
			if (s.strokeColor) os << "\n\tstrokeColor: " << *s.strokeColor;
			if (s.fovAlpha) os << "\n\tfovAlpha: " << *s.fovAlpha;
			if (s.fovRed) os << "\n\tfovRed: " << *s.fovRed;
			if (s.fovGreen) os << "\n\tfovGreen: " << *s.fovGreen;
			if (s.fovBlue) os << "\n\tfovBlue: " << *s.fovBlue;
			if (!s.model.empty()) os << "\n\tmodel: " << s.model;
			if (!s.type.empty()) os << "\n\ttype: " << s.type;
			if (!s.version.empty()) os << "\n\tversion: " << s.version;
			os << "\n";
			return os;
		}

		double azimuth{ INVALID_VALUE };                        ///< Sensor azimuth angle in degrees
		double elevation{ INVALID_VALUE };                      ///< Sensor elevation angle in degrees
		double roll{ INVALID_VALUE };                           ///< Sensor roll angle in degrees
		double fov{ INVALID_VALUE };                            ///< Horizontal field of view in degrees
		double vfov{ INVALID_VALUE };                           ///< Vertical field of view in degrees
		double north{ INVALID_VALUE };                          ///< North reference angle in degrees
		double range{ INVALID_VALUE };                          ///< Maximum sensor range in meters
		std::optional<int> displayMagneticReference;            ///< Magnetic reference display mode
		std::optional<bool> hideFov;                            ///< Flag indicating whether FOV is hidden
		std::optional<int> rangeLines;                          ///< Number of range lines/rings to draw
		std::optional<std::int64_t> rangeLineStrokeColor;       ///< Stroke color for range lines
		double rangeLineStrokeWeight{ INVALID_VALUE };          ///< Stroke weight/width for range lines
		std::optional<std::int64_t> strokeColor;                ///< Primary stroke color for sensor/FOV
		double strokeWeight{ INVALID_VALUE };                   ///< Primary stroke weight/width for sensor/FOV
		std::optional<int> fovAlpha;                            ///< Alpha channel for FOV fill
		std::optional<int> fovRed;                              ///< Red channel for FOV fill
		std::optional<int> fovGreen;                            ///< Green channel for FOV fill
		std::optional<int> fovBlue;                             ///< Blue channel for FOV fill
		std::string model;                                      ///< Sensor model identifier
		std::string type;                                       ///< Sensor type identifier
		std::string version;                                    ///< Sensor schema or firmware version

	private:
		inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN(); ///< Sentinel value for unset doubles
	};
}