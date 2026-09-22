#pragma once
#include <Rynex/Renderer/RendererAPI.h>
#include <Rynex/Renderer/Camera/Camera.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Renderer/Camera/CamerRenderPack.h>
#include <Rynex/Renderer/Rendering/StoreSubmite.h>
#include <Rynex/Renderer/Rendering/PiplineVec.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

#define RY_SHADOW_COUNT 1
#define RY_PILINE_STAIC_COUNT RY_SHADOW_COUNT + 1
#define RY_VIEWPORT_PASS 0
#define RY_RENERER_DESIGN_CURENT_MAIN
// #define RY_RENDERPASS_DATA_ELEMENT_ARRAY

namespace Rynex {

	class Renderer3D;

	namespace RendererGlobalResourceValues
	{
		enum
		{
			None = 0
			, FromOtherRenderTarget = BIT(0)
			, FromOtherSpecifEntity = BIT(1)

			, CameraModelMatrix = BIT(2) // model don't mean from 3D-Model, it means camer tranfomrtion matrix befor its inversed to view matrix.
			, CameraViewMatrix = BIT(3)
			, CameraProjectionMatrix = BIT(4)

			, CameraViewProjectionMatrix = BIT(6)
			, CameraPosition = BIT(7)
			, CameraDirection = BIT(8) // normalized
			, CameraScaledTranslationMatrix = BIT(9) // is am matrix that, can be used for shadow Texture mapping, it tranfomrs all values in a texture st-Coord ( most pelpule woude say uv-coord, but its whrong 0-1 is st-Coord ) space.

			, DealtTimeMilliseconds = BIT(10)
			, AlphaTimeMilliseconds = BIT(11)

			, DealtTimeSeconds = BIT(12)
			, AlphaTimeSeconds = BIT(13)


			, RenderTexture = BIT(14)

			, DynamicDataStruct = BIT(15)
		};
	};

	enum class RenderGlobalResecure
	{
		None = 0,
	};

	struct RenderSettings
	{

		int   m_SceneRenderMode = 0;
		float m_GammaCorrection = 2.2f;

		bool useSceneRenderModeInMainPass = true;
		bool m_GammaBackgroundCorrection = true;

		bool m_DrawPipelinesFromRenderTarget = false;
		bool m_SortBeforeDrawFromRenderTarget = false;// sortBeforeDrawFromRenderTarget
		bool m_DrawPipelinesFromRenderPass = false;
		bool m_DrawShaderDrawListFromRenderTarget = false;
		bool m_DrawShaderDrawListFromRenderPass = true;

		bool m_DrawRenderProxy = true;// true;
		bool m_SubmitSceneEntityTo3DRender = false; // false;
	};

	struct CameraPackege
	{
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewProjectionMatrix; // VP
		// glm::mat4 ScaledTransformViewProjectionMatrix; // Matrix: STVP  that is M = VP * 0.5 + 0.5, for texure world space
		glm::vec4 m_Position;
		glm::vec4 m_Direction;
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
		std::string m_Name;
		glm::mat4 m_ModelMatrix;
		CameraPackege m_CameraDataPackege;
		glm::ivec4 m_ImageSize;
		glm::mat4 m_DebugMatrix;
		std::array<Plane, 6> m_BoundingArray;
		PiplineRefBaseVec m_PiplineRefBaseVec;
		std::vector<ShaderDrawResource> m_ShaderDrawVec;

		Ref<RenderTarget> m_Target;
		Ref<UniformBuffer> m_ViewMatrixUB;
		Ref<UniformBuffer> m_ProjectionMatrixUB;
		Ref<UniformBuffer> m_ProjectionViewMatrixUB;
		Ref<UniformBuffer> m_PositionUB;
		Ref<UniformBuffer> m_CameraDataPackBufferUB;
		Ref<UniformBuffer> m_ImageSizeUB;
		Ref<UniformBuffer> m_DirectionUB;

		RenderPass() = default;
		RenderPass(const RenderPass&) = default;
	};
	using RenderPassPtr = RenderPass*;
#endif
	struct ViewPassData
	{
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionViewMatrix;
		glm::mat4 m_InverseProjectionViewMatrix;
		glm::vec3 m_Position;
		glm::vec4 m_BackGroundColor;
		glm::ivec4 m_ViewSpace;	// w/z = withe/heigth | x/y = offset(x/y)
		int m_Modes;
		float m_GammeCorrection = 2.2f;
		bool m_RenderEcheFrame = true;
		bool m_RenderFrame = true;

