#include "rypch.h"
#include "OpenGLBuffer.h"

#include <Platform/OpenGL/OpenGLBase.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>

#define RY_USE_OLD_OPENGL 0



namespace Rynex {
	namespace Utils {

		static void AddMemoryToTracker(uint32_t memory)
		{
			Application& app = Application::Get();
			Window& window = app.GetWindow();
			GraphicsContext* conext = window.GetGraphicsContext();
			OpenGLContext* OpenGLconext = static_cast<OpenGLContext*>(conext);
			OpenGLconext->AddCurentAllocMemory(static_cast<uint64_t>(memory));
		}

		static void RemoveMemoryToTracker(uint32_t memory)
		{
			Application& app = Application::Get();
			Window& window = app.GetWindow();
			GraphicsContext* conext = window.GetGraphicsContext();
			OpenGLContext* OpenGLconext = static_cast<OpenGLContext*>(conext);
			OpenGLconext->RemoveCurentAllocMemory(static_cast<uint64_t>(memory));

		}

		static GLenum GetBufferDataUsage(BufferDataUsage usage)
		{

			switch (usage)
			{
				RY_CASE_RETURN(BufferDataUsage::None, GL_STATIC_DRAW);
				RY_CASE_RETURN(BufferDataUsage::DynamicDraw, GL_DYNAMIC_DRAW);
				RY_CASE_RETURN(BufferDataUsage::StaticDraw, GL_STATIC_DRAW);
			default:
				RY_CORE_ASSERT(false, "BufferData, GL_STATIC_DRAW");
				return GL_STATIC_DRAW;
			}
			return GL_STATIC_DRAW;
		}



		static GLenum GetFlagsFromFlagTypes(BufferFlagGPU flag)
		{
			return GL_DYNAMIC_STORAGE_BIT;
		}

		static GLenum GetTarget(BufferType target)
		{
			switch (target)
			{
				RY_CASE_RETURN(BufferType::None, 0u);
				RY_CASE_RETURN(BufferType::ShaderStorage, GL_SHADER_STORAGE_BUFFER);
				RY_CASE_RETURN(BufferType::Index, GL_ELEMENT_ARRAY_BUFFER);
				RY_CASE_RETURN(BufferType::Vertex, GL_ARRAY_BUFFER);
				RY_CASE_RETURN(BufferType::DrawIndrirect, GL_DRAW_INDIRECT_BUFFER);
				RY_CASE_RETURN(BufferType::Uniform, GL_UNIFORM_BUFFER);
			default:
				RY_CORE_ASSERT(false, "BufferData, GL_STATIC_DRAW");
				return 0u;
			}
			return 0u;
		}

		static GLint GetMaxUniforms()
		{
			GLint maxUniformBlockSize = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
			return maxUniformBlockSize;
		}

	}

#pragma region VertexBuffer


	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Buffer(s_Target, nullptr, size, Utils::GetFlagsFromFlagTypes(BufferFlag::None))
		, m_Flag(BufferFlag::None)
	{
	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
	{
	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_Layout(layout)
	{
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<unsigned char>&& data, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout)
		: m_Buffer(s_Target, std::move(data), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_Layout(layout)
	{
	}

	
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
		: m_Buffer(s_Target, nullptr, size, Utils::GetFlagsFromFlagTypes(BufferFlag::None))
		, m_Flag(BufferFlag::None)
	{
	}

	

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
	}

	bool OpenGLVertexBuffer::IsTransferd()
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	void OpenGLVertexBuffer::Bind() const
	{
		m_Buffer.Bind(s_Target);
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		m_Buffer.UnBind(s_Target);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t byteSize)
	{
		uint32_t offestByteSize = 0;
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(data), offestByteSize, byteSize);
		OnDataChangeCall();
	}

	void OpenGLVertexBuffer::ResizeBuffer(const void* data, uint32_t byteSize)
	{
		uint32_t flag = Utils::GetFlagsFromFlagTypes(m_Flag);
		m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, flag);
		OnSpecifcationChangeCall();
	}

