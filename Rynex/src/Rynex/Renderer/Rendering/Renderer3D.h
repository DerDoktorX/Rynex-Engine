#pragma once

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Scene/Entity.h"

namespace Rynex {

	namespace Ligths {

		

		struct PointLigtheData
		{
			glm::vec3 Color = { -10.0f,-11.0f,-12.0f };
			float Distence = -13.0f;
			glm::vec3 Postion = { -14.0f,-15.0f,-16.0f };
			float Intensitie = -17.0f;


			PointLigtheData() = default;
			PointLigtheData(glm::vec3& color, glm::vec3& postion, float intensitie, float distence) 
				: Color(color)
				, Postion(postion)
				, Intensitie(intensitie)
				, Distence(distence) { }
		};

		struct DrirectionLigtheData
		{
			glm::mat4 ShadowMatrix = {
			   1.0f, 0.0f, 0.0f, 0.0f,
			   0.0f, 1.0f, 0.0f, 0.0f,
			   0.0f, 0.0f, 1.0f, 0.0f,
			   0.0f, 0.0f, 0.0f, 1.0f
			};
			glm::vec4 ShadowMapPos = { -1.0f, -2.0f,-3.0f, -4.0f };

			glm::vec3 Color = { -10.0f,-11.0f,-12.0f };
			float Intensitie = -13.0f;

			glm::vec3 Dirction = { -14.0f, -15.0f, -16.0f };
			float ShadowMapSize = -17.0f;

			DrirectionLigtheData() = default;

			DrirectionLigtheData(const glm::vec3& color, float intensitie, const glm::mat4& view, const glm::mat4& viewProjetionShadow, uint32_t texSize)
				: Color(color)
				, Dirction(-glm::vec3(view[3]))
				, Intensitie(intensitie)
				, ShadowMapSize((float)texSize)
				// , ShadowMatrix(viewProjetionShadow)
			{ 
#if 1
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
				
				// Verschiebung: Schiebt den Bereich nach unten links
				glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
				
				ShadowMatrix = translate * scale * viewProjetionShadow;
#endif
			}
		};

		struct SpotLigtheData
		{
			glm::mat4 ShadowMatrix = {
			   1.0f, 0.0f, 0.0f, 0.0f,
			   0.0f, 1.0f, 0.0f, 0.0f,
			   0.0f, 0.0f, 1.0f, 0.0f,
			   0.0f, 0.0f, 0.0f, 1.0f
			};
			glm::vec4 ShadowMapPos = { -1.0f, -2.0f,-3.0f, -4.0f };

			glm::vec3 Color = { -10.0f,-11.0f,-12.0f };
			float Intensitie = -13.0f;

			glm::vec3 Postion = { -14.0f,-15.0f,-16.0f };
			float Distence = -17.0f;

			glm::vec3 Direction = { -18.0f,-19.0f,-20.0f };
			float Inner = -21.0f;

			float Outer = -22.0f;
			glm::vec3 Empty = { -23.0f, -24.0f ,-25.0f };
			glm::vec4 Empty2 = { -26.0f, -27.0f ,-28.0f, -29.0f };

			SpotLigtheData() = default;
			SpotLigtheData(const glm::vec3& color, const glm::vec3& postion, const glm::vec3& direction, float intensitie, float distence, float inner, float outer, const glm::mat4& viewProjetionShadow)
				: Color(color)
				, Intensitie(intensitie)
				, Distence(distence)
				, Inner(inner)
				, Outer(outer)
				, Postion(postion)
				, Direction(direction)
				
				// , ShadowMatrix(viewProjetionShadow)
			{ 
			
				glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

				// Verschiebung: Schiebt den Bereich nach unten links
				glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));

				ShadowMatrix = translate * scale * viewProjetionShadow;
			}
		};
		

		
	}
	
	struct MeshInstences
	{
		Ref<Mesh> Mesh;
		Ref<Material> Material;
		std::vector<glm::mat4> ModelMatrix;
		std::vector<int> EntityIDs;
		MeshInstences()
			: Mesh()
			, ModelMatrix(std::vector<glm::mat4>())
			, EntityIDs(std::vector<int>())
		{
		};
	};

	struct CameraData
	{
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::vec3 Position;
		int Empty = -1;
	};


	using RenderFunc = std::function<void(const glm::mat4&, const Ref<Mesh>&, const Ref<Material>&, int)>;

	class RYNEX_API Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();


		
		static glm::mat4 CalculateShadowDirectionelMatrix(const glm::mat4& view, const glm::vec3& min, const glm::vec3& max);

		static void SetLigthUniform(AmbientLigthComponent* ambient);
		static void SetLigthUniform(DrirektionleLigthComponent& drirection, const glm::mat4& matrix, const glm::mat4& viewProjetionShadow);
		static void SetLigthUniform(SpotLigthComponent& spot, const glm::mat4& matrix, const glm::mat4& viewProjetionShadow);
		static void SetLigthUniform(PointLigthComponent& point, const glm::mat4& matrix);
		static void SetShadowsUniform();
		
		static void BeginFrame();
		static void EndFrame();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void AddShadowMapSpotlLigth(const glm::mat4& viewProjtion);
		static void AddShadowMapDirectionelLigth(const glm::mat4& viewProjtion);
		static void BeginSceneShadow();
		static void EndSceneShadow();
		
