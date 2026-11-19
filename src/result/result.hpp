#pragma once
namespace cot
{
	/// @brief Result structure for CoT operations
	struct result {
		/// @brief Error code indicating the operation outcome
		enum class error_code : int {
			Success,                ///< Operation completed successfully
			InvalidEvent,           ///< XML is missing or has invalid <event> tag
			InvalidPoint,           ///< XML is missing or has invalid <point> tag
			InvalidDetail,          ///< XML has invalid <detail> tag
			InvalidColor,           ///< Invalid color data (strokecolor, fillcolor, color)
			InvalidFillColor,       ///< Invalid fillColor data
			InvalidImage,           ///< Invalid image data
			InvalidInclination,     ///< Invalid inclination data
			InvalidLabelsOn,        ///< Invalid labels_on data
			InvalidStrokeColor,     ///< Invalid strokeColor data
			InvalidStrokeStyle,     ///< Invalid strokeStyle data
			InvalidStrokeWeight,    ///< Invalid strokeWeight data
			InvalidUserIcon,        ///< Invalid usericon data
			InvalidModel,           ///< Invalid model data
			InvalidTrack,           ///< Invalid track data
			InvalidLink,            ///< Invalid link data
			InvalidLinkAttr,        ///< Invalid link_attr data
			InvalidRemarks,         ///< Invalid remarks data
			InvalidUnitModifier,    ///< Invalid unitmodifier data
			InvalidMilIcon,         ///< Invalid milicon data
			InvalidMilSym,          ///< Invalid milsym data
			InvalidCreator,         ///< Invalid creator data
			InvalidDisplay,         ///< Invalid display data
			InvalidEmergency,       ///< Invalid emergency data
			InvalidAddress,         ///< Invalid address data
			InvalidCustomDetail,    ///< Invalid custom detail data
			InvalidTakv,            ///< Invalid takv data
			InvalidContact,         ///< Invalid contact data
			InvalidUid,             ///< Invalid uid data
			InvalidTog,             ///< Invalid tog data
			InvalidRange,           ///< Invalid range data
			InvalidRangeUnits,      ///< Invalid rangeunits data
			InvalidPrecisionLocation, ///< Invalid precisionlocation data
			InvalidGroup,           ///< Invalid group data
			InvalidStatus,          ///< Invalid status data
			InvalidDate,            ///< Invalid date; must have Year, Month, Day
			InvalidTime,            ///< Invalid time; must have Hour, Minute, Seconds
			InvalidHow,             ///< Invalid how attribute
			InvalidType,            ///< Invalid type attribute
			InvalidXml,             ///< String is not valid XML
			InvalidInput,           ///< Input is empty or malformed
			InsufficientData,       ///< Not enough data provided
			ProcessingError,        ///< Generic processing failure
			NoModificationMade,     ///< No modification was made
			StructureDataInvalid,   ///< Structure has invalid data (restored)
			InvalidData             ///< Invalid data (restored)
		};

		/// @brief Constructs a successful result
		result() noexcept : m_code(error_code::Success) {}

		/// @brief Constructs a result with an error code
		result(error_code code) noexcept : m_code(code) {}

		/// @brief Constructs a result with an error code and description
		result(error_code code, std::string desc) noexcept
			: m_code(code), m_description(std::move(desc)) {
		}

		/// @brief Copy constructor
		result(const result& other) noexcept = default;

		/// @brief Copy assignment operator
		result& operator=(const result& other) noexcept = default;

		/// @brief Move constructor
		result(result&& other) noexcept = default;

		/// @brief Move assignment operator
		result& operator=(result&& other) noexcept = default;

		/// @brief Checks if the operation was successful
		[[nodiscard]] bool is_success() const noexcept { return m_code == error_code::Success; }

		/// @brief Checks if the operation failed
		[[nodiscard]] bool is_failed() const noexcept { return !is_success(); }

		/// @brief Converts to string for debugging or logging
		[[nodiscard]] std::string to_string() const {
			std::string result = "Result: ";
			auto code_str = to_string(m_code);
			result += code_str;
			if (!m_description.empty() && m_description != code_str) {
				result += "; Description: " + m_description;
			}
			return result;
		}

		/// @brief Implicit conversion to string
		operator std::string() const { return to_string(); }

		/// @brief Gets the error description
		[[nodiscard]] const std::string& description() const noexcept { return m_description; }

	private:
		/// @brief Converts error code to string
		[[nodiscard]] static constexpr std::string_view to_string(error_code code) noexcept {
			switch (code) {
			case error_code::Success: return "Success";
			case error_code::InvalidEvent: return "XML has invalid Event tag";
			case error_code::InvalidPoint: return "XML has invalid Point tag";
			case error_code::InvalidDetail: return "XML has invalid Detail tag";
			case error_code::InvalidColor: return "Invalid color data";
			case error_code::InvalidFillColor: return "Invalid fillcolor data";
			case error_code::InvalidImage: return "Invalid image data";
			case error_code::InvalidInclination: return "Invalid inclination data";
			case error_code::InvalidLabelsOn: return "Invalid labels_on data";
			case error_code::InvalidStrokeColor: return "Invalid strokecolor data";
			case error_code::InvalidStrokeStyle: return "Invalid strokestyle data";
			case error_code::InvalidStrokeWeight: return "Invalid strokeweight data";
			case error_code::InvalidUserIcon: return "Invalid usericon data";
			case error_code::InvalidModel: return "Invalid model data";
			case error_code::InvalidTrack: return "Invalid track data";
			case error_code::InvalidLink: return "Invalid link data";
			case error_code::InvalidLinkAttr: return "Invalid link_attr data";
			case error_code::InvalidRemarks: return "Invalid remarks data";
			case error_code::InvalidUnitModifier: return "Invalid unitmodifier data";
			case error_code::InvalidMilIcon: return "Invalid milicon data";
			case error_code::InvalidMilSym: return "Invalid milsym data";
			case error_code::InvalidCreator: return "Invalid creator data";
			case error_code::InvalidDisplay: return "Invalid display data";
			case error_code::InvalidEmergency: return "Invalid emergency data";
			case error_code::InvalidAddress: return "Invalid address data";
			case error_code::InvalidCustomDetail: return "Invalid custom detail data";
			case error_code::InvalidTakv: return "Invalid takv data";
			case error_code::InvalidContact: return "Invalid contact data";
			case error_code::InvalidUid: return "Invalid uid data";
			case error_code::InvalidTog: return "Invalid tog data";
			case error_code::InvalidRange: return "Invalid range data";
			case error_code::InvalidRangeUnits: return "Invalid rangeunits data";
			case error_code::InvalidPrecisionLocation: return "Invalid precisionlocation data";
			case error_code::InvalidGroup: return "Invalid group data";
			case error_code::InvalidStatus: return "Invalid status data";
			case error_code::InvalidDate: return "Invalid date; must have Year, Month, Day";
			case error_code::InvalidTime: return "Invalid time; must have Hour, Minute, Seconds";
			case error_code::InvalidHow: return "Invalid how attribute";
			case error_code::InvalidType: return "Invalid type attribute";
			case error_code::InvalidXml: return "Invalid XML input";
			case error_code::InvalidInput: return "Invalid or empty input";
			case error_code::InsufficientData: return "Insufficient data";
			case error_code::ProcessingError: return "Processing error";
			case error_code::NoModificationMade: return "No modification made";
			case error_code::StructureDataInvalid: return "Structure has invalid data";
			case error_code::InvalidData: return "Invalid data";
			default: return "Unknown error";
			}
		}

		error_code m_code;          ///< Specific error code
		std::string m_description;  ///< Detailed error message
	};
}