	void OpenGLVertexBuffer::CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<VertexBuffer> vb)
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLVertexBuffer::CopyData, fromPoint, toPoint, byteSize, vb);

		Ref<OpenGLVertexBuffer> vbOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vb);
		uint32_t readVB_ID = vbOpenGL->GetRenderID();
		uint32_t size = vbOpenGL->GetByteSize();
		int32_t space = byteSize + fromPoint;
		RY_CORE_ASSERT(space <= size);
		int32_t space2 = byteSize + toPoint;
		RY_CORE_ASSERT(space2 <= m_Buffer.GetByteSize(), "Whrite Buffer is not large enugh");

		uint32_t rendererID = m_Buffer.GetRenderID();
		glCopyNamedBufferSubData(readVB_ID, rendererID, fromPoint, toPoint, byteSize);
		m_Buffer.LoadeGPUDataOnCPU();

		RY_CORE_TRACE("Copy Action Finsihed!");
		OnDataChangeCall();
	}

	void OpenGLVertexBuffer::AddCopyData( Ref<VertexBuffer> vb)
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLVertexBuffer::AddCopyData, vb);

		Ref<OpenGLVertexBuffer> vbOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vb);
		uint32_t readVB_ID = vbOpenGL->GetRenderID();
		uint32_t sizeRead = vbOpenGL->GetByteSize();
		uint32_t oldSizeWrith = m_Buffer.GetByteSize();

		m_Buffer.LoadeGPUDataOnCPU();
		{
			std::vector<uint8_t> data = m_Buffer.GetData();
			RY_CORE_ASSERT(data.size() == oldSizeWrith);
			uint32_t resizeToByteSize = oldSizeWrith + sizeRead;
			data.resize(resizeToByteSize);

			uint32_t flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			uint8_t* dataPtr = data.data();
			m_Buffer.ResizeData(GL_ARRAY_BUFFER, dataPtr, resizeToByteSize, flag);

			OnSpecifcationChangeCall();
		}
		uint32_t rendererID = m_Buffer.GetRenderID();
		glCopyNamedBufferSubData(readVB_ID, rendererID, 0u, oldSizeWrith, sizeRead);
		m_Buffer.LoadeGPUDataOnCPU();
		RY_CORE_TRACE("Add Copy New Data Action Finsihed!");
		OnDataChangeCall();
	}



	const std::vector<unsigned char>& OpenGLVertexBuffer::GetBufferData() const
	{
		return m_Buffer.GetData();
	}

	void OpenGLVertexBuffer::FreeBufferData()
	{
		RY_CORE_NOT_IMPL();
	}

	uint32_t OpenGLVertexBuffer::GetVertexCount() const
	{
		uint32_t size = m_Buffer.GetByteSize() / m_Layout.GetStride();
		return size;
	}

	void OpenGLVertexBuffer::AddParent(OpenGLVertexArray* vertexArrayPtr)
	{


		Ref<OpenGLVertexArray> openGLVertexArrayRef = Asset::GetRefInPlaceType(vertexArrayPtr);
		for (auto& parent : m_ParentVec)
		{
			if (std::visit([&openGLVertexArrayRef](auto& openGLWeak)
					{
						if constexpr (std::is_same_v<std::decay_t<decltype(openGLWeak)>, Weak<OpenGLVertexArray>>)
						{
							return openGLWeak.lock() == openGLVertexArrayRef;
						}
						return false;
					}, parent))
			{
				return;
			}
		}
		m_ParentVec.emplace_back(openGLVertexArrayRef);

	}

	void OpenGLVertexBuffer::RemoveParent(OpenGLVertexArray* vertexArrayPtr)
	{
		Ref<OpenGLVertexArray> openGLVertexArrayRef = Asset::GetRefInPlaceType(vertexArrayPtr);
		uint32_t index = 0u;

		for (auto& parent : m_ParentVec)
		{
			if (std::visit([&openGLVertexArrayRef](auto& openGLWeak)
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(openGLWeak)>, Weak<OpenGLVertexArray>>)
					{
						return openGLWeak.lock() == openGLVertexArrayRef;
					}
					return false;
				}, parent))
			{
				break;
			}
			index++;
		}
		if (m_ParentVec.size() == index)
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);

	}

	void OpenGLVertexBuffer::OnSpecifcationChangeCall()
	{
		for (auto& parent : m_ParentVec)
		{
			std::visit(
				[vertexBufferClass = this](auto& openGLWeak)
				{
					if (auto openGLRef = openGLWeak.lock())
					{
						openGLRef->OnChildeSpecifcationChange(vertexBufferClass);
					}
				}
			, parent);
			
			
		}

	}

	void OpenGLVertexBuffer::OnDataChangeCall()
	{
		for (auto& parent : m_ParentVec)
		{
			std::visit(
				[vertexBufferClass = this](auto& openGLWeak)
				{
					if (auto openGLRef = openGLWeak.lock())
					{
						openGLRef->OnChildeDataChange(vertexBufferClass);
					}
				}
			, parent);


		}
	}

	void OpenGLVertexBuffer::OnDestroyCall()
	{
		for (auto& parent : m_ParentVec)
		{
			std::visit(
				[vertexBufferClass = this](auto& openGLWeak)
				{
					if (auto openGLRef = openGLWeak.lock())
					{
						openGLRef->OnChildeDestroy(vertexBufferClass);
					}
				}
			, parent);


		}
	}
	

