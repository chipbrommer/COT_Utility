#pragma once
/////////////////////////////////////////////////////////////////////////////////
// @file            cot_types.hpp
// @brief           CoT type and "how" taxonomy enums and string conversions.
// @author          Chip Brommer
/////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <string_view>
#include <unordered_map>

namespace cot
{
	namespace Root
	{
		enum class Type : int
		{
			a,
			b,
			t,
			r,
			c,
			res,
			Error
		};

		inline const std::unordered_map<Type, std::string> TypeToString = {
			{ Type::a, "Atoms" },
			{ Type::b, "Bits" },
			{ Type::t, "Tasking" },
			{ Type::r, "Reply" },
			{ Type::c, "Capability" },
			{ Type::res, "Reservation" },
			{ Type::Error, "Error" }
		};

		inline const std::unordered_map<std::string, Type> StringToType = {
			{ "Atoms", Type::a },
			{ "Bits", Type::b },
			{ "Tasking", Type::t },
			{ "Reply", Type::r },
			{ "Capability", Type::c },
			{ "Reservation", Type::res }
		};

		/// @brief Converts a CoT root token into a Root::Type enumeration value
		/// @param root Root token string to convert
		/// @return Root::Type enum conversion (Root::Type::Error if unrecognized)
		[[nodiscard]] inline Type CharToType(std::string_view root) noexcept
		{
			if (root == "a")   return Type::a;
			if (root == "b")   return Type::b;
			if (root == "t")   return Type::t;
			if (root == "r")   return Type::r;
			if (root == "c")   return Type::c;
			if (root == "res") return Type::res;
			return Type::Error;
		}
	} // namespace Root

	namespace Point
	{
		enum class Type : int
		{
			p,
			u,
			a,
			f,
			n,
			s,
			h,
			j,
			k,
			o,
			x,
			Error
		};

		inline const std::unordered_map<Type, std::string> TypeToString = {
			{ Type::p, "Pending" },
			{ Type::u, "Unknown" },
			{ Type::a, "Assumed Friend" },
			{ Type::f, "Friend" },
			{ Type::n, "Neutral" },
			{ Type::s, "Suspect" },
			{ Type::h, "Hostile" },
			{ Type::j, "Joker" },
			{ Type::k, "Faker" },
			{ Type::o, "None Specified" },
			{ Type::x, "Other" },
			{ Type::Error, "Error" }
		};

		inline const std::unordered_map<std::string, Type> StringToType = {
			{ "Pending", Type::p },
			{ "Unknown", Type::u },
			{ "Assumed Friend", Type::a },
			{ "Friend", Type::f },
			{ "Neutral", Type::n },
			{ "Suspect", Type::s },
			{ "Hostile", Type::h },
			{ "Joker", Type::j },
			{ "Faker", Type::k },
			{ "None Specified", Type::o },
			{ "Other", Type::x }
		};

		/// @brief Converts a CoT affiliation token into a Point::Type enumeration value
		/// @param type Affiliation token string to convert
		/// @return Point::Type enum conversion (Point::Type::Error if unrecognized)
		[[nodiscard]] inline Type CharToType(std::string_view type) noexcept
		{
			if (type == "p") return Type::p;
			if (type == "u") return Type::u;
			if (type == "a") return Type::a;
			if (type == "f") return Type::f;
			if (type == "n") return Type::n;
			if (type == "s") return Type::s;
			if (type == "h") return Type::h;
			if (type == "j") return Type::j;
			if (type == "k") return Type::k;
			if (type == "o") return Type::o;
			if (type == "x") return Type::x;
			return Type::Error;
		}
	} // namespace Point

	namespace Location
	{
		enum class Type : int
		{
			P,
			A,
			G,
			S,
			U,
			X,
			Error
		};

		inline const std::unordered_map<Type, std::string> TypeToString = {
			{ Type::P, "Space" },
			{ Type::A, "Air" },
			{ Type::G, "Ground" },
			{ Type::S, "Sea Surface" },
			{ Type::U, "Sea Subsurface" },
			{ Type::X, "Other" },
			{ Type::Error, "Error" }
		};

		inline const std::unordered_map<std::string, Type> StringToType = {
			{ "Space", Type::P },
			{ "Air", Type::A },
			{ "Ground", Type::G },
			{ "Sea Surface", Type::S },
			{ "Sea Subsurface", Type::U },
			{ "Other", Type::X }
		};

