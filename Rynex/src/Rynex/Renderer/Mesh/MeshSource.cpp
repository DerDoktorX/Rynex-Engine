#include "rypch.h"
#include "MeshSource.h"
#include <Rynex/Renderer/Materials/DefaultMaterial.h>
#include <Rynex/Asset/Base/AssetManager.h>


#include <queue>
#include <meshoptimizer.h>

namespace Rynex {

	namespace Utils {

		namespace GenrateLoop {

			static void GenareteTextureInPlace(const std::filesystem::path& path, Ref<Texture>& materielTexture)
			{
				Ref<Texture> texture = AssetManager::GetAsset<Texture>(path);
				if (nullptr == texture)
					return;

				if (nullptr == materielTexture)
					materielTexture = texture;
				else if (materielTexture != texture)
					RY_CORE_WARN("For Materiel we have now two Different Texture for the same Index!");

			}

			static Ref<Material> GetMaterielTexturesSingle(const MeshSource::_Material& materielSource)
			{
				const std::vector<std::filesystem::path>& pathVec = materielSource.TexturesMatieral;
				uint32_t count = pathVec.size();

				std::vector<Ref<Texture>> materielTextureVec;
				materielTextureVec.resize(count);

				for (uint32_t i = 0; i < count; i++)
				{
					const std::filesystem::path& path = pathVec.at(i);
					Ref<Texture>& texture = materielTextureVec.at(i);
					GenareteTextureInPlace(path, texture);
				}
				const glm::vec4& baseColor = materielSource.BaseColor;
				glm::vec3 color = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
				float alpha = baseColor.a;
				float shinines = 0.0f;
				float specular = 0.0f;
				float ambientLigthe = 0.01f;

				MaterielShaderData msd = MaterielShaderData{
					color
					, alpha
					, shinines
					, specular
					, ambientLigthe
				};
				Ref<Texture> texture = nullptr;
				if(!materielTextureVec.empty())
					texture = materielTextureVec.front();
				Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
				Ref<Shader> instenceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
				Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));


				Ref<Material> materiel = CreateRef<DefaultMaterial>(msd, texture, instenceShaderShade, instenceShaderDepth);
				return materiel;
			}


			static void MaterielBufferSingle(const MeshSource::SourceMesh& meshSource, const std::vector<MeshSource::_Material>& materielsSources, std::vector<Ref<Material>>& materiels)
			{

				uint32_t materielIndex = meshSource.MaterielIndex;
				uint32_t sizeSources = materielsSources.size();
				RY_CORE_ASSERT(materielIndex < sizeSources, "Buffer OverFlow");

				const MeshSource::_Material& materielSource = materielsSources.at(materielIndex);

				uint32_t size = materiels.size();
				RY_CORE_ASSERT(materielIndex < size, "Buffer OverFlow");

				Ref<Material>& materiel = materiels[materielIndex];
				if (nullptr == materiel)
				{
					materiels.at(materielIndex) = GetMaterielTexturesSingle(materielSource);
				}
			}

			struct SingleMeshBuffers
			{
				Ref<VertexBuffer>& VertexBufferAtributeRef;
				Ref<IndexBuffer>& ShadeIndexBufferRef;
				Ref<IndexBuffer>& DepthIndexBufferRef;

				Mesh::PerDrawObject& ShadePerDrawObjectRef;
				Mesh::PerDrawObject& DepthPerDrawObjectRef;

				Ref<MeshSingle>& MeshSingleRef;

				operator bool() const 
				{
					return nullptr == VertexBufferAtributeRef
						&& nullptr == ShadeIndexBufferRef
						&& nullptr == DepthIndexBufferRef


						&& nullptr == MeshSingleRef;
				}

				bool IsEveryRefInit() const
				{
					return nullptr != VertexBufferAtributeRef
						&& nullptr != ShadeIndexBufferRef
						&& nullptr != DepthIndexBufferRef


						&& nullptr != MeshSingleRef;
				}

				
			};

			static void SetMeshBufferSetup(const Ref<VertexBuffer>& vba, Ref<IndexBuffer>& ib
				, const std::vector<uint32_t>& indicesVec, Mesh::PerDrawObject& perDrawObject)
			{
				ib = IndexBuffer::Create(
					indicesVec.data()
					, indicesVec.size()
					, BufferFlag::None
				);
				
				perDrawObject.BaseInstance = 0u;
				perDrawObject.BaseVertex = 0u;
				perDrawObject.FirstIndex = 0u;
				perDrawObject.Count = indicesVec.size();
				perDrawObject.InstancesCount = 0;
			}

			static void GenaretSingleMeshBuffer(const MeshSource::OptimizeMeshData& data, SingleMeshBuffers& gpuMeshBufferRef, const MeshSource::SourceMesh& vertexSource, const UUID& meshSourceHandle)
			{

				BufferLayout layoute = BufferLayout({
						  { ShaderDataType::Float3, MeshSource::s_VerexBufferAtributePostionName	}
						, { ShaderDataType::Float2, MeshSource::s_VerexBufferAtributeTextureCoordName }
						, { ShaderDataType::Float3,  MeshSource::s_VerexBufferAtributeNormaleName	}
				});
				gpuMeshBufferRef.VertexBufferAtributeRef = VertexBuffer::Create(
					data.VerticesDataVec.data()
					, data.VerticesDataVec.size() * sizeof(MeshVerteices)
					, BufferFlag::None
					, layoute
				);


				SetMeshBufferSetup(
					gpuMeshBufferRef.VertexBufferAtributeRef
					, gpuMeshBufferRef.ShadeIndexBufferRef
					, data.ShadeIndicesDataVec
					, gpuMeshBufferRef.ShadePerDrawObjectRef
				);

				SetMeshBufferSetup(
					gpuMeshBufferRef.VertexBufferAtributeRef
					, gpuMeshBufferRef.DepthIndexBufferRef
					, data.DepthIndicesDataVec
					, gpuMeshBufferRef.DepthPerDrawObjectRef
				);
				uint32_t meshIndex = vertexSource.MeshIndex;
				uint32_t meshDataIndex = vertexSource.MeshDataIndex;

				const BoundingVolume& volume = vertexSource.Volume;
				const UUID& meshHandle = vertexSource.MeshHandle;
				const std::string& meshName = vertexSource.MeshName;

				gpuMeshBufferRef.MeshSingleRef = CreateRef<MeshSingle>(
					meshDataIndex
					, meshHandle
					, meshSourceHandle
					, meshName
					, volume
				);
			}

