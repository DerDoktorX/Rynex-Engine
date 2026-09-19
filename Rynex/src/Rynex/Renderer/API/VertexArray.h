#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>

namespace Rynex {

	class RYNEX_API VertexArray : public Asset
	{
	public:
		struct VertexElements
		{
			BufferLayout		m_UseLayout;
			Ref<VertexBuffer>	m_Buffer;
		};
	// enum ------------------------------------------------------------------------------------------------------------------
		enum class Primitive //Triangle
		{
			None = 0,
			Triangle, TriangleStrips, TriangleFan,
			Line, LineLoop, LineStrips,
			Points,
			Patches,
		};
	// interface methods ------------------------------------------------------------------------------------------------------
        // ReSharper disable once CppEnforceOverridingDestructorStyle
        virtual ~VertexArray() {}
		static Ref<VertexArray> Create();

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual uint32_t GetRenderID() const = 0;


		virtual void ClearVertexBuffers() = 0;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;
		virtual void SetVertexBufferNew(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual uint32_t GetVertexBuffersCount() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		virtual void SetPrimitive(Primitive primitive) = 0;
		virtual Primitive GetPrimitive() = 0;

		virtual const std::vector<VertexArray::VertexElements>& GetVertexBuffers() const = 0;
		
		virtual void SetBoxAABB(BufferElement element) = 0;
		virtual const BoundingVolume GetBoxAABB() const = 0;
		virtual uint32_t GetTringleCount() const = 0;
		virtual const char* GetPrimitiveChar() const = 0;
	// enum ------------------------------------------------------------------------------------------------------------------
		static AssetType GetStaticType() { return AssetType::VertexArray; }
		AssetType GetType() const override { return GetStaticType(); }
	};

	
}