#pragma once

namespace Rynex {

	enum class ResurceType
	{
		Nono = 0,
		Texture, Texture2D,
		Shader,
		Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
		Entity, Scene, SceneCamera,
		FrameBuffer,
		VertexBuffer, IndexBuffer, Uniform,
	};

	

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

