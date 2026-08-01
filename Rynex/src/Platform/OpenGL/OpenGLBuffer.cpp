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


#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Buffer(s_Target, nullptr, size, Utils::GetBufferDataUsage(BufferDataUsage::StaticDraw))
		, m_Usage(BufferDataUsage::StaticDraw)
#else
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Buffer(s_Target, nullptr, size, Utils::GetFlagsFromFlagTypes(BufferFlag::None))
		, m_Flag(BufferFlag::None)
#endif
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if(OpenGLThreadContext::IsActive())
		{
			
		}
		else
		{
			RY_SUBMITE_MEMBER_FUNC_TO_MAINTHREED_QUEUE(OpenGLVertexBuffer::InitAsync);
		}
		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetBufferDataUsage(usage))
		, m_Usage(usage)
#else
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
#endif
	{
#ifdef RY_OPENGL_MAIN_THREADE

		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::InitAsync, this));
			return;
		}
		OpenGLVertexBuffer::InitAsync();
#else
#endif

	}


#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetBufferDataUsage(usage))
		, m_Usage(usage)
#else
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(vertices), size, Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
#endif
		, m_Layout(layout)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::InitAsync, this));
			return;
		}

		OpenGLVertexBuffer::InitAsync();
#else
#endif

	}
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<unsigned char>&& data, uint32_t size, BufferDataUsage usage, const BufferLayout& layout)
		: m_Buffer(s_Target, std::move(data), Utils::GetBufferDataUsage(usage))
		, m_Usage(usage)
#else
	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<unsigned char>&& data, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout)
		: m_Buffer(s_Target, std::move(data), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
#endif
		, m_Layout(layout)
	{
#ifdef RY_OPENGL_MAIN_THREADE

		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::InitAsync, this));
			return;
		}
		
		OpenGLVertexBuffer::InitAsync();
#else
#endif

	}

	
	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		: m_Buffer(s_Target, nullptr, size, GL_STATIC_DRAW)
		, m_Usage(BufferDataUsage::StaticDraw)
#else
		: m_Buffer(s_Target, nullptr, size, Utils::GetFlagsFromFlagTypes(BufferFlag::None))
		, m_Flag(BufferFlag::None)
#endif
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
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
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize);
#else
		uint32_t offestByteSize = 0;
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(data), offestByteSize, byteSize);
#endif
		OnDataChangeCall();
	}

	void OpenGLVertexBuffer::ResizeBuffer(const void* data, uint32_t byteSize)
	{
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		uint32_t usage = Utils::GetBufferDataUsage(m_Usage);
		m_Buffer.ResizeData(GL_ARRAY_BUFFER, reinterpret_cast<const uint8_t*>(data), byteSize, usage);
#else
		uint32_t flag = Utils::GetFlagsFromFlagTypes(m_Flag);
		m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, flag);
#endif
		OnSpecifcationChangeCall();
	}

	void OpenGLVertexBuffer::CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<VertexBuffer> vb)
	{
#ifdef RY_OPENGL_MAIN_THREADE

		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::AddCopyData, this, fromPoint, toPoint, byteSize, vb));
			return;
		}
		InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLVertexBuffer::CopyData, fromPoint, toPoint, byteSize, vb);
#endif

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

#ifdef RY_USE_GRAFIC_API_FANCE
		m_Buffer.SetupFance();
#endif
		RY_CORE_TRACE("Copy Action Finsihed!");
		OnDataChangeCall();
	}

	void OpenGLVertexBuffer::AddCopyData( Ref<VertexBuffer> vb)
	{
#ifdef RY_OPENGL_MAIN_THREADE

		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLVertexBuffer::AddCopyData, this, vertexArrayPtr));
			return;
		}
		InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLVertexBuffer::AddCopyData, vb);
#endif
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
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			uint32_t usage = Utils::GetBufferDataUsage(m_Usage);

			uint8_t* dataPtr = data.data();
			m_Buffer.ResizeData(GL_ARRAY_BUFFER, dataPtr, resizeToByteSize, usage);
#else
			uint32_t flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			uint8_t* dataPtr = data.data();
			m_Buffer.ResizeData(GL_ARRAY_BUFFER, dataPtr, resizeToByteSize, flag);

#endif
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



#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count  * sizeof(uint32_t), Utils::GetBufferDataUsage(usage))
		, m_Usage(usage)
