#pragma once
#include <Rynex/Core/Range.h>
#include <Rynex/Renderer/RendererAPI.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/Camera/Camera.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Renderer/Camera/CameraBufferLayouts.h>
#include <Rynex/Renderer/Camera/CamerRenderPack.h>
#include <Rynex/Renderer/Rendering/StoreSubmite.h>
#include <Rynex/Renderer/Rendering/PiplineVec.h>
#include <Rynex/Renderer/Rendering/DrawContext.h>


#define RY_SHADOW_COUNT 1
#define RY_PILINE_STAIC_COUNT RY_SHADOW_COUNT + 1
#define RY_VIEWPORT_PASS 0
#define RY_RENERER_DESIGN_CURENT_MAIN
// #define RY_RENDERPASS_DATA_ELEMENT_ARRAY

namespace Rynex {

	class Renderer3D;

	namespace RendererGlobleResourceValues
	{
		enum
		{
			None = 0
			, FromOtherRenderTarget = BIT(0)
			, FromOtherSpecifEntiity = BIT(1)

			, CameraModelMatrix = BIT(2) // model don't mean from 3D-Model, it means camer tranfomrtion matrix befor its inversed to view matrix.
			, CameraViewMatrix = BIT(3)
			, CameraProjtionMatrix = BIT(4)

			, CameraViewProjtionMatrix = BIT(6)
			, CameraPostion = BIT(7)
			, CameraDriection = BIT(8) // normielzed
			, CameraScaledTranstionMatrix = BIT(9) // is am matrix that, can be used for shadow Texture mapping, it tranfomrs all values in a texture st-Coord ( most pelpule woude say uv-coord, but its whrong 0-1 is st-Coord ) space.


			, DealtTimeMilisecounds = BIT(10)
			, AlphaTimeMilisecounds = BIT(11)

			, DealtTimeSecounds = BIT(12)
			, AlphaTimeSecounds = BIT(13)


			, RenderTexture = BIT(14)

			, DynamicDataStruct = BIT(15)
		};
	};

	enum class RendereGlobleResurce
	{
		None = 0,


	};

	struct RenderSettings
	{

		int sceneRenderMode = 0;
		float gammaCorection = 2.2f;

		bool useSceneRenderModeInMainPass = true;
		bool gammaBackgroundCorction = true;

		bool drawPiplinesFromRenderTarget = false;
		bool sortBeforDrawFromRenderTarget = false;
		bool drawPiplinesFromRenderPass = false;
		bool drawShaderDrawListFromRenderTarget = false;
		bool drawShaderDrawListFromRenderPass = true;

		bool drawRenderProxy = true;// true;
		bool submiteSceneEntityTo3DRender = false; // false;
	};

	struct CameraPackege
	{
		glm::mat4 ViewMatrix;
		glm::mat4 ProjetionMatrix;
		glm::mat4 ViewProjectionMatrix; // VP
		// glm::mat4 ScaledTranfomrViewProjectionMatrix; // Matrix: STVP  that is M = VP * 0.5 + 0.5, for texure world space
		glm::vec4 Postion;
		glm::vec4 Direction;
	};
#ifdef RY_RENDERPASS_DATA_ELEMENT_ARRAY
	struct RenderPassPtr;

	struct RenderPassVec
	{
		Memory::StoreSubmite<std::string>			NameVec;
		Memory::StoreSubmite<glm::mat4>				ModelMatrixVec;
		Memory::StoreSubmite<CameraPackege>			CameraDataPackegeVec;
		Memory::StoreSubmite<glm::ivec4>			ImageSizeVec;
		Memory::StoreSubmite<glm::mat4>				DebugeMatrixVec;
		Memory::StoreSubmite<std::array<Plane, 6>>	BoundingArrayVec;


		Memory::StoreSubmite<Ref<RenderTarget>>		TargetVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	ViewMatrixUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	ProjetionMatrixUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	ProjetionViewMatrixUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	PostionUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	CameraDataPackBufferUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	ImageSizeUBVec;
		Memory::StoreSubmite<Ref<UniformBuffer>>	DirectionUBVec;

		RenderPassVec() = default;
		RenderPassVec(const RenderPassVec&) = default;

