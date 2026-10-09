#pragma once

namespace Rynex {

    using Hash64 = uint64_t;
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

	class UUID
	{
	public:
		UUID();
        explicit UUID(Hash64 uuid);
		UUID(const UUID&) = default;

	    UUID& operator=(const UUID& right) noexcept
	    {
	        m_UUID = right.m_UUID;
	        return *this;
	    }

	    operator Hash64() const { return m_UUID; }
	    explicit operator bool() const { return 0ull != m_UUID; }
	    bool operator == (const UUID& uuid) const
		{
		    return uuid.m_UUID == m_UUID;
		}

	    bool operator==(const Hash64& hash) const
	    {
	        return m_UUID == hash;
	    }

	    bool operator != (const UUID& uuid) const
		{
		    return uuid.m_UUID != m_UUID;
		}

	    bool operator!=(const Hash64& hash) const
	    {
	        return m_UUID == hash;
	    }

	    Hash64 GetHash() const { return m_UUID; }

	    static UUID Zero() { return UUID(0ull); }
    private:
		Hash64 m_UUID;
	};

	
}

namespace std {

	template<>
	struct hash<Rynex::UUID>
	{
		[[nodiscard]] static std::size_t operator()(const Rynex::UUID& uuid) noexcept
		{
			return uuid.GetHash();
		}
	};

}

namespace robin_hood {

    template<>
    struct hash<Rynex::UUID>
    {
        [[nodiscard]] std::size_t operator()(const Rynex::UUID& uuid) const noexcept
        {
            return uuid.GetHash();
        }
    };

}



