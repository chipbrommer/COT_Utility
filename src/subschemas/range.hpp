#pragma once
namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for range data
	class range {
	public:
		/// @brief Constructor - Initializes everything
		explicit range(double value = INVALID_VALUE) noexcept : value(value) {}

		/// @brief Equality comparison
		bool operator==(const range& other) const noexcept {
			constexpr double EPSILON = 1e-6;
			return (std::isnan(value) && std::isnan(other.value)) ||
				std::abs(value - other.value) < EPSILON;
		}

		/// @brief Inequality comparison
		bool operator!=(const range& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (std::isnan(value)) {
				auto res = result{ result::error_code::InvalidRange, "range value is unset (NaN)" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return false;
			}
			if (!std::isfinite(value)) {
				auto res = result{ result::error_code::InvalidRange, "range value is non-finite" };
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
				auto res = result{ result::error_code::InvalidRange, "Invalid range data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("range");
			node.append_attribute("value") = value;
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized range
		static range from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			range r;
			if (node.name() != std::string("range")) {
				auto res = result{ result::error_code::InvalidRange, "Expected <range> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return r;
			}
			if (auto attr = node.attribute("value")) {
				try {
					r.value = std::stod(attr.as_string());
				}
				catch (const std::exception& e) {
					auto res = result{ result::error_code::InvalidRange,
									  "Error parsing range value: " + std::string(e.what()) };
					if (rslt) *rslt = res;
					if (error_handler) error_handler(res);
					r.value = INVALID_VALUE;
				}
			}
			if (!r.is_valid(rslt) && rslt && rslt->is_success()) {
				*rslt = result{ result::error_code::InvalidRange, "Invalid range data after parsing" };
				if (error_handler) error_handler(*rslt);
			}
			return r;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const range& r) {
			os << "range: ";
			if (!r.is_valid()) os << " -NOT VALID- (Unset)";
			else os << "\n\tvalue: " << r.value;
			os << "\n";
			return os;
		}

		double value{ INVALID_VALUE };

	private:
		inline static constexpr double INVALID_VALUE = std::numeric_limits<double>::quiet_NaN();
	};
}