		void Incroment()
		{
			NameVec.Incroment();
			ModelMatrixVec.Incroment();
			CameraDataPackegeVec.Incroment();
			ImageSizeVec.Incroment();
			DebugeMatrixVec.Incroment();
			BoundingArrayVec.Incroment();
			TargetVec.Incroment();
			ViewMatrixUBVec.Incroment();;
			ProjetionMatrixUBVec.Incroment();;
			ProjetionViewMatrixUBVec.Incroment();;
			PostionUBVec.Incroment();
			CameraDataPackBufferUBVec.Incroment();
			ImageSizeUBVec.Incroment();
			DirectionUBVec.Incroment();
		}
		void Deincroment()
		{
			NameVec.Deincroment();
			ModelMatrixVec.Deincroment();
			CameraDataPackegeVec.Deincroment();
			ImageSizeVec.Deincroment();
			DebugeMatrixVec.Deincroment();
			BoundingArrayVec.Deincroment();
			TargetVec.Deincroment();
			ViewMatrixUBVec.Deincroment();;
			ProjetionMatrixUBVec.Deincroment();;
			ProjetionViewMatrixUBVec.Deincroment();;
			PostionUBVec.Deincroment();
			CameraDataPackBufferUBVec.Deincroment();
			ImageSizeUBVec.Deincroment();
			DirectionUBVec.Deincroment();
		}

		void Reset()
		{
			NameVec.Reset();
			ModelMatrixVec.Reset();
			CameraDataPackegeVec.Reset();
			ImageSizeVec.Reset();
			DebugeMatrixVec.Reset();
			BoundingArrayVec.Reset();
			TargetVec.Reset();
			ViewMatrixUBVec.Reset();;
			ProjetionMatrixUBVec.Reset();;
			ProjetionViewMatrixUBVec.Reset();;
			PostionUBVec.Reset();
			CameraDataPackBufferUBVec.Reset();
			ImageSizeUBVec.Reset();
			DirectionUBVec.Reset();
		}

		void Check()
		{
			NameVec.Check();
			ModelMatrixVec.Check();
			CameraDataPackegeVec.Check();
			ImageSizeVec.Check();
			DebugeMatrixVec.Check();
			BoundingArrayVec.Check();
			TargetVec.Check();
			ViewMatrixUBVec.Check();;
			ProjetionMatrixUBVec.Check();;
			ProjetionViewMatrixUBVec.Check();;
			PostionUBVec.Check();
			CameraDataPackBufferUBVec.Check();
			ImageSizeUBVec.Check();
			DirectionUBVec.Check();
		}

		void Destroy()
		{
			NameVec.Destroy();
			ModelMatrixVec.Destroy();
			CameraDataPackegeVec.Destroy();
			ImageSizeVec.Destroy();
			DebugeMatrixVec.Destroy();
			BoundingArrayVec.Destroy();
			TargetVec.Destroy();
			ViewMatrixUBVec.Destroy();
			ProjetionMatrixUBVec.Destroy();
			ProjetionViewMatrixUBVec.Destroy();
			PostionUBVec.Destroy();
			CameraDataPackBufferUBVec.Destroy();
			ImageSizeUBVec.Destroy();
			DirectionUBVec.Destroy();
		}

		RenderPassPtr GetCurent();
	};

	struct RenderPassPtr
	{
		std::string* Name = nullptr;
		glm::mat4* ModelMatrix = nullptr;
		CameraPackege* CameraDataPackege = nullptr;
		glm::ivec4* ImageSize = nullptr;
		glm::mat4* DebugeMatrix = nullptr;
		std::array<Plane, 6>* BoundingArray = nullptr;

		Ref<RenderTarget>* Target = nullptr;
		Ref<UniformBuffer>* ViewMatrixUB = nullptr;
		Ref<UniformBuffer>* ProjetionMatrixUB = nullptr;
		Ref<UniformBuffer>* ProjetionViewMatrixUB = nullptr;
		Ref<UniformBuffer>* PostionUB = nullptr;
		Ref<UniformBuffer>* CameraDataPackBufferUB = nullptr;
		Ref<UniformBuffer>* ImageSizeUB = nullptr;
		Ref<UniformBuffer>* DirectionUB = nullptr;

		RenderPass() = default;
		RenderPass(const RenderPass&) = default;

