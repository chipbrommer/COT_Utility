#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            track.hpp
// @brief           A CoT Message subschema class for track data.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <string>

#include <pugixml.hpp>

#include "../../handlers/error_handler.hpp"
#include "../../result/result.hpp"

namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for track data
	class track {
	public:
		/// @brief Constructor - Initializes everything
		explicit track(double course = INVALID_VALUE,
			double speed = INVALID_VALUE,
			double slope = INVALID_VALUE,
			double eCourse = INVALID_VALUE,
			double eSpeed = INVALID_VALUE,
			double eSlope = INVALID_VALUE,
			double version = INVALID_VALUE) noexcept
			: course(course), speed(speed), slope(slope),
			eCourse(eCourse), eSpeed(eSpeed), eSlope(eSlope), version(version) {
		}

		/// @brief Equality comparison
		bool operator==(const track& other) const noexcept {
			constexpr double EPSILON = 1e-6;
			auto equal = [EPSILON](double a, double b) noexcept {
				return (std::isnan(a) && std::isnan(b)) ||
					(!std::isnan(a) && !std::isnan(b) && std::abs(a - b) < EPSILON);
				};
			return equal(course, other.course) &&
				equal(speed, other.speed) &&
				equal(slope, other.slope) &&
				equal(eCourse, other.eCourse) &&
				equal(eSpeed, other.eSpeed) &&
				equal(eSlope, other.eSlope) &&
				equal(version, other.version);
		}

		/// @brief Inequality comparison
		bool operator!=(const track& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (std::isnan(course)) {
				auto res = result{ result::error_code::InvalidTrack, "Course is NaN" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isfinite(course) || course < COURSE_MIN || course > COURSE_MAX) {
				auto res = result{ result::error_code::InvalidTrack, "Course out of range [0, 360]" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (std::isnan(speed)) {
				auto res = result{ result::error_code::InvalidTrack, "Speed is NaN" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isfinite(speed) || speed < SPEED_MIN) {
				auto res = result{ result::error_code::InvalidTrack, "Speed is invalid or negative" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(slope) && (!std::isfinite(slope) || slope < SLOPE_MIN || slope > SLOPE_MAX)) {
				auto res = result{ result::error_code::InvalidTrack, "Slope out of range [-90, 90]" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(eCourse) && (!std::isfinite(eCourse) || eCourse < 0.0)) {
				auto res = result{ result::error_code::InvalidTrack, "eCourse is invalid or negative" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(eSpeed) && (!std::isfinite(eSpeed) || eSpeed < 0.0)) {
				auto res = result{ result::error_code::InvalidTrack, "eSpeed is invalid or negative" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(eSlope) && (!std::isfinite(eSlope) || eSlope < 0.0)) {
				auto res = result{ result::error_code::InvalidTrack, "eSlope is invalid or negative" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isnan(version) && (!std::isfinite(version) || version <= 0.0)) {
				auto res = result{ result::error_code::InvalidTrack, "Version is invalid or non-positive" };
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
				auto res = result{ result::error_code::InvalidTrack, "Invalid track data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("track");
			node.append_attribute("course") = course;
			node.append_attribute("speed") = speed;
			if (!std::isnan(slope)) node.append_attribute("slope") = slope;
			if (!std::isnan(eCourse)) node.append_attribute("eCourse") = eCourse;
			if (!std::isnan(eSpeed)) node.append_attribute("eSpeed") = eSpeed;
			if (!std::isnan(eSlope)) node.append_attribute("eSlope") = eSlope;
			if (!std::isnan(version)) node.append_attribute("version") = version;
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized track
		static track from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			track t;
			if (node.name() != std::string("track")) {
				auto res = result{ result::error_code::InvalidTrack, "Expected <track> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return t;
			}
			t.course = node.attribute("course").as_double(INVALID_VALUE);
			t.speed = node.attribute("speed").as_double(INVALID_VALUE);
			t.slope = node.attribute("slope").as_double(INVALID_VALUE);
			t.eCourse = node.attribute("eCourse").as_double(INVALID_VALUE);
			t.eSpeed = node.attribute("eSpeed").as_double(INVALID_VALUE);
			t.eSlope = node.attribute("eSlope").as_double(INVALID_VALUE);
			t.version = node.attribute("version").as_double(INVALID_VALUE);
			if (!t.is_valid(rslt)) {
				return t;
			}
			return t;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const track& t) {
			os << "track: ";
			if (!t.is_valid()) os << " -NOT VALID- ";
			os << "\n" << std::fixed << std::setprecision(6)
				<< "\tcourse: " << t.course << "\n"
				<< "\tspeed: " << t.speed << "\n";
			if (!std::isnan(t.slope)) os << "\tslope: " << t.slope << "\n";
			if (!std::isnan(t.eCourse)) os << "\teCourse: " << t.eCourse << "\n";
			if (!std::isnan(t.eSpeed)) os << "\teSpeed: " << t.eSpeed << "\n";
			if (!std::isnan(t.eSlope)) os << "\teSlope: " << t.eSlope << "\n";
			if (!std::isnan(t.version)) os << "\tversion: " << t.version << "\n";
			os << std::defaultfloat;
			return os;
		}

		double course{ INVALID_VALUE };
		double speed{ INVALID_VALUE };
		double slope{ INVALID_VALUE };
		double eCourse{ INVALID_VALUE };
		double eSpeed{ INVALID_VALUE };
		double eSlope{ INVALID_VALUE };
		double version{ INVALID_VALUE };

	private:
		inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
		inline static constexpr double COURSE_MIN = 0.0;
		inline static constexpr double COURSE_MAX = 360.0;
		inline static constexpr double SPEED_MIN = 0.0;
		inline static constexpr double SLOPE_MIN = -90.0;
		inline static constexpr double SLOPE_MAX = 90.0;
	};
}