#pragma endregion

	
#pragma region IndexBuffer


	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count * sizeof(uint32_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_EllementByte(sizeof(uint32_t))
	{
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count * sizeof(uint16_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_EllementByte(sizeof(uint16_t))
	{

	}


	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, nullptr, count * sizeof(uint32_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_EllementByte(sizeof(uint32_t))
	{
		std::vector<uint32_t> dataMove = std::move(data);
		const uint32_t* indices = dataMove.data();
		uint32_t offset = 0u;
		uint32_t bytesSize = count * sizeof(uint32_t);
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, bytesSize);


	}

	

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
	}


	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{ 
		m_EllementByte = sizeof(uint32_t);
		uint32_t byteSize = count * m_EllementByte;

		if(byteSize <= m_Buffer.GetByteSize())
		{
			uint32_t offset = 0u;
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, byteSize);

		}
		else
		{
			GLenum flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize, flag);
		}
		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::SetData(const uint16_t* indices, uint32_t count)
	{
		m_EllementByte = sizeof(uint16_t);
		uint32_t byteSize = count * m_EllementByte;

		if (byteSize <= m_Buffer.GetByteSize())
		{
			uint32_t offset = 0u;
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, byteSize);
		}
		else
		{
			GLenum flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize, flag);
		}

		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<IndexBuffer> ib)
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLIndexBuffer::CopyData, fromPoint, toPoint, byteSize, ib);
		RY_CORE_ASSERT(0u != m_Buffer.GetRenderID(), "Not set Buffer ID!");

		Ref<OpenGLIndexBuffer> vbOpenGL = std::static_pointer_cast<OpenGLIndexBuffer>(ib);
		uint32_t readVB_ID = vbOpenGL->GetRenderID();
		RY_CORE_ASSERT(0u != readVB_ID, "Not set Buffer ID!");
		uint32_t sizebyteRead = vbOpenGL->GetByteSize();
		int32_t spaceRead = byteSize + fromPoint;
		RY_CORE_ASSERT(spaceRead <= sizebyteRead);
		int32_t spaceWhrit = byteSize + toPoint;
		RY_CORE_ASSERT(spaceWhrit <= GetByteSize(), "Whrite Buffer is not large enugh");

		glCopyNamedBufferSubData(readVB_ID, m_Buffer.GetRenderID(), fromPoint, toPoint, byteSize);		

		std::vector<uint8_t>& byteVec = m_Buffer.GetData();
		uint8_t* dataBytePtr = byteVec.data();
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0, GetByteSize(), dataBytePtr);

		RY_CORE_TRACE("Copy Action Finsihed!");
		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::AddCopyData(Ref<IndexBuffer> ib)
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLIndexBuffer::AddCopyData, ib);

		Ref<OpenGLIndexBuffer> vbOpenGL = std::static_pointer_cast<OpenGLIndexBuffer>(ib);
		uint32_t readVB_ID = vbOpenGL->GetRenderID();
		RY_CORE_ASSERT(0u != m_Buffer.GetRenderID(), "Not set Buffer ID!");
		RY_CORE_ASSERT(0u != readVB_ID, "Not set Buffer ID!");

		uint32_t countRead = vbOpenGL->GetCount();
		uint32_t byteSizeRead = vbOpenGL->GetByteSize();
		uint32_t oldCountWhrit = GetCount();
		uint32_t oldByteSizeWhrit = GetByteSize();

		std::vector<uint8_t>& byteVec = m_Buffer.GetData();
		uint8_t* dataBytePtr = byteVec.data();
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, oldByteSizeWhrit, dataBytePtr);

		uint32_t newByteSizeWhrit = GetByteSize();

		glNamedBufferStorage(m_Buffer.GetRenderID(), newByteSizeWhrit, dataBytePtr, Utils::GetFlagsFromFlagTypes(m_Flag));
		glCopyNamedBufferSubData(readVB_ID, m_Buffer.GetRenderID(), 0u, oldByteSizeWhrit, byteSizeRead);
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, GetByteSize(), dataBytePtr);


		RY_CORE_TRACE("Add Copy New Data Action Finsihed!");
		OnSpecifcationChangeCall();
	}


	const std::vector<uint8_t>& OpenGLIndexBuffer::GetBufferData() const
	{
		return m_Buffer.GetData();
	}

	void OpenGLIndexBuffer::FreeBufferData()
	{
		RY_CORE_NOT_IMPL();

	}


	bool OpenGLIndexBuffer::IsTransferd()
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	void OpenGLIndexBuffer::Bind() const
	{
		m_Buffer.Bind(s_Target);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		m_Buffer.UnBind(GL_ELEMENT_ARRAY_BUFFER);
	}

	

	void OpenGLIndexBuffer::AddParent(OpenGLVertexArray* vertexArrayPtr)
	{
		Ref<OpenGLVertexArray> openGLStorageBufferRef = Asset::GetRefInPlaceType(vertexArrayPtr);
		for (Weak<OpenGLVertexArray>& openGLStorageBufferWeek : m_ParentVec)
		{
			if (openGLStorageBufferWeek.lock() == openGLStorageBufferRef)
				return;
		}
		m_ParentVec.emplace_back(openGLStorageBufferRef);
	}

	void OpenGLIndexBuffer::RemoveParent(OpenGLVertexArray* vertexArrayPtr)
	{
		Ref<OpenGLVertexArray> openGLVertexArrayRef = Asset::GetRefInPlaceType(vertexArrayPtr);
		uint32_t index = 0u;
		for (Weak<OpenGLVertexArray>& openGLVertexArrayWeek : m_ParentVec)
		{
			if (openGLVertexArrayWeek.lock() == openGLVertexArrayRef)
				return;
		}
		if (index == m_ParentVec.size())
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);
	}

	void OpenGLIndexBuffer::OnSpecifcationChangeCall()
	{
		for (Weak<OpenGLVertexArray>& vaoWeak : m_ParentVec)
		{
			if (Ref<OpenGLVertexArray> vaoRef = vaoWeak.lock())
			{
				vaoRef->OnChildeSpecifcationChange(this);
			}
		}
	}

	void OpenGLIndexBuffer::OnDataChangeCall()
	{
		for (Weak<OpenGLVertexArray>& vaoWeak : m_ParentVec)
		{
			if (Ref<OpenGLVertexArray> vaoRef = vaoWeak.lock())
			{
				vaoRef->OnChildeDataChange(this);
			}
		}
	}

	void OpenGLIndexBuffer::OnDestroyCall()
	{
		for (Weak<OpenGLVertexArray>& vaoWeak : m_ParentVec)
		{
			if (Ref<OpenGLVertexArray> vaoRef = vaoWeak.lock())
			{
				vaoRef->OnChildeDestroy(this);
			}
		}
	}

