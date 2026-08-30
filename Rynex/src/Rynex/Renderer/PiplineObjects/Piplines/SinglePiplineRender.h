#pragma once
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

namespace Rynex {

	class SingleMeshPiplineRenderShade : public PiplineRenderBase
	{
	private:
		enum 
		{
			State_MaxEntityRender = 1,
			State_MaxNotUpdateDraws = 100,

			TextureBinding_Abldoe = 0,
			TextureBinding_Shadow = 1,


			UniformBinding_RenderObject = 0,
			UniformBinding_MainCamer = 1,
			UniformBinding_LigthCamera = 2,
			UniformBinding_Materiel = 3
		};
		struct RenderObject
		{
			glm::mat4 ModelMatrix;
			glm::mat4 NormalMatrix;
			int EnitityID;
			int Empty[3] = { -10, -11, -12 };
		};

		struct RenderObjectState
		{
			RenderObject Object;
			int EntityID;
			bool Update;

			RenderObjectState()
				: Object({ glm::mat4(0.0f), glm::mat4(0.0f), -1, { -10, -11, -12 } })
				, EntityID(-1)
				, Update(true)
			{
			}

			RenderObjectState(const glm::mat4& modelMatrix, const int entityID)
			{
				EntityID = entityID;
				
				glm::mat4 modelInverse = glm::inverse(modelMatrix);
				glm::mat4 modelTranspose = glm::transpose(modelInverse);
				Object.EnitityID = EntityID;
				Object.ModelMatrix = modelMatrix;
				Object.NormalMatrix = modelTranspose;
				Update = true;
			}

			RenderObjectState(const RenderObjectState&) = default;

			void SetObject(const glm::mat4& modelMatrix, const int entityID)
			{
				if (entityID != EntityID || modelMatrix != Object.ModelMatrix)
				{
					glm::mat4 modelInverse = glm::inverse(modelMatrix);
					glm::mat4 modelTranspose = glm::transpose(modelInverse);

					EntityID = entityID;
					Object.ModelMatrix = modelMatrix;
					Object.EnitityID = EntityID;
				
					Object.NormalMatrix = modelTranspose;
					Update = true;
				}
			}
			void Updated()
			{
				Update = false;
			}

			bool NeedUpdate() const
			{
				return Update;
			}
			

			bool operator==(const RenderObjectState& renderObject) const
			{
				bool resultEntt = this->EntityID == renderObject.EntityID;
				bool resultModel = this->Object.ModelMatrix == renderObject.Object.ModelMatrix;
				return resultEntt && resultModel;
			}

			
		};
	public:
		SingleMeshPiplineRenderShade();
		SingleMeshPiplineRenderShade(const SingleMeshPiplineRenderShade&) = default;

		virtual ~SingleMeshPiplineRenderShade();

