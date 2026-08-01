#pragma once
#include <Rynex/Renderer/API/VertexArray.h>
#include <Platform/OpenGL/OpenGLPrototypes.h>


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
		virtual void ClearVertexBuffers() override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout)override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
		virtual void SetVertexBufferNew(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual uint32_t GetVertexBuffersCount()const override;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override;
		
		virtual uint32_t GetRenderID() const override { return m_RendererID; }
		virtual void SetBoxAABB(BufferElement postionElement) override;
		virtual const BoundingVolume GetBoxAABB() const override { return m_Box; }
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
#else
		virtual const std::vector<VertexArray::VertexElements>& GetVertexBuffers() const override { return m_VertexElements; }
#endif
		

		virtual uint32_t GetTringleCount() const override;
		virtual const char* GetPrimitvChar() const override;
		// void RegenerateOpenGLVertexBuffer(OpenGLVertexBuffer* bufferPtr);
		// void RegenerateOpenGLIndexBuffer(OpenGLIndexBuffer* bufferPtr);
		uint32_t GetPrimitvOpenGLMode() const { return m_PrimitvGL; }

		void OnChildeSpecifcationChange(OpenGLVertexBuffer* vertexBufferPtr);
		void OnChildeDataChange(OpenGLVertexBuffer* vertexBufferPtr);
		void OnChildeDestroy(OpenGLVertexBuffer* vertexBufferPtr);

		void OnChildeSpecifcationChange(OpenGLIndexBuffer* vertexBufferPtr);
		void OnChildeDataChange(OpenGLIndexBuffer* vertexBufferPtr);
		void OnChildeDestroy(OpenGLIndexBuffer* vertexBufferPtr);

	private:
		void AddVertexBufferToList(const Ref<VertexBuffer>& vb, const BufferLayout& layout);
		void CreateID();
		void DestroyID();
		void Invalidate();

		static bool IsLayouteEqual(const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL);
		bool SetupVertexBuffersToVAO(uint32_t bindIndex, const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL);
		void SetupVertexBuffersToLayoutVAO(uint32_t bindIndex, uint32_t& offsetEllment, const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL);
		uint32_t AddFloatBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts);
		uint32_t AddIntBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts);
		uint32_t AddUintBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts);


		// void AddUintBufferEllementToVAO(uint32_t indexElemment, uint32_t bindIndex, const BufferElement& element);

	private:
		uint32_t m_RendererID = 0;
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
#endif
		std::vector<VertexElements> m_VertexElements;
		
		Ref<IndexBuffer> m_IndexBuffer;
		Primitv m_Primitv = Primitv::None;
		BoundingVolume m_Box;
		uint32_t m_RenderResterRiezer = 0;
		uint32_t m_PrimitvGL = 0;
		static uint32_t s_LastBindVAO;
	};

}

