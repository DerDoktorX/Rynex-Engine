#pragma once

#include <xhash>

namespace Rynex {
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std {

	template<>
	struct hash<Rynex::UUID>
	{
		std::size_t operator()(const Rynex::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}

