#pragma once

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Scene/Entity.h"

namespace Rynex {

	class RYNEX_API Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BindDefault3DShader(const Ref<Texture>& bindeTexure, const glm::mat4& modelMatrix, int entityID = 0, const glm::vec3& objectColor = { 1.0f, 0.0f,1.0f });

		static void SetLigthUniform(AmbientLigthComponent& ambient, int index);
		static void SetLigthUniform(DrirektionleLigthComponent& drirektion, const glm::mat4& matrix, int index);
		static void SetLigthUniform(SpotLigthComponent& spot, const glm::mat4& matrix, int index);
		static void SetLigthUniform(PointLigthComponent& point, const glm::mat4& matrix, int index);


		static void BeginScene(const Ref<EditorCamera>& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void DrawObjectRender3D(const Ref<VertexArray>& vertexArray);
		static void EndScene();

		static void BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model, int entityID);
		static void AfterDrawEntity(const MaterialComponent& material);

		static void SetMaterial(const MaterialComponent& material, const glm::mat4& modelMatrix, int entityID);
		static void DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID);
		static void DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID);
		static void DrawModdelMesh(const MaterialComponent& material, const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID);
		static void DrawLineBoxAABB(const BoxAABB& aabb, const glm::mat4& modelMatrix, int entityID);


		static void AktivePolyGunMode(bool active = true);
		static void SetDethTest(bool active = true);
		static void SetFace(CallFace callFace = CallFace::None);

		static void DrawMesh(const Ref<VertexArray>& vertexArray);
		static void DrawMeshStrips(const Ref<VertexArray>& vertexArray);
		static void DrawLine(const Ref<VertexArray>& vertexArray);
		static void DrawLineLoop(const Ref<VertexArray>& vertexArray);
		static void DrawPoints(const Ref<VertexArray>& vertexArray);
		static void DrawPatches(const Ref<VertexArray>& vertexArray);

		static void DrawError();

		static void BeginFrame();
		static void EndeFrame();

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


		};

		static void ResetStats();
		static Statistics GetStats();
	};
}
