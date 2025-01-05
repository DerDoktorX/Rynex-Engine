#pragma once
namespace Rynex {

	template<typename T>
	class AsyncVector
	{
	public:
		AsyncVector();


	private:
		std::vector<T> m_Data;
		mutable std::mutex m_Mutex;
	};
}
