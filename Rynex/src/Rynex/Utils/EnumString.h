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

		template<typename T>
		static std::string_view GetStringFromEnum(T e)
		{
			return std::string_view("");
		}
	};
};