		/// @brief Converts a CoT dimension token into a Location::Type enumeration value
		/// @param loc Dimension token string to convert
		/// @return Location::Type enum conversion (Location::Type::Error if unrecognized)
		[[nodiscard]] inline Type CharToType(std::string_view loc) noexcept
		{
			if (loc == "P") return Type::P;
			if (loc == "A") return Type::A;
			if (loc == "G") return Type::G;
			if (loc == "S") return Type::S;
			if (loc == "U") return Type::U;
			if (loc == "X") return Type::X;
			return Type::Error;
		}
	} // namespace Location

	namespace How
	{
		/// @brief Type section of the 'how' data
		namespace Entry
		{
			enum class Type : int
			{
				h,
				m,
				Unknown,
				Error
			};

			inline const std::unordered_map<Type, std::string> TypeToString = {
				{ Type::h, "Human" },
				{ Type::m, "Machine" },
				{ Type::Unknown, "Unknown" },
				{ Type::Error, "Error" }
			};

			inline const std::unordered_map<std::string, Type> StringToType = {
				{ "Human", Type::h },
				{ "Machine", Type::m },
				{ "Unknown", Type::Unknown },
				{ "Error", Type::Error }
			};

			/// @brief Converts a 'how' entry token into an Entry::Type enumeration value
			/// @param entry Entry token string to convert
			/// @return Entry::Type enum conversion
			[[nodiscard]] inline Type CharToType(std::string_view entry) noexcept
			{
				if (entry == "h") return Type::h;
				if (entry == "m") return Type::m;
				return Type::Error;
			}
		} // namespace Entry

		/// @brief Data section of the 'how' data
		namespace Data
		{
			enum class Type : int
			{
				e,
				cal,
				t,
				paste,
				i,
				g,
				m,
				s,
				f,
				con,
				pred,
				r,
				Unknown,
				Error
			};

			inline const std::unordered_map<Type, std::string> TypeToString = {
				{ Type::e, "Estimated" },
				{ Type::cal, "Calculated" },
				{ Type::t, "Transcribed" },
				{ Type::paste, "Cut and Paste" },
				{ Type::i, "Mensurated" },
				{ Type::g, "Derived From GPS" },
				{ Type::m, "Magnetic" },
				{ Type::s, "Simulated" },
				{ Type::f, "Fused" },
				{ Type::con, "Configured" },
				{ Type::pred, "Predicted" },
				{ Type::r, "Relayed" },
				{ Type::Unknown, "Unknown" },
				{ Type::Error, "Error" }
			};

			inline const std::unordered_map<std::string, Type> StringToType = {
				{ "Estimated", Type::e },
				{ "Calculated", Type::cal },
				{ "Transcribed", Type::t },
				{ "Cut and Paste", Type::paste },
				{ "Mensurated", Type::i },
				{ "Derived From GPS", Type::g },
				{ "Magnetic", Type::m },
				{ "Simulated", Type::s },
				{ "Fused", Type::f },
				{ "Configured", Type::con },
				{ "Predicted", Type::pred },
				{ "Relayed", Type::r },
				{ "Unknown", Type::Unknown },
				{ "Error", Type::Error }
			};

			/// @brief Converts a 'how' data token into a Data::Type enumeration value
			/// @param data Data token string to convert
			/// @param entry Corresponding How::Entry::Type context
			/// @return Data::Type enum conversion
			[[nodiscard]] inline Type CharToType(std::string_view data, Entry::Type entry) noexcept
			{
				if (entry == Entry::Type::h) {
					if (data == "e") return Type::e;
					if (data == "c") return Type::cal;
					if (data == "t") return Type::t;
					if (data == "p") return Type::paste;
					return Type::Error;
				}
				if (entry == Entry::Type::m) {
					if (data == "i") return Type::i;
					if (data == "g") return Type::g;
					if (data == "m") return Type::m;
					if (data == "s") return Type::s;
					if (data == "f") return Type::f;
					if (data == "c") return Type::con;
					if (data == "p") return Type::pred;
					if (data == "r") return Type::r;
					return Type::Error;
				}
				return Type::Error;
			}
		} // namespace Data
	} // namespace How
} // namespace cot