#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>

namespace Rynex {

	class RYNEX_API VertexArray : public Asset
	{
	public:
		struct VertexElements
		{
			BufferLayout		UseLayout;
			Ref<VertexBuffer>	Buffer;
		};
	public:
		enum class Primitv
		{
			None = 0,
			Traingle, TraingleStrips, TraingleFan,
			Line, LineLoop, LineStrips,
			Points,
			Patches,
		};
	public:
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

		virtual void SetPrimitv(Primitv primitv) = 0;
		virtual Primitv GetPrimitv() = 0;

		virtual const std::vector<VertexArray::VertexElements>& GetVertexBuffers() const = 0;
		
		virtual void SetBoxAABB(BufferElement element) = 0;
		virtual const BoundingVolume GetBoxAABB() const = 0;
		virtual uint32_t GetTringleCount() const = 0;
		virtual const char* GetPrimitvChar() const = 0;

		static AssetType GetStaticType() { return AssetType::VertexArray; }
		AssetType GetType() const { return GetStaticType(); }
	};

	
}