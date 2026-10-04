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
		UUID(Hash64 uuid);
		UUID(const UUID&) = default;

		operator Hash64() const { return m_UUID; }
	    bool operator == (const UUID& uuid) const
		{
		    return uuid.m_UUID == m_UUID;
		}

	    bool operator != (const UUID& uuid) const
		{
		    return uuid.m_UUID != m_UUID;
		}

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
			return (Rynex::Hash64)uuid;
		}
	};

}



