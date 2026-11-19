#pragma once
namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for uid data
	class uid {
	public:
		/// @brief Constructor - Initializes everything
		explicit uid(std::string droid = INVALID_VALUE) : droid(std::move(droid)) {}

		/// @brief Equality comparison
		bool operator==(const uid& other) const noexcept { return droid == other.droid; }

		/// @brief Inequality comparison
		bool operator!=(const uid& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (droid.empty()) {
				auto res = result{ result::error_code::InvalidUid, "Empty droid field" };
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
				auto res = result{ result::error_code::InvalidUid, "Invalid uid data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("uid");
			node.append_attribute("droid") = droid.c_str();
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized uid
		static uid from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			uid u;
			if (node.name() != std::string("uid")) {
				auto res = result{ result::error_code::InvalidUid, "Expected <uid> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return u;
			}
			if (auto attr = node.attribute("droid")) {
				u.droid = attr.as_string();
			}
			if (!u.is_valid(rslt) && rslt && rslt->is_success()) {
				*rslt = result{ result::error_code::InvalidUid, "Invalid uid data after parsing" };
				if (error_handler) error_handler(*rslt);
			}
			return u;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const uid& u) {
			os << "uid: ";
			if (!u.is_valid()) os << " -NOT VALID- ";
			os << "\n\tdroid: " << (u.droid.empty() ? "None" : u.droid) << "\n";
			return os;
		}

		std::string droid{ "" };

	private:
		inline static constexpr const char* INVALID_VALUE = "";
	};
}