		void SetPtrFromPos(RenderPassVec& passVec)
		{
			passVec.Check();

			Name = passVec.NameVec.GetDataEndPtr();
			ModelMatrix = passVec.ModelMatrixVec.GetDataEndPtr();
			CameraDataPackege = passVec.CameraDataPackegeVec.GetDataEndPtr();
			ImageSize = passVec.ImageSizeVec.GetDataEndPtr();
			DebugeMatrix = passVec.DebugeMatrixVec.GetDataEndPtr();
			BoundingArray = passVec.BoundingArrayVec.GetDataEndPtr();
			Target = passVec.TargetVec.GetDataEndPtr();
			ViewMatrixUB = passVec.ViewMatrixUBVec.GetDataEndPtr();
			ProjetionMatrixUB = passVec.ProjetionMatrixUBVec.GetDataEndPtr();
			ProjetionViewMatrixUB = passVec.ProjetionViewMatrixUBVec.GetDataEndPtr();
			PostionUB = passVec.PostionUBVec.GetDataEndPtr();
			CameraDataPackBufferUB = passVec.CameraDataPackBufferUBVec.GetDataEndPtr();
			ImageSizeUB = passVec.ImageSizeUBVec.GetDataEndPtr();
			DirectionUB = passVec.DirectionUBVec.GetDataEndPtr();
		}
	};

	RenderPassPtr RenderPassVec::GetCurent()
	{
		RenderPassPtr renderPassPtr;
		renderPassPtr.SetPtrFromPos(*this);
		return renderPassPtr;
	}
#else
	
	struct RenderPass
	{
		std::string Name;
		glm::mat4 ModelMatrix;
		CameraPackege CameraDataPackege;
		glm::ivec4 ImageSize;
		glm::mat4 DebugeMatrix;
		std::array<Plane, 6> BoundingArray;
		PiplineRefBaseVec PiplineRefBaseVec;
		std::vector<ShaderDrawResource> shaderDrawVec;

		Ref<RenderTarget> Target;
		Ref<UniformBuffer> ViewMatrixUB;
		Ref<UniformBuffer> ProjetionMatrixUB;
		Ref<UniformBuffer> ProjetionViewMatrixUB;
		Ref<UniformBuffer> PostionUB;
		Ref<UniformBuffer> CameraDataPackBufferUB;
		Ref<UniformBuffer> ImageSizeUB;
		Ref<UniformBuffer> DirectionUB;

		RenderPass() = default;
		RenderPass(const RenderPass&) = default;
	};
	using RenderPassPtr = RenderPass*;
#endif
	struct ViewPassData
	{
		glm::mat4 ProjetionMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjetionViewMatrix;
		glm::mat4 InverseProjetionViewMatrix;
		glm::vec3 Postion;
		glm::vec4 BackGroundColor;
		glm::ivec4 ViewSpace;	// w/z = withe/heigth | x/y = offset(x/y)
		int Modes;
		float GammeCorection = 2.2f;
		bool RenderEcheFrame = true;
		bool RenderFrame = true;

		Ref<Framebuffer> FrameBuffer;

		ViewPassData(const glm::mat4& projetionMatrix, const glm::mat4& viewMatrix, const glm::vec3& postion,
			const glm::vec4& backGroundColor, const glm::ivec4& viewSpace,
			int modes, float gammeCorection, const Ref<Framebuffer>& frameBuffer,
			bool renderEcheFrame = true, bool renderFrame = true)
			: ProjetionMatrix(projetionMatrix)
			, ViewMatrix(viewMatrix)
			, ProjetionViewMatrix(projetionMatrix* viewMatrix)
			, InverseProjetionViewMatrix(glm::mat4(0.0f))
			, Postion(postion)
			, BackGroundColor(backGroundColor)
			, ViewSpace(viewSpace)
			, Modes(modes)
			, GammeCorection(gammeCorection)
			, FrameBuffer(frameBuffer)
			, RenderEcheFrame(renderEcheFrame)
			, RenderFrame(renderFrame)
		{
			InverseProjetionViewMatrix = glm::inverse(ProjetionViewMatrix);
		}
	};
	

	enum class ViewPassType
	{
		None = 0,
		DepthTest,
		Shadow,
		RenderTarget,
		SlectedView,
		Custemiced,
		MainView
	};

	struct StatusRenderPasses
	{
		int64_t TimeElpassed;
		uint32_t DrawCallsCount;
		uint32_t PiplineCallsCount;
	};

	struct StatusRender
	{
		StatusRenderPasses MainPass;
		std::vector<StatusRenderPasses> SecundaryPasses;

	};

