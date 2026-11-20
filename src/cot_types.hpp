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
	/// @brief CoT root message category ("a" = Atoms, "b" = Bits, etc.)
	namespace Root
	{
		/// @brief Top-level CoT event type root token
		enum class Type : int
		{
			a,     ///< Atoms ("Atoms") — physical entities, tracks, and locations
			b,     ///< Bits ("Bits") — data, chat, files, and capabilities
			t,     ///< Tasking ("Tasking") — operational commands and tasking
			r,     ///< Reply ("Reply") — responses and acknowledgments
			c,     ///< Capability ("Capability") — system or sensor capabilities
			res,   ///< Reservation ("Reservation") — resource reservations
			Error  ///< Unrecognized or invalid root token
		};

		/// @brief Maps Root::Type enum values to human-readable category names
		inline const std::unordered_map<Type, std::string> TypeToString = {
			{ Type::a, "Atoms" },
			{ Type::b, "Bits" },
			{ Type::t, "Tasking" },
			{ Type::r, "Reply" },
			{ Type::c, "Capability" },
			{ Type::res, "Reservation" },
			{ Type::Error, "Error" }
		};

		/// @brief Maps human-readable root category names to Root::Type enum values
		inline const std::unordered_map<std::string, Type> StringToType = {
			{ "Atoms", Type::a },
			{ "Bits", Type::b },
			{ "Tasking", Type::t },
			{ "Reply", Type::r },
			{ "Capability", Type::c },
			{ "Reservation", Type::res }
		};

		/// @brief Converts a CoT root token into a Root::Type enumeration value
		/// @param root Root token string to convert (e.g., "a", "b", "t")
		/// @return Corresponding Root::Type value, or Root::Type::Error if unrecognized
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

	/// @brief CoT affiliation / point-type taxonomy ("p", "u", "f", "h", etc.)
	namespace Point
	{
		/// @brief CoT affiliation or point identity token
		enum class Type : int
		{
			p,     ///< Pending ("Pending")
			u,     ///< Unknown ("Unknown")
			a,     ///< Assumed Friend ("Assumed Friend")
			f,     ///< Friend ("Friend")
			n,     ///< Neutral ("Neutral")
			s,     ///< Suspect ("Suspect")
			h,     ///< Hostile ("Hostile")
			j,     ///< Joker ("Joker")
			k,     ///< Faker ("Faker")
			o,     ///< None Specified ("None Specified")
			x,     ///< Other ("Other")
			Error  ///< Unrecognized or invalid affiliation token
		};

		/// @brief Maps Point::Type enum values to human-readable affiliation names
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

		/// @brief Maps human-readable affiliation names to Point::Type enum values
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
		/// @param type Affiliation token string to convert (e.g., "f", "h", "u")
		/// @return Corresponding Point::Type value, or Point::Type::Error if unrecognized
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

	/// @brief CoT spatial dimension / operational domain ("P", "A", "G", "S", "U", "X")
	namespace Location
	{
		/// @brief CoT spatial domain or dimension token
		enum class Type : int
		{
			P,     ///< Space ("Space")
			A,     ///< Air ("Air")
			G,     ///< Ground ("Ground")
			S,     ///< Sea Surface ("Sea Surface")
			U,     ///< Sea Subsurface ("Sea Subsurface")
			X,     ///< Other ("Other")
			Error  ///< Unrecognized or invalid dimension token
		};

		/// @brief Maps Location::Type enum values to human-readable domain names
		inline const std::unordered_map<Type, std::string> TypeToString = {
			{ Type::P, "Space" },
			{ Type::A, "Air" },
			{ Type::G, "Ground" },
			{ Type::S, "Sea Surface" },
			{ Type::U, "Sea Subsurface" },
			{ Type::X, "Other" },
			{ Type::Error, "Error" }
		};

		/// @brief Maps human-readable domain names to Location::Type enum values
		inline const std::unordered_map<std::string, Type> StringToType = {
			{ "Space", Type::P },
			{ "Air", Type::A },
			{ "Ground", Type::G },
			{ "Sea Surface", Type::S },
			{ "Sea Subsurface", Type::U },
			{ "Other", Type::X }
		};

		/// @brief Converts a CoT dimension token into a Location::Type enumeration value
		/// @param loc Dimension token string to convert (e.g., "A", "G", "S")
		/// @return Corresponding Location::Type value, or Location::Type::Error if unrecognized
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

	/// @brief CoT "how" provenance taxonomy (how the data was generated or entered)
	namespace How
	{
		/// @brief Origin/entry portion of the CoT "how" attribute ("h" = Human, "m" = Machine)
		namespace Entry
		{
			/// @brief Source mechanism for how a CoT event was originated
			enum class Type : int
			{
				h,        ///< Human ("Human") — manually entered or observed by a person
				m,        ///< Machine ("Machine") — automatically generated by sensor or system
				Unknown,  ///< Unknown ("Unknown") — origin mechanism not specified
				Error     ///< Error ("Error") — unrecognized or invalid entry token
			};

			/// @brief Maps How::Entry::Type enum values to human-readable descriptions
			inline const std::unordered_map<Type, std::string> TypeToString = {
				{ Type::h, "Human" },
				{ Type::m, "Machine" },
				{ Type::Unknown, "Unknown" },
				{ Type::Error, "Error" }
			};

			/// @brief Maps human-readable entry descriptions to How::Entry::Type enum values
			inline const std::unordered_map<std::string, Type> StringToType = {
				{ "Human", Type::h },
				{ "Machine", Type::m },
				{ "Unknown", Type::Unknown },
				{ "Error", Type::Error }
			};

			/// @brief Converts a 'how' entry token into an Entry::Type enumeration value
			/// @param entry Entry token string to convert ("h" or "m")
			/// @return Corresponding Entry::Type value, or Entry::Type::Error if unrecognized
			[[nodiscard]] inline Type CharToType(std::string_view entry) noexcept
			{
				if (entry == "h") return Type::h;
				if (entry == "m") return Type::m;
				return Type::Error;
			}
		} // namespace Entry

		/// @brief Method/source detail portion of the CoT "how" attribute
		namespace Data
		{
			/// @brief Specific method or sensor source for CoT position/data generation
			enum class Type : int
			{
				e,        ///< Estimated ("Estimated")
				cal,      ///< Calculated ("Calculated")
				t,        ///< Transcribed ("Transcribed")
				paste,    ///< Cut and Paste ("Cut and Paste")
				i,        ///< Mensurated ("Mensurated")
				g,        ///< Derived From GPS ("Derived From GPS")
				m,        ///< Magnetic ("Magnetic")
				s,        ///< Simulated ("Simulated")
				f,        ///< Fused ("Fused")
				con,      ///< Configured ("Configured")
				pred,     ///< Predicted ("Predicted")
				r,        ///< Relayed ("Relayed")
				Unknown,  ///< Unknown ("Unknown")
				Error     ///< Error ("Error") — unrecognized or invalid data token
			};

			/// @brief Maps How::Data::Type enum values to human-readable method names
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

			/// @brief Maps human-readable method names to How::Data::Type enum values
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
			/// @param data Data token string to convert (e.g., "g", "m", "e", "cal")
			/// @param entry Corresponding How::Entry::Type context ("h" vs. "m" token rules)
			/// @return Corresponding Data::Type value, or Data::Type::Error if unrecognized
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