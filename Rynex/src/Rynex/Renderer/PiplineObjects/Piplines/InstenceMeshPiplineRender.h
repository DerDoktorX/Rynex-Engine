#pragma once
#include <Rynex/Renderer/PiplineObjects/Piplines/InstanceMeshPiplineRenderBase.h>


namespace Rynex {
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDER_TEMPLATE
	class InstanceMeshPiplineRenderShade : public InstanceMeshPiplineRenderBase
	{
	private:
		enum
		{
		   //
			TextureBinding_Albedo = 0,
			TextureBinding_Shadow = 1,


			UniformBinding_LightCamera = 2,
			UniformBinding_Materiel = 3,

		};
		struct RenderObject
		{
			glm::mat4 m_ModelMatrix;
			glm::mat4 m_NormalMatrix;
			int m_EntityID;
			int m_Empty[3];

			RenderObject()
				: m_ModelMatrix(glm::mat4(-1.0f))
				, m_NormalMatrix(glm::mat4(-1.0f))
				, m_EntityID(-1)
				, m_Empty{ -10, -11, -12 }
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> m_ObjectVec;
			std::vector<int> m_EntityIDVec;
			bool m_Update;

			RenderObjectState()
				: m_ObjectVec()
				, m_EntityIDVec()
				, m_Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instanceIndex)
			{
				if (instanceIndex >= m_EntityIDVec.size() || instanceIndex == static_cast<uint32_t>(-1))
				{
					instanceIndex = m_EntityIDVec.size();
					m_ObjectVec.emplace_back(RenderObject());
					m_EntityIDVec.emplace_back<int>(-1);
				}
			}

			void SetObject(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);

				if (object.m_EntityID != entityID || modelMatrix != object.m_ModelMatrix)
				{
					glm::mat4 modelInverse = glm::inverse(modelMatrix);
					glm::mat4 modelTranspose = glm::transpose(modelInverse);
					Set(modelMatrix, entityID, object, entity);
				}

			}

			void SetObjectForce(uint32_t& instanceIndex, const glm::mat4 & modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);

				Set(modelMatrix, entityID, object, entity);

			}

			inline void Set(const glm::mat4 & modelMatrix, const int entityID, RenderObject & elementObject, int& elementEntityID)
			{
				glm::mat4 modelInverse = glm::inverse(modelMatrix);
				glm::mat4 modelTranspose = glm::transpose(modelInverse);


				elementEntityID = entityID;
				elementObject.m_ModelMatrix = modelMatrix;
				elementObject.m_EntityID = entityID;

				elementObject.m_NormalMatrix = modelTranspose;
				m_Update = true;

			}

			void Updated()
			{

				m_Update = false;
			}

			bool NeedUpdate() const
			{
				return m_Update;
			}

			bool operator==(const RenderObjectState & renderObject) const
			{

				bool resultEntt = this->m_EntityIDVec == renderObject.m_EntityIDVec;
				return resultEntt;
			}


				};
	public:
		InstanceMeshPiplineRenderShade();
		InstanceMeshPiplineRenderShade(const InstanceMeshPiplineRenderShade&) = default;
		virtual ~InstanceMeshPiplineRenderShade();

		virtual void SubmitRenderTargetResource(ViewPassStorage & viewPass) override;
		virtual void SubmitRenderTargetResourceReadImg(const Ref<Texture>&texture) override;
		virtual void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>&buffer)override;


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
		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstanceMeshPiplineRenderShade>(*this); }

	protected:
		int CheckSubmitMeshObject(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);
		bool SubmitResources(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);

		

		virtual void BeforeDrawCall() override;
		virtual void BindResources() override;
		virtual void UnbindResources() override;
	private:
	    Ref<UniformBuffer>	m_LightBuffer;
		Ref<UniformBuffer>	m_MaterielBuffer;

		Ref<Texture>		m_AlbedoTex;
		Ref<Texture>		m_ShadowTex;

		RenderObjectState   m_RenderObject;
	};

	class InstanceMeshPiplineRenderDepth : public InstanceMeshPiplineRenderBase
	{
	private:
		struct RenderObject
		{
			glm::mat4 m_ModelMatrix;

			RenderObject()
				: m_ModelMatrix(glm::mat4(-1.0f))
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> m_ObjectVec;
			std::vector<int> m_EntityIDVec;
			bool m_Update;

			RenderObjectState()
				: m_ObjectVec()
				, m_EntityIDVec()
				, m_Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instanceIndex)
			{
				if (instanceIndex >= m_EntityIDVec.size() || instanceIndex == static_cast<uint32_t>(-1))
				{
					instanceIndex = m_EntityIDVec.size();
					m_ObjectVec.emplace_back(RenderObject());
					m_EntityIDVec.emplace_back<int>(-1);
				}
			}

			void SetObject(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);
				if (entityID != entity || modelMatrix != object.m_ModelMatrix)
				{
					Set(modelMatrix, entityID, object, entity);
				}
			}

			void SetObjectForce(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);

				Set(modelMatrix, entityID, object, entity);

			}

			inline void Set(const glm::mat4& modelMatrix, const int entityID, RenderObject& elementObject, int& elementEntityID)
			{
				elementEntityID = entityID;
				elementObject.m_ModelMatrix = modelMatrix;
				m_Update = true;
			}

			void Updated()
			{
				m_Update = false;
			}

			bool NeedUpdate() const
			{
				return m_Update;
			}




		};
	public:
		InstanceMeshPiplineRenderDepth();
		InstanceMeshPiplineRenderDepth(const InstanceMeshPiplineRenderDepth&) = default;
		virtual ~InstanceMeshPiplineRenderDepth();

		virtual void SubmitRenderTargetResource(ViewPassStorage& viewPass) override;
		virtual void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) override;
		virtual void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)override;


		virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) override;

		virtual void DrawNow(int flags) override;

		virtual bool Empty() const override;
		virtual bool IsFull() const override;

		virtual void Clear();

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> cameraBuffer) override;
		virtual uint64_t GetVertexBufferNumber() const override;
		virtual uint64_t GetIndexBufferNumber() const override;
		virtual uint64_t GetIndirectBufferNumber() const override;
		virtual uint64_t GetTextureNumber() const override;
		virtual uint64_t GetUniformBufferNumber() const override;

		virtual uint64_t GetStorageBufferNumber() const override;

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstanceMeshPiplineRenderDepth>(*this); }

	protected:
		int CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		bool SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);


		virtual void BeforeDrawCall() override;
		virtual void BindResources() override;
		virtual void UnbindResources() override;
	private:
		RenderObjectState m_RenderObject;
	};