	struct ViewPassStorage
	{
		ViewPassType Type;
		uint32_t Slot;
		CamerRenderPackages CameraPackege;

		ViewPassStorage()
			: Type(ViewPassType())
			, Slot(0u)
			, CameraPackege(CamerRenderPackages())
		{
		}

		RenderTarget& GetRenderTaget()
		{
			RenderTarget& target = CameraPackege.GetRenderTarget();
			return target;
		}

		Ref<UniformBuffer> GetUniformCamera()
		{
			CamerRenderPackages::CamerPackage& packeg = CameraPackege.GetCamerPackage();
			Ref<UniformBuffer> uniformBuffer = packeg.GetBuffer();
			return uniformBuffer;
		}

		Ref<UniformBuffer> GetUniformDisplay()
		{
			CamerRenderPackages::DisplayPackage& packeg = CameraPackege.GetDisplayPackage();
			Ref<UniformBuffer> uniformBuffer = packeg.GetBuffer();
			return uniformBuffer;
		}

		Ref<UniformBuffer> GetUniformDebug()
		{
			CamerRenderPackages::DebugCamerPackage& packeg = CameraPackege.GetDebugPackage();
			Ref<UniformBuffer> uniformBuffer = packeg.GetBuffer();
			return uniformBuffer;
		}
	};

	typedef std::vector<ViewPassStorage> VectorViewPassStorage;
	typedef Ref<VectorViewPassStorage> RefVectorViewPassStorage;

	struct ElementViewPassStorage
	{
		uint32_t PassSlot = 0;
		RefVectorViewPassStorage Vec;
	};

	typedef std::map<std::string, ElementViewPassStorage> MapRefVectorViewPassStorage;

	struct ObjectRendereIndex {
		uint32_t BatchIndex = 0xFFFFFFFFu;
		uint32_t PiplineIndex = 0xFFFFFFFFu;

		void Reset()
		{
			BatchIndex = 0xFFFFFFFFu;
			PiplineIndex = 0xFFFFFFFFu;
		}
	};

	struct SingleMeshRender : public MeshStatic::SingleObjectMeshData
	{
		glm::mat4 GlobelNodeMatrix;

		std::vector<ObjectRendereIndex> IndexVec;
		SingleMeshRender()
			: IndexVec()
			, GlobelNodeMatrix(glm::mat4(1.0f))
			, MeshStatic::SingleObjectMeshData(nullptr, nullptr, glm::mat4(0.0f), "Default-Name", 0xFFFFFFFFu, 0xFFFFFFFFu)

		{

		}


		SingleMeshRender(const MeshStatic::SingleObjectMeshData& singleObjectMeshRender)
			: IndexVec()
			, GlobelNodeMatrix(glm::mat4(1.0f))
			, MeshStatic::SingleObjectMeshData(singleObjectMeshRender)
		{
		}

		SingleMeshRender(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel, const glm::mat4& localeMatrix)
			: IndexVec()
			, GlobelNodeMatrix(glm::mat4(0.0f))
			, MeshStatic::SingleObjectMeshData(meshSingle, materiel, localeMatrix, "Set-No-Same", 0xFFFFFFFFu, 0xFFFFFFFFu)
		{
		}

		SingleMeshRender(const SingleMeshRender&) = default;

		void SetEntityMatrix(const glm::mat4& matrix)
		{
			GlobelNodeMatrix = matrix * LocaleCildrenMatrix;
		}


		void ResetIndex()
		{
			IndexVec.clear();
		}
	};


	typedef uint16_t RederModeType;

	namespace RenderMode {
		enum RenderMode : uint16_t
		{
			// Empty = BIT(31),
			None = 0,
			CallFace_None = BIT(0),
			CallFace_Front = BIT(1),
			CallFace_Back = BIT(2),
			CallFace_FrontBack = BIT(3),
			WireFrame = BIT(4),
			A_Buffer = BIT(5),
			Death_Buffer = BIT(6),
			Gamma = BIT(7),
			PrimitivReset = BIT(8)
		};
		static constexpr const size_t s_Count = 10;
	}

	class RYNEX_API Renderer
	{
	public:
		static void Init();
		static void InitEditor();
		static void Shutdown();
		static void ShutdownEditor();

		static void BeginFrame();
		static void EndeFrame();

		static StatusRender& GetStatus();
		static void ClearState();

