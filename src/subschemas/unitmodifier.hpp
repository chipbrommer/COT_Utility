#pragma once
namespace cot::subschemas
{
	/// @brief A CoT Message subschema class for military symbol unit modifier data
	class unitmodifier {
	public:
		/// @brief Constructor - Initializes everything
		explicit unitmodifier(std::string code = INVALID_VALUE,
			std::string text = INVALID_VALUE)
			: code(std::move(code)), text(std::move(text)) {
		}

		/// @brief Equality comparison
		bool operator==(const unitmodifier& other) const noexcept {
			return code == other.code && text == other.text;
		}

		/// @brief Inequality comparison
		bool operator!=(const unitmodifier& other) const noexcept { return !(*this == other); }

		/// @brief Checks if the class has valid data
		/// @param rslt Optional result structure for invalid reason
		/// @return True if valid, false otherwise
		bool is_valid(result* rslt = nullptr) const {
			if (code.empty()) {
				auto res = result{ result::error_code::InvalidUnitModifier, "Empty code field" };
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
				auto res = result{ result::error_code::InvalidUnitModifier, "Invalid unitmodifier data" };
				if (error_handler) error_handler(res);
				return res;
			}
			auto node = parent.append_child("unitmodifier");
			node.append_attribute("code") = code.c_str();
			if (!text.empty()) node.text().set(text.c_str());
			return result{};
		}

		/// @brief Deserializes from an XML node
		/// @param node The XML node to parse
		/// @param rslt Optional result for error details
		/// @return Deserialized unitmodifier
		static unitmodifier from_xml(const pugi::xml_node& node, result* rslt = nullptr) {
			unitmodifier um;
			if (node.name() != std::string("unitmodifier")) {
				auto res = result{ result::error_code::InvalidUnitModifier, "Expected <unitmodifier> node" };
				if (rslt) *rslt = res;
				if (error_handler) error_handler(res);
				return um;
			}
			if (auto attr = node.attribute("code")) um.code = attr.as_string();
			if (auto txt = node.text()) um.text = txt.as_string();
			if (!um.is_valid(rslt) && rslt && rslt->is_success()) {
				*rslt = result{ result::error_code::InvalidUnitModifier, "Invalid unitmodifier data after parsing" };
				if (error_handler) error_handler(*rslt);
			}
			return um;
		}

		/// @brief Stream output for printing
		friend std::ostream& operator<<(std::ostream& os, const unitmodifier& um) {
			os << "unitmodifier: ";
			if (!um.is_valid()) os << " -NOT VALID- ";
			os << "\n\tcode: " << (um.code.empty() ? "None" : um.code) << "\n"
				<< "\ttext: " << (um.text.empty() ? "None" : um.text) << "\n";
			return os;
		}

		std::string code{ "" };
		std::string text{ "" };

	private:
		inline static constexpr const char* INVALID_VALUE = "";
	};
}