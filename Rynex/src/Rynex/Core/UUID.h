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

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	    bool operator == (const UUID& uuid) const
		{
		    return uuid.m_UUID == m_UUID;
		}

	    static UUID Zero() { return UUID(0ull); }
	private:
		uint64_t m_UUID;
	};

	
}

namespace std {

	template<>
	struct hash<Rynex::UUID>
	{
		[[nodiscard]] static std::size_t operator()(const Rynex::UUID& uuid) noexcept
		{
			return (uint64_t)uuid;
		}
	};

}