		static void SetRenderPassNameMain(const std::string& name);
		static void SetRenderCameraMain(const glm::mat4& model, const Camera& camera);
		static void SetRenderTargetMain(const Ref<RenderTarget>& target);
		// imgeSize [ Size(x,y) / Offset(z,w) ]
		static void SetViewSizeMain(const glm::ivec4& imgeSize);

		static DrawContext& GetDrawContext();


		static void SetOnMainCameraCurentCamera();
		static void SetOnCurentPassMainPass();
		static Ref<UniformBuffer>& GetPackegeCamerUniformMain();
		static Ref<UniformBuffer>& GetProjetionUniformMain();
		static Ref<UniformBuffer>& GetProjetionViewUniformMain();
		static Ref<UniformBuffer>& GetPostionUniformMain();
		static Ref<UniformBuffer>& GetImageSizeUniformMain();
		static Ref<UniformBuffer>& GetViewUniformMain();
		static PiplineRefBaseVec& GetRenderPiplinesMain();
		static std::vector<ShaderDrawResource>& GetShaderDrawResourceMain();
		static std::array<Plane, 6>& GetCamerFustremMain();
		static Ref<RenderTarget>& GetRenderTargetMain();
		static Ref<Framebuffer> GetFramebufferMain();
		static const CameraPackege& GetCameraPackegeMain();

		static void RenderingPassMain();
		static void ClearMainPiplines();
		static bool IsInsideMainViewFustrem(const AABB& aabb, const glm::mat4& modelMatrix);
		static void ResetMainRenderPassPiplines();

		static uint32_t GetCurentIndex();
		static bool IsCurentEmpty();
		static void SetNextCurentRenderPass(uint32_t& index);
		static void CheckCurentRenderPass();
		static void IncromentCurentRenderPass();
		static void ResetCurentRenderPassFrame();
		


		static bool SetCurentPassOnOldIndex(uint32_t index);
		static void SetCurentPassOnEnd();

		static void SetRenderPassNameCurent(const std::string& name);

		
		static void SetRenderCameraCurent(const glm::mat4& model, const Camera& camera);
		static void SetRenderCameraCurentUB();
		static void SetRenderTargetCurent(const Ref<RenderTarget>& target);

		// imgeSize [ Size(x,y) / Offset(z,w) ]
		static void SetViewSizeCurent(const glm::ivec4& imgeSize);
		static void SetViewSizeCurentUB();

		static Ref<UniformBuffer>& GetPackegeCamerUniformCurent();
		static Ref<UniformBuffer>& GetViewUniformCurent();
		static Ref<UniformBuffer>& GetProjetionUniformCurent();
		static Ref<UniformBuffer>& GetProjetionViewUniformCurent();
		static Ref<UniformBuffer>& GetPostionUniformCurent();
		static Ref<UniformBuffer>& GetImageSizeUniformCurent();
		static Ref<RenderTarget>& GetRenderTargetCurent();
		static Ref<Framebuffer> GetFramebufferCurent();
		static PiplineRefBaseVec& GetRenderPiplinesCurent();
		static std::vector<ShaderDrawResource>& GetShaderDrawResourceCurent();

		static std::array<Plane, 6>& GetCamerFustremCurent();
		static const CameraPackege& GetCameraPackegeCurent();
		static void ResetCurentRenderPassPiplines();

		static void RenderingPassCurent();
		static void ClearCurentPiplines();
		static bool IsInsideCurentViewFustrem(const AABB& aabb, const glm::mat4& modelMatrix);


		static void ForEchStoredPassedRenderPass(const std::function<void(const RenderPass& pass)>& forEchElementFunc);
		static void ForEchStoredPassedRenderPassRef(const std::function<void(RenderPass& pass)>& forEchElementFunc);
		static void ForEchStoredPassedRenderPassIndex(const std::function<void(const RenderPass& pass, uint32_t index)>& forEchElementFunc);
		static void ForEchStoredPassedRenderPassIndexRef(const std::function<void(RenderPass& pass, uint32_t index)>& forEchElementFunc);
		static void PassedRenderPassToStart();
		
		static void RenderSubmitSceneMain(); 

		static void OnWindowsResize(uint32_t width, uint32_t height);
		static void SetBackgroundGamma(bool mode);
		static bool GetBackgroundGamma();


		static void SetGammaValue(float gamma);
		static float GetGammaValue();

		static void SetSceneMode(int mode);
		static int GetSceneMode();