#else
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count * sizeof(uint32_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
#endif
		, m_EllementByte(sizeof(uint32_t))
	{
#ifdef RY_OPENGL_MAIN_THREADE

		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndexBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count * sizeof(uint16_t), Utils::GetBufferDataUsage(usage))
		, m_EllementByte(sizeof(uint16_t))
		, m_Usage(usage)
#else
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(indices), count * sizeof(uint16_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
		, m_EllementByte(sizeof(uint16_t))
#endif
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndexBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t count, BufferDataUsage usage)
		: m_Buffer(s_Target, std::move(data), Utils::GetBufferDataUsage(usage))
		, m_Usage(usage)
#else
	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t count, BufferFlagGPU flag)
		: m_Buffer(s_Target, nullptr, count * sizeof(uint32_t), Utils::GetFlagsFromFlagTypes(flag))
		, m_Flag(flag)
#endif
		, m_EllementByte(sizeof(uint32_t))
	{
#ifndef RY_OPENGL_USE_ARRAY_BUFFER
		std::vector<uint32_t> dataMove = std::move(data);
		const uint32_t* indices = dataMove.data();
		uint32_t offset = 0u;
		uint32_t bytesSize = count * sizeof(uint32_t);
		m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, bytesSize);
#endif

#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndexBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
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
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize);

#else
			uint32_t offset = 0u;
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, byteSize);
#endif

		}
		else
		{
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			GLenum usageGL = Utils::GetBufferDataUsage(m_Usage);
			m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize, usageGL);
#else
			GLenum flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize, flag);
#endif

		}
		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::SetData(const uint16_t* indices, uint32_t count)
	{
		m_EllementByte = sizeof(uint16_t);
		uint32_t byteSize = count * m_EllementByte;

		if (byteSize <= m_Buffer.GetByteSize())
		{
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize);

#else
			uint32_t offset = 0u;
			m_Buffer.SetData(s_Target, reinterpret_cast<const uint8_t*>(indices), offset, byteSize);
#endif

		}
		else
		{
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			GLenum usageGL = Utils::GetBufferDataUsage(m_Usage);
			m_Buffer.ResizeData(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<const uint8_t*>(indices), byteSize, usageGL);
#else
			GLenum flag = Utils::GetFlagsFromFlagTypes(m_Flag);
			m_Buffer.ResizeData(s_Target, reinterpret_cast<const uint8_t*>(indices), byteSize, flag);
#endif

		}

		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<IndexBuffer> ib)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndexBuffer::CopyData, this, fromPoint, toPoint, byteSize, ib));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLIndexBuffer::CopyData, fromPoint, toPoint, byteSize, ib);
#endif
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

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0, GetByteSize(), m_Buffer.GetDataPtr());
#else
		std::vector<uint8_t>& byteVec = m_Buffer.GetData();
		uint8_t* dataBytePtr = byteVec.data();
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0, GetByteSize(), dataBytePtr);
#endif

		RY_CORE_TRACE("Copy Action Finsihed!");
		OnSpecifcationChangeCall();
	}

	void OpenGLIndexBuffer::AddCopyData(Ref<IndexBuffer> ib)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndexBuffer::CopyData, this, fromPoint, toPoint, byteSize, ib));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLIndexBuffer::AddCopyData, ib);
#endif

		Ref<OpenGLIndexBuffer> vbOpenGL = std::static_pointer_cast<OpenGLIndexBuffer>(ib);
		uint32_t readVB_ID = vbOpenGL->GetRenderID();
		RY_CORE_ASSERT(0u != m_Buffer.GetRenderID(), "Not set Buffer ID!");
		RY_CORE_ASSERT(0u != readVB_ID, "Not set Buffer ID!");

		uint32_t countRead = vbOpenGL->GetCount();
		uint32_t byteSizeRead = vbOpenGL->GetByteSize();
		uint32_t oldCountWhrit = GetCount();
		uint32_t oldByteSizeWhrit = GetByteSize();

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, oldByteSizeWhrit, m_Buffer.GetDataPtr());
#else
		std::vector<uint8_t>& byteVec = m_Buffer.GetData();
		uint8_t* dataBytePtr = byteVec.data();
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, oldByteSizeWhrit, dataBytePtr);
#endif

		uint32_t newByteSizeWhrit = GetByteSize();

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		glNamedBufferData(m_Buffer.GetRenderID(), newByteSizeWhrit, m_Buffer.GetDataPtr(), Utils::GetBufferDataUsage(m_Usage));
#else
		glNamedBufferStorage(m_Buffer.GetRenderID(), newByteSizeWhrit, dataBytePtr, Utils::GetFlagsFromFlagTypes(m_Flag));
