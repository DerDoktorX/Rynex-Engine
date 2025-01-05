#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Renderer/Objects/BoxAABB.h"

namespace Rynex {

	class RYNEX_API VertexArray : public Asset
	{
	public:
		enum class Primitv
		{
			Nono = 0,
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

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual void SetPrimitv(Primitv primitv) = 0;
		virtual Primitv GetPrimitv() = 0;
		virtual void InitAsync() = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffers() const = 0;
		
		virtual void SetBoxAABB(BufferElement element) = 0;
		virtual const BoxAABB GetBoxAABB() const = 0;

		virtual const char* GetPrimitvChar() const = 0;

		static AssetType GetStaticType() { return AssetType::VertexArray; }
		AssetType GetType() const { return GetStaticType(); }
	};

	
}