		static void SetMode(int mode);
		static int GetMode();

		static RenderSettings& GetRenderSettings();
		static bool IsSceneSubmite3DAktive();
		static bool IsInit() { return s_Init; }

		inline static RendererAPI::API GetAPI()
		{
			if (s_Init)
				return RendererAPI::GetAPI();
			return RendererAPI::API::None;
		}

	private:
		
		inline static void SetRenderPassNameFromRenderPass(RenderPass& renderPass, const std::string& name);
		inline static void SetRenderTargetFromRenderPass(RenderPass& renderPass, const Ref<RenderTarget>& target);
		
		inline static void InitFromRenderPassDisplayUB(RenderPass& renderPass);
		inline static void InitFromRenderPassCamerUB(RenderPass& renderPass);

		inline static void ShutdownRenderPass(RenderPass& renderPass);
		inline static void ShutdownFromRenderPassDisplayUB(RenderPass& renderPass);
		inline static void ShutdownFromRenderPassCamerUB(RenderPass& renderPass);

		inline static void SetFromRenderPassCamerData(RenderPass& renderPass, const Camera& camer, const glm::mat4& matrix);
		inline static void SetFromRenderPassCamerUB(RenderPass& renderPass);

		// imgeSize [ Size(x,y) / Offset(z,w) ]
		inline static void SetFromRenderPassDisplayData(RenderPass& renderPass, const glm::ivec4& imgeSize);
		inline static void SetFromRenderPassDisplayUB(RenderPass& renderPass);

		inline static Ref<UniformBuffer>& GetPackegeCamerUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetViewUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetProjetionUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetProjetionViewUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetPostionUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetImageSizeUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<RenderTarget>& GetRenderTargetFromRenderPass(RenderPass& renderPass);
		inline static Ref<Framebuffer> GetFramebufferFromRenderPass(RenderPass& renderPass);
		inline static std::array<Plane, 6>& GetCamerFustremFromRenderPass(RenderPass& renderPass);
		inline static glm::ivec4& GetImageSizeFromRenderPass(RenderPass& renderPass);
		inline static const CameraPackege& GetCameraPackegeFromRenderPass(RenderPass& renderPass);

		inline static const std::string& GetFromRenderPassName(RenderPass& renderPass);
		inline static bool IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const AABB& aabb, const glm::mat4& modelMatrix);
		inline static bool IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const Sphere& aabb, const glm::mat4& modelMatrix);
		inline static bool IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const Plane& plane, const glm::mat4& modelMatrix);
		inline static void RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass);
		inline static void RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, const int mode);
		inline static void ClearPiplinesFromRenderPass(RenderPass& renderPass);
		inline static void ResetFromRenderPassRenderPassPiplines(RenderPass& renderPass);
		inline static PiplineRefBaseVec& GetRenderPiplinesFromRenderPass(RenderPass& renderPass);
		inline static std::vector<ShaderDrawResource>& GetShaderDrawResourceFromRenderPass(RenderPass& renderPass);

		inline static void RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass);
		inline static void RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, int mode);

		inline static bool IsInsideFromRenderPassViewFustremAABB(const std::array<Plane, 6>& fustrem, const glm::vec4& max, const glm::vec4& min);

		inline static glm::vec4 GetGlobleEge(const glm::mat4& modelMatrix, const glm::vec3& ege);
		inline static bool IsAABBIntersectWithePlane(const Plane& plane, const glm::vec4& max, const glm::vec4& min);


		static void DrawRenderFromRenderPass(Ref<RenderTarget>& target);
		static void DrawRenderFromRenderPass(Ref<RenderTarget>& target, int mode);
		static void DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target);
		static void DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target, int mode);
		static void ClearRenderTargetFromRenderPass(Ref<RenderTarget>& target);

		static void ExtractFrustum(const glm::mat4& viewProj, std::array<Plane, 6>& planes);
		static Plane CaculateCorectViewFustremPlaneAdd(const glm::vec4& matrixColum, const glm::vec4& matrixRow);
		static Plane CaculateCorectViewFustremPlaneSub(const glm::vec4& matrixColum, const glm::vec4& matrixRow);
		static Plane CreateViewFustremPlane(glm::vec4& planeVec4);

		static void RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec);
		static void RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec, const int moode);


	private:
		inline static bool s_Init = false;
	private:
		friend Renderer3D;
	};
}