#pragma endregion


#pragma region StorageBuffer

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t byteSize, BufferFlagGPU flag)
		: m_FlagTypes(flag)
		, m_Target(GL_SHADER_STORAGE_BUFFER)
		, m_Buffer(GL_SHADER_STORAGE_BUFFER, nullptr, byteSize, Utils::GetFlagsFromFlagTypes(flag))
	{
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(const void* dataPtr, uint32_t byteSize, BufferType buffertype, BufferFlagGPU flag)
		: m_FlagTypes(flag)
		, m_Target(Utils::GetTarget(buffertype))
		, m_Buffer(Utils::GetTarget(buffertype), reinterpret_cast<const uint8_t*>(dataPtr), byteSize, Utils::GetFlagsFromFlagTypes(flag))
	{
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(const void* dataPtr, uint32_t byteSize, BufferFlagGPU flag)
		: m_FlagTypes(flag)
		, m_Target(GL_SHADER_STORAGE_BUFFER)
		, m_Buffer(GL_SHADER_STORAGE_BUFFER, reinterpret_cast<const uint8_t*>(dataPtr), byteSize, Utils::GetFlagsFromFlagTypes(flag))
	{
	}
	
	OpenGLStorageBuffer::~OpenGLStorageBuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
	}

	bool OpenGLStorageBuffer::IsTransferd()
	{
		return m_Buffer.IsTransfered();
	}

	void OpenGLStorageBuffer::Bind(uint32_t slot)
	{
		m_Buffer.BindSlot(m_Target, slot);
	}

	void OpenGLStorageBuffer::UnBind(uint32_t slot)
	{
		m_Buffer.UnBindSlot(m_Target, slot);
	}


	

	void OpenGLStorageBuffer::SetData(const void* dataPtr, uint32_t byteSize)
	{
		m_Buffer.SetData(m_Target, reinterpret_cast<const uint8_t*>(dataPtr), 0u, byteSize);
	}

	void OpenGLStorageBuffer::SetData(const void* dataPtr, uint32_t offset, uint32_t byteSize)
	{
		m_Buffer.SetData(m_Target, reinterpret_cast<const uint8_t*>(dataPtr), offset, byteSize);
	}

	void OpenGLStorageBuffer::ResizeBuffer(uint32_t byteSize)
	{
		m_Buffer.ResizeData(m_Target, byteSize);
	}

	void OpenGLStorageBuffer::ResizeBuffer(const void* dataPtr, uint32_t byteSize)
	{
		uint32_t falgs = m_Buffer.GetFlags();
		m_Buffer.ResizeData(m_Target, reinterpret_cast<const uint8_t*>(dataPtr), byteSize, falgs);
	}

	void OpenGLStorageBuffer::OnChildeSpecifcationChange(OpenGLUniformBuffer* uniformBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLStorageBuffer::OnChildeDataChange(OpenGLUniformBuffer* uniformBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLStorageBuffer::OnChildeDestroy(OpenGLUniformBuffer* uniformBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLStorageBuffer::OnChildeSpecifcationChange(OpenGLVertexBuffer* vertexBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLStorageBuffer::OnChildeDataChange(OpenGLVertexBuffer* vertexBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLStorageBuffer::OnChildeDestroy(OpenGLVertexBuffer* vertexBufferPtr)
	{
		RY_CORE_NOT_IMPL();
	}

#pragma endregion


#pragma region UniformBuffer

	OpenGLUniformBuffer::OpenGLUniformBuffer(std::vector<uint8_t>&& data, const BufferLayout& layout, BufferFlagGPU flag)
		: m_Layout(layout)
		, m_Buffer(s_Target, std::move(data), Utils::GetFlagsFromFlagTypes(flag))
		, m_FlagTypes(flag)
	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t byteSize)
		: m_Layout()
		, m_Buffer(s_Target, nullptr,byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)
	{
		
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize)
		: m_Layout()
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)

	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() < Utils::GetMaxUniforms(), "to large!");
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		
		CheckLayout();
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)
	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferFlagGPU flag)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(flag))
		, m_FlagTypes(flag)
	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const Ref<OpenGLUniformBuffer>& uniformBufferOpenGL)
		: m_Layout(uniformBufferOpenGL->m_Layout)
		, m_Buffer(s_Target, uniformBufferOpenGL->m_Buffer)
		, m_FlagTypes(uniformBufferOpenGL->m_FlagTypes)
	{
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
	}	
	

	bool OpenGLUniformBuffer::IsTransferd()
	{
		return m_Buffer.IsTransfered();
	}


	void OpenGLUniformBuffer::Bind(uint32_t slot)
	{
		m_Buffer.BindSlot(s_Target, slot);
	}

	void OpenGLUniformBuffer::UnBind(uint32_t slot)
	{
		m_Buffer.BindSlot(s_Target, slot);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t byteSize)
	{
		SetData(data, 0u, byteSize);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t byteSize)
	{
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(data), offset, byteSize);
	}

	const std::vector<uint8_t>& OpenGLUniformBuffer::GetBufferData()const
	{
		return m_Buffer.GetData();
	}

	void OpenGLUniformBuffer::FreeBufferData()
	{
		auto& vecData = m_Buffer.GetData();
		vecData.clear();
	}


	void OpenGLUniformBuffer::AddParent(OpenGLStorageBuffer* storageBufferPtr)
	{	
		Ref<OpenGLStorageBuffer> openGLStorageBufferRef = Asset::GetRefInPlaceType(storageBufferPtr);
		for (Weak<OpenGLStorageBuffer>& openGLStorageBufferWeek : m_ParentVec)
		{
			if (openGLStorageBufferWeek.lock() == openGLStorageBufferRef)
				return;
		}
		m_ParentVec.emplace_back(openGLStorageBufferRef);

	}

	

	void OpenGLUniformBuffer::RemoveParent(OpenGLStorageBuffer* storageBufferPtr)
	{
		Ref<OpenGLStorageBuffer> openGLStorageBufferRef = Asset::GetRefInPlaceType(storageBufferPtr);
		uint32_t index = 0u;
		for (Weak<OpenGLStorageBuffer>& openGLStorageBufferWeek : m_ParentVec)
		{
			if (openGLStorageBufferWeek.lock() == openGLStorageBufferRef)
				break;
			index++;
		}
		if (index == m_ParentVec.size())
			return;

		m_ParentVec.erase(m_ParentVec.begin() + index);
	}

	void OpenGLUniformBuffer::CheckLayout()
	{
		uint32_t size = 0ul;
		for (const BufferElement& element : m_Layout)
		{
			size += element.size;
			uint8_t size16 = size % 16;
      		RY_CORE_ASSERT(!(element.size > 16 && size16 != 0));

		}
	}

