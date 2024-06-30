#pragma once
#include<memory>
#include "Rynex/Renderer/Buffer.h"

namespace Rynex {
	class VertexArray 
	{
	public:
		enum class Primitv
		{
			Nono = 0,
			Traingle, TraingleStrips, TraingleFan,
			Line, LineLoop, LineStrips,
			Points,
		};
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual void SetPrimitv(Primitv primitv) = 0;
		virtual Primitv GetPrimitv() = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffers() const = 0;
		
		virtual const char* GetPrimitvChar() const = 0;

		static Ref<VertexArray> Create();
	};

	
}