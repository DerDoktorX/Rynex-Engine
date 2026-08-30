#pragma once

#include <Rynex/Renderer/Mesh/MeshSource.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>

#define RY_TRANSFOMR_VEC 0

namespace Rynex {

	struct MeshEntityObjectIncies
	{
		int EntityID;
		uint32_t MeshLoclaeIndex;
		int MaterielShaderIndex;
	};

	struct MeshRenderObjectTransform
	{
		glm::mat4 TransformMatrix = glm::mat4(-1.0f);
		glm::mat4 NormleMatrix = glm::mat4(-1.0f);
		int EntityID = -1;
		int MaterielIndex = -1;
		int Empty1 = -10; 
		int Empty2 = -11;
	};

	


	struct RenderMeshObjectData
	{
		glm::mat4 TransformMatrix = glm::mat4(-1.0f);
		glm::mat4 NormleMatrix = glm::mat4(-1.0f);

		
		// _mat4 TransformMatrix;
		int EnitityID = -1;
		int MaterielID = -1;
		//_mat3 NormleMatrix;
		
		
		int Empty0 = -10;
		int Empty1 = -11;
	};




#if RY_TRANSFOMR_VEC
	template<typename T, typename N = glm::mat4>
#else
	template<typename T>
#endif
	class MeshRenderData
	{
	public:
		using _RenderObjectData = T;

	private:
		struct MeshObjectRenderData
		{
			int EnitityID;
			uint32_t EnityRenderIndex = static_cast<uint32_t>(-1); // Index Wich MeshIndex This Is

			_RenderObjectData RenderObjectData;
		};
	public:
		MeshRenderData() = delete;
		MeshRenderData(const Mesh::PerDrawObject& meshIndrect, const UUID& meshRenderID)
			: m_MeshIndrectOrig(meshIndrect)
			, m_MeshIndrect(meshIndrect)
			, m_MeshRenderID(meshRenderID)
			, m_Count(0u)
		{
		}
		
		~MeshRenderData()
		{
			Clear();
		}

		void AddMeshObjectRenderData(int localeMeshesOffset, int entityID, const _RenderObjectData& instenceData)
		{
			m_InstenceData.emplace_back(MeshObjectRenderData{
				entityID,
				static_cast<uint32_t>(-1),
				instenceData
			});
			m_Count++;
			m_MeshIndrect.InstancesCount = m_MeshIndrectOrig.InstancesCount * m_Count;

			m_LocaleMeshesOffset = localeMeshesOffset;
		}

		uint32_t RemoveMeshObjectRenderData(int localeMeshesOffset, int enitityID)
		{
			using It = typename std::vector<MeshObjectRenderData>::iterator;
			uint32_t index = FindeIndex(enitityID);
			m_Count--;
			It itIndex = m_InstenceData.begin() + index;
			m_InstenceData.erase(itIndex);

			m_MeshIndrect.InstancesCount = m_MeshIndrectOrig.InstancesCount * m_Count;
			m_LocaleMeshesOffset = localeMeshesOffset;
			return index;
		}

		void Clear()
		{
			m_Count = 0;
			m_InstenceData.clear();
		}

		uint32_t UpdateObjectIndex(int enitityID, const _RenderObjectData& renderObjectData)
		{
			uint32_t index = FindeIndex(enitityID);
			MeshObjectRenderData& objectData = m_InstenceData.at(index);
			objectData.RenderObjectData = renderObjectData;

			uint32_t meshObjectIndex = objectData.EnityRenderIndex;
			uint32_t notVaildIndex = static_cast<uint32_t>(-1);
			RY_CORE_ASSERT(meshObjectIndex != notVaildIndex, "ObjectIndex Not Set!");

			return meshObjectIndex;
		}

		uint32_t InserData(int& golblelastOffset, std::vector<Mesh::PerDrawObject>& indrectMeshDraw, std::vector<_RenderObjectData>& renderObjectData)
		{
			for (MeshObjectRenderData& objectData : m_InstenceData)
			{
				SetMeshObjectRenderDataIndex(objectData, renderObjectData);
				InsertRenderDataToVector(objectData.RenderObjectData, renderObjectData);
			}
			IncreseOffset(golblelastOffset);
			uint32_t index = InsertDrawBufferObjectToVector(indrectMeshDraw);
			return index;
		}

		const UUID& GetID() { return m_MeshRenderID; }
	private:
		uint32_t FindeIndex(int enttiyID)
		{
			uint32_t index = 0;
			for (const MeshObjectRenderData& meshObject : m_InstenceData)
			{
				if (meshObject.EnitityID == enttiyID)
					return index;
				index++;
			}
			RY_CORE_ASSERT(false, "Don't Found Entity ID");
			return static_cast<uint32_t>(-1);
		}

