#pragma once
#include "Rynex/Renderer/API/VertexArray.h"



namespace Rynex{

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetPrimitv(Primitv primitv) override;
		virtual Primitv GetPrimitv() override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual void SetBoxAABB(BufferElement postionElement) override;
		virtual const BoxAABB GetBoxAABB() const override { return m_Box; }
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; };
		virtual const Ref<IndexBuffer>& GetIndexBuffers() const { return m_IndexBuffer; }

		virtual const char* GetPrimitvChar() const override;
		virtual void InitAsync() override;

	private:
		uint32_t m_RendererID = 0;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
		Primitv m_Primitv = Primitv::Nono;
		BoxAABB m_Box;
	};

}

