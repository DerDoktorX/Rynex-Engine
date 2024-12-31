#include "rypch.h"
#include "EnumString.h"

#include <magic_enum.hpp>

namespace Rynex {

	 // template<typename T>
	 // T EnumString::GetEnumFormString(std::string_view e)
	 // {
	 // 	std::optional<T> type = magic_enum::enum_cast<T>(e, magic_enum::case_insensitive);
	 // 	if (type.has_value())
	 // 		return type.value();
	 // 	return T;
	 // 	// return T::None;
	 // }

	 // template<typename T>
	 // std::string_view EnumString::GetStringFromEnum(T e)
	 // {
	 // 	// return std::string_view();
	 // 	return magic_enum::enum_name(e);
	 // }
}