#endif

		glCopyNamedBufferSubData(readVB_ID, m_Buffer.GetRenderID(), 0u, oldByteSizeWhrit, byteSizeRead);
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, GetByteSize(), m_Buffer.GetDataPtr());
#else
		glGetNamedBufferSubData(m_Buffer.GetRenderID(), 0u, GetByteSize(), dataBytePtr);
#endif



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
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLStorageBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(const void* dataPtr, uint32_t byteSize, BufferType buffertype, BufferFlagGPU flag)
		: m_FlagTypes(flag)
		, m_Target(Utils::GetTarget(buffertype))
		, m_Buffer(Utils::GetTarget(buffertype), reinterpret_cast<const uint8_t*>(dataPtr), byteSize, Utils::GetFlagsFromFlagTypes(flag))
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLStorageBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif

	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(const void* dataPtr, uint32_t byteSize, BufferFlagGPU flag)
		: m_FlagTypes(flag)
		, m_Target(GL_SHADER_STORAGE_BUFFER)
		, m_Buffer(GL_SHADER_STORAGE_BUFFER, reinterpret_cast<const uint8_t*>(dataPtr), byteSize, Utils::GetFlagsFromFlagTypes(flag))
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLStorageBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif

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

#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(std::vector<uint8_t>&& data, const BufferLayout& layout, BufferDataUsage usage)
		: m_Layout(layout)
		, m_Buffer(s_Target, std::move(data), Utils::GetBufferDataUsage(usage))
		, m_ParentVec()
		, m_ChangeOffset(0u)
		, m_ChangeSize(0u)
		, m_Usage(usage)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(std::vector<uint8_t>&& data, const BufferLayout& layout, BufferFlagGPU flag)
		: m_Layout(layout)
		, m_Buffer(s_Target, std::move(data), Utils::GetFlagsFromFlagTypes(flag))
		, m_FlagTypes(flag)
#endif
	{

		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");


#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLUniformBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t byteSize)
		: m_Layout()
		, m_Buffer(s_Target, nullptr, byteSize, Utils::GetBufferDataUsage(BufferDataUsage::DynamicDraw))
		, m_ParentVec()
		, m_ChangeOffset(0u)
		, m_ChangeSize(0u)
		, m_Usage(BufferDataUsage::DynamicDraw)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t byteSize)
		: m_Layout()
		, m_Buffer(s_Target, nullptr,byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)
#endif
	{
		
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLUniformBuffer::InitAsync, this));
			return;
		}
		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize)
		: m_Layout()
		, m_Buffer(OpenGLArrayBuffer(GL_UNIFORM_BUFFER, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetBufferDataUsage(BufferDataUsage::DynamicDraw)))
		, m_ChangeOffset(0u)
		, m_ChangeSize(0u)
		, m_Usage(BufferDataUsage::DynamicDraw)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize)
		: m_Layout()
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)
#endif

	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() < Utils::GetMaxUniforms(), "to large!");
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		
		CheckLayout();
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLUniformBuffer::InitAsync, this));
			return;
		}
		OpenGLUniformBuffer::InitAsync();
#else
#endif
		
		
	}

#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetBufferDataUsage(BufferDataUsage::DynamicDraw))
		, m_ChangeOffset(0u)
		, m_ChangeSize(0u)
		, m_Usage(BufferDataUsage::DynamicDraw)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(BufferFlag::Dynamic))
		, m_FlagTypes(BufferFlag::Dynamic)
#endif
	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLUniformBuffer::InitAsync, this));
			return;
		}

		InitAsync();
#else
#endif
	}

#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferDataUsage usage)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetBufferDataUsage(usage))
		, m_ChangeOffset(0u)
		, m_ChangeSize(0u)
		, m_Usage(usage)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferFlagGPU flag)
		: m_Layout(layout)
		, m_Buffer(s_Target, reinterpret_cast<const uint8_t*>(data), byteSize, Utils::GetFlagsFromFlagTypes(flag))
		, m_FlagTypes(flag)