#pragma endregion

#pragma region IndrectBuffer

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(uint32_t byteSize)
		: m_Buffer(s_Target, nullptr, byteSize, s_FlagsOptimz)
		, m_Layout({
			{ShaderDataType::Uint, "Count"},
			{ShaderDataType::Uint, "InstanceCount"},
			{ShaderDataType::Uint, "FirstIndex"},
			{ShaderDataType::Int, "BaseVertex"},
			{ShaderDataType::Uint, "BaseInstance"}
		}, 0u)
	{
		uint32_t byteSizeBuffer = GetByteSize();
		uint32_t stride = m_Layout.GetStride();
		uint32_t byteModuleStride = byteSizeBuffer % stride;
		RY_CORE_ASSERT(16u <= byteSizeBuffer, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(0u == byteModuleStride, "OpenGL IndriectBuffer");

	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(uint32_t byteSize, const BufferLayout& layout)
		: m_Buffer(s_Target, nullptr, byteSize, s_FlagsOptimz)
		, m_Layout(layout)
	{
		uint32_t byteSizeBuffer = GetByteSize();
		uint32_t stride = m_Layout.GetStride();
		uint32_t byteModuleStride = byteSizeBuffer % stride;
		RY_CORE_ASSERT(16u <= byteSizeBuffer, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(0u == byteModuleStride, "OpenGL IndriectBuffer");

	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(const void* data, uint32_t byteSize)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, s_FlagsOptimz)
		, m_Layout({
			{ShaderDataType::Uint, "Count"},
			{ShaderDataType::Uint, "InstanceCount"},
			{ShaderDataType::Uint, "FirstIndex"},
			{ShaderDataType::Int, "BaseVertex"},
			{ShaderDataType::Uint, "BaseInstance"}
		}, 0u)
	{
		uint32_t byteSizeBuffer = GetByteSize();
		uint32_t stride = m_Layout.GetStride();
		uint32_t byteModuleStride = byteSizeBuffer % stride;
		RY_CORE_ASSERT(16u <= byteSizeBuffer, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(0u == byteModuleStride, "OpenGL IndriectBuffer");
	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, s_FlagsOptimz)
		, m_Layout(layout)
	{
		uint32_t byteSizeBuffer = GetByteSize();
		uint32_t stride = m_Layout.GetStride();
		uint32_t byteModuleStride = byteSizeBuffer % stride;
		RY_CORE_ASSERT(16u <= byteSizeBuffer, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(0u == byteModuleStride, "OpenGL IndriectBuffer");
	}

	OpenGLIndriectBuffer::~OpenGLIndriectBuffer()
	{
	}

	bool OpenGLIndriectBuffer::IsTransferd()
	{
		return m_Buffer.IsTransfered();
	}

	void OpenGLIndriectBuffer::Bind() const
	{
		m_Buffer.Bind(s_Target);
	}

	void OpenGLIndriectBuffer::UnBind() const
	{
		m_Buffer.UnBind(s_Target);
	}

	

	void OpenGLIndriectBuffer::SetData(const void* data, uint32_t byteSize)
	{
		const uint8_t* byteDataPtr = reinterpret_cast<const uint8_t*>(data);
		m_Buffer.SetData(s_Target, byteDataPtr, 0u, byteSize);
	}



	void OpenGLIndriectBuffer::ResizeBuffer(uint32_t byteSize)
	{
		m_Buffer.ResizeData(s_Target, byteSize);
	}

	void OpenGLIndriectBuffer::ResizeBuffer(const void* data, uint32_t byteSize)
	{
		const uint8_t* byteDataPtr = reinterpret_cast<const uint8_t*>(data);
		m_Buffer.ResizeData(s_Target, byteDataPtr, byteSize, s_FlagsOptimz);
	}

	void OpenGLIndriectBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
		uint32_t byteSizeBuffer = GetByteSize();
		uint32_t stride = m_Layout.GetStride();
		uint32_t byteModuleStride = byteSizeBuffer % stride;
		RY_CORE_ASSERT(16u <= byteSizeBuffer, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(0u == byteModuleStride, "OpenGL IndriectBuffer");
	}


#pragma endregion
	
}