		static void InsertRenderDataToVector(_RenderObjectData& objectData, std::vector<_RenderObjectData>& renderObjectData)
		{
			using ItConst = typename std::vector<MeshObjectRenderData>::iterator;
			ItConst itEnd = renderObjectData.end();
			renderObjectData.insert(itEnd, objectData);
		}

		static void SetMeshObjectRenderDataIndex(MeshObjectRenderData& objectData, std::vector<_RenderObjectData>& renderObjectData)
		{
			uint32_t index = renderObjectData.size();
			objectData.EnityRenderIndex = index;
		}

		uint32_t InsertDrawBufferObjectToVector(std::vector<Mesh::PerDrawObject>& indrectMeshDraw) const
		{
			uint32_t index = indrectMeshDraw.size();
			using ItConst = typename std::vector<MeshObjectRenderData>::iterator;
			ItConst itEnd = indrectMeshDraw.end();
			indrectMeshDraw.insert(itEnd, m_MeshIndrect);
			return index;
		}

		void IncreseOffset(int& golblelastOffset)const
		{
			golblelastOffset += m_MeshIndrect.InstancesCount;
		}
	private:
		const Mesh::PerDrawObject m_MeshIndrectOrig; // This is The Origenale, for one Don't Touche
		const UUID m_MeshRenderID;
		uint32_t m_Count;
		std::vector<MeshObjectRenderData> m_InstenceData;
		Mesh::PerDrawObject m_MeshIndrect; // This is to Modife
		int m_LocaleMeshesOffset; // Meshes Instence Offset Set From Meshes 
	};


	template<typename T>
	class IndirectDrawMap
	{
	public:
		using _ObjectRenderData = T;
		using Vector_ObjectRenderData = std::vector<_ObjectRenderData>;
		using _MeshRenderData = MeshRenderData<_ObjectRenderData>;
		using Vector_MeshRenderData = std::vector<_MeshRenderData>;
	private:
		struct DrawData
		{
			Vector_ObjectRenderData					DrawsObjectsData;
			std::vector<Mesh::PerDrawObject>	IndrectDrawsData;
			std::vector<int>						OffsetData;

			void Reset()
			{
				DrawsObjectsData.clear();
				IndrectDrawsData.clear();
				OffsetData.clear();
			}
		};
	public:
		struct RenderMeshesIndrect
		{
			Ref<MeshStatic> RenderMesh;
			Vector_MeshRenderData Meshes;
		};
	public:
		IndirectDrawMap()
			: m_UpdateRangeObject(MaxMinRange())
			, m_GenaretData(true)
		{
		}

		~IndirectDrawMap()
		{
		}

		void SubmitMeshes(const Ref<MeshStatic>& meshStatic, const MeshEntityObjectIncies& objectIndices, const glm::mat4& enityMatrix, glm::uvec2 indirectOffset)
		{
			const MeshStatic::SingleObjectMeshData& singleMeshData = meshStatic->GetSingleObjectData(objectIndices.MeshLoclaeIndex);
			const UUID& meshID = singleMeshData.GetHandle();
			const glm::mat4& matrixChildren = singleMeshData.LocaleCildrenMatrix;

			glm::mat4 matrix = enityMatrix * matrixChildren;

			uint32_t index = FindMeshesDataIndex(meshID);

			SetUpdateRangeRender(index);

			if (index == m_MeshData.size())
			{
				Mesh::PerDrawObject meshIndrect = meshStatic->GetPerDrawObjectFromMeshObject(objectIndices.MeshLoclaeIndex);
				meshIndrect.BaseVertex += indirectOffset.x;
				meshIndrect.FirstIndex += indirectOffset.y;

				m_MeshData.emplace_back(_MeshRenderData{
					meshIndrect,
					meshID
					});
			}
			RY_CORE_ASSERT(index < m_MeshData.size());


			_ObjectRenderData objectData = SetRendererDataType(matrix, objectIndices.EntityID, objectIndices.MaterielShaderIndex);

			_MeshRenderData& meshDrawData = m_MeshData.at(index);
			meshDrawData.AddMeshObjectRenderData(static_cast<int>(objectIndices.MeshLoclaeIndex), objectIndices.EntityID, objectData);

			m_GenaretData = true;
		}

		void UpdateMeshesObject(const Ref<MeshStatic>& meshStatic, const MeshEntityObjectIncies& objectIndices, const glm::mat4& matrixEnity)
		{
			const MeshStatic::SingleObjectMeshData& singleMeshData = meshStatic->GetSingleObjectData(objectIndices.MeshLoclaeIndex);
			const UUID& meshID = singleMeshData.GetHandle();
			const glm::mat4& matrixChildren = singleMeshData.LocaleCildrenMatrix;

			glm::mat4 matrix = matrixEnity * matrixChildren;

			uint32_t index = FindMeshesDataIndex(meshID);
			uint32_t size = m_MeshData.size();
			RY_CORE_ASSERT(index < size);

			_ObjectRenderData objectData = SetRendererDataType(matrix, objectIndices.EntityID, objectIndices.MaterielShaderIndex);

			_MeshRenderData& meshDrawData = m_MeshData.at(index);
			uint32_t drawDataRenderDataIndex = meshDrawData.UpdateObjectIndex(objectIndices.EntityID, objectData);

			Vector_ObjectRenderData& drawsObjectsData = m_DrawData.DrawsObjectsData;
			_ObjectRenderData& drawDataMeshDrawData = drawsObjectsData.at(drawDataRenderDataIndex);
			drawDataMeshDrawData = objectData;

			SetUpdateRangeObject(drawDataRenderDataIndex);
		}