#endif
	{
		RY_CORE_ASSERT(m_Buffer.GetByteSize() % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLUniformBuffer::InitAsync, this));
			return;
		}
		OpenGLUniformBuffer::InitAsync();
#else
#endif

	}
#ifdef RY_OPENGL_OLD_UNIFORM
	OpenGLUniformBuffer::OpenGLUniformBuffer(const Ref<OpenGLUniformBuffer>& uniformBufferOpenGL)
		: m_Layout(uniformBufferOpenGL->m_Layout)
		, m_Buffer(s_Target, uniformBufferOpenGL->m_Buffer)
		, m_ChangeOffset(uniformBufferOpenGL->m_ChangeOffset)
		, m_ChangeSize(uniformBufferOpenGL->m_ChangeSize)
		, m_Usage(uniformBufferOpenGL->m_Usage)
#else
	OpenGLUniformBuffer::OpenGLUniformBuffer(const Ref<OpenGLUniformBuffer>& uniformBufferOpenGL)
		: m_Layout(uniformBufferOpenGL->m_Layout)
		, m_Buffer(s_Target, uniformBufferOpenGL->m_Buffer)
		, m_FlagTypes(uniformBufferOpenGL->m_FlagTypes)
#endif
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
#ifdef RY_OPENGL_INDRECT_API_SHADER_STORAGE_BUFFER_USE
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

#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndriectBuffer::InitAsync, this));
			return;
		}
		OpenGLIndriectBuffer::InitAsync();
#endif

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
#else

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(uint32_t byteSize)
		: m_Data()
		, m_Layout({
			{ShaderDataType::Uint, "Count"},
			{ShaderDataType::Uint, "InstanceCount"},
			{ShaderDataType::Uint, "FirstIndex"},
			{ShaderDataType::Uint, "BaseVertex"},
			{ShaderDataType::Uint, "BaseInstance"}
		})
		, m_RendererID(0u)
		, m_ByteSize(byteSize)
		, m_Target(GL_DRAW_INDIRECT_BUFFER)
		, m_StrideSize(5u * sizeof(uint32_t))
		, m_Count(byteSize / (5u * sizeof(uint32_t)))
		, m_Usage(BufferDataUsage::DynamicDraw)

	{
		RY_CORE_ASSERT(m_ByteSize >= 16, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(m_ByteSize % m_StrideSize == 0, "OpenGL IndriectBuffer");
		m_Data.resize(byteSize);
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndriectBuffer::InitAsync, this));
			return;
		}

		OpenGLIndriectBuffer::InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_FUNCTION(OpenGLIndriectBuffer::InitAsync);
#endif
	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(uint32_t byteSize, const BufferLayout& layout)
		: m_Data()
		, m_Layout(layout)
		, m_ByteSize(byteSize)
		, m_Target(GL_DRAW_INDIRECT_BUFFER)
		, m_Count(byteSize / layout.GetStride())
		, m_StrideSize(layout.GetStride())
		, m_RendererID(0u)
		, m_Usage(BufferDataUsage::None)
	{
		RY_CORE_ASSERT(m_ByteSize >= 16, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(m_ByteSize % m_StrideSize == 0, "OpenGL IndriectBuffer");
		m_Data.resize(byteSize);

#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndriectBuffer::InitAsync, this));
			return;
		}

		OpenGLIndriectBuffer::InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_FUNCTION(OpenGLIndriectBuffer::InitAsync);
#endif

	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(const void* data, uint32_t byteSize)
		: m_Data()
		, m_Layout({
			{ ShaderDataType::Uint, "Count" },
			{ ShaderDataType::Uint, "InstanceCount" },
			{ ShaderDataType::Uint, "FirstIndex" },
			{ ShaderDataType::Uint, "BaseVertex" },
			{ ShaderDataType::Uint, "BaseInstance" }
		})
		, m_ByteSize(byteSize)
		, m_Target(GL_DRAW_INDIRECT_BUFFER)
		, m_StrideSize(5u * sizeof(uint32_t))
		, m_Count(byteSize / (5u * sizeof(uint32_t)))
		, m_RendererID(0u)
		, m_Usage(BufferDataUsage::DynamicDraw)
	{
		RY_CORE_ASSERT(m_ByteSize >= 16, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		RY_CORE_ASSERT(m_ByteSize % m_StrideSize == 0, "OpenGL IndriectBuffer");
		m_Data.resize(m_ByteSize);
		if (data != nullptr)
			std::memcpy(m_Data.data(), data, byteSize);


#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndriectBuffer::InitAsync, this));
			return;
		}
		OpenGLIndriectBuffer::InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_FUNCTION(OpenGLIndriectBuffer::InitAsync);
#endif

	}

	OpenGLIndriectBuffer::OpenGLIndriectBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout)
		: m_Data()
		, m_Layout(layout)
		, m_ByteSize(byteSize)
		, m_Target(GL_DRAW_INDIRECT_BUFFER)
		, m_StrideSize(layout.GetStride())
		, m_Count(byteSize / layout.GetStride())
		, m_RendererID(0u)
		, m_Usage(BufferDataUsage::DynamicDraw)
	{
		RY_CORE_ASSERT(m_ByteSize >= 16, "OpenGL IndriectBuffer need to be 16 bytes (or more)");
		m_Data.resize(m_ByteSize);
		if (data != nullptr)
			std::memcpy(m_Data.data(), data, byteSize);

#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLIndriectBuffer::InitAsync, this));
			return;
		}
		OpenGLIndriectBuffer::InitAsync();