		Ref<Framebuffer> m_FrameBuffer;

		ViewPassData(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& postion,
			const glm::vec4& backGroundColor, const glm::ivec4& viewSpace,
			int modes, float gammeCorrection, const Ref<Framebuffer>& frameBuffer,
			bool renderEcheFrame = true, bool renderFrame = true)
			: m_ProjectionMatrix(projectionMatrix)
			, m_ViewMatrix(viewMatrix)
			, m_ProjectionViewMatrix(projectionMatrix * viewMatrix)
			, m_InverseProjectionViewMatrix(glm::mat4(0.0f))
			, m_Position(postion)
			, m_BackGroundColor(backGroundColor)
			, m_ViewSpace(viewSpace)
			, m_Modes(modes)
			, m_GammeCorrection(gammeCorrection)
			, m_RenderEcheFrame(renderEcheFrame)
			, m_RenderFrame(renderFrame)
			, m_FrameBuffer(frameBuffer)
		{
			m_InverseProjectionViewMatrix = glm::inverse(m_ProjectionViewMatrix);
		}
	};
	

	enum class ViewPassType
	{
		None = 0,
		DepthTest,
		Shadow,
		RenderTarget,
		SelectedView,
		Costumed,
		MainView
	};

	struct StatusRenderPasses
	{
		int64_t m_TimeElapsed;
		uint32_t m_DrawCallsCount;
		uint32_t m_PiplineCallsCount;
	};

	struct StatusRender
	{
		StatusRenderPasses m_MainPass;
		std::vector<StatusRenderPasses> m_SecondaryPasses;

	};

	struct ViewPassStorage
	{
		ViewPassType m_Type;
		uint32_t m_Slot;
		CamerRenderPackages m_CameraPackege;

		ViewPassStorage()
			: m_Type(ViewPassType())
			, m_Slot(0u)
			, m_CameraPackege(CamerRenderPackages())
		{
		}

		RenderTarget& GetRenderTaget()
		{
			RenderTarget& target = m_CameraPackege.GetRenderTarget();
			return target;
		}

		Ref<UniformBuffer> GetUniformCamera()
		{
			const CamerRenderPackages::CamerPackage& packed = m_CameraPackege.GetCamerPackage();
			Ref<UniformBuffer> uniformBuffer = packed.GetBuffer();
			return uniformBuffer;
		}

		Ref<UniformBuffer> GetUniformDisplay()
		{
			const CamerRenderPackages::DisplayPackage& packed = m_CameraPackege.GetDisplayPackage();
			Ref<UniformBuffer> uniformBuffer = packed.GetBuffer();
			return uniformBuffer;
		}

		Ref<UniformBuffer> GetUniformDebug()
		{
			const CamerRenderPackages::DebugCamerPackage& packed = m_CameraPackege.GetDebugPackage();
			Ref<UniformBuffer> uniformBuffer = packed.GetBuffer();
			return uniformBuffer;
		}
	};

	using VectorViewPassStorage = std::vector<ViewPassStorage>;
	using RefVectorViewPassStorage = Ref<VectorViewPassStorage>;

	struct ElementViewPassStorage
	{
		uint32_t m_PassSlot = 0;
		RefVectorViewPassStorage m_Vec;
	};

	using MapRefVectorViewPassStorage = std::map<std::string, ElementViewPassStorage>;

	struct ObjectRenderIndex {
		uint32_t m_BatchIndex = 0xFFFFFFFFu;
		uint32_t m_PiplineIndex = 0xFFFFFFFFu;

		void Reset()
		{
			m_BatchIndex = 0xFFFFFFFFu;
			m_PiplineIndex = 0xFFFFFFFFu;
		}
	};

	struct SingleMeshRender : MeshStatic::SingleObjectMeshData
	{
		glm::mat4 m_GlobalNodeMatrix;

		std::vector<ObjectRenderIndex> m_IndexVec;
		SingleMeshRender()
			: MeshStatic::SingleObjectMeshData(nullptr, nullptr, glm::mat4(0.0f), "Default-Name", 0xFFFFFFFFu, 0xFFFFFFFFu)
			, m_GlobalNodeMatrix(glm::mat4(1.0f))
			, m_IndexVec()