			static void GenaretSingleMeshBufferCheck(const MeshSource::OptimizeMeshData& data, SingleMeshBuffers& gpuMeshBufferRef, const MeshSource::SourceMesh& vertexSource, const UUID& meshSourceHandle)
			{
				if (!gpuMeshBufferRef)
				{
					RY_CORE_ERROR("We try create some parts more then ons!");
					if(!gpuMeshBufferRef.IsEveryRefInit())
					{
						RY_CORE_FATAL("Not All Resource are Set!");
					}
					return;
				}

				GenaretSingleMeshBuffer(data, gpuMeshBufferRef, vertexSource, meshSourceHandle);

				RY_CORE_ASSERT(gpuMeshBufferRef.IsEveryRefInit(), "Now Every Resource shoud be Set!");
			}
#pragma region BatchedResouces

			static Ref<Material> GetMaterielTexturesBatching(const MeshSource::_Material& materielSource, MapVector<int64_t, Ref<Texture>>& textureMap)
			{
				const std::vector<std::filesystem::path>& paths = materielSource.TexturesMatieral;
				Ref<Texture> materielTexture = nullptr;
				for (const std::filesystem::path& p : paths)
				{

					Ref<Texture> tex = AssetManager::GetAsset<Texture>(p);
					if (nullptr == tex)
						continue;

					Texture* texPtr = tex.get();
					int64_t key = (int64_t)texPtr;

					if (nullptr == materielTexture)
						materielTexture = tex;


					if (textureMap.HasKey(key))
						continue;

					textureMap.AddData(key, tex);


				}
				const glm::vec4& baseColor = materielSource.BaseColor;
				glm::vec3 color = glm::vec3(baseColor.r, baseColor.g, baseColor.b);
				float alpha = baseColor.a;
				float shinines = 0.0f;
				float specular = 0.0f;
				float ambientLigthe = 0.01f;
				MaterielShaderData msd = MaterielShaderData{
					color
					, alpha
					, shinines
					, specular
					, ambientLigthe
				};
				Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
				Ref<Shader> instenceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
				Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

				Ref<Material> materiel = CreateRef<DefaultMaterial>(msd, materielTexture, instenceShaderShade, instenceShaderDepth);
				return materiel;
			}

			static void MaterielBufferBatching(const MeshSource::SourceMesh& meshSource, const std::vector<MeshSource::_Material>& materielsSources, std::vector<Mesh::MeshMaterielIndex>& materielBuffer, std::vector<Ref<Material>>& materiels, MapVector<int64_t, Ref<Texture>>& textureMap)
			{
				uint32_t materielIndex = meshSource.MaterielIndex;
				uint32_t sizeSources = materielsSources.size();
				RY_CORE_ASSERT(materielIndex < sizeSources, "Buffer OverFlow");
				
				const MeshSource::_Material& materielSource = materielsSources.at(materielIndex);

				uint32_t size = materiels.size();
				RY_CORE_ASSERT(materielIndex < size, "Buffer OverFlow");

				Ref<Material>& materiel = materiels[materielIndex];
				if (nullptr == materiel)
				{
					materiels.at(materielIndex) = GetMaterielTexturesBatching(materielSource, textureMap);
				}


				materielBuffer.emplace_back<Mesh::MeshMaterielIndex>(
					Mesh::MeshMaterielIndex{
						static_cast<int>(materielIndex)
					}
				);
			}



			static void PerDrawObjectBufferBatching(const std::vector<uint32_t>& meshIndex, std::vector<Mesh::PerDrawObject>& pdoIndicesVec, std::vector<uint32_t>& indicies, uint32_t vertexCount)
			{
				uint32_t indiciesSize = static_cast<uint32_t>(indicies.size());
				uint32_t meshSize = static_cast<uint32_t>(meshIndex.size());
				uint32_t instence = 0u;
				int baseVertex = static_cast<int>(vertexCount);
				uint32_t baseInstance = 0u;

				pdoIndicesVec.emplace_back<Mesh::PerDrawObject>(
					Mesh::PerDrawObject{
						meshSize,
						instence,
						indiciesSize,
						baseVertex,
						baseInstance
					}
				);
			}

			static void PerDrawObjectBufferBatching(const MeshSource::SourceMesh& meshSource, std::vector<Mesh::PerDrawObject>& pdoIndicesVec, std::vector<uint32_t>& indicies, uint32_t vertexCount)
			{
				const std::vector<uint32_t>& meshIndex = meshSource.ObjectMeshIndexVec;
				PerDrawObjectBufferBatching(meshIndex, pdoIndicesVec, indicies, vertexCount);
			}

			

			static void CreateMeshIDBatching(const MeshSource::SourceMesh& meshSource, std::vector<UUID>& meshesID)
			{
				UUID meshHandle = UUID();
				meshesID.emplace_back<UUID>(
					UUID{
						meshHandle
					}
				);
			}


			static void BatchIncicies(const std::vector<uint32_t>& meshIndex, std::vector<uint32_t>& indicies)
			{
				std::vector<uint32_t>::const_iterator beginMeshIndex = meshIndex.begin();
				std::vector<uint32_t>::const_iterator endMeshIndex = meshIndex.end();

				std::vector<uint32_t>::const_iterator postionBatchedIndex = indicies.end();

				indicies.insert<std::vector<uint32_t>::const_iterator>(postionBatchedIndex, beginMeshIndex, endMeshIndex);
			}

			static void BatchIncicies(const MeshSource::SourceMesh& meshSource, std::vector<uint32_t>& indicies)
			{
				const std::vector<uint32_t>& meshIndex = meshSource.ObjectMeshIndexVec;
				BatchIncicies(meshIndex, indicies);
			}



