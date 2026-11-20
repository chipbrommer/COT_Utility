#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            version.hpp
// @brief           Version constants for cot_utility.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <string_view>

namespace cot
{
	/// @brief Major version number
	inline constexpr int COT_UTILITY_MAJOR = 0;

	/// @brief Minor version number
	inline constexpr int COT_UTILITY_MINOR = 5;

	/// @brief Build / patch version number
	inline constexpr int COT_UTILITY_BUILD = 0;

	/// @brief Semantic version string (MAJOR.MINOR.BUILD)
	inline constexpr std::string_view COT_UTILITY_VERSION_STR = "0.5.0";
}