		{

		}


        explicit SingleMeshRender(const MeshStatic::SingleObjectMeshData& singleObjectMeshRender)
			: MeshStatic::SingleObjectMeshData(singleObjectMeshRender)
			, m_GlobalNodeMatrix(glm::mat4(1.0f))
			, m_IndexVec()
		{
		}

		SingleMeshRender(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel, const glm::mat4& localeMatrix)
			: MeshStatic::SingleObjectMeshData(meshSingle, materiel, localeMatrix, "Set-No-Same", 0xFFFFFFFFu, 0xFFFFFFFFu)
			, m_GlobalNodeMatrix(glm::mat4(0.0f))
			, m_IndexVec()
		{
		}

		SingleMeshRender(const SingleMeshRender&) = default;

		void SetEntityMatrix(const glm::mat4& matrix)
		{
			m_GlobalNodeMatrix = matrix * m_LocaleCildrenMatrix;
		}


		void ResetIndex()
		{
			m_IndexVec.clear();
		}
	};


	using RenderModeType = uint16_t;

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
		// imageSize [ Size(x,y) / Offset(z,w) ]
		static void SetViewSizeMain(const glm::ivec4& imageSize);




		static void SetOnMainCameraCurrentCamera();
		static void SetOnCurrentPassMainPass();
		static Ref<UniformBuffer>& GetPackegeCameraUniformMain();
		static Ref<UniformBuffer>& GetProjectionUniformMain();
		static Ref<UniformBuffer>& GetProjectionViewUniformMain();
		static Ref<UniformBuffer>& GetPositionUniformMain();
		static Ref<UniformBuffer>& GetImageSizeUniformMain();
		static Ref<UniformBuffer>& GetViewUniformMain();
		static PiplineRefBaseVec& GetRenderPiplineMain();
		static std::vector<ShaderDrawResource>& GetShaderDrawResourceMain();
		static std::array<Plane, 6>& GetCameraFrustumMain();
		static Ref<RenderTarget>& GetRenderTargetMain();
		static Ref<Framebuffer> GetFramebufferMain();
		static const CameraPackege& GetCameraPackegeMain();

		static void RenderingPassMain();
		static void ClearMainPipline();
		static bool IsInsideMainViewFrustum(const AABB& aabb, const glm::mat4& modelMatrix);
		static void ResetMainRenderPassPipline();

		static uint32_t GetCurrentIndex();
		static bool IsCurrentEmpty();
		static void SetNextCurrentRenderPass(uint32_t& index);
		static void CheckCurrentRenderPass();
		static void IncrementCurrentRenderPass();
		static void ResetCurrentRenderPassFrame();
		


		static bool SetCurrentPassOnOldIndex(uint32_t index);
		static void SetCurrentPassOnEnd();

		static void SetRenderPassNameCurrent(const std::string& name);

		
		static void SetRenderCameraCurrent(const glm::mat4& model, const Camera& camera);
		static void SetRenderCameraCurrentUB();
		static void SetRenderTargetCurrent(const Ref<RenderTarget>& target);

		// imageSize [ Size(x,y) / Offset(z,w) ]
		static void SetViewSizeCurrent(const glm::ivec4& imageSize);
		static void SetViewSizeCurrentUB();

		static Ref<UniformBuffer>& GetPackegeCameraUniformCurrent();
		static Ref<UniformBuffer>& GetViewUniformCurrent();
		static Ref<UniformBuffer>& GetProjectionUniformCurrent();
		static Ref<UniformBuffer>& GetProjectionViewUniformCurrent();
		static Ref<UniformBuffer>& GetPositionUniformCurrent();
		static Ref<UniformBuffer>& GetImageSizeUniformCurrent();
		static Ref<RenderTarget>& GetRenderTargetCurrent();
		static Ref<Framebuffer> GetFramebufferCurrent();
		static PiplineRefBaseVec& GetRenderPiplineCurrent();
		static std::vector<ShaderDrawResource>& GetShaderDrawResourceCurrent();

		static std::array<Plane, 6>& GetCameraFrustumCurrent();
		static const CameraPackege& GetCameraPackegeCurrent();
		static void ResetCurrentRenderPassPipline();