#endif
	class InstanceMeshPiplineRenderShape : public InstanceMeshPiplineRenderBase
	{
	private:
		enum
		{
			TextureBinding_Albedo = 0,
		};
		struct RenderObject
		{
			glm::mat4 m_ModelMatrix;


			RenderObject()
				: m_ModelMatrix(glm::mat4(-1.0f))
			{
			}
		};

		struct RenderObjectState
		{
			std::vector<RenderObject> m_ObjectVec;
			std::vector<int> m_EntityIDVec;
			bool m_Update;

			RenderObjectState()
				: m_ObjectVec()
				, m_EntityIDVec()
				, m_Update(true)
			{
			}


			RenderObjectState(const RenderObjectState&) = default;

			void CheckBatchIndex(uint32_t& instanceIndex)
			{
				if (instanceIndex < m_EntityIDVec.size() && instanceIndex != static_cast<uint32_t>(-1))
					return;

				instanceIndex = m_EntityIDVec.size();
				m_ObjectVec.emplace_back(RenderObject());
				m_EntityIDVec.emplace_back<int>(-1);
				
			}

			void SetObject(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);
				if (entityID != entity || modelMatrix != object.m_ModelMatrix)
				{
					Set(modelMatrix, entityID, object, entity);
				}
			}

			void SetObjectForce(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
			{
				CheckBatchIndex(instanceIndex);
				int& entity = m_EntityIDVec.at(instanceIndex);
				RenderObject& object = m_ObjectVec.at(instanceIndex);

				Set(modelMatrix, entityID, object,  entity);
			}

			inline void Set(const glm::mat4& modelMatrix, const int entityID, RenderObject& elementObject, int& elementEntityID)
			{
				elementEntityID = entityID;
				elementObject.m_ModelMatrix = modelMatrix;
				m_Update = true;
			}

			void Updated()
			{
				m_Update = false;
			}

			bool NeedUpdate() const
			{
				return m_Update;
			}




		};
	public:
		InstanceMeshPiplineRenderShape();
		InstanceMeshPiplineRenderShape(const InstanceMeshPiplineRenderShape&) = default;

		virtual ~InstanceMeshPiplineRenderShape();

		virtual void SubmitRenderTargetResource(ViewPassStorage& viewPass) override;
		virtual void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) override;
		virtual void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)override;

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

		virtual Ref<PiplineRenderBase> Copy() const override { return CreateRef<InstanceMeshPiplineRenderShape>(*this); }
	protected:
		int CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);
		bool SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh);

		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if (nullptr == b)
				result = result | notVaildState;
		}

		virtual void BeforeDrawCall() override;
		virtual void BindResources() override;
		virtual void UnbindResources() override;
	private:
		Ref<Texture> m_AlbedoTex;
		RenderObjectState m_RenderObject;
	};

}

