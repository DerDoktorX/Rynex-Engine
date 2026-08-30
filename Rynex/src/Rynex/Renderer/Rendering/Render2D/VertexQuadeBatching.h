#pragma once
#include <Rynex/Renderer/Rendering/Render2D/Batching.h>
#include <Rynex/Asset/Base/AssetManager.h>



namespace Rynex {
	class VertexArray;
	class Texture;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;
	class BufferLayout;
	enum class BufferDataUsage : uint8_t;
	
	template<typename T, int MaxTex>
	class VertexQuadeBatching : public Batching<T>
	{
	public:
		VertexQuadeBatching()
			: Batching<T>()
			, m_MaxQuad(0u)
			, m_VAA(nullptr)
			, m_VAB(nullptr)
			, m_IndexBuffer(nullptr)
			, m_Shader(nullptr)
			, m_Samplers()
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			, m_DrawList(nullptr)
#else
			, m_DrawList()
#endif
			, m_CameraSlot(-1)
			, m_DisplaySlot(-1)
		{
		};

		~VertexQuadeBatching()
		{
			Destroy();
		};

		virtual void Create(const std::filesystem::path& path, uint32_t quadeCount, const BufferLayout& layout, const Ref<Texture>& defaultTex)
		{
			
			m_MaxQuad = quadeCount;
			Batching<T>::Create(m_MaxQuad);
			m_Shader = AssetManager::GetAsset<Shader>(path);
		
			for (uint32_t i = 0; i < MaxTex; i++)
				m_Samplers[i] = i;
			m_TextureSlots[0] = defaultTex;
			
		
			m_VAA = VertexArray::Create();
			Vericies(m_MaxQuad, layout);
			Inidicies(m_MaxQuad);
			
		};

		void SetCameraSlot(uint32_t slot)
		{
			m_CameraSlot = slot;
		}
		void SetDisplaySlot(uint32_t slot)
		{
			m_DisplaySlot = slot;
		}

		virtual void Create(const std::filesystem::path& path, uint32_t quadeCount, const BufferLayout& layout, const Ref<Texture>& defaultTex, const std::string& textureName)
		{
			
			m_MaxQuad = quadeCount;
			Batching<T>::Create(m_MaxQuad);
			m_Shader = AssetManager::GetAsset<Shader>(path);

			for (uint32_t i = 0; i < MaxTex; i++)
				m_Samplers[i] = i;

			m_Shader->Bind();
			m_Shader->SetIntArray(textureName, m_Samplers, MaxTex);
			m_TextureSlots[0] = defaultTex;
			m_Shader->UnBind();
			

			m_VAA = VertexArray::Create();
			Vericies(m_MaxQuad, layout);
			Inidicies(m_MaxQuad);

		};

		virtual void Destroy() override
		{
			Batching<T>::Destroy();
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			RY_DESTROY_REF(m_DrawList);
#else
			m_DrawList.Clear();
#endif
			RY_DESTROY_REF(m_VAA);
			RY_DESTROY_REF(m_VAB);
			RY_DESTROY_REF(m_IndexBuffer);
			RY_DESTROY_REF(m_Shader);
		

			m_MaxQuad = 0u;

			for (Weak<Texture>& texW : m_TextureSlots)
			{
				texW.reset();
			}
			
		};

	

		int AddTexture(const Ref<Texture>& texture)
		{
			if (m_TextureSlotsIndex >= MaxTex)
				return 0;

			int textureIndex = 0;
			for (uint32_t i = 1; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
				{
					if (*tex.get() == *texture.get())
					{
						textureIndex = (int)i;
						return textureIndex;
					}
				}
			}

			if (textureIndex == 0)
			{
				textureIndex = static_cast<int>(m_TextureSlotsIndex);
				m_TextureSlots[textureIndex] = texture;
				m_TextureSlotsIndex++;
			}

			return textureIndex;
		};



		virtual uint32_t Bind(const std::string& textureName)
		{
			uint32_t count = FlushData();
			if (count == 0)
				return 0;
			
			m_Shader->Bind();
			m_Shader->SetIntArray(textureName, m_Samplers, MaxTex);
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					tex->Bind(i);
			}
			return count;
		};

		void AddShadeDrawListToRenderTarget(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
		{
			uint32_t count = 0u;
			count = FlushData();

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			if (count == 0)
			{
				RY_DESTROY_REF(m_DrawList);
				return;
			}
			if (nullptr == m_DrawList)
			{
				m_DrawList = CreateShaderDrawResource();
				m_DrawList->shaderProgramm = m_Shader;
				m_DrawList->vao = m_VAA;
				m_DrawList->renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;
				
			}
			if (!target.HasDrawPass(m_DrawList))
			{
				target.AddDrawPass(m_DrawList);
			}
			m_DrawList->indicesCount = count;

			UniformBindArray& uniformBindArray = m_DrawList->GetBindeUniform();
			if (-1 != m_CameraSlot)
				uniformBindArray[m_CameraSlot] = camerbuffer;
			if (-1 != m_DisplaySlot)
				uniformBindArray[m_DisplaySlot] = displaybuffer;

			TextureBindArray& textureBindArray = m_DrawList->GetBindeTextures();
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					textureBindArray[i] = tex;
			}
#else
			if (count == 0)
			{
				m_DrawList.Clear();
				return;
			}
			if (nullptr == m_DrawList.shaderProgramm)
			{
				m_DrawList.shaderProgramm = m_Shader;
				m_DrawList.vao = m_VAA;
				m_DrawList.renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;

			}

			UniformBindArray& uniformBindArray = m_DrawList.GetBindeUniform();
			if(-1 != m_CameraSlot)
				uniformBindArray[m_CameraSlot] = camerbuffer;
			if (-1 != m_DisplaySlot)
				uniformBindArray[m_DisplaySlot] = displaybuffer;

			TextureBindArray& textureBindArray = m_DrawList.GetBindeTextures();
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					textureBindArray[i] = tex;
			}

			target.AddDrawPass(m_DrawList);


			
#endif
		}

		virtual void UnBind()
		{
			Batching<T>::Clear();
			m_Shader->UnBind();
			m_VAA->UnBind();
			
		};

		void RestTextur()
		{
			m_TextureSlotsIndex = 1;
		}

		const Ref<VertexArray>& GetVertexArray() { return m_VAA; }
		const Ref<Shader>& GetShader() { return m_Shader; }
		const int32_t* GetSampler() { return m_Samplers; }
		const uint32_t GetMaxSampler() { return MaxTex; }
	private:

		void Inidicies(uint32_t countQuades)
		{
			uint32_t indeciesCount = countQuades * 6;
			RY_CORE_ASSERT(indeciesCount % 6 == 0, "Not in Quade Fomat! count {}", indeciesCount);

			uint32_t* quadIndecies = new uint32_t[indeciesCount];
			const uint32_t* quadBegin = quadIndecies;
			const uint32_t* quadEnde = quadIndecies + indeciesCount;
			uint32_t* quadePtr = quadIndecies;


			uint32_t squareIndicesSequenc[] = {
				0u, 1u, 2u,
				2u, 3u, 0u
			};

			uint32_t offsetIndex[] = {
				0u, 0u, 0u,
				0u, 0u, 4u
			};

			uint32_t offset = 0u;
			for (uint32_t i = 0u; i < indeciesCount && quadePtr != quadEnde; i++)
			{
#if 0
				*quadePtr = offset + 0;
				quadePtr++;
				*quadePtr = offset + 1;
				quadePtr++;
				*quadePtr = offset + 2;
				quadePtr++;

				*quadePtr = offset + 2;
				quadePtr++;
				*quadePtr = offset + 3;
				quadePtr++;
				*quadePtr = offset + 0;
				quadePtr++;
#else
				uint32_t x = i % 6;
				*quadePtr = squareIndicesSequenc[x];
				squareIndicesSequenc[x] += 4;
				quadePtr++;
#endif

			}
			if (m_IndexBuffer == nullptr)
				m_IndexBuffer = IndexBuffer::Create(quadBegin, indeciesCount);
			else
			{
				m_IndexBuffer->SetData(quadBegin, indeciesCount);
			}

			m_VAA->SetIndexBuffer(m_IndexBuffer);
			delete[] quadIndecies;
		};

		void Vericies(uint32_t countQuades, const BufferLayout& layout)
		{
			uint32_t verticesCount = countQuades * 4u;
			RY_CORE_ASSERT(verticesCount % 4u == 0u, "not Quade Format count: {}", verticesCount);
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_VAB = VertexBuffer::Create(nullptr, verticesCount * sizeof(T), BufferDataUsage::DynamicDraw);
#else
			m_VAB = VertexBuffer::Create(nullptr, verticesCount * sizeof(T), BufferFlag::Dynamic);
#endif
			m_VAB->SetLayout(layout);
			Batching<T>::PushSize(4u);
			Batching<T>::Reserve(verticesCount);
			m_VAA->ClearVertexBuffers();
			m_VAA->AddVertexBuffer(m_VAB);
		};
		
		uint32_t FlushData()
		{
			
			uint32_t countVertices = this->Size();
			uint32_t quadeCount = countVertices / 4u;
			uint32_t indiciesCount = quadeCount * 6u;
			if ((this->HasChanaged()))
				return indiciesCount;

			uint32_t maxVerticesCount = m_MaxQuad * 4;
			RY_CORE_ASSERT(countVertices % 4u == 0u, "not Quade Format count: {}", countVertices);
			if (maxVerticesCount < countVertices)
			{
				uint32_t countQuad = countVertices / 4u;
				m_MaxQuad = countQuad;
				Inidicies(m_MaxQuad);
				Vericies(m_MaxQuad, m_VAB->GetLayout());
				
				countVertices = this->Size();
				RY_CORE_ASSERT(countVertices % 4u == 0u, "not Quade Format count: {}", countVertices);
			}
			T* data = this->Data();
			uint32_t verexSize = sizeof(T);
			uint32_t sizeByte = countVertices * verexSize;
			if(0 < sizeByte)
				m_VAB->SetData(data, sizeByte);

			return indiciesCount;
		};
		
	private:
		Ref<VertexArray> m_VAA;
		Ref<VertexBuffer> m_VAB;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Shader> m_Shader;
		ShaderDrawResource m_DrawList;

		std::array<Weak<Texture>, MaxTex> m_TextureSlots;
		uint32_t m_MaxQuad;

		int32_t m_Samplers[MaxTex];
		uint32_t m_TextureSlotsIndex;
		uint32_t m_DisplaySlot;
		uint32_t m_CameraSlot;
	};
}
