#include "rypch.h"
#include "MeshSingle.h"
#include <Rynex/Renderer/Mesh/MeshSource.h>
#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

	MeshSingle::MeshSingle(uint32_t modelLocaleIndex, const UUID& handle, const UUID& sourceHandle)
		: Mesh(handle)
		, m_ModelLocaleIndex(modelLocaleIndex)
		, m_Name("")
	{
		m_Source = AssetManager::GetAsset<MeshSource>(sourceHandle);
		SetupFromSource();
	}

	MeshSingle::MeshSingle(uint32_t modelLocaleIndex, const UUID& handle, const UUID& sourceHandle, const std::string& name, const BoundingVolume& volume)
		: Mesh(handle)
		, m_ModelLocaleIndex(modelLocaleIndex)
		, m_Name(name)
		, m_Bounding(volume)
	{
		m_Source = AssetManager::GetAsset<MeshSource>(sourceHandle);
		SetupFromSource();
	}



	MeshSingle::~MeshSingle()
	{
		Destroy();
	}

	
	bool MeshSingle::IsViewFustrum(const glm::mat4& model, const glm::mat4& viewProjtion)
	{
		glm::mat4 mvp = viewProjtion * model;
		const AABB& aabb = m_Bounding.GetAABB();
		bool isInsideViewFustrem = IsAABBInsideFrustum(mvp, aabb);
		return isInsideViewFustrem;
	}

	

	void MeshSingle::ExtractFrustum(const glm::mat4& m, glm::vec4 planes[6])
	{
#if 0
		glm::vec3 plane0Normle = glm::vec3(
			viewProj[0][3] - viewProj[0][0],
			viewProj[1][3] - viewProj[1][0],
			viewProj[2][3] - viewProj[2][0]
		);
		float plane0Constatnt = viewProj[3][3] - viewProj[3][0];
		float plane0NormleLength = glm::length(plane0Normle);
		float plane0inverseNormalLength = 1.0f / plane0NormleLength;
		plane0Normle *= plane0inverseNormalLength;
		plane0Constatnt *= plane0inverseNormalLength;
		

		planes[0] = glm::vec4(
			viewProj[0][3] - viewProj[0][0],
			viewProj[1][3] - viewProj[1][0],
			viewProj[2][3] - viewProj[2][0],
			viewProj[3][3] - viewProj[3][0]
		);

		planes[2] = viewProj[3] + viewProj[1];
		planes[3] = viewProj[3] - viewProj[1];
		planes[4] = viewProj[3] + viewProj[2];
		planes[5] = viewProj[3] - viewProj[2];
#else
		glm::mat4 mTranspose = glm::transpose(m);
		for(int i = 0; i < 6; i+=2)
		{
			int indexPlaneGroup = i;
			CaculateCorectViewFustremPlane(m, mTranspose, i, &planes[indexPlaneGroup]);
		}
#endif
	}

	bool MeshSingle::IsNotPointInFiewFustrem(glm::vec3 max, glm::vec3 min, const glm::vec4& planeSide)
	{
		glm::vec3 postion(
			planeSide.x < 0.0f ? max.x : min.x,
			planeSide.y < 0.0f ? max.y : min.y,
			planeSide.z < 0.0f ? max.z : min.z
		);
		glm::vec3 plane3(planeSide);

		float dotProduct = glm::dot(plane3, postion);
		dotProduct += planeSide.w;

		bool isInsideViewFustrem = dotProduct >= 0.0f;
		
		return isInsideViewFustrem;
	}

	bool MeshSingle::IsAABBInsideFrustum(const glm::mat4& model, const AABB& box, const glm::mat4& viewProjtion)
	{
		glm::vec4 planes[6];
		ExtractFrustum(viewProjtion, planes);
		glm::vec4 max = model * glm::vec4(box.Max, 1.0f);
		glm::vec4 min = model * glm::vec4(box.Min, 1.0f);
		for (const glm::vec4& p : planes)
		{
			if (IsNotPointInFiewFustrem(max, min, p))
				return true;
		}
		return false;
	}

	bool MeshSingle::IsAABBInsideFrustum(const glm::mat4& m, const AABB& box)
	{
		if (box.Max == glm::vec3(0.0f) && box.Min == glm::vec3(0.0f))
			return true;

		glm::vec4 planes[6];
		ExtractFrustum(m, planes);
		
		glm::vec4 max = glm::vec4(box.Max, 1.0f);
		glm::vec4 min = glm::vec4(box.Min, 1.0f);
		
		for (const glm::vec4& p : planes)
		{
#if 0
			if (IsNotPointInFiewFustrem(max, min, p))
			{
				glm::vec4 postion(
					p.x < 0.0f ? max.x : min.x,
					p.y < 0.0f ? max.y : min.y,
					p.z < 0.0f ? max.z : min.z,
					1.0f
				);
				glm::vec4 postionClip = m * postion;
				bool checkX = (-postionClip.w <= postionClip.x) && (postionClip.x <= postionClip.w);
				bool checkY = (-postionClip.w <= postionClip.y) && (postionClip.y <= postionClip.w);
				bool checkZ = (-postionClip.w <= postionClip.z) && (postionClip.z <= postionClip.w);
				bool check = checkX || checkY || checkZ;
				// RY_CORE_ASSERT(check, "Somthing is Whrong!");
				return true;
			}
#endif
			glm::vec4 postion(
				p.x < 0.0f ? max.x : min.x,
				p.y < 0.0f ? max.y : min.y,
				p.z < 0.0f ? max.z : min.z,
				1.0f
			);
			glm::vec4 postionClip = m * postion;
			bool checkX = (-postionClip.w <= postionClip.x) && (postionClip.x <= postionClip.w);
			bool checkY = (-postionClip.w <= postionClip.y) && (postionClip.y <= postionClip.w);
			bool checkZ = (-postionClip.w <= postionClip.z) && (postionClip.z <= postionClip.w);
			bool check = checkX || checkY || checkZ;
			if (check)
				return true;
		}
		return false;
	}

	glm::vec4 MeshSingle::CaculatePlaneViewFustremPlaneNormilze(const glm::vec3& normale, float constant)
	{
		float normleLength = glm::length(normale);
		float inverseNormalLength = 1.0f / normleLength;

		glm::vec3 normaleV = normale * inverseNormalLength;
		float constatntV = constant * inverseNormalLength;

		glm::vec4 plane = glm::vec4(normaleV, constatntV);
		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane0(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] - viewProj[0][0],
			viewProj[1][3] - viewProj[1][0],
			viewProj[2][3] - viewProj[2][0]
		);
		float constatnt = viewProj[3][3] - viewProj[3][0];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);
		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane1(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] + viewProj[0][0],
			viewProj[1][3] + viewProj[1][0],
			viewProj[2][3] + viewProj[2][0]
		);
		float constatnt = viewProj[3][3] - viewProj[3][0];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);
		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane2(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] + viewProj[0][1],
			viewProj[1][3] + viewProj[1][1],
			viewProj[2][3] + viewProj[2][1]
		);
		float constatnt = viewProj[3][3] - viewProj[3][1];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);

		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane3(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] - viewProj[0][1],
			viewProj[1][3] - viewProj[1][1],
			viewProj[2][3] - viewProj[2][1]
		);
		float constatnt = viewProj[3][3] - viewProj[3][1];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);

		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane4(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] - viewProj[0][2],
			viewProj[1][3] - viewProj[1][2],
			viewProj[2][3] - viewProj[2][2]
		);
		float constatnt = viewProj[3][3] - viewProj[3][2];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);

		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlane5(const glm::mat4& viewProj)
	{
		glm::vec3 normle = glm::vec3(
			viewProj[0][3] + viewProj[0][2],
			viewProj[1][3] + viewProj[1][2],
			viewProj[2][3] + viewProj[2][2]
		);
		float constatnt = viewProj[3][3] + viewProj[3][2];
		glm::vec4 plane = CaculatePlaneViewFustremPlaneNormilze(normle, constatnt);

		return plane;
	}

	void MeshSingle::CaculateCorectViewFustremPlane(const glm::mat4& m, const glm::mat4& mTranspose, int row, glm::vec4 plane[2])
	{
		const glm::vec4& m3Comume = m[3];
		const glm::vec4& mRow = mTranspose[row];

		
		plane[0] = CaculateCorectViewFustremPlaneAdd(m3Comume, mRow);
		plane[1] = CaculateCorectViewFustremPlaneSub(m3Comume, mRow);
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlaneAdd(const glm::vec4& mC, const glm::vec4& mR)
	{
		glm::vec4 plane = mC + mR;
		float aPow2 = std::powf(plane.x, 2.0f);
		float bPow2 = std::powf(plane.y, 2.0f);
		float cPow2 = std::powf(plane.z, 2.0f);
		float sum = 0.0f;

		sum += aPow2;
		sum += bPow2;
		sum += cPow2;

		float length = std::sqrtf(sum);
		plane = plane / length;
		return plane;
	}

	glm::vec4 MeshSingle::CaculateCorectViewFustremPlaneSub(const glm::vec4& mC, const glm::vec4& mR)
	{
		glm::vec4 plane = mC - mR;

		float aPow2 = std::powf(plane.x, 2.0f);
		float bPow2 = std::powf(plane.y, 2.0f);
		float cPow2 = std::powf(plane.z, 2.0f);
		float sum = 0.0f;

		sum += aPow2;
		sum += bPow2;
		sum += cPow2;

		float length = std::sqrtf(sum);
		plane = plane / length;
		return plane;
	}



	

	

	void MeshSingle::SetupFromSource()
	{
		Ref<MeshSource> source = m_Source.lock();

		if (nullptr == source)
		{
			RY_CORE_ASSERT("Not Found Source Asset!");
			return;
		}
		uint32_t count = source->m_VABvec.size();
		uint32_t i = m_ModelLocaleIndex < count ? m_ModelLocaleIndex : 0;
		RY_CORE_ASSERT(i < count || count == 1, "unexpected Source ShaderArray Count not 1 not anyhing higer then SingleMesh LocaleIndex!");
		
		m_VAB = source->GetVertexBuffer(i);
		m_ShadeVIB = source->GetShadeIndexBuffer(i);
		m_DepthVIB = source->GetDepthIndexBuffer(i);

		const std::vector<Mesh::PerDrawObject>& shadeIndrectPDOVec = source->GetPerDrawObjectsShadeVec();
		SetupPDOIndrect(shadeIndrectPDOVec, m_ShadePDOIndrect);

		const std::vector<Mesh::PerDrawObject>& depthIndrectPDOVec = source->GetPerDrawObjectsDepthVec();
		SetupPDOIndrect(depthIndrectPDOVec, m_DepthPDOIndrect);
#if 0
		m_ShadeVAA->SetBoxAABB({ ShaderDataType::Float3, "a_Postion" });
#endif
	}

	void MeshSingle::Destroy()
	{
		m_Source.reset();
		RY_DESTROY_REF(m_VAB);

		RY_DESTROY_REF(m_ShadeVIB);
		RY_DESTROY_REF(m_DepthVIB);
	}

	void MeshSingle::SetupPDOIndrect(const std::vector<Mesh::PerDrawObject>& indrectPDOVec, Mesh::PerDrawObject& pdoIndrect) const
	{
		const Mesh::PerDrawObject& indrectPDO = indrectPDOVec.at(m_ModelLocaleIndex);

		pdoIndrect.Count = indrectPDO.Count;
		pdoIndrect.InstancesCount = 1;
		pdoIndrect.FirstIndex = indrectPDO.FirstIndex;
		pdoIndrect.BaseVertex = indrectPDO.BaseVertex;
		pdoIndrect.BaseInstance = 0;
	}


	bool SingleMeshObject::operator==(const SingleMeshObject& object) const
	{
		bool resultMateriel = object._Material == this->_Material;
		bool resultMesh = object._MeshSingle == this->_MeshSingle;

		return resultMateriel && resultMesh;
	}

	bool SingleMeshObject::operator!=(const SingleMeshObject& object) const
	{
		bool resultMateriel = object._Material != this->_Material;
		bool resultMesh = object._MeshSingle != this->_MeshSingle;

		return resultMateriel || resultMesh;
	}

	uint64_t SingleMeshObject::GetUUID() const
	{
		uint64_t hashMateriel = std::hash<Ref<Material>>{}(_Material);
		uint64_t hashMesh = std::hash<Ref<MeshSingle>>{}(_MeshSingle);
		uint64_t hash = hashMateriel ^ (hashMesh << 1);
		return hash;
	}
}
