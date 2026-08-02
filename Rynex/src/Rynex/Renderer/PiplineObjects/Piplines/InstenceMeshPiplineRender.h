#pragma once
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>



#ifndef RY_RENDER_PIPLINES_CHACH_2
	#define RY_RENDER_PIPLINES_CHACH_1
#endif // RY_RENDER_PIPLINES_CHACH_2

#define RY_RENDER_PIPLINES_CHACH RY_RENDER_PIPLINES_CHACH_1 || RY_RENDER_PIPLINES_CHACH_2


#define RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
namespace Rynex {



	class InstenceMeshPiplineRenderBase : public PiplineRenderBase
	{
	protected:
		enum
		{
			State_MaxEntityRender = 100000,
			State_MaxNotUpdateDraws = 100,


			UniformBinding_MainCamer = 1,
		};

	public:
		InstenceMeshPiplineRenderBase();
		InstenceMeshPiplineRenderBase(const InstenceMeshPiplineRenderBase&) = default;
		~InstenceMeshPiplineRenderBase();

		virtual void SubmitRenderTargetResurces(ViewPassStorage & viewPass) = 0;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>&texture) = 0;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>&buffer) = 0;

		virtual const BufferLayout& GetExpetedOutput() const override;
		virtual void SetExpetedOutput(const BufferLayout& output) override;

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer) override;
		virtual void SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer) override;

		virtual void SubmitRenderObject(const glm::mat4 & model, uint32_t & storeIndex, int entityID) = 0;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) = 0;

		

		virtual void SetDataMangingFlags(PiplineManagingState flags) override;
		virtual PiplineManagingState GetDataMangingFlags() const override { return m_ManagingMode; }

		virtual void SetRenderFlags(int flags) override { m_RenderMode = flags; }
		virtual int GetRenderFlags() const override { return m_RenderMode; };

		virtual void DrawNow() override;
		virtual void DrawNow(int flags) = 0;

		virtual uint32_t GetCurentEntityRender() const override { return m_InstencCount; }
		virtual uint32_t GetMaxEntityRender() const override { return State_MaxEntityRender; }

		virtual uint32_t GetFrameCountNotUpdate() const override { return m_DrawsAfterLastUpdate; }
		virtual uint32_t GetMaxFrameCountNotUpdate() const override { return State_MaxNotUpdateDraws; }

		virtual bool Empty() const = 0;
		virtual bool IsFull()const override = 0;

		virtual void Clear() override;
		virtual void ClearRenderObjects() override;
		
		virtual uint64_t GetShaderNumber() const override { return reinterpret_cast<uint64_t>(m_Shader.get()); }
		virtual uint64_t GetVertexBufferNumber() const = 0;
		virtual uint64_t GetIndexBufferNumber() const = 0;
		virtual uint64_t GetIndirectBufferNumber() const = 0;
		virtual uint64_t GetTextureNumber() const = 0;
		virtual uint64_t GetUniformBufferNumber() const = 0;

		virtual uint64_t GetStorageBufferNumber() const = 0;
		virtual Ref<PiplineRenderBase> Copy() const = 0;

	protected:
		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if (nullptr == b)
				result = result | notVaildState;
		}

		virtual bool IsExpextedOutPut(const Ref<Shader>& shader) const;

		virtual void BeforeDrawCall() = 0;
		virtual void BindResources();
		virtual void UnbindResources();
	protected:
		BufferLayout		m_OutPut;
		SingleMeshObject	m_SingleMeshObject;
		uint32_t			m_InstencCount;
		uint32_t			m_DrawsAfterLastUpdate;
		
		
		PiplineManagingState m_ManagingMode;

		Ref<Shader>			m_Shader;
		Ref<UniformBuffer>	m_CameraBuffer;
		
		Ref<VertexBuffer>	m_ModelBufferVAO;
		Ref<VertexArray>	m_VertexArray;
		int					m_RenderMode;
	};


	class InstenceMeshPiplineRenderShade : public InstenceMeshPiplineRenderBase
	{
	private:
		enum
		{
			TextureBinding_Abldoe = 0,
			TextureBinding_Shadow = 1,


			UniformBinding_LigthCamera = 2,
			UniformBinding_Materiel = 3,

		};
		struct RenderObject
		{
			glm::mat4 ModelMatrix;
			glm::mat4 NormalMatrix;
			int EnitityID;
			int Empty[3];

			RenderObject()
				: EnitityID(-1)
				, ModelMatrix(glm::mat4(-1.0f))
				, NormalMatrix(glm::mat4(-1.0f))
				, Empty{ -10, -11, -12 }
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> ObjectVec;
			std::vector<int> EntityIDVec;
			bool Update;

			RenderObjectState()
				: ObjectVec()
				, EntityIDVec()
				, Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instenIndex)
			{
				if (instenIndex >= EntityIDVec.size() || instenIndex == static_cast<uint32_t>(-1))
				{
					instenIndex = EntityIDVec.size();
					ObjectVec.emplace_back(RenderObject());
					EntityIDVec.emplace_back<int>(-1);
				}
			}

			void SetObject(uint32_t& instenIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);

				if (object.EnitityID != entityID || modelMatrix != object.ModelMatrix)
				{
					glm::mat4 modelInverse = glm::inverse(modelMatrix);
					glm::mat4 modelTranspose = glm::transpose(modelInverse);
					Set(modelMatrix, entityID, object, entity);
				}

			}

			void SetObjectForce(uint32_t & instenIndex, const glm::mat4 & modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);

				Set(modelMatrix, entityID, object, entity);

			}

			inline void Set(const glm::mat4 & modelMatrix, const int entityID, RenderObject & elementObject, int& elementEntityID)
			{
				glm::mat4 modelInverse = glm::inverse(modelMatrix);
				glm::mat4 modelTranspose = glm::transpose(modelInverse);


				elementEntityID = entityID;
				elementObject.ModelMatrix = modelMatrix;
				elementObject.EnitityID = entityID;

				elementObject.NormalMatrix = modelTranspose;
				Update = true;

			}

			void Updated()
			{

				Update = false;
			}

			bool NeedUpdate() const
			{
				return Update;
			}

			bool operator==(const RenderObjectState & renderObject) const
			{

				bool resultEntt = this->EntityIDVec == renderObject.EntityIDVec;
				return resultEntt;
			}


				};
	public:
		InstenceMeshPiplineRenderShade();
		InstenceMeshPiplineRenderShade(const InstenceMeshPiplineRenderShade&) = default;
		~InstenceMeshPiplineRenderShade();

		virtual void SubmitRenderTargetResurces(ViewPassStorage & viewPass) override;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>&texture) override;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>&buffer)override;


		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject & singleMesh, const Ref<Shader>&shader
			, const glm::mat4 & model, uint32_t & storeIndex, int entityID) override;

		virtual void DrawNow(int flags) override;

		virtual bool Empty() const override;
		virtual bool IsFull()const override;

		virtual void Clear();

		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;
		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstenceMeshPiplineRenderShade>(*this); }

	private:
		int CheckSubmiteMeshObject(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);
		bool SubmiteResources(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);

		

		virtual void BeforeDrawCall() override;
		virtual void BindResources();
		virtual void UnbindResources();
	private:

		Ref<UniformBuffer>	m_LigthBuffer;
		Ref<UniformBuffer>	m_MaterielBuffer;
		
		

		Ref<Texture>		m_AlbdeoTex;
		Ref<Texture>		m_ShadowTex;
		
		RenderObjectState m_RenderObject;


	};

	class InstenceMeshPiplineRenderDepth : public InstenceMeshPiplineRenderBase
	{
	private:
		struct RenderObject
		{
			glm::mat4 ModelMatrix;

			RenderObject()
				: ModelMatrix(glm::mat4(-1.0f))
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> ObjectVec;
			std::vector<int> EntityIDVec;
			bool Update;

			RenderObjectState()
				: ObjectVec()
				, EntityIDVec()
				, Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instenIndex)
			{
				if (instenIndex >= EntityIDVec.size() || instenIndex == static_cast<uint32_t>(-1))
				{
					instenIndex = EntityIDVec.size();
					ObjectVec.emplace_back(RenderObject());
					EntityIDVec.emplace_back<int>(-1);
				}
			}

			void SetObject(uint32_t& instenIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);
				if (entityID != entity || modelMatrix != object.ModelMatrix)
				{
					Set(modelMatrix, entityID, object, entity);
				}
			}

			void SetObjectForce(uint32_t& instenIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);

				Set(modelMatrix, entityID, object, entity);

			}

			inline void Set(const glm::mat4& modelMatrix, const int entityID, RenderObject& elementObject, int& elementEntityID)
			{
				elementEntityID = entityID;
				elementObject.ModelMatrix = modelMatrix;
				Update = true;
			}

			void Updated()
			{
				Update = false;
			}

			bool NeedUpdate() const
			{
				return Update;
			}




		};
	public:
		InstenceMeshPiplineRenderDepth();
		InstenceMeshPiplineRenderDepth(const InstenceMeshPiplineRenderDepth&) = default;
		~InstenceMeshPiplineRenderDepth();

		virtual void SubmitRenderTargetResurces(ViewPassStorage& viewPass) override;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture) override;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)override;


		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) override;

		virtual void DrawNow(int flags) override;

		virtual bool Empty() const override;
		virtual bool IsFull() const override;

		virtual void Clear();

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer) override;
		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstenceMeshPiplineRenderDepth>(*this); }

	private:
		int CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		bool SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);


		virtual void BeforeDrawCall() override;
		virtual void BindResources();
		virtual void UnbindResources();
	private:
		RenderObjectState m_RenderObject;
	};


	class InstenceMeshPiplineRenderShape : public InstenceMeshPiplineRenderBase
	{
	private:
		enum
		{
			TextureBinding_Abldoe = 0,
		};
		struct RenderObject
		{
			glm::mat4 ModelMatrix;


			RenderObject()
				: ModelMatrix(glm::mat4(-1.0f))
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> ObjectVec;
			std::vector<int> EntityIDVec;
			bool Update;

			RenderObjectState()
				: ObjectVec()
				, EntityIDVec()
				, Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instenIndex)
			{
				if (instenIndex < EntityIDVec.size() && instenIndex != static_cast<uint32_t>(-1))
					return;

				instenIndex = EntityIDVec.size();
				ObjectVec.emplace_back(RenderObject());
				EntityIDVec.emplace_back<int>(-1);
				
			}

			void SetObject(uint32_t& instenIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);
				if (entityID != entity || modelMatrix != object.ModelMatrix)
				{
					Set(modelMatrix, entityID, object, entity);
				}
			}

			void SetObjectForce(uint32_t& instenIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instenIndex);
				int& entity = EntityIDVec.at(instenIndex);
				RenderObject& object = ObjectVec.at(instenIndex);

				Set(modelMatrix, entityID, object,  entity);
			}

			inline void Set(const glm::mat4& modelMatrix, const int entityID, RenderObject& elementObject, int& elementEntityID)
			{
				elementEntityID = entityID;
				elementObject.ModelMatrix = modelMatrix;
				Update = true;
			}

			void Updated()
			{
				Update = false;
			}

			bool NeedUpdate() const
			{
				return Update;
			}




		};
	public:
		InstenceMeshPiplineRenderShape();
		InstenceMeshPiplineRenderShape(const InstenceMeshPiplineRenderShape&) = default;

		~InstenceMeshPiplineRenderShape();

		virtual void SubmitRenderTargetResurces(ViewPassStorage& viewPass) override;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture) override;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)override;

		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) override;

		virtual void DrawNow(int flags) override;


		virtual bool Empty() const override;
		virtual bool IsFull( )const override;

		virtual void Clear();


		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstenceMeshPiplineRenderShape>(*this); }
	private:
		int CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		bool SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);

		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if (nullptr == b)
				result = result | notVaildState;
		}

		virtual void BeforeDrawCall() override;
		virtual void BindResources();
		virtual void UnbindResources();
	private:
		Ref<Texture> m_AlbdeoTex;
		RenderObjectState m_RenderObject;
	};

}
