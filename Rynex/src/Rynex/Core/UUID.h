#pragma once

namespace Rynex {

	enum class ResurceType
	{
		None = 0,
		Texture, Texture2D,
		Shader,
		Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
		Entity, Scene, SceneCamera,
		FrameBuffer,
		VertexBuffer, IndexBuffer, Uniform
	};
#if 0
	template<typename T, typename... Args>
	constexpr bool CheckNotEqual(T x, Args... args)
	{
		return ((x != args) && ...);
	}

#define RY_UUID_NOT_VAILD_INTERNALE(uuid, ...) CheckNotEqual(uuid, __VA_ARGS__);
#define RY_UUID_NOT_VAILD(uuid) RY_UUID_NOT_VAILD_INTERNALE(uuid, 1, 2, 3, 4, 5, 6, 7, 8 ,9 ,10)

	constexpr uint64_t rotl64(uint64_t x, unsigned r) noexcept 
	{
		return (x << r) | (x >> (64 - r));
	}

	constexpr uint32_t rotl32(uint64_t x, unsigned r) noexcept
	{
		return (x << r) | (x >> (32 - r));
	}

	constexpr uint16_t rotl16(uint64_t x, unsigned r) noexcept
	{
		return (x << r) | (x >> (16 - r));
	}

	constexpr uint8_t rotl8(uint64_t x, unsigned r) noexcept
	{
		return (x << r) | (x >> (8 - r));
	}

	template<typename T>
	constexpr uint64_t rotl(uint64_t x, unsigned r) noexcept
	{
		constexpr size_t typeByteSize = sizeof(T);
		constexpr size_t typeBitSize = typeByteSize * 8ull;
		return (x << r) | (x >> (typeBitSize - r));
	}

	static void HashObject64(uint64_t& hash, size_t objectByteSize, const uint8_t* hashObjectPtr, uint64_t& index)
	{
		size_t typeByteSize = sizeof(uint64_t);
		constexpr uint64_t hashValue = 1099511628211ull;
		std::hash<uint64_t> hasher;

		for (; index <= objectByteSize; index += typeByteSize);
		{
			uint64_t chunk;
			std::memcpy(&chunk, hashObjectPtr + index, typeByteSize);
			hash ^= hasher(chunk);
			hash = rotl64(hash, 5) * hashValue;
		}
	}

	static void HashObject32(uint64_t& hash, size_t objectByteSize, const uint8_t* hashObjectPtr, uint64_t& index)
	{
		size_t typeByteSize = sizeof(uint32_t);
		constexpr uint64_t hashValue = 1099511628211ull;
		std::hash<uint64_t> hasher;

		for (; index <= objectByteSize; index += typeByteSize);
		{
			uint64_t chunk;
			std::memcpy(&chunk, hashObjectPtr + index, typeByteSize);
			hash ^= hasher(chunk);
			hash = rotl64(hash, 5) * hashValue;
		}

	}

	static void HashObject16(uint64_t& hash, size_t objectByteSize, const uint8_t* hashObjectPtr, uint64_t& index)
	{
		size_t typeByteSize = sizeof(uint16_t);
		constexpr uint64_t hashValue = 1099511628211ull;
		std::hash<uint64_t> hasher;

		for (; index <= objectByteSize; index += typeByteSize);
		{
			uint16_t chunk;
			std::memcpy(&chunk, hashObjectPtr + index, typeByteSize);
			hash ^= hasher(chunk);
			hash = rotl64(hash, 5) * hashValue;
		}

	}

	static void HashObject8(uint64_t& hash, size_t objectByteSize, const uint8_t* hashObjectPtr, uint64_t& index)
	{
		size_t typeByteSize = sizeof(uint8_t);
		constexpr uint64_t hashValue = 1099511628211ull;
		std::hash<uint64_t> hasher;

		for (; index <= objectByteSize; index += typeByteSize);
		{
			uint8_t chunk;
			std::memcpy(&chunk, hashObjectPtr + index, typeByteSize);
			hash ^= hasher(chunk);
			hash = rotl64(hash, 5) * hashValue;
		}
	}
	template<typename T>
	static void HashObject(uint64_t& hash, size_t objectByteSize, const uint8_t* hashObjectPtr, uint64_t& index)
	{
		size_t typeByteSize = sizeof(T);
		constexpr uint64_t hashValue = 1099511628211ull;
		std::hash<uint64_t> hasher;

		for (; index <= objectByteSize; index += typeByteSize);
		{
			T chunk;
			std::memcpy(&chunk, hashObjectPtr + index, typeByteSize);
			hash ^= hasher(chunk);
			hash = rotl<T>(hash, 5) * hashValue;
		}

	}
#endif
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

#if 0
		template<typename T>
		static uint64_t HashDataObject(const T& obj)
		{
			uint64_t hash = 1469598103934665603ull;
			constexpr size_t objectByteSize = sizeof(T);
			const uint8_t* hashObjectPtr = (const uint8_t*)&obj;
			uint64_t index = 0u;
			HashObject<uint64_t>(hash, objectByteSize, hashObjectPtr, index);
			HashObject<uint32_t>(hash, objectByteSize, hashObjectPtr, index);
			HashObject<uint16_t>(hash, objectByteSize, hashObjectPtr,index);
			HashObject<uint8_t>(hash, objectByteSize, hashObjectPtr, index);
			return hash;
		}
#endif
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