		virtual void SubmitRenderTargetResurces(ViewPassStorage& viewPass) override;
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture) override;
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)override;

		virtual BufferLayout GetExpetedOutput() const override
		{
			RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
			return BufferLayout();
		}
		virtual void SetExpetedOutput(const BufferLayout& output) override
		{
			RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
		}

		

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer) override;
		virtual void SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer) override;

		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) override;


		virtual void SetDataMangingFlags(PiplineManagingState flags) override;
		virtual PiplineManagingState GetDataMangingFlags() const override;

		virtual void SetRenderFlags(int flags) override;
		virtual int GetRenderFlags() const override;
		
		virtual void DrawNow() override;
		virtual void DrawNow(int flags) override;

		virtual uint32_t GetCurentEntityRender() const override;
		virtual uint32_t GetMaxEntityRender() const override;

		virtual uint32_t GetFrameCountNotUpdate() const override;
		virtual uint32_t GetMaxFrameCountNotUpdate() const override { return State_MaxNotUpdateDraws; }

		virtual bool Empty() const override;
		virtual bool IsFull()const override;

		virtual void Clear() override;
		virtual void ClearRenderObjects() override;

		virtual uint64_t GetShaderNumber() const override { return reinterpret_cast<uint64_t>(m_Shader.get()); }
		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<SingleMeshPiplineRenderShade>(*this); }

	private:
		int CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		void SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);

		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if(nullptr == b)
				result = result | notVaildState;
		}

		
		void BeforeDrawCall();
		void BindResources();
		void UnbindResources();
	private:
		Ref<Shader> m_Shader;
		Ref<UniformBuffer> m_CameraBuffer;
		Ref<UniformBuffer> m_LigthBuffer;
		Ref<UniformBuffer> m_ModelBuffer;
		Ref<UniformBuffer> m_MaterielBuffer;

		Ref<Texture> m_AlbdeoTex;
		Ref<Texture> m_ShadowTex;
		Ref<VertexArray> m_VertexArray;

		RenderObjectState m_RenderObject;
		SingleMeshObject m_SingleMeshObject;
		uint32_t m_InstencCount;
		uint32_t m_DrawsAfterLastUpdate;
		PiplineManagingState m_ManagingMode;
		int m_RenderMode;
	};

	class SingleMeshPiplineRenderDepth : public PiplineRenderBase
	{
	private:
		enum
		{
			State_MaxEntityRender = 1,
			State_MaxNotUpdateDraws = 100,

			TextureBinding_Abldoe = 0,
			TextureBinding_Shadow = 1,


			UniformBinding_RenderObject = 0,
			UniformBinding_LigthCamera = 1,
		};
		struct RenderObject
		{
			glm::mat4 ModelMatrix;
		};

		struct RenderObjectState
		{
			RenderObject Object;
			int EntityID;
			bool Update;

			RenderObjectState()
				: Object({ glm::mat4(0.0f) })
				, EntityID(-1)
				, Update(true)
			{
			}

			RenderObjectState(const glm::mat4& modelMatrix, const int entityID)
			{
				EntityID = entityID;

				glm::mat4 modelInverse = glm::inverse(modelMatrix);
				glm::mat4 modelTranspose = glm::transpose(modelInverse);
				Object.ModelMatrix = modelMatrix;
				Update = true;
			}

			RenderObjectState(const RenderObjectState&) = default;

			void SetObject(const glm::mat4& modelMatrix, const int entityID)
			{
				if (entityID != EntityID || modelMatrix != Object.ModelMatrix)
				{
					glm::mat4 modelInverse = glm::inverse(modelMatrix);
					glm::mat4 modelTranspose = glm::transpose(modelInverse);

					EntityID = entityID;
					Object.ModelMatrix = modelMatrix;

					Update = true;
				}
			}
			void Updated()
			{
				Update = false;
			}

			bool NeedUpdate() const
			{
				return Update;
			}


			bool operator==(const RenderObjectState& renderObject) const
			{
				bool resultEntt = this->EntityID == renderObject.EntityID;
				bool resultModel = this->Object.ModelMatrix == renderObject.Object.ModelMatrix;
				return resultEntt && resultModel;
			}


		};
	public:
		SingleMeshPiplineRenderDepth();
		SingleMeshPiplineRenderDepth(const SingleMeshPiplineRenderDepth&) = default;

		virtual ~SingleMeshPiplineRenderDepth();

		virtual void SubmitRenderTargetResurces(ViewPassStorage& viewPass) override {};
		virtual void SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)override {};
		virtual void SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture) override {};

		virtual BufferLayout GetExpetedOutput() const override
		{
			RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
			return BufferLayout();
		}

		virtual void SetExpetedOutput(const BufferLayout& output) override
		{
			RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
		}
		

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer) override;
		virtual void SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer) override;

		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;

		virtual void SetDataMangingFlags(PiplineManagingState flags) override;
		virtual PiplineManagingState GetDataMangingFlags() const override;

		virtual void SetRenderFlags(int flags) override;
		virtual int GetRenderFlags() const override;

		virtual void DrawNow() override;
		virtual void DrawNow(int flags) override;

		virtual uint32_t GetCurentEntityRender() const override;
		virtual uint32_t GetMaxEntityRender() const override;

		virtual uint32_t GetFrameCountNotUpdate() const override;
		virtual uint32_t GetMaxFrameCountNotUpdate() const override { return State_MaxNotUpdateDraws; }

		virtual bool Empty() const override;
		virtual bool IsFull()const override;

		virtual void Clear() override;
		virtual void ClearRenderObjects() override;

		virtual uint64_t GetShaderNumber() const override { return reinterpret_cast<uint64_t>(m_Shader.get()); }
		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<SingleMeshPiplineRenderDepth>(*this); }
	private:
		int CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		void SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);

		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if (nullptr == b)
				result = result | notVaildState;
		}

		void BeforeDrawCall();
		void BindResources();
		void UnbindResources();
	private:
		Ref<Shader> m_Shader;
		Ref<UniformBuffer> m_LigthBuffer;
		Ref<UniformBuffer> m_ModelBuffer;

		Ref<VertexArray> m_VertexArray;
		RenderObjectState m_RenderObject;
		SingleMeshObject m_SingleMeshObject;
		uint32_t m_InstencCount;
		uint32_t m_DrawsAfterLastUpdate;
		PiplineManagingState m_ManagingMode;
		int m_RenderMode;
	};
}