		void DestroyMeshes(const Ref<MeshStatic>& meshStatic, const MeshEntityObjectIncies& objectIndices, const int enitityID)
		{
			const MeshStatic::SingleObjectMeshData& singleMeshData = meshStatic->GetSingleObjectData(objectIndices.MeshLoclaeIndex);
			const UUID& meshID = singleMeshData.GetHandle();

			
			uint32_t index = FindMeshesDataIndex(meshID);
			uint32_t size = m_MeshData.size();
			RY_CORE_ASSERT(index < size);
			
			_MeshRenderData& meshDrawData = m_MeshData.at(index);
			
			uint32_t drawDataRenderDataIndex =  meshDrawData.RemoveMeshObjectRenderData(static_cast<int>(objectIndices.MeshLoclaeIndex), enitityID);
			SetUpdateRangeObject(drawDataRenderDataIndex);
			
			m_DrawData.Reset();
			m_GenaretData = true;
		}

		const std::vector<int>& GetOffset() { GenarateData(); return m_DrawData.OffsetData; }
		const Vector_ObjectRenderData& GetDrawObjects() { GenarateData(); return m_DrawData.DrawsObjectsData; }
		
		const std::vector<Mesh::PerDrawObject>& GetDrawMesh() { GenarateData(); return m_DrawData.IndrectDrawsData; }
		glm::uvec2 GetUpdateRangeObject() const { return m_UpdateRangeObject; }
		glm::uvec2 GetUpdateRangeRender() const { return m_RenderDataUpdateRange; }
		
		bool HasResize() const{ return m_GenaretData;}
		
		static glm::uvec2 MaxMinRange() { return glm::uvec2{ static_cast<uint32_t>(-1), 0u }; }
	private:
		uint32_t FindMeshesDataIndex(const UUID& handle)
		{
			using ITConst = std::unordered_map<UUID, uint32_t>::const_iterator;
			ITConst it = m_MeshObject.find(handle);
			if (it != m_MeshObject.end())
				return it->second;
			uint32_t& index = m_MeshObject[handle];
			index = m_MeshData.size();
			return index;
		}

		void GenarateData()
		{
			if (!m_GenaretData)
			{
				if(IndirectDrawMap::MaxMinRange() != m_UpdateRangeObject)
					m_UpdateRangeObject = IndirectDrawMap::MaxMinRange();
				return;
			}
			int globlelastOffset = 0;
			m_DrawData.Reset();
			m_UpdateRangeObject = IndirectDrawMap::MaxMinRange();
			
			for (_MeshRenderData& meshData : m_MeshData)
			{
				InsertMeshData(meshData, globlelastOffset);
			}
			m_GenaretData = false;
		}


		void InsertMeshData(_MeshRenderData& meshData, int& globlelastOffset)
		{
			Vector_ObjectRenderData& drawsObjectsData = m_DrawData.DrawsObjectsData;
			std::vector<Mesh::PerDrawObject>& indrectDrawsData = m_DrawData.IndrectDrawsData;
			std::vector<int>& offsetData = m_DrawData.OffsetData;

			offsetData.emplace_back(globlelastOffset);
			uint32_t index = meshData.InserData(globlelastOffset, indrectDrawsData, drawsObjectsData);
			SetUpdateRangeObject(index);

			const UUID& id = meshData.GetID();
		}

		

		void SetUpdateRangeObject(uint32_t index)
		{
			Range(m_UpdateRangeObject, index);
		}

		void SetUpdateRangeRender(uint32_t index)
		{
			Range(m_RenderDataUpdateRange, index);
		}

		static void Range(glm::uvec2& range, uint32_t index)
		{
			uint32_t x = range.x > index ? index : range.x;
			uint32_t iAdd = index + 1u;
			uint32_t y = range.y < (iAdd) ? (iAdd) : range.y;
			range = glm::uvec2{ x,  y };
		}


		static _ObjectRenderData SetRendererDataType(const glm::mat4& modelMatrix, const int entityID, const int materielIndex);
	private:
		std::unordered_map<UUID, uint32_t> m_MeshObject;
		Vector_MeshRenderData m_MeshData;
		DrawData m_DrawData;

		glm::uvec2 m_UpdateRangeObject;
		glm::uvec2 m_RenderDataUpdateRange;
		bool m_GenaretData;

	};



	
	
}