		static void RenderingPassCurrent();
		static void ClearCurrentPipline();
		static bool IsInsideCurrentViewFrustum(const AABB& aabb, const glm::mat4& modelMatrix);


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
		static bool IsSceneSubmit3DActive();
		static bool IsInit() { return s_Init; }
	    static bool IsEditorInit() { return s_EditorInit; }

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
		inline static void InitFromRenderPassCameraUB(RenderPass& renderPass);

		inline static void ShutdownRenderPass(RenderPass& renderPass);
		inline static void ShutdownFromRenderPassDisplayUB(RenderPass& renderPass);
		inline static void ShutdownFromRenderPassCameraUB(RenderPass& renderPass);

		inline static void SetFromRenderPassCameraData(RenderPass& renderPass, const Camera& camer, const glm::mat4& matrix);
		inline static void SetFromRenderPassCameraUB(RenderPass& renderPass);

		// imageSize [ Size(x,y) / Offset(z,w) ]
		inline static void SetFromRenderPassDisplayData(RenderPass& renderPass, const glm::ivec4& imgeSize);
		inline static void SetFromRenderPassDisplayUB(RenderPass& renderPass);

		inline static Ref<UniformBuffer>& GetPackegeCameraUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetViewUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetProjectionUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetProjectionViewUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetPositionUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<UniformBuffer>& GetImageSizeUniformFromRenderPass(RenderPass& renderPass);
		inline static Ref<RenderTarget>& GetRenderTargetFromRenderPass(RenderPass& renderPass);
		inline static Ref<Framebuffer> GetFramebufferFromRenderPass(RenderPass& renderPass);
		inline static std::array<Plane, 6>& GetCameraFrustumFromRenderPass(RenderPass& renderPass);
		inline static glm::ivec4& GetImageSizeFromRenderPass(RenderPass& renderPass);
		inline static const CameraPackege& GetCameraPackegeFromRenderPass(RenderPass& renderPass);

		inline static const std::string& GetFromRenderPassName(RenderPass& renderPass);
		inline static bool IsInsideFromRenderPassViewFrustum(RenderPass& renderPass, const AABB& aabb, const glm::mat4& modelMatrix);
		inline static bool IsInsideFromRenderPassViewFrustum(RenderPass& renderPass, const Sphere& aabb, const glm::mat4& modelMatrix);
		inline static bool IsInsideFromRenderPassViewFrustum(RenderPass& renderPass, const Plane& plane, const glm::mat4& modelMatrix);
		inline static void RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass);
		inline static void RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, const int mode);
		inline static void ClearPiplineFromRenderPass(RenderPass& renderPass);
		inline static void ResetFromRenderPassRenderPassPipline(RenderPass& renderPass);
		inline static PiplineRefBaseVec& GetRenderPiplineFromRenderPass(RenderPass& renderPass);
		inline static std::vector<ShaderDrawResource>& GetShaderDrawResourceFromRenderPass(RenderPass& renderPass);

		inline static void RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass);
		inline static void RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, int mode);

		inline static bool IsInsideFromRenderPassViewFrustumAABB(const std::array<Plane, 6>& fustrem, const glm::vec4& max, const glm::vec4& min);

		inline static glm::vec4 GetGlobalEge(const glm::mat4& modelMatrix, const glm::vec3& ege);
		inline static bool IsAABBIntersectWithePlane(const Plane& plane, const glm::vec4& max, const glm::vec4& min);


		static void DrawRenderFromRenderPass(Ref<RenderTarget>& target);
		static void DrawRenderFromRenderPass(Ref<RenderTarget>& target, int mode);
		static void DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target);
		static void DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target, int mode);
		static void ClearRenderTargetFromRenderPass(Ref<RenderTarget>& target);

		static void ExtractFrustum(const glm::mat4& viewProj, std::array<Plane, 6>& planes);
		static Plane CalculateCorrectViewFrustumPlaneAdd(const glm::vec4& matrixColum, const glm::vec4& matrixRow);
		static Plane CalculateCorrectViewFrustumPlaneSub(const glm::vec4& matrixColum, const glm::vec4& matrixRow);
		static Plane CreateViewFrustumPlane(glm::vec4& planeVec4);

		static void RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec);
		static void RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec, const int moode);



		inline static bool s_Init = false;
	    inline static bool s_EditorInit = false;
	// private friend class --------------------------------------------------------------------------------------------------
		friend Renderer3D;
	};
}