#else
		RY_EXE_ON_MAIN_THREAD_FUNCTION(OpenGLIndriectBuffer::InitAsync);
#endif

	}

	OpenGLIndriectBuffer::~OpenGLIndriectBuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
		if (m_RendererID)
		{
			Utils::RemoveMemoryToTracker(m_ByteSize);
			RY_GRAFIC_DELETE(m_RendererID, OpenGLIndriectBuffer);
			glDeleteBuffers(1u, &m_RendererID);
			m_RendererID = 0u;
		}
	}

	bool OpenGLIndriectBuffer::IsTransferd()
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	void OpenGLIndriectBuffer::Bind() const
	{
		RY_CORE_ASSERT(m_Target != 0 && m_RendererID != 0);
		OpenGLRenderCommand::BindBuffer(m_Target, m_RendererID);
	}

	void OpenGLIndriectBuffer::UnBind() const
	{
		RY_CORE_ASSERT(m_Target != 0 && m_RendererID != 0);
		OpenGLRenderCommand::BindBuffer(m_Target, 0u);
	}

	void OpenGLIndriectBuffer::InitAsync()
	{
		if(m_RendererID == 0)
		{
			glCreateBuffers(1, &m_RendererID);
			RY_GRAFIC_CREATE(m_RendererID, OpenGLIndriectBuffer);
		}
		else
		{
			Utils::AddMemoryToTracker(m_ByteSize);
		}
		glNamedBufferStorage(m_RendererID, m_ByteSize, m_Data.data(), GL_DYNAMIC_STORAGE_BIT);
	}

	void OpenGLIndriectBuffer::SetData(const void* data, uint32_t byteSize)
	{
		RY_CORE_ASSERT(m_ByteSize == byteSize, "not korrekt byteSize");
		if (data)
			std::memcpy(m_Data.data(), data, m_ByteSize);
		glNamedBufferSubData(m_RendererID, 0, m_ByteSize, m_Data.data());
	}

	

	void OpenGLIndriectBuffer::ResizeBuffer(uint32_t byteSize)
	{
		if (m_ByteSize != byteSize)
		{
			m_ByteSize = byteSize;
			m_Data.resize(byteSize);
		
			m_StrideSize = m_Layout.GetStride() != 0 ? m_Layout.GetStride() : 5u * sizeof(uint32_t);
			m_Count = m_ByteSize / m_StrideSize;
		}
		glNamedBufferStorage(m_RendererID, m_ByteSize, m_Data.data(), GL_DYNAMIC_STORAGE_BIT);
	}

	void OpenGLIndriectBuffer::ResizeBuffer(const void* data, uint32_t byteSize)
	{
		if (m_ByteSize != byteSize)
		{
			m_ByteSize = byteSize;
			m_Data.resize(byteSize);

			m_StrideSize = m_Layout.GetStride() != 0 ? m_Layout.GetStride() : 5u * sizeof(uint32_t);
			m_Count = m_ByteSize / m_StrideSize;
		}
		if (nullptr != data)
			std::memcpy(m_Data.data(), data, m_ByteSize);
		ResizeBuffer(m_ByteSize);
	}
#endif

#pragma endregion
	
}