			static void BatchVertices(const std::vector<MeshVerteices>& meshVertices, std::vector<MeshVerteices>& vertices)
			{
				std::vector<MeshVerteices>::const_iterator beginMeshVertices = meshVertices.begin();
				std::vector<MeshVerteices>::const_iterator endMeshVertices = meshVertices.end();

				std::vector<MeshVerteices>::const_iterator postionBatchedVertices = vertices.end();

				vertices.insert<std::vector<MeshVerteices>::const_iterator>(postionBatchedVertices, beginMeshVertices, endMeshVertices);
			}

			static void BatchVertices(const MeshSource::SourceMesh& meshSource, std::vector<MeshVerteices>& vertices)
			{
				const std::vector<MeshVerteices>& meshVertices = meshSource.MeshVerteices;
				BatchVertices(meshVertices, vertices);
			}
#pragma endregion
		}

		namespace MeshSepration {

			using Triangle = std::array<uint32_t, 3>; // Indizes der drei Vertices

			struct Edge
			{
				Edge(uint32_t first, uint32_t second, uint32_t index)
					: First(first), Second(second), Index(index)
				{
				}

				Edge(const Edge&) = default;

				uint32_t First;
				uint32_t Second;
				uint32_t Index;
			}; // immer: kleinerer Index zuerst


			struct EdgeHash {
				size_t operator()(const Edge& e) const noexcept {
					// Kombiniere beide Indizes zu einem eindeutigen 64-Bit-Wert
					uint64_t key = (static_cast<uint64_t>(e.First) << 32) | e.Second;
					return std::hash<uint64_t>{}(key);

					// Alternative für 32-Bit-Systeme:
					// return std::hash<uint32_t>{}(e.first) ^ (std::hash<uint32_t>{}(e.second) << 1);
				}
			};

			struct EdgeEqual {
				bool operator()(const Edge& a, const Edge& b) const noexcept
				{
					// Da Kanten immer sortiert sind, einfacher Vergleich
					return a.First == b.First && a.Second == b.Second;
				}
			};

			using EdgeHashMap = typename std::unordered_map<typename Edge, typename std::vector<typename uint32_t>, typename EdgeHash, typename EdgeEqual>;

			static std::vector<Triangle> CreateTringleIndieces(const std::vector<uint32_t>& indices)
			{
				std::vector<Triangle> triangles;

				for (size_t i = 0; i < indices.size(); i += 3)
				{
					triangles.push_back({ indices[i], indices[i + 1], indices[i + 2] });
				}
				return triangles;
			}

			static Edge make_edge(uint32_t a, uint32_t b, uint32_t i)
			{
				return a < b ? Edge(a, b, i) : Edge(b, a, i);
			}

			static EdgeHashMap CreateEdgeMap(const std::vector<Triangle>& triangles)
			{
				uint32_t triangle_count = triangles.size();
				EdgeHashMap edge_triangle_map;
				for (uint32_t tri_idx = 0; tri_idx < triangle_count; ++tri_idx)
				{
					const Triangle& tri = triangles[tri_idx];
					Edge edges[3] = {
						make_edge(tri[0], tri[1], tri_idx),
						make_edge(tri[1], tri[2], tri_idx),
						make_edge(tri[2], tri[0], tri_idx)
					};

					for (const Edge& e : edges)
					{
						edge_triangle_map[e].push_back(tri_idx);
					}
				}
				return edge_triangle_map;
			}

			static void FindeNabourseOnEdge(const std::array<Edge, 3>& tri_edges
				, const EdgeHashMap& edge_triangle_map
				, std::vector<bool>& visited, std::queue<uint32_t>& queue)
			{



				for (const Edge& edge : tri_edges)
				{
					const auto& triangleVec = edge_triangle_map.at(edge);
					for (const uint32_t& neighbor : triangleVec)
					{
						if (!visited[neighbor])
						{
							visited[neighbor] = true;
							queue.push(neighbor);
						}
					}
				}
			}

			static std::vector<uint32_t> GetMeshComponent(const std::vector<Triangle>& triangles
				, const EdgeHashMap& edge_triangle_map
				, std::vector<bool>& visited, uint32_t index)
			{
				std::vector<uint32_t> component;
				std::queue<uint32_t> queue;
				queue.push(index);
				visited[index] = true;

				while (!queue.empty())
				{
					uint32_t current = queue.front();
					queue.pop();


					const Triangle& tri = triangles[current];
					component.push_back(current);

					// Nachbarn über Kanten finden


					std::array<Edge, 3> tri_edges = {
						make_edge(tri[0], tri[1], current),
						make_edge(tri[1], tri[2], current),
						make_edge(tri[2], tri[0], current)
					};

					FindeNabourseOnEdge(tri_edges, edge_triangle_map, visited, queue);
				}
				return component;
			}

			static std::vector<std::vector<uint32_t>> GetSpiltMeshCompents(const std::vector<Triangle>& triangles
				, const EdgeHashMap& edge_triangle_map)
			{
				uint32_t triangle_count = triangles.size();
				std::vector<bool> visited(triangle_count, false);
				std::vector<std::vector<uint32_t>> components;

				for (uint32_t i = 0; i < triangle_count; ++i)
				{
					if (visited[i])
						continue;

					std::vector<uint32_t> component = GetMeshComponent(triangles, edge_triangle_map, visited, i);
					components.push_back(std::move(component));

				}

				return components;
			}

#define RY_MAX_TRINAGLE_PER_MESH 3

			static void InfoPrint(std::vector<std::vector<uint32_t>>& components)
			{

				uint32_t i = 0;
				for (const std::vector<uint32_t>& comp : components)
				{

					i = comp.size() > RY_MAX_TRINAGLE_PER_MESH ? i + 1 : i;

				}

				RY_ASSET_TRACE("We found for Mesh in One Buffer {} posible other Mehes! actuely {} Mehses", i, components.size());
			}

			static std::vector<std::vector<Triangle>> SortRigtheOrder(std::vector<std::vector<uint32_t>>& components, std::vector<Triangle>& triangles)
			{
				std::vector<std::vector<Triangle>> traingle;
				traingle.reserve(components.size());
				for (std::vector<uint32_t>& comp : components)
				{
					std::sort(comp.begin(), comp.end(),
						[](uint32_t& a, uint32_t& b)
						{
							return a < b;
						}
					);

					std::vector<Triangle> trai;
					trai.reserve(comp.size());
					for (uint32_t& i : comp)
					{
						trai.emplace_back(triangles[i]);
					}
					traingle.emplace_back(trai);

				}
				return traingle;
			}

			static uint32_t SepaerateBFS_DFS(const std::vector<uint32_t>& indices)
			{
				std::vector<Triangle> triangles = CreateTringleIndieces(indices);
				EdgeHashMap edge_triangle_map = CreateEdgeMap(triangles);
				std::vector<std::vector<uint32_t>> components = GetSpiltMeshCompents(triangles, edge_triangle_map);


				InfoPrint(components);
				return components.size();
			}
		}

	}

	MeshSource::MeshSource(std::vector<MeshSource::_Material>&& materiel, std::vector<MeshSource::SourceMesh>&& meshes, std::vector<MeshSource::SourceVertex>&& vertexSourceData, EntityNodes&& nodes)
		: m_SourceMateriel(std::move(materiel))
		, m_SourceMeshes(std::move(meshes))
		, m_SourceVertexData(std::move(vertexSourceData))
		, m_Nodes(std::move(nodes))
	{
	}

	MeshSource::~MeshSource()
	{
		RY_DESTROY_REF(m_MeshStatic);

		ClearVecData();
		
		RY_DESTROY_REF(m_StorageBuffer);
		
		
	}

	void MeshSource::SetTextures(const std::vector<Ref<Texture>>& textures)
	{
		m_TexturesMap.Clear();
		for (const Ref<Texture>& tex : textures)
		{
			if(nullptr == tex)
				continue;

			Texture* texPtr = tex.get();
			int64_t key = (int64_t)texPtr;

			if (m_TexturesMap.HasKey(key))
				continue;

			m_TexturesMap.AddData(key, tex);
		}
	}

	void MeshSource::SepareteMeshes()
	{


		uint32_t countMehes = 0;
		for(MeshSource::SourceMesh& m : m_SourceMeshes)
		{
			countMehes += Utils::MeshSepration::SepaerateBFS_DFS(m.ObjectMeshIndexVec);
		}
		RY_CORE_INFO("This Moddel has Maby {} Meshes Insted off {}!", countMehes, m_SourceMeshes.size());
	}

	void MeshSource::OptimizeMesh(std::vector<MeshVerteices>& verticies, std::vector<uint32_t>& indicies, OptimizeMeshData& optMesh)
	{
		size_t numVerticies = verticies.size();
		size_t numIndicies = indicies.size();


		MeshVerteices* verticiesPtr = verticies.data();
		uint32_t* indiciesPtr = indicies.data();

		std::vector<uint32_t> remap(numIndicies);

		size_t optVertexCount = meshopt_generateVertexRemap(remap.data(),
			indicies.data(), numIndicies,
			verticies.data(), numVerticies,
			sizeof(MeshVerteices)
		);

		optMesh.VerticesDataVec.resize(optVertexCount);
		std::vector<unsigned int> optInicies;
		optInicies.resize(numIndicies);


		MeshVerteices* optVerticiesPtr = optMesh.VerticesDataVec.data();

		uint32_t* optIndiciesPtr = optInicies.data();


		meshopt_remapIndexBuffer(optIndiciesPtr, indiciesPtr, numIndicies, remap.data());
		meshopt_remapVertexBuffer(optVerticiesPtr, verticiesPtr, numVerticies, sizeof(MeshVerteices), remap.data());


		meshopt_optimizeVertexCache(optIndiciesPtr, optIndiciesPtr, numIndicies, optVertexCount);

		meshopt_optimizeOverdraw(optIndiciesPtr, optIndiciesPtr, numIndicies,
			&(optVerticiesPtr[0].Postion.x), optVertexCount, sizeof(MeshVerteices), 1.05f);

		meshopt_optimizeVertexFetch(optVerticiesPtr,
			optIndiciesPtr, numIndicies,
			optVerticiesPtr, optVertexCount, sizeof(MeshVerteices));


		float thershold = 0.75f;
		size_t targetIndexCount = (size_t)(numIndicies * thershold);
		float targetError = 0.2f;
		optMesh.ShadeIndicesDataVec.resize(numIndicies);
		uint32_t* simpliefiedIniciesPtr = optMesh.ShadeIndicesDataVec.data();

		size_t optIndexCount = meshopt_simplify(simpliefiedIniciesPtr, optIndiciesPtr, numIndicies,
			&optVerticiesPtr[0].Postion.x, optVertexCount, sizeof(MeshVerteices), targetIndexCount, targetError);
		optMesh.ShadeIndicesDataVec.resize(optIndexCount);

	}

	bool MeshSource::HasMeshDataGerated()
	{
		bool value =!m_VABvec.empty()
			&& !m_ShadeIABvec.empty()
			&& !m_DepthIABvec.empty()
			&& !m_MaterialsVec.empty()
			&& CheckAllRefsVaild(m_StorageBuffer)
			&& m_TexturesMap.Empty();

		return value;
	}

	void MeshSource::GenarteMeshesGPUBufferBatched()
	{
		ClearVecData();
		uint32_t vertexCount = 0;

		std::vector<glm::mat4> sboMattrices;
		std::vector<MeshVerteices> vertices;

		std::vector<uint32_t> shadeIndicies;
		std::vector<uint32_t> depthIndicies;

		std::vector<Mesh::MeshMaterielIndex> materielBuffer;
		
		const uint32_t size = m_SourceMeshes.size();


		materielBuffer.reserve(size);
		sboMattrices.reserve(size);

		{
			const uint32_t sizeMateriel = m_SourceMateriel.size();
			m_MaterialsVec.resize(sizeMateriel);
		}
		for (MeshSource::SourceMesh& meshSour : m_SourceMeshes)
		{

			Utils::GenrateLoop::MaterielBufferBatching(meshSour, m_SourceMateriel, materielBuffer, m_MaterialsVec, m_TexturesMap);
			
			
			MeshSource::OptimizeMeshData optMesh = std::move(MeshSource::OptimizeMeshVec(meshSour));
			Utils::GenrateLoop::PerDrawObjectBufferBatching(optMesh.ShadeIndicesDataVec, m_ShadePerDrawObjectVec, shadeIndicies, vertexCount);
			Utils::GenrateLoop::PerDrawObjectBufferBatching(optMesh.DepthIndicesDataVec, m_DepthPerDrawObjectVec, depthIndicies, vertexCount);

			Utils::GenrateLoop::BatchIncicies(optMesh.ShadeIndicesDataVec, shadeIndicies);
			Utils::GenrateLoop::BatchIncicies(optMesh.DepthIndicesDataVec, depthIndicies);
			Utils::GenrateLoop::BatchVertices(optMesh.VerticesDataVec, vertices);
			vertexCount = vertices.size();

		}

		Ref<VertexBuffer>& vab = m_VABvec.emplace_back(VertexBuffer::Create(
			vertices.data(),
			vertices.size() * sizeof(MeshVerteices),
			BufferFlag::None,
			{
				{ ShaderDataType::Float3, MeshSource::s_VerexBufferAtributePostionName	},
				{ ShaderDataType::Float2, MeshSource::s_VerexBufferAtributeTextureCoordName	},
				{ ShaderDataType::Float3,  MeshSource::s_VerexBufferAtributeNormaleName		}
			}
		));
		Ref<IndexBuffer>& ibShade = m_ShadeIABvec.emplace_back(IndexBuffer::Create(shadeIndicies.data(), shadeIndicies.size(), BufferFlag::None));
		Ref<IndexBuffer>& ibDepth = m_DepthIABvec.emplace_back(IndexBuffer::Create(depthIndicies.data(), depthIndicies.size(), BufferFlag::None));

		m_StorageBuffer = StorageBuffer::Create(materielBuffer.data(), materielBuffer.size() * sizeof(Mesh::MeshMaterielIndex), BufferFlag::None);


		m_SingleMeshVec.reserve(size);
		for (uint32_t i = 0; i < size; i++)
		{
			const MeshSource::SourceMesh& meshSour = m_SourceMeshes.at(i);
			const UUID& meshHandle = meshSour.MeshHandle;
			const UUID& meshSourceHandle = this->Handle;
			const std::string& meshName = meshSour.MeshName;
			const BoundingVolume& volume = meshSour.Volume;
			Ref<MeshSingle> singleMesh = CreateRef<MeshSingle>(i, meshHandle, meshSourceHandle, meshName, volume);

			Ref<MeshSingle>& singleMeshRef = m_SingleMeshVec.emplace_back<Ref<MeshSingle>>( Ref<MeshSingle>{ singleMesh } );
			RY_CORE_ASSERT(singleMeshRef->Handle == meshHandle, "not The Same");
		}

	}

	void MeshSource::GenarteMeshDataIndevdiuelBuffer()
	{
		ClearVecData();
		SearchInSourceDataForIdenticalVerteices();

		uint32_t countMesh = m_SourceMeshes.size();
		uint32_t countDataMesh = m_SourceVertexData.size();

		uint32_t countMateriel = m_SourceMateriel.size();
		ReisizeMeshVecData(countMesh);
		ReisizeMaterielVecData(countMateriel);
		const UUID& meshSourceHandle = Handle;
		

		for (uint32_t i = 0; i < countMesh; i++)
		{
			MeshSource::SourceMesh& meshSour = m_SourceMeshes.at(i);
			uint32_t meshIndex = meshSour.MeshIndex;
			uint32_t meshDataIndex = meshSour.MeshDataIndex;
			if (i != meshIndex)
			{
				RY_CORE_WARN("In MeshSource GenarteMeshesGPUBufferSingle we expexted now after index {} thesame Index as Meshindex {}!", i, meshIndex);
			}
			Utils::GenrateLoop::MaterielBufferSingle(meshSour, m_SourceMateriel, m_MaterialsVec);
			RY_CORE_ASSERT(meshDataIndex < countDataMesh, "Buffer over vlow with meshData!");
			MeshSource::SourceVertex& sourceVertex = m_SourceVertexData.at(meshDataIndex);
			MeshSource::OptimizeMeshData optMesh = std::move(MeshSource::OptimizeMeshVec(sourceVertex));

			Utils::GenrateLoop::SingleMeshBuffers gpuMeshBufferRef{
				m_VABvec.at(meshDataIndex)
				, m_ShadeIABvec.at(meshDataIndex)
				, m_DepthIABvec.at(meshDataIndex)
				, m_ShadePerDrawObjectVec.at(meshDataIndex)
				, m_DepthPerDrawObjectVec.at(meshDataIndex)
				, m_SingleMeshVec.at(meshDataIndex)
			};

			if (gpuMeshBufferRef)
			{
				Utils::GenrateLoop::GenaretSingleMeshBuffer(optMesh, gpuMeshBufferRef, meshSour, meshSourceHandle);
			}
					
		}

	}

	

	

	bool MeshSource::IsOneToOne(Ref<MeshStatic> meshStatic) const
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshStatic->GetSingleObjectMesDataVec();
		const std::vector<MeshStatic::SingleObjectMeshData>& sourceSingleObjectMeshDataVec = meshStatic->GetSingleObjectMesDataVec();
		uint32_t count = singleObjectMeshDataVec.size();
		uint32_t countSource = sourceSingleObjectMeshDataVec.size();
		if (count != countSource)
			return false;

		for (uint32_t i = 0; i < count; i++)
		{
			const MeshStatic::SingleObjectMeshData& singleObjectMeshData = singleObjectMeshDataVec.at(i);
			const MeshStatic::SingleObjectMeshData& sourceSingleObjectMeshData = sourceSingleObjectMeshDataVec.at(i);

			if (singleObjectMeshData != sourceSingleObjectMeshData)
				return false;
		}
		return true;
	}

	void MeshSource::OptimizeMeshes()
	{
		for (MeshSource::SourceMesh& m : m_SourceMeshes)
		{
			OptimizeMeshData optMesh;
			OptimizeMeshVec(m.MeshVerteices, m.ObjectMeshIndexVec);
		}
	}

	void MeshSource::OptimizeMeshVec(std::vector<MeshVerteices>& verticies, std::vector<uint32_t>& indiciesShade)
	{
		OptimizeMeshData origMesh;
		origMesh.ShadeIndicesDataVec = indiciesShade;
		origMesh.VerticesDataVec = verticies;

		OptimizeMeshData optMesh;
		OptimizeMeshDublication(origMesh, optMesh);
		OptimizeMeshTrinagleChache(optMesh);

		OptimizeMeshOverdraw(optMesh, 1.03f);
		OptimizeMeshFatch(optMesh);
		OptimizeMeshTrinagleStripsGenerate(optMesh);

		indiciesShade = std::move(optMesh.ShadeIndicesDataVec);
		verticies = std::move(optMesh.VerticesDataVec);
	}

	MeshSource::OptimizeMeshData MeshSource::OptimizeMeshVec(MeshSource::SourceMesh& m)
	{
		MeshSource::SourceVertex sourceVertex= MeshSource::SourceVertex{
			m.MeshVerteices
			, m.ObjectMeshIndexVec
		};
		return MeshSource::OptimizeMeshVec(sourceVertex);
	}

	MeshSource::OptimizeMeshData MeshSource::OptimizeMeshVec(MeshSource::SourceVertex& m)
	{
		
		OptimizeMeshData origMesh;
		origMesh.VerticesDataVec = m.MeshVerteices;
		origMesh.ShadeIndicesDataVec = m.ObjectMeshIndexVec;

		OptimizeMeshData optMesh;
		OptimizeMeshDublication(origMesh, optMesh);
		optMesh.DepthIndicesDataVec = optMesh.ShadeIndicesDataVec;

		float chacheOpt = RY_MESH_OPTIMIZE_OVERDRAW_VALUE;
		OptimizeMeshTrinagleChache(optMesh);
		OptimizeMeshOverdraw(optMesh, chacheOpt);
		OptimizeMeshFatch(optMesh);


		optMesh.DepthIndicesDataVec = optMesh.ShadeIndicesDataVec;
		return optMesh;
	}

	void MeshSource::OptimizeMeshDublication(OptimizeMeshData& origMesh, OptimizeMeshData& optMesh)
	{
		std::vector<MeshVerteices>& verticiesOrig = origMesh.VerticesDataVec;
		std::vector<uint32_t>& indiciesOrig = origMesh.ShadeIndicesDataVec;

		size_t numVerticiesOrig = verticiesOrig.size();
		size_t numIndiciesOrig = indiciesOrig.size();

		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		const MeshVerteices* verticiesOrigReadPtr = verticiesOrig.data();
		const uint32_t* indiciesOrigReadPtr = indiciesOrig.data();
		
		RY_CORE_ASSERT(!verticiesOrig.empty());
		RY_CORE_ASSERT(!indiciesOrig.empty());

		std::vector<uint32_t> remap;
		remap.resize(numIndiciesOrig);
		uint32_t* remapPtr = remap.data();

		size_t optVertexCount = meshopt_generateVertexRemap(
			remapPtr,
			indiciesOrigReadPtr, numIndiciesOrig,
			verticiesOrigReadPtr, numVerticiesOrig,
			vertexByteSizeLayoute
		);

		OptimizeMeshDublicationRemapVertexBuffer(verticiesOrig, optMesh.VerticesDataVec, remap, optVertexCount);
		OptimizeMeshDublicationRemapIndexBuffer(indiciesOrig, optMesh.ShadeIndicesDataVec, remap);
	}

	void MeshSource::OptimizeMeshDublicationRemapVertexBuffer(const std::vector<MeshVerteices>& origVericesVec, std::vector<MeshVerteices>& optVericesVec, const std::vector<uint32_t>& remap, uint32_t optVertexCount)
	{
		size_t numVerticiesOrig = origVericesVec.size();
		const MeshVerteices* verticiesOrigPtr = origVericesVec.data();

		RY_CORE_ASSERT(!origVericesVec.empty());
		RY_CORE_ASSERT(!remap.empty());

		optVericesVec.resize(optVertexCount);
		MeshVerteices* verticiesOptPtr = optVericesVec.data();

		const uint32_t* remapPtr = remap.data();
		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		meshopt_remapVertexBuffer(verticiesOptPtr, verticiesOrigPtr, numVerticiesOrig, vertexByteSizeLayoute, remapPtr);
	}

	void MeshSource::OptimizeMeshDublicationRemapIndexBuffer(const std::vector<uint32_t>& origIndiciesVec, std::vector<uint32_t>& optIndiciesVec, const std::vector<uint32_t>& remap)
	{
		size_t numIndiciesOrig = origIndiciesVec.size();
		const uint32_t* indiciesOrigPtr = origIndiciesVec.data();

		RY_CORE_ASSERT(!origIndiciesVec.empty());
		RY_CORE_ASSERT(!remap.empty());

		optIndiciesVec.resize(numIndiciesOrig);
		uint32_t* indiciesOptPtr = optIndiciesVec.data();

		const uint32_t* remapPtr = remap.data();

		meshopt_remapIndexBuffer(indiciesOptPtr, indiciesOrigPtr, numIndiciesOrig, remapPtr);
	}

	void MeshSource::OptimizeMeshTrinagleChache(OptimizeMeshData& meshBuffers)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesShadeVec = meshBuffers.ShadeIndicesDataVec;
		std::vector<uint32_t>& indiciesDepthVec = meshBuffers.DepthIndicesDataVec;


		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesShadeVec.empty());
		RY_CORE_ASSERT(!indiciesDepthVec.empty());

		size_t numVerticies = verticiesVec.size();
		size_t numShadeIndicies = indiciesShadeVec.size();
		size_t numDepthIndicies = indiciesDepthVec.size();


		const uint32_t* indiciesShadeOrigPtrRead = indiciesShadeVec.data();
		uint32_t* indiciesShadeOrigPtrWhrite = indiciesShadeVec.data();

		meshopt_optimizeVertexCache(indiciesShadeOrigPtrWhrite, indiciesShadeOrigPtrRead, numShadeIndicies, numVerticies);

		const uint32_t* indiciesDepthOrigPtrRead = indiciesDepthVec.data();
		uint32_t* indiciesDepthOrigPtrWhrite = indiciesDepthVec.data();

		meshopt_optimizeVertexCache(indiciesDepthOrigPtrWhrite, indiciesDepthOrigPtrRead, numDepthIndicies, numVerticies);
	}

	void MeshSource::OptimizeMeshTrinagleStripsChache(OptimizeMeshData& meshBuffers)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesVec = meshBuffers.ShadeIndicesDataVec;		

		size_t numVerticies = verticiesVec.size();
		size_t numIndicies = indiciesVec.size();
		
		const uint32_t* indiciesOrigPtrRead = indiciesVec.data();
		uint32_t* indiciesOrigPtrWhrite = indiciesVec.data();

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesVec.empty());

		meshopt_optimizeVertexCacheStrip(indiciesOrigPtrWhrite, indiciesOrigPtrRead, numIndicies, numVerticies);
	}

	void MeshSource::OptimizeMeshTrinagleStripsGenerate(OptimizeMeshData& meshBuffers)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesVec = meshBuffers.ShadeIndicesDataVec;

		size_t numVerticies = verticiesVec.size();
		size_t numIndicies = indiciesVec.size();

		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesVec.empty());
		
		const uint32_t* indiciesReadPtr = indiciesVec.data();

		size_t stripIndexCount = meshopt_stripifyBound(numIndicies);
		std::vector<uint32_t> stripIndicies;
		stripIndicies.resize(stripIndexCount);

		uint32_t* stripIndiciesReadPtr = stripIndicies.data();

		uint32_t indexResetPrimtivValue = static_cast<uint32_t>(-1);

		size_t numIndiciesSize = meshopt_stripify(
			stripIndiciesReadPtr, indiciesReadPtr, numIndicies,
			numVerticies,
			indexResetPrimtivValue
		);
		stripIndicies.resize(numIndiciesSize);

		indiciesVec = std::move(stripIndicies);
	}

	void MeshSource::OptimizeMeshOverdraw(OptimizeMeshData& meshBuffers, float efficiently)
	{

		if (efficiently > 1.05f)
		{
			float higerEfficientlySetting = efficiently - 1.05f;
			RY_CORE_WARN("efficencis Settings is {} higer then sugested! Setting Efficiently: {}", higerEfficientlySetting, efficiently);
		}
		else if (efficiently < 1.00f)
		{
			float lowerEfficientlySetting = 1.00f - efficiently;
			RY_CORE_FATAL("efficencis Settings is {} under 1.0f then sugested! Setting Efficiently: {}", lowerEfficientlySetting, efficiently);
		}

		const std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesVec = meshBuffers.ShadeIndicesDataVec;

		size_t numVerticies = verticiesVec.size();
		size_t numIndicies = indiciesVec.size();

		const uint32_t* indiciesOrigPtrRead = indiciesVec.data();
		uint32_t* indiciesOrigPtrWhrite = indiciesVec.data();

		const float* offsetVertexLayoutPostionReadPtr = &verticiesVec[0].Postion.x;
		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesVec.empty());

		meshopt_optimizeOverdraw(
			indiciesOrigPtrWhrite, indiciesOrigPtrRead, numIndicies,
			offsetVertexLayoutPostionReadPtr, numVerticies, vertexByteSizeLayoute, 
			efficiently
		);

	}

	void MeshSource::OptimizeMeshFatch(OptimizeMeshData& meshBuffers)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesVec = meshBuffers.ShadeIndicesDataVec;

		size_t numVerticies = verticiesVec.size();
		size_t numIndicies = indiciesVec.size();

		const MeshVerteices* verticiesPtrRead = verticiesVec.data();
		MeshVerteices* verticiesPtrWhrite = verticiesVec.data();
		uint32_t* indiciesPtrWhrite = indiciesVec.data();

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesVec.empty());

		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		size_t uniceVertices = meshopt_optimizeVertexFetch(
			verticiesPtrWhrite,
			indiciesPtrWhrite, numIndicies,
			verticiesPtrRead, numVerticies, vertexByteSizeLayoute);

	}

	void MeshSource::OptimizeMeshShadow(OptimizeMeshData& meshBuffers)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& shadeIndiciesVec = meshBuffers.ShadeIndicesDataVec;
		std::vector<uint32_t>& depthIndiciesVec = meshBuffers.DepthIndicesDataVec;

		size_t numVerticies = verticiesVec.size();
		size_t numShadeIndicies = shadeIndiciesVec.size();

		

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!shadeIndiciesVec.empty());
		RY_CORE_ASSERT(depthIndiciesVec.empty());

		depthIndiciesVec.resize(numShadeIndicies);

		RY_CORE_ASSERT(!depthIndiciesVec.empty());

		size_t numDepthIndicies = depthIndiciesVec.size();
		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);
		size_t postionByteSizeLayoute = sizeof(glm::vec3);


		const float* offsetVertexLayoutPostionReadPtr = &verticiesVec[0].Postion.x;
		const uint32_t* indiciesShadePtrRead = depthIndiciesVec.data();
		uint32_t* indiciesDepthPtrWhrite = depthIndiciesVec.data();


		meshopt_generateShadowIndexBuffer(
			indiciesDepthPtrWhrite, indiciesShadePtrRead, numShadeIndicies, 
			offsetVertexLayoutPostionReadPtr, numVerticies, 
			postionByteSizeLayoute, vertexByteSizeLayoute
		);
	}

	void MeshSource::OptimizeMeshLOD(OptimizeMeshData& meshBuffers, float prozentigReduseIndices, float errorSimplefly)
	{
		std::vector<MeshVerteices>& verticiesVec = meshBuffers.VerticesDataVec;
		std::vector<uint32_t>& indiciesVec = meshBuffers.ShadeIndicesDataVec;

		size_t numVerticies = verticiesVec.size();
		size_t numIndicies = indiciesVec.size();

		size_t vertexByteSizeLayoute = sizeof(MeshVerteices);

		RY_CORE_ASSERT(!verticiesVec.empty());
		RY_CORE_ASSERT(!indiciesVec.empty());

		const float* offsetVertexLayoutPostionReadPtr = &verticiesVec[0].Postion.x;
		const uint32_t* indiciesReadPtr = indiciesVec.data();

		size_t targetIndexCount = (size_t)(numIndicies * prozentigReduseIndices);
		std::vector<uint32_t> simpleflyIndicies;
		simpleflyIndicies.resize(numIndicies);
		
		uint32_t* simpliefiedIniciesWhritePtr = simpleflyIndicies.data();

		float* result_error = (float*)0;
		uint32_t options = 0u;
#if 0
		options |= meshopt_SimplifyLockBorder;
#elif 0
		options |= meshopt_SimplifyErrorAbsolute;
#elif 0
		options |= meshopt_SimplifyPrune;
#elif 0
		options |= meshopt_SimplifySparse;
#endif

		size_t optIndexCount = meshopt_simplify(
			simpliefiedIniciesWhritePtr, indiciesReadPtr, numIndicies,
			offsetVertexLayoutPostionReadPtr, numVerticies, vertexByteSizeLayoute,
			targetIndexCount, errorSimplefly, options, result_error
		);
		simpleflyIndicies.resize(optIndexCount);

		meshBuffers.ShadeIndicesDataVec = std::move(simpleflyIndicies);
	}



	void MeshSource::ClearVecData()
	{
		m_ShadePerDrawObjectVec.clear();
		m_ShadePerDrawObjectVec.shrink_to_fit();
		m_ShadeIABvec.clear();
		m_DepthIABvec.clear();
		m_VABvec.clear();
		m_SingleMeshVec.clear();

		for (Ref<Material>& m : m_MaterialsVec)
		{
			RY_DESTROY_REF(m);
		}
		m_MaterialsVec.clear();
		m_MaterialsVec.shrink_to_fit();
		m_TexturesMap.Clear();

	}

	void MeshSource::ReisizeMeshVecData(uint32_t count)
	{
		m_ShadePerDrawObjectVec.resize(count);
		m_DepthPerDrawObjectVec.resize(count);

		m_ShadeIABvec.resize(count, nullptr);
		m_DepthIABvec.resize(count, nullptr);

		m_VABvec.resize(count, nullptr);

		m_SingleMeshVec.resize(count, nullptr);
	}

	void MeshSource::ReisizeMaterielVecData(uint32_t count)
	{
		m_MaterialsVec.resize(count, nullptr);
	}

	void MeshSource::SearchInSourceDataForIdenticalVerteices()
	{
		uint32_t size = m_SourceVertexData.size();
		uint32_t count = 0u;
		std::string msg = "";
		uint32_t last = 0;
		std::vector<uint32_t> dublicatedListB;
		for (uint32_t x = 0; x < size; x++)
		{
			if (IsIndexPresentInVec(dublicatedListB, x))
				continue;

			uint32_t localeMeshCount = 0u;
			for (uint32_t y = x + 1; y < size; y++)
			{
				if (IsIndexPresentInVec(dublicatedListB, y))
					continue;

				const std::vector<MeshVerteices>* aVec = &m_SourceVertexData.at(x).MeshVerteices;
				const std::vector<MeshVerteices>* bVec = &m_SourceVertexData.at(y).MeshVerteices;

				if (CheckVertexArraysIdentical(aVec, bVec))
				{
					dublicatedListB.emplace_back(y);
					count++;
					localeMeshCount++;
				}
			}

			if (0u < localeMeshCount)
			{
				RY_CORE_INFO("We Have found from Mesh Index {}, {} other Meshes they are completly Identical", x, localeMeshCount);
				
				for (uint32_t i = last; i < dublicatedListB.size(); i++)
				{

					if(msg.empty())
						msg += std::to_string(i);
					else
						msg += " , " + std::to_string(i);
				}
				RY_CORE_TRACE("\t[ {} ]", msg);
				msg.clear();
				last = count;

			}
		}

		if (0u < count)
		{
			RY_CORE_INFO("We Have found {} Meshes they are completly Identical", count);
			
			
		}
	}

	

	bool MeshSource::CheckVertexArraysIdentical(const std::vector<MeshVerteices>* aVec, const std::vector<MeshVerteices>* bVec)
	{
		uint32_t size = aVec->size();
		if (size != bVec->size())
		{
			return false;
		}
		else if(aVec == bVec)
		{
			RY_CORE_FATAL("We trie to test two Identical Meshes Ptr! (!Fixing Logic!)");
			return false;
		}

		for (uint32_t i = 0; i < size; i++)
		{
			const MeshVerteices& a = aVec->at(i);
			const MeshVerteices& b = bVec->at(i);
			if (a != b)
				return false;
		}
		RY_CORE_TRACE("We have two have Found A complet Identical Meshes!");
		return true;
	}

	bool MeshSource::CheckVertexArraysIdentical(const std::vector<uint32_t>* aVec, const std::vector<uint32_t>* bVec)
	{
		uint32_t size = aVec->size();
		if (size != bVec->size())
		{
			return false;
		}
		else if (aVec == bVec)
		{
			RY_CORE_FATAL("We trie to test two Identical Meshes Ptr! (!Fixing Logic!)");
			return false;
		}

		for (uint32_t i = 0; i < size; i++)
		{
			const uint32_t& a = aVec->at(i);
			const uint32_t& b = bVec->at(i);
			if (a != b)
				return false;
		}
		RY_CORE_TRACE("We have two have Found A complet Identical Meshes!");
		return true;
	}

	uint32_t MeshSource::GetIndexVertexArraysIdentical(const std::vector<MeshSource::SourceVertex>& list, const std::vector<MeshVerteices>* verticesVec, const std::vector<uint32_t>* indiciesVec)
	{
		for (uint32_t i = 0; i < list.size(); i++)
		{
			const MeshSource::SourceVertex& e = list.at(i);
			const std::vector<MeshVerteices>* vecVertices = &e.MeshVerteices;
			const std::vector<uint32_t>* vecIndicies = &e.ObjectMeshIndexVec;
			if (CheckVertexArraysIdentical(vecVertices, verticesVec) 
				&& CheckVertexArraysIdentical(vecIndicies, indiciesVec))
				return i;
		}
		return MAXUINT32;
	}

	

	bool MeshSource::IsIndexPresentInVec(const std::vector<uint32_t>& indexVec, uint32_t index)
	{

		for (const uint32_t& i : indexVec)
		{
			if (i == index)
				return true;
		}

		return false;
	}

	

	
}

