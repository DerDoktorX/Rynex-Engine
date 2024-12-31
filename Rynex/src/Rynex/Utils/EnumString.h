#pragma once
namespace Rynex {
	class EnumString
	{
		public:
		template<typename T>
		static T GetEnumFormString(std::string_view e)
		{
			return T::None;
		}
		//{
		//	std::optional<T> type = magic_enum::enum_cast<T>(e, magic_enum::case_insensitive);
		//	if (type.has_value())
		//		return type.value();
		//	return T::None;
		//}

		template<typename T>
		static std::string_view GetStringFromEnum(T e)
		{
			return std::string_view();
		}
		//{
		//	return magic_enum::enum_name(e);
		//}
	};
};