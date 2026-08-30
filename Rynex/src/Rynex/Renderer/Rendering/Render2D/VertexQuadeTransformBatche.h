#pragma once
#include <Rynex/Renderer/Rendering/Render2D/Batching.h>
#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

	template<typename T, typename N, int MaxTex, int VerticiesQuadMax = 4, int UpladeUpdateSize = 50>
	class VertexQuadeTransformBatche : public Batching<T>
	{
	public:
		VertexQuadeTransformBatche()
			: Batching<T>()
			, m_MaxQuad(0u)
			, m_VAA(nullptr)
			, m_VAB(nullptr)
			, m_IndexBuffer(nullptr)
			, m_Shader(nullptr)
			, m_TransformStroageBuffer(nullptr)
			, m_Samplers()
			, m_DefaultVertex()
			, m_NewData(false)
			, m_CameraSlot(-1)
			, m_DisplaySlot(-1)
		{
			
		};

		~VertexQuadeTransformBatche()
		{
			Destroy();
		};

		void SetCameraSlot(uint32_t slot)
		{
			m_CameraSlot = slot;
		}
		void SetDisplaySlot(uint32_t slot)
		{
			m_DisplaySlot = slot;
		}

		virtual void Create(const std::filesystem::path& path, uint32_t quadeCount, const N* defaultVertex, const BufferLayout& layout, const Ref<Texture>& defaultTex, const std::string& textureName)
		{
			N* defaultVertexPtr = (N*)defaultVertex;
			N* defaultVertexBegin = defaultVertexPtr; 
			N* defaultVertexEnde = defaultVertexPtr + VerticiesQuadMax;
			for (int i = 0; i < VerticiesQuadMax && defaultVertexPtr != defaultVertexEnde; i++, defaultVertexPtr++)
			{
				uint32_t indexD = (i % VerticiesQuadMax);
				m_DefaultVertex[indexD] = *defaultVertexPtr;
			}
			m_NewData = false;
			m_MaxQuad = quadeCount;
			Batching<T>::Create(m_MaxQuad);
			m_Shader = AssetManager::GetAsset<Shader>(path);

			for (uint32_t i = 0; i < MaxTex; i++)
				m_Samplers[i] = i;

			m_Shader->Bind();
			m_Shader->SetIntArray(textureName, m_Samplers, MaxTex);
			m_TextureSlots[0] = defaultTex;
			m_Shader->UnBind();

			m_TextureSlotsIndex = 1u;
			m_VAA = VertexArray::Create();
			Vericies(m_MaxQuad, layout);
			TransformStroage(m_MaxQuad);
			Inidicies(m_MaxQuad);

		};

		virtual void Destroy() override
		{
			Batching<T>::Destroy();

			RY_DESTROY_REF(m_TransformStroageBuffer);
			RY_DESTROY_REF(m_IndexBuffer);
			RY_DESTROY_REF(m_VAB);
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			RY_DESTROY_REF(m_DrawList);
#else
			m_DrawList.Clear();
#endif
			RY_DESTROY_REF(m_Shader);
			RY_DESTROY_REF(m_VAA);

			m_MaxQuad = 0u;

			for (Weak<Texture>& texW : m_TextureSlots)
			{
				texW.reset();
			}

		};

		virtual bool Emplace_Back_Push(T& element)
		{
			m_NewData = true;
			return Batching<T>::Emplace_Back_Push(element);
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
				textureIndex = (float)m_TextureSlotsIndex;
				m_TextureSlots[m_TextureSlotsIndex] = texture;
				m_TextureSlotsIndex++;
				m_NewData = true;
			}

			return textureIndex;
		};



		virtual void Bind(const std::string& textureName, uint32_t slot)
		{
			if (m_NewData)
				FlushData();
			
			m_Shader->Bind();
			m_Shader->SetIntArray(textureName, m_Samplers, MaxTex);
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					tex->Bind(i);
			}
			m_TransformStroageBuffer->Bind(slot);
		};


		bool GenartateShaderDrawList(const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
		{
			uint32_t count = 0u;
			if (m_NewData)
				count = FlushData();

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			if (0u == count)
			{
				RY_DESTROY_REF(m_DrawList);
				return;
			}

			if (nullptr == m_DrawList)
			{
				m_DrawList = CreateRef<ShaderDrawList>();
				m_DrawList->shaderProgramm = m_Shader;
				m_DrawList->vao = m_VAA;
				m_DrawList->renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;

				StorageBindArray& storageBindArray = m_DrawList->GetBindeStorage();
				storageBindArray[1] = m_TransformStroageBuffer;

			}
			if (!target.HasDrawPass(m_DrawList))
			{
				target.AddDrawPass(m_DrawList);
			}
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
			if (0u == count)
			{
				m_DrawList.Clear();
				return false;
			}

			if (nullptr == m_DrawList.shaderProgramm)
			{
				m_DrawList = CreateShaderDrawResource();
				m_DrawList.shaderProgramm = m_Shader;
				m_DrawList.vao = m_VAA;
				m_DrawList.renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;
				m_DrawList.drawElement.Count = count;
				m_DrawList.drawElement.InstancesCount = 1u;
				m_DrawList.drawElement.BaseVertex = 0u;
				StorageBindArray& storageBindArray = m_DrawList.GetBindeStorage();
				storageBindArray[1] = m_TransformStroageBuffer;

			}

			if (m_NewData)
				FlushData();

			UniformBindArray& uniformBindArray = m_DrawList.GetBindeUniform();

			if (-1 != m_CameraSlot)
				uniformBindArray[m_CameraSlot] = camerbuffer;
			if (-1 != m_DisplaySlot)
				uniformBindArray[m_DisplaySlot] = displaybuffer;
			TextureBindArray& textureBindArray = m_DrawList.GetBindeTextures();
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					textureBindArray[i] = tex;
			}
#endif

			return true;
		}

		void AddShadeDrawListToRenderTarget(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
		{
#if 0
			uint32_t count = 0u;
			if (m_NewData)
				count = FlushData();

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
			if (0u == count)
			{
				RY_DESTROY_REF(m_DrawList);
				return;
			}

			if (nullptr == m_DrawList)
			{
				m_DrawList = CreateRef<ShaderDrawList>();
				m_DrawList->shaderProgramm = m_Shader;
				m_DrawList->vao = m_VAA;
				m_DrawList->renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;

				StorageBindArray& storageBindArray = m_DrawList->GetBindeStorage();
				storageBindArray[1] = m_TransformStroageBuffer;

			}
			if (!target.HasDrawPass(m_DrawList))
			{
				target.AddDrawPass(m_DrawList);
			}
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
			if (0u == count)
			{
				m_DrawList.Clear();
				return;
			}

			if (nullptr == m_DrawList.shaderProgramm)
			{
				m_DrawList = CreateShaderDrawResource();
				m_DrawList.shaderProgramm = m_Shader;
				m_DrawList.vao = m_VAA;
				m_DrawList.renderMode = RenderMode::A_Buffer | RenderMode::CallFace_None | RenderMode::Death_Buffer;
				m_DrawList.drawElement.Count = count;
				m_DrawList.drawElement.InstancesCount = 1u;
				m_DrawList.drawElement.BaseVertex = 0u;
				StorageBindArray& storageBindArray = m_DrawList.GetBindeStorage();
				storageBindArray[1] = m_TransformStroageBuffer;

			}

			target.AddDrawPass(m_DrawList);


			if (m_NewData)
				FlushData();

			UniformBindArray& uniformBindArray = m_DrawList.GetBindeUniform();

			if (-1 != m_CameraSlot)
				uniformBindArray[m_CameraSlot] = camerbuffer;
			if (-1 != m_DisplaySlot)
				uniformBindArray[m_DisplaySlot] = displaybuffer;
			TextureBindArray& textureBindArray = m_DrawList.GetBindeTextures();
			for (uint32_t i = 0; i < m_TextureSlotsIndex; i++)
			{
				if (Ref<Texture> tex = m_TextureSlots[i].lock())
					textureBindArray[i] = tex;
			}
#endif
#else
			if(GenartateShaderDrawList(camerbuffer, displaybuffer))
				target.AddDrawPass(m_DrawList);
#endif
		}

		const ShaderDrawResource& GetShaderDrawList() const { return m_DrawList; }



		virtual void UnBind(uint32_t slot)
		{
			m_Shader->UnBind();
			m_VAA->UnBind();
			m_TransformStroageBuffer->UnBind(slot);
		};

		void RestTextur()
		{
			m_TextureSlotsIndex = 1;
		}

		const Ref<VertexArray>& GetVertexArray() { return m_VAA; }
		const Ref<StorageBuffer>& GetOffsetBuffer() { return m_TransformStroageBuffer; }
		const Ref<Shader>& GetShader() { return m_Shader; }

		void SetVertexArray(const Ref<VertexArray>& va) { m_VAA = va; }
		void SetStorageBuffer(const Ref<StorageBuffer>& sb) { m_TransformStroageBuffer = sb; }
		void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }

		const int32_t* GetSampler() { return m_Samplers; }
		const uint32_t GetMaxSampler() { return MaxTex; }
		const uint32_t GetMaxQuade() { return m_MaxQuad; }


		uint32_t FlushData()
		{
			
			uint32_t countTransfrom = this->Size();
			if (countTransfrom == 0u)
				return 0u;
			uint32_t maxTransfromCount = m_MaxQuad;
			RY_CORE_ASSERT(countTransfrom % 1u == 0u, "not Quade Format count: {}", countTransfrom);
			if (countTransfrom > maxTransfromCount)
			{
				uint32_t countQuad = countTransfrom / 1u;
				m_MaxQuad = countQuad;
				Inidicies(m_MaxQuad);
				Vericies(m_MaxQuad, m_VAB->GetLayout());
				TransformStroage(m_MaxQuad);
			
				countTransfrom = this->Size();
			}
			uint32_t transformSize = sizeof(T);
			uint32_t sizeByte = countTransfrom * transformSize;
			T* data = this->Data();
			m_TransformStroageBuffer->SetData(data, sizeByte);
			uint32_t quadeCount = countTransfrom;
			uint32_t indiciesCount = quadeCount * 6u;
			m_NewData = false;
			return indiciesCount;
		};
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
				uint32_t x = i % 6;

				*quadePtr = squareIndicesSequenc[x];
				squareIndicesSequenc[x] += 4;
				quadePtr++;
			}
			if (m_IndexBuffer == nullptr)
			{
				m_IndexBuffer = IndexBuffer::Create(quadBegin, indeciesCount);
			}
			else
			{
				m_IndexBuffer->SetData(quadBegin, indeciesCount);
			}

			m_VAA->SetIndexBuffer(m_IndexBuffer);
			delete[] quadIndecies;
		};

		void Vericies(uint32_t countQuades, const BufferLayout& layout)
		{
			uint32_t verticesCount = countQuades * VerticiesQuadMax;
			

			N* quadVertices = new N[verticesCount];
			const N* quadBegin = quadVertices;
			const N* quadEnde = quadVertices + verticesCount;
			N* quadePtr = quadVertices;


			for (uint32_t i = 0u; i < verticesCount && quadePtr != quadEnde; i++)
			{
				uint32_t indexD = (i % VerticiesQuadMax);
				*quadePtr = m_DefaultVertex[indexD];
				quadePtr++;
			}
			uint32_t byteSizeVerices = sizeof(N);
			
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_VAB = VertexBuffer::Create(quadVertices, verticesCount * byteSizeVerices, BufferDataUsage::StaticDraw);
#else
			m_VAB = VertexBuffer::Create(quadVertices, verticesCount * byteSizeVerices, BufferFlag::None);
#endif
			m_VAB->SetLayout(layout);


			m_VAA->ClearVertexBuffers();
			m_VAA->AddVertexBuffer(m_VAB);
			delete[] quadVertices;
		};

		void TransformStroage(uint32_t countQuades)
		{
			uint32_t transfromCount = countQuades;
			
			m_TransformStroageBuffer = StorageBuffer::Create(nullptr, transfromCount * sizeof(T), BufferType::ShaderStorage, BufferFlag::None);
			
			Batching<T>::PushSize(1u);
			Batching<T>::Reserve(transfromCount);

		};

		

	

	private:
		ShaderDrawResource m_DrawList;
		Ref<VertexArray> m_VAA;
		Ref<VertexBuffer> m_VAB;
		Ref<StorageBuffer> m_TransformStroageBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Shader> m_Shader;
		uint32_t m_MaxQuad;
		N m_DefaultVertex[VerticiesQuadMax];
		std::array<Weak<Texture>, MaxTex> m_TextureSlots;
		int32_t m_Samplers[MaxTex];
		uint32_t m_TextureSlotsIndex;

		uint32_t m_CameraSlot;
		uint32_t m_DisplaySlot;
		bool m_NewData;


	};

}