#if 0
		static void BeginScene();
		static void BeginScene(const Ref<EditorCamera>& camera);
		static void SetRenderCamera(const Camera& camera, const glm::mat4& transform, const glm::vec4& centerViewFustrum);
		static void DrawModdelMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID);

		static void BindDefault3DShader(const Ref<Texture>& bindeTexure, const glm::mat4& modelMatrix, int entityID = 0, const glm::vec3& objectColor = { 1.0f, 0.0f,1.0f });
		static void DrawModdelMeshShadow(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID);
		static void DrawModdelShadow(const glm::mat4& modelMatrix, const std::vector<NodeData>& nodeData, int entityID);
		static void DrawModdelShadow(const glm::mat4& modelMatrix, const std::vector<NodeData>& nodeData, int entityID);
		static void DrawModdelShadow(const glm::mat4& modelMatrix, const std::vector<Ref<Mesh>>& models, int entityID);

		static void BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model, int entityID);
		static void AfterDrawEntity(const MaterialComponent& material);

		static void SetMaterial(const MaterialComponent& material, const glm::mat4& modelMatrix, int entityID);
		static void DrawModdelMesh(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID);
		static void DrawModdelMesh(const Ref<Mesh>& mesh, std::vector<glm::mat4>& modelMatricies, std::vector<int>& entityIDs);
#endif

		// Render Component Function

		static void RenderComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID, RenderFunc func);
		static void RenderComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID, RenderFunc func);

		static void SubmitComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID, RenderFunc func);
		static void SubmitComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID, RenderFunc func);

		static void RenderComponet(const glm::mat4& modelMatrix, const DynamicMeshComponent& model, const Ref<Material>& materiel, int entityID, RenderFunc func);
		static void RenderComponet(const glm::mat4& modelMatrix, const StaticMeshComponent& model, const Ref<Material>& materiel, int entityID, RenderFunc func);
		

		static void DrawMesh3D(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID);
		static void DrawMeshFrambuffer(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID);
		static void DrawMeshMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID);
		static void SubmiteDrawMeshMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, const Ref<Material>& materiel, int entityID);


		
		static void DrawLineBoxAABB(const BoxAABB& aabb, const glm::mat4& modelMatrix, int entityID, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));
		static void DrawLineBoxAABB(const std::array<glm::vec4, 8>& viewFustrum, const glm::mat4& matrix, const glm::vec3& position, int entityID, const glm::vec3& color = glm::vec3(1.0f, 0.0f, 0.0f));


		static void DrawObjectRender3D(const Ref<VertexArray>& vertexArray);
		static void DrawMesh(const Ref<VertexArray>& vertexArray);
		static void DrawMeshStrips(const Ref<VertexArray>& vertexArray);
		static void DrawLine(const Ref<VertexArray>& vertexArray);
		static void DrawLineLoop(const Ref<VertexArray>& vertexArray);
		static void DrawPoints(const Ref<VertexArray>& vertexArray);
		static void DrawPatches(const Ref<VertexArray>& vertexArray);
		// Not Implemntet
		static void DrawError();
		

				

		static void AktivePolyGunMode(bool active = true);
		static void SetDethTest(bool active = true);
		static void SetFace(CallFace callFace = CallFace::None);

		struct StatisticsShader
		{
			uint32_t Fragment = 0;
			uint32_t Vertex = 0;

			uint32_t Geomtry = 0;

			//Tess = Tesselation
			uint32_t TessControler = 0;
			uint32_t TessEvalution = 0;

			uint32_t Compute = 0;

			//ShaderNeed to Bind
			uint32_t DifferentBind = 0;
		};

		struct StatisticsScene
		{
			uint32_t Traingls = 0, TrainglsStrips = 0, TrainglsFan = 0, TrainglsAdjacency = 0, TrainglsStripsAdjacency = 0;
			uint32_t Lines = 0, LinseStrips = 0, LineAdjacency = 0, LineLoop = 0, LineSmooth = 0, LineSmoothHint = 0, LineStripsAdjacency = 0;
			uint32_t Points = 0;
			//Imeges
			uint32_t Frambuffer = 0;
			uint32_t TextursColor = 0;
			uint32_t TextursShadow = 0;
			uint32_t TextursLigth = 0;
			uint32_t TextursNormale = 0;

			//EntitysCount
			uint32_t DrawEntityCount = 0;
			uint32_t LigthEntityCount = 0;
			uint32_t CameraEntityCount = 0;

			//Shader
			StatisticsShader BindShader;

		};

		struct Statistics
		{
			// Frame
			uint32_t FrameCount = 0;
			std::chrono::time_point<std::chrono::steady_clock> StartTimePoint;
			std::chrono::time_point<std::chrono::steady_clock> EndTimePoint;
			std::vector<int64_t> DrawTime;
			uint32_t DrawPass = 0;
			uint32_t LargesVertexBufferByts = 0;

			// Draw
			uint32_t NewDrawCalls = 0;
			uint32_t DrawCalls = 0;

			StatisticsScene Scene;

			uint32_t ChachSize = 0;

			// uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			// uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
			Ref<Texture> ShadowsTex;
			uint32_t* MaxShadowsLigthsCount;
		};

		static void ResetStats();
		static Statistics GetStats();
	};

}

