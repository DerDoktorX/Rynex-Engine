#include "rypch.h"
#include "Renderer.h"

#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/Text/Font.h>

#include <Rynex/Renderer/RenderCommand.h>
#include <Rynex/Renderer/Rendering/Render2D/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

#define RY_MULTY_IMAGE_RENDERING 1

namespace Rynex {




	struct Storage
	{
#ifdef RY_RENDERPASS_DATA_ELEMENT_ARRAY
		RenderPassVec AllRenderPassStorage;
		RenderPass MainRenderPassStorage;
#else
		RenderPass MainRenderPassStorage;
		Memory::StoreSubmite<RenderPass> RenderPassStorage;
#endif
		DrawContext drawContext;
	};

	

	struct RenderStorage
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
		std::array<Plane, 6> BoundingArray;



		Ref<UniformBuffer> CamerUB;
		Ref<UniformBuffer> DisplayUB;
	};


	static Storage s_Storage;
	static RenderSettings s_Settings;

	static StatusRender s_State;


	void Renderer::Init()
	{
		s_Init = true;

#ifdef RY_RENDERPASS_DATA_ELEMENT_ARRAY
		RenderPassVec allRenderPassStorage;
		allRenderPassStorage.Check();
#else
		RenderPass& mainViewPass = s_Storage.MainRenderPassStorage;
		s_Storage.RenderPassStorage.Check();
#endif
		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
	}

	void Renderer::InitEditor()
	{
		Renderer2D::InitEditor();
		Renderer3D::InitEditor();
	}





	void Renderer::Shutdown()
	{
		AssetManager::Shutdown();
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
		Texture::Shutdown();
		Shader::Shutdown();
		Font::ResetDefault();
		RenderCommand::Shutdown();
#ifndef RY_RENDERPASS_DATA_ELEMENT_ARRAY
		Renderer::ShutdownRenderPass(s_Storage.MainRenderPassStorage);

		for (RenderPass& renderPass : s_Storage.RenderPassStorage)
		{
			Renderer::ShutdownRenderPass(renderPass);
		}
		s_Storage.RenderPassStorage.Destroy();
		s_Storage.drawContext.Clear();

#else
		for (RenderPass& renderPassPtr : s_Storage.AllRenderPassStorage)
		{
			Renderer::ShutdownRenderPass(renderPassPtr);
		}
		s_Storage.RenderPassStorage.Destroy();
		s_State.SecundaryPasses.clear();
#endif

		s_Init = false;

	}

	void Renderer::ShutdownEditor()
	{
		Renderer2D::ShutdownEditor();
		Renderer3D::ShutdownEditor();
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::EndeFrame()
	{
	}

	StatusRender& Renderer::GetStatus()
	{
		return s_State;
	}

	void Renderer::ClearState()
	{
		s_State.SecundaryPasses.clear();
	}


#pragma region Main

	DrawContext& Renderer::GetDrawContext()
	{
		return s_Storage.drawContext;
	}

	void Renderer::SetOnMainCameraCurentCamera()
	{

		RenderPass& passMain = s_Storage.MainRenderPassStorage;
		RenderPass& passCurent = s_Storage.RenderPassStorage.GetDataRef();
		passCurent.BoundingArray = passMain.BoundingArray;
		passCurent.CameraDataPackBufferUB = passMain.CameraDataPackBufferUB;
		passCurent.DirectionUB = passMain.DirectionUB;
		passCurent.ModelMatrix = passMain.ModelMatrix;
		passCurent.CameraDataPackege = passMain.CameraDataPackege;
		passCurent.ProjetionViewMatrixUB = passMain.ProjetionViewMatrixUB;
	}

	void Renderer::SetOnCurentPassMainPass()
	{
		RenderPass& passMain = s_Storage.MainRenderPassStorage;
		RenderPass* passCurentPtr = s_Storage.RenderPassStorage.GetDataEndPtr();
		*passCurentPtr = passMain;
	}

	void Renderer::SetRenderPassNameMain(const std::string& name)
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		SetRenderPassNameFromRenderPass(pass, name);
	}

	void Renderer::SetRenderCameraMain(const glm::mat4& model, const Camera& camera)
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		SetFromRenderPassCamerData(pass, camera, model);
		SetFromRenderPassCamerUB(pass);
	}

	void Renderer::SetRenderTargetMain(const Ref<RenderTarget>& target)
	{
		const glm::vec4& viewSize = target->GetRenderViewSize();
		glm::ivec4 viewSizeInt = static_cast<glm::ivec4>(viewSize);

		RenderPass& pass = s_Storage.MainRenderPassStorage;
		SetRenderTargetFromRenderPass(pass, target);
		SetViewSizeMain(viewSizeInt);
	}

	void Renderer::SetViewSizeMain(const glm::ivec4& imgeSize)
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;

		SetFromRenderPassDisplayData(pass, imgeSize);
		SetFromRenderPassDisplayUB(pass);
	}

	Ref<UniformBuffer>& Renderer::GetPackegeCamerUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetPackegeCamerUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetProjetionUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetProjetionUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetProjetionViewUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetProjetionViewUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetPostionUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetPostionUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetImageSizeUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetImageSizeUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetViewUniformMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetViewUniformFromRenderPass(pass);
	}

	PiplineRefBaseVec& Renderer::GetRenderPiplinesMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetRenderPiplinesFromRenderPass(pass);
	}

	std::vector<ShaderDrawResource>& Renderer::GetShaderDrawResourceMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetShaderDrawResourceFromRenderPass(pass);
	}

	std::array<Plane, 6>& Renderer::GetCamerFustremMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetCamerFustremFromRenderPass(pass);
	}

	Ref<RenderTarget>& Renderer::GetRenderTargetMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetRenderTargetFromRenderPass(pass);
	}

	Ref<Framebuffer> Renderer::GetFramebufferMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetFramebufferFromRenderPass(pass);
	}

	const CameraPackege& Renderer::GetCameraPackegeMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return GetCameraPackegeFromRenderPass(pass);
	}

	void Renderer::RenderingPassMain()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		StatusRenderPasses& status = s_State.MainPass;
		int mode = s_Settings.sceneRenderMode;
		if (s_Settings.drawRenderProxy)
		{
			Renderer3D::RenderProxysMain();
		}
		if(s_Settings.useSceneRenderModeInMainPass)
		{
			RenderingPassFromRenderPass(pass, status, mode);
		}
		else
		{
			RenderingPassFromRenderPass(pass, status);
		}
	}

	void Renderer::ResetMainRenderPassPiplines()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		ResetFromRenderPassRenderPassPiplines(pass);
	}

	void Renderer::ClearMainPiplines()
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		ClearPiplinesFromRenderPass(pass);
	}

	bool Renderer::IsInsideMainViewFustrem(const AABB& aabb, const glm::mat4& modelMatrix)
	{
		RenderPass& pass = s_Storage.MainRenderPassStorage;
		return IsInsideFromRenderPassViewFustrem(pass, aabb, modelMatrix);
	}


#pragma endregion	


#pragma region Curent

	uint32_t Renderer::GetCurentIndex()
	{
		Memory::StoreSubmite<RenderPass>& renderPass = s_Storage.RenderPassStorage;
		uint32_t index = renderPass.GetCurentCount();
		return index;
	}

	bool Renderer::IsCurentEmpty()
	{
		return s_Storage.RenderPassStorage.IsEmpty();
	}

	bool Renderer::SetCurentPassOnOldIndex(uint32_t index)
	{
		RY_REMBER_FUNC_CHANGE("Finsh Implemt from Func");
		return false;
	}

	void Renderer::SetCurentPassOnEnd()
	{
		RenderPass* ptr = s_Storage.RenderPassStorage.GetDataEndPtr();
	}



	void Renderer::SetRenderPassNameCurent(const std::string& name)
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		uint32_t index = s_Storage.RenderPassStorage.GetCurentCount<uint32_t>();
		SetRenderPassNameFromRenderPass(pass, name);

	}

	void Renderer::SetNextCurentRenderPass(uint32_t& index)
	{
		CheckCurentRenderPass();
		RenderPass* ptr = s_Storage.RenderPassStorage.GetDataEndPtr(index);
	}
	
	void Renderer::ResetCurentRenderPassFrame()
	{
		s_Storage.RenderPassStorage.Reset();
	}

	void Renderer::CheckCurentRenderPass()
	{
		s_Storage.RenderPassStorage.Check();
	}

	void Renderer::IncromentCurentRenderPass()
	{		
		s_Storage.RenderPassStorage.Incroment();
	}

	void Renderer::SetRenderCameraCurent(const glm::mat4& model, const Camera& camera)
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		SetFromRenderPassCamerData(pass, camera, model);
	}

	void Renderer::SetRenderCameraCurentUB()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		Renderer::SetFromRenderPassCamerUB(pass);
	}

	void Renderer::SetRenderTargetCurent(const Ref<RenderTarget>& target)
	{
		const glm::vec4& viewSize = target->GetRenderViewSize();
		glm::ivec4 viewSizeInt = static_cast<glm::ivec4>(viewSize);

		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		SetRenderTargetFromRenderPass(pass, target);
		SetViewSizeCurent(viewSizeInt);
	}

	void Renderer::SetViewSizeCurent(const glm::ivec4& imgeSize)
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();

		SetFromRenderPassDisplayData(pass, imgeSize);
		SetFromRenderPassDisplayUB(pass);
	}

	void Renderer::SetViewSizeCurentUB()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();

		SetFromRenderPassDisplayUB(pass);
	}

	Ref<UniformBuffer>& Renderer::GetPackegeCamerUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetPackegeCamerUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetViewUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetViewUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetProjetionUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetProjetionUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetProjetionViewUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetProjetionViewUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetPostionUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetPostionUniformFromRenderPass(pass);
	}

	Ref<UniformBuffer>& Renderer::GetImageSizeUniformCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetImageSizeUniformFromRenderPass(pass);
	}

	Ref<RenderTarget>& Renderer::GetRenderTargetCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetRenderTargetFromRenderPass(pass);
	}

	Ref<Framebuffer> Renderer::GetFramebufferCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetFramebufferFromRenderPass(pass);
	}

	PiplineRefBaseVec& Renderer::GetRenderPiplinesCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetRenderPiplinesFromRenderPass(pass);
	}

	std::vector<ShaderDrawResource>& Renderer::GetShaderDrawResourceCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetShaderDrawResourceFromRenderPass(pass);
	}

	std::array<Plane, 6>& Renderer::GetCamerFustremCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetCamerFustremFromRenderPass(pass);
	}

	const CameraPackege& Renderer::GetCameraPackegeCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return GetCameraPackegeFromRenderPass(pass);
	}

	void Renderer::ResetCurentRenderPassPiplines()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		ResetFromRenderPassRenderPassPiplines(pass);
	}

	bool Renderer::IsInsideCurentViewFustrem(const AABB& aabb, const glm::mat4& modelMatrix)
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		return IsInsideFromRenderPassViewFustrem(pass, aabb, modelMatrix);
	}

	void Renderer::ForEchStoredPassedRenderPass(const std::function<void(const RenderPass& pass)>& forEchElementFunc)
	{
		for (const RenderPass& pass : s_Storage.RenderPassStorage)
		{
			forEchElementFunc(pass);
		}
	}

	void Renderer::ForEchStoredPassedRenderPassRef(const std::function<void(RenderPass& pass)>& forEchElementFunc)
	{
		for (RenderPass& pass : s_Storage.RenderPassStorage)
		{
			forEchElementFunc(pass);
		}

	}

	void Renderer::ForEchStoredPassedRenderPassIndex(const std::function<void(const RenderPass& pass, uint32_t index)>& forEchElementFunc)
	{
		uint32_t index = 0u;
		for (const RenderPass& pass : s_Storage.RenderPassStorage)
		{
			forEchElementFunc(pass, index);
			index++;
		}
	}

	void Renderer::ForEchStoredPassedRenderPassIndexRef(const std::function<void(RenderPass& pass, uint32_t index)>& forEchElementFunc)
	{
		uint32_t index = 0u;
		for (RenderPass& pass : s_Storage.RenderPassStorage)
		{
			forEchElementFunc(pass, index);
			index++;
		}
	}

	void Renderer::PassedRenderPassToStart()
	{
		s_Storage.RenderPassStorage.Reset();
	}


	void Renderer::RenderingPassCurent()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		uint32_t index = s_Storage.RenderPassStorage.GetCurentCount();
		if (s_State.SecundaryPasses.size() <= index)
		{
			index = s_State.SecundaryPasses.size();
			s_State.SecundaryPasses.emplace_back(StatusRenderPasses{0,0,0});
		}
		StatusRenderPasses& status = s_State.SecundaryPasses.at(index);
		if (s_Settings.drawRenderProxy)
		{
			Renderer3D::RenderProxysCurent();
		}
		RenderingPassFromRenderPass(pass, status);
	}

	void Renderer::ClearCurentPiplines()
	{
		RenderPass& pass = s_Storage.RenderPassStorage.GetDataRef();
		ClearPiplinesFromRenderPass(pass);
	}

#pragma endregion	

	void Renderer::RenderSubmitSceneMain()
	{
		RY_PROFILE_FUNCTION();

		int renderMode = Renderer::GetMode();
		Renderer::RenderingPassMain();
		Renderer::ClearMainPiplines();
		Renderer::SetMode(renderMode);
		RenderCommand::RestPipline();
		Renderer3D::FrameFinshed();
	}

	void Renderer::OnWindowsResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewPort(0, 0, width, height);
	}

	void Renderer::SetBackgroundGamma(bool mode)
	{
		s_Settings.gammaBackgroundCorction = mode;
	}

	bool Renderer::GetBackgroundGamma()
	{
		return s_Settings.gammaBackgroundCorction;
	}

	void Renderer::SetGammaValue(float gamma)
	{
		s_Settings.gammaCorection = gamma;
	}

	float Renderer::GetGammaValue()
	{
		return s_Settings.gammaCorection;
	}

	void Renderer::SetMode(int mode)
	{
		RenderCommand::SetMode(mode);
	}

	int Renderer::GetMode()
	{
		return RenderCommand::GetMode();
	}

	RenderSettings& Renderer::GetRenderSettings()
	{
		return s_Settings;
	}

	bool Renderer::IsSceneSubmite3DAktive()
	{
		return s_Settings.submiteSceneEntityTo3DRender;
	}

	void Renderer::SetSceneMode(int mode)
	{
		s_Settings.sceneRenderMode = mode;
	}

	int Renderer::GetSceneMode()
	{
		return s_Settings.sceneRenderMode;
	}



#pragma region RenderPassFunc


	inline void Renderer::SetRenderPassNameFromRenderPass(RenderPass& renderPass, const std::string& name)
	{
#ifdef RY_RENDERPASS_DATA_ELEMENT_ARRAY
		renderPass.
#else
		renderPass.Name = name;
#endif
	}

	inline void Renderer::SetRenderTargetFromRenderPass(RenderPass& renderPass, const Ref<RenderTarget>& target)
	{
		RY_DESTROY_REF(renderPass.Target);
		renderPass.Target = target;
	}


	inline void Renderer::RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass)
	{
		RY_SCOPE_TIMER(statePass.TimeElpassed);
		Ref<RenderTarget>& target = renderPass.Target;
		statePass.DrawCallsCount = target->GetDrawListCount();
		statePass.PiplineCallsCount = target->GetPilineBaseCount();

		DrawRenderFromRenderPass(target);
	}



	inline void Renderer::RenderFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, int mode)
	{
		RY_SCOPE_TIMER(statePass.TimeElpassed);
		Ref<RenderTarget>& target = renderPass.Target;
		statePass.DrawCallsCount = target->GetDrawListCount();
		statePass.PiplineCallsCount = target->GetPilineBaseCount();


		DrawRenderFromRenderPass(target, mode);
	}

	inline void Renderer::DrawRenderFromRenderPass(Ref<RenderTarget>& target)
	{
		target->BindFramebuffer();
		target->ClearFramebufferImges();
		target->ClearFramebufferDepth();

		if (s_Settings.sortBeforDrawFromRenderTarget)
		{
			target->SortePilineList();
		}
	}

	inline void Renderer::DrawRenderFromRenderPass(Ref<RenderTarget>& target, int mode)
	{
		DrawRenderFromRenderPass(target);
		DrawRenderTargetFromRenderPass(target, mode);
		ClearRenderTargetFromRenderPass(target);
	}

	inline void Renderer::DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target)
	{
		target->DrawPilines();
		target->DrawBufferList();
#if 0
		target->DrawPilinesBase();
#endif
		target->SortePilineAlphaList();
		target->DrawAlphaPilines();
	}

	inline void Renderer::DrawRenderTargetFromRenderPass(Ref<RenderTarget>& target, int mode)
	{
		target->DrawPilines(mode);
		target->DrawBufferList(mode);
#if 0
		target->DrawPilinesBase(mode);
#endif
		target->SortePilineAlphaList();
		target->DrawAlphaPilines(mode);
	}

	inline void Renderer::ClearRenderTargetFromRenderPass(Ref<RenderTarget>& target)
	{
		target->ClearPilines();
		target->ClearAlphaPilines();
	}

	inline void Renderer::InitFromRenderPassDisplayUB(RenderPass& renderPass)
	{
#ifdef RY_OPENGL_OLD_UNIFORM
		renderPass.ImageSizeUB = UniformBuffer::Create(
			&renderPass.ImageSize, sizeof(renderPass.ImageSize)
			, { {SDT::Int4, "ImageSize"} }, BufferDataUsage::DynamicDraw
		);
#else
		renderPass.ImageSizeUB = UniformBuffer::Create(
			&renderPass.ImageSize, sizeof(renderPass.ImageSize)
			, { {SDT::Int4, "ImageSize"} }, BufferFlag::Dynamic
		);
#endif
	}

	inline void Renderer::InitFromRenderPassCamerUB(RenderPass& renderPass)
	{
		CameraPackege* packeg = &renderPass.CameraDataPackege;
#ifdef RY_OPENGL_OLD_UNIFORM

		renderPass.ProjetionMatrixUB = UniformBuffer::Create(
			&packeg->ProjetionMatrix, sizeof(packeg->ProjetionMatrix)
			, { {SDT::Float4x4, "ProjetionMatrix"} }
			, BufferDataUsage::DynamicDraw
		);
		renderPass.ViewMatrixUB = UniformBuffer::Create(
			&packeg->ViewMatrix, sizeof(packeg->ViewMatrix)
			, { {SDT::Float4x4, "ViewMatrix"} }
			, BufferDataUsage::DynamicDraw
		);
		renderPass.ProjetionViewMatrixUB = UniformBuffer::Create(
			&packeg->ViewProjectionMatrix, sizeof(packeg->ViewProjectionMatrix)
			, { {SDT::Float4x4, "ViewProjectionMatrix"} }
			, BufferDataUsage::DynamicDraw
		);
		renderPass.PostionUB = UniformBuffer::Create(
			&packeg->Postion, sizeof(packeg->Postion)
			, { {SDT::Float4, "Postion"} }
			, BufferDataUsage::DynamicDraw
		);
		renderPass.DirectionUB = UniformBuffer::Create(
			&packeg->Direction, sizeof(packeg->Direction)
			, { {SDT::Float4, "Direction"} }
			, BufferDataUsage::DynamicDraw
		);

		renderPass.CameraDataPackBufferUB = UniformBuffer::Create(
			packeg, sizeof(CameraPackege)
			, {
				{ SDT::Float4x4,	"ViewMatrix" },
				{ SDT::Float4x4,	"ProjetionMatrix" },
				{ SDT::Float4x4,	"ViewProjectionMatrix" },
				{ SDT::Float4,		"Postion" },
				{ SDT::Float4,		"Direction" }
			}, BufferDataUsage::DynamicDraw
		);
#else
		renderPass.ProjetionMatrixUB = UniformBuffer::Create(
			&packeg->ProjetionMatrix, sizeof(packeg->ProjetionMatrix)
			, { {SDT::Float4x4, "ProjetionMatrix"} }
			, BufferFlag::Dynamic
		);
		renderPass.ViewMatrixUB = UniformBuffer::Create(
			&packeg->ViewMatrix, sizeof(packeg->ViewMatrix)
			, { {SDT::Float4x4, "ViewMatrix"} }
			, BufferFlag::Dynamic
		);
		renderPass.ProjetionViewMatrixUB = UniformBuffer::Create(
			&packeg->ViewProjectionMatrix, sizeof(packeg->ViewProjectionMatrix)
			, { {SDT::Float4x4, "ViewProjectionMatrix"} }
			, BufferFlag::Dynamic
		);
		renderPass.PostionUB = UniformBuffer::Create(
			&packeg->Postion, sizeof(packeg->Postion)
			, { {SDT::Float4, "Postion"} }
			, BufferFlag::Dynamic
		);
		renderPass.DirectionUB = UniformBuffer::Create(
			&packeg->Direction, sizeof(packeg->Direction)
			, { {SDT::Float4, "Direction"} }
			, BufferFlag::Dynamic
		);

		renderPass.CameraDataPackBufferUB = UniformBuffer::Create(
			packeg, sizeof(CameraPackege)
			, {
				{ SDT::Float4x4,	"ViewMatrix" },
				{ SDT::Float4x4,	"ProjetionMatrix" },
				{ SDT::Float4x4,	"ViewProjectionMatrix" },
				{ SDT::Float4,		"Postion" },
				{ SDT::Float4,		"Direction" }
			}, BufferFlag::Dynamic
		);
#endif

	}

	inline void Renderer::ShutdownRenderPass(RenderPass& renderPass)
	{
		ShutdownFromRenderPassDisplayUB(renderPass);
		ShutdownFromRenderPassCamerUB(renderPass);
		RY_DESTROY_REF(renderPass.Target);
		PiplineRefBaseVec& vec = renderPass.PiplineRefBaseVec;
		vec.Destroy();
		renderPass.Name.clear();
	}

	inline void Renderer::ShutdownFromRenderPassDisplayUB(RenderPass& renderPass)
	{
		RY_DESTROY_REF(renderPass.ImageSizeUB);
	}

	inline void Renderer::ShutdownFromRenderPassCamerUB(RenderPass& renderPass)
	{
		RY_DESTROY_REF(renderPass.ProjetionMatrixUB);
		RY_DESTROY_REF(renderPass.ProjetionViewMatrixUB);
		RY_DESTROY_REF(renderPass.ViewMatrixUB);
		RY_DESTROY_REF(renderPass.PostionUB);
		RY_DESTROY_REF(renderPass.DirectionUB);
		RY_DESTROY_REF(renderPass.CameraDataPackBufferUB);

	}

	inline void Renderer::SetFromRenderPassCamerData(RenderPass& renderPass, const Camera& camer, const glm::mat4& matrix)
	{
		renderPass.ModelMatrix = matrix;
		CameraPackege& packeg = renderPass.CameraDataPackege;
		packeg.ViewMatrix = glm::inverse(renderPass.ModelMatrix);
		packeg.Postion = glm::vec4(renderPass.ModelMatrix[3]);
		packeg.Direction = glm::vec4(
			packeg.ViewMatrix[0].z,
			packeg.ViewMatrix[1].z,
			packeg.ViewMatrix[2].z,
			0.0f
		); // NOT FORWARD DIRECTION
		packeg.ProjetionMatrix = camer.GetProjektion();
		// packeg.ProjetionViewMatrix = packeg.ViewMatrix * packeg.ProjetionMatrix;
		packeg.ViewProjectionMatrix = packeg.ProjetionMatrix * packeg.ViewMatrix;

		Renderer::ExtractFrustum(packeg.ViewProjectionMatrix, renderPass.BoundingArray);
	}

	inline void Renderer::SetFromRenderPassCamerUB(RenderPass& renderPass)
	{
		CameraPackege* packeg = &renderPass.CameraDataPackege;
		if (nullptr == renderPass.ProjetionMatrixUB)
		{
			InitFromRenderPassCamerUB(renderPass);
		}
		else
		{
			// Renderer::GetDrawContext().CreateBuffer("ProjetionMatrixUB", renderPass.ProjetionMatrixUB, renderPass.ProjetionMatrixUB->GetLayout(), 1u, false);
			// Renderer::GetDrawContext().CreateBuffer("ViewMatrixUB", renderPass.ViewMatrixUB, renderPass.ViewMatrixUB->GetLayout(), 1u, false);
			// Renderer::GetDrawContext().CreateBuffer("ProjetionViewMatrixUB", renderPass.ProjetionViewMatrixUB, renderPass.ProjetionViewMatrixUB->GetLayout(), 1u, false);
			// Renderer::GetDrawContext().CreateBuffer("PostionUB", renderPass.PostionUB, renderPass.PostionUB->GetLayout(), 1u, false);
			// Renderer::GetDrawContext().CreateBuffer("DirectionUB", renderPass.DirectionUB, renderPass.DirectionUB->GetLayout(), 1u, false);
			// Renderer::GetDrawContext().CreateBuffer("CameraDataPackBufferUB", renderPass.CameraDataPackBufferUB, renderPass.CameraDataPackBufferUB->GetLayout(), 1u, true);

			renderPass.ProjetionMatrixUB->SetData(&packeg->ProjetionMatrix, sizeof(packeg->ProjetionMatrix));
			renderPass.ViewMatrixUB->SetData(&packeg->ViewMatrix, sizeof(packeg->ViewMatrix));
			renderPass.ProjetionViewMatrixUB->SetData(&packeg->ViewProjectionMatrix, sizeof(packeg->ViewProjectionMatrix));
			renderPass.PostionUB->SetData(&packeg->Postion, sizeof(packeg->Postion));
			renderPass.DirectionUB->SetData(&packeg->Direction, sizeof(packeg->Direction));
			renderPass.CameraDataPackBufferUB->SetData(packeg, sizeof(*packeg));
		}
	}

	inline void Renderer::SetFromRenderPassDisplayData(RenderPass& renderPass, const glm::ivec4& imgeSize)
	{
		renderPass.ImageSize = imgeSize;
		if (nullptr != renderPass.Target)
		{
			renderPass.Target->ResizeView(imgeSize);
		}
	}

	inline void Renderer::SetFromRenderPassDisplayUB(RenderPass& renderPass)
	{
		if (nullptr == renderPass.ImageSizeUB)
		{
			InitFromRenderPassDisplayUB(renderPass);
		}
		else
		{
			renderPass.ImageSizeUB->SetData(&renderPass.ImageSize, sizeof(renderPass.ImageSize));
		}
	}

	inline Ref<UniformBuffer>& Renderer::GetPackegeCamerUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.CameraDataPackBufferUB)
			InitFromRenderPassCamerUB(renderPass);
		return renderPass.CameraDataPackBufferUB;
	}

	inline Ref<UniformBuffer>& Renderer::GetViewUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.ViewMatrixUB)
			InitFromRenderPassCamerUB(renderPass);
		return renderPass.ViewMatrixUB;
	}

	inline Ref<UniformBuffer>& Renderer::GetProjetionUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.ProjetionMatrixUB)
			InitFromRenderPassCamerUB(renderPass);
		return renderPass.ProjetionMatrixUB;
	}

	inline Ref<UniformBuffer>& Renderer::GetProjetionViewUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.ProjetionViewMatrixUB)
			InitFromRenderPassCamerUB(renderPass);
		return renderPass.ProjetionViewMatrixUB;
	}

	inline Ref<UniformBuffer>& Renderer::GetPostionUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.PostionUB)
			InitFromRenderPassCamerUB(renderPass);
		return renderPass.PostionUB;
	}

	inline Ref<UniformBuffer>& Renderer::GetImageSizeUniformFromRenderPass(RenderPass& renderPass)
	{
		if (nullptr == renderPass.ImageSizeUB)
			InitFromRenderPassDisplayUB(renderPass);
		return renderPass.ImageSizeUB;
	}

	inline Ref<RenderTarget>& Renderer::GetRenderTargetFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.Target;
	}

	inline Ref<Framebuffer> Renderer::GetFramebufferFromRenderPass(RenderPass& renderPass)
	{
		Ref<RenderTarget>& target = GetRenderTargetFromRenderPass(renderPass);
		return target->GetFramebuffer();
	}

	inline std::array<Plane, 6>& Renderer::GetCamerFustremFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.BoundingArray;
	}

	inline glm::ivec4& Renderer::GetImageSizeFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.ImageSize;
	}

	inline const CameraPackege& Renderer::GetCameraPackegeFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.CameraDataPackege;
	}

	inline const std::string& Renderer::GetFromRenderPassName(RenderPass& renderPass)
	{
		return renderPass.Name;
	}

	inline bool Renderer::IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const AABB& aabb, const glm::mat4& modelMatrix)
	{
		glm::vec4 max4 = GetGlobleEge(modelMatrix, aabb.Max);
		glm::vec4 min4 = GetGlobleEge(modelMatrix, aabb.Min);
		return IsInsideFromRenderPassViewFustremAABB(renderPass.BoundingArray, max4, min4);
	}

	inline bool Renderer::IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const Sphere& aabb, const glm::mat4& modelMatrix)
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	inline bool Renderer::IsInsideFromRenderPassViewFustrem(RenderPass& renderPass, const Plane& plane, const glm::mat4& modelMatrix)
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	inline glm::vec4 Renderer::GetGlobleEge(const glm::mat4& modelMatrix, const glm::vec3& ege)
	{
		glm::vec4 ege4 = modelMatrix * glm::vec4(ege, 1.0f);
		return ege4;
	}

	inline bool Renderer::IsAABBIntersectWithePlane(const Plane& plane, const glm::vec4& max, const glm::vec4& min)
	{
		glm::vec3 postion(
			plane.Normale.x < 0.0f ? max.x : min.x,
			plane.Normale.y < 0.0f ? max.y : min.y,
			plane.Normale.z < 0.0f ? max.z : min.z
		);

		float dotProduct = glm::dot(plane.Normale, postion);
		dotProduct += plane.Constant;

		bool isInsideViewFustrem = dotProduct >= 0.0f;
		return isInsideViewFustrem;
	}

	inline void Renderer::RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass)
	{
		RY_PROFILE_FUNCTION();
		RY_SCOPE_TIMER(statePass.TimeElpassed);
		Ref<RenderTarget>& renderTarget = GetRenderTargetFromRenderPass(renderPass);

		statePass.DrawCallsCount = renderTarget->GetDrawListCount();
		statePass.PiplineCallsCount = renderTarget->GetPilineBaseCount();

		renderTarget->BindFramebuffer();
		renderTarget->ClearFramebufferImges();
		renderTarget->ClearFramebufferDepth();



		if (s_Settings.drawPiplinesFromRenderTarget && s_Settings.sortBeforDrawFromRenderTarget)
		{
			renderTarget->SortePilineList();
			renderTarget->DrawPilines();
		} 
		else if (s_Settings.drawPiplinesFromRenderTarget)
		{
			renderTarget->DrawPilines();
		}

		if (s_Settings.drawShaderDrawListFromRenderTarget)
		{
			renderTarget->DrawBufferList();
		}

		if (s_Settings.drawPiplinesFromRenderPass)
		{
			for (Ref<PiplineRenderBase>& pipline : renderPass.PiplineRefBaseVec)
			{
				pipline->DrawNow();
			}
		}
		if (s_Settings.drawShaderDrawListFromRenderPass)
		{

			RenderShaderDrawResourceVec(renderPass.shaderDrawVec);
		}

		if (s_Settings.drawPiplinesFromRenderTarget)
		{
			renderTarget->DrawAlphaPilines();
		}

	}

	inline void Renderer::RenderingPassFromRenderPass(RenderPass& renderPass, StatusRenderPasses& statePass, const int mode)
	{
		RY_PROFILE_FUNCTION();
		RY_SCOPE_TIMER(statePass.TimeElpassed);
		Ref<RenderTarget>& renderTarget = GetRenderTargetFromRenderPass(renderPass);

		statePass.DrawCallsCount = renderTarget->GetDrawListCount();
		statePass.PiplineCallsCount = renderTarget->GetPilineBaseCount();


		renderTarget->BindFramebuffer();
		renderTarget->ClearFramebufferImges();
		renderTarget->ClearFramebufferDepth();
		
		if (s_Settings.drawPiplinesFromRenderTarget && s_Settings.sortBeforDrawFromRenderTarget)
		{
			renderTarget->SortePilineList();
			renderTarget->DrawPilines(mode);
		}
		else if (s_Settings.drawPiplinesFromRenderTarget)
		{
			renderTarget->DrawPilines(mode);
		}

		if (s_Settings.drawShaderDrawListFromRenderTarget)
		{
			renderTarget->DrawBufferList(mode);
		}


		
		if (s_Settings.drawPiplinesFromRenderPass)
		{
			for (Ref<PiplineRenderBase>& pipline : renderPass.PiplineRefBaseVec)
			{
				pipline->DrawNow(mode);
			}
		}
		if (s_Settings.drawShaderDrawListFromRenderPass)
		{

			RenderShaderDrawResourceVec(renderPass.shaderDrawVec, mode);
		}

		if (s_Settings.drawPiplinesFromRenderTarget)
		{
			renderTarget->DrawAlphaPilines(mode);
		}
	}

	inline void Renderer::ClearPiplinesFromRenderPass(RenderPass& renderPass)
	{
		ResetFromRenderPassRenderPassPiplines(renderPass);
		Ref<RenderTarget>& renderTarget = GetRenderTargetFromRenderPass(renderPass);
		renderTarget->ClearPilines();
		renderTarget->ClearAlphaPilines();
		renderTarget->ClearShaderDrawList();
	}

	inline void Renderer::ResetFromRenderPassRenderPassPiplines(RenderPass& renderPass)
	{
		PiplineRefBaseVec& vec = renderPass.PiplineRefBaseVec;
		vec.ResetFramePipline();
		renderPass.shaderDrawVec.clear();
	}

	inline PiplineRefBaseVec& Renderer::GetRenderPiplinesFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.PiplineRefBaseVec;
	}

	inline std::vector<ShaderDrawResource>& Renderer::GetShaderDrawResourceFromRenderPass(RenderPass& renderPass)
	{
		return renderPass.shaderDrawVec;
	}



	inline bool Renderer::IsInsideFromRenderPassViewFustremAABB(const std::array<Plane, 6>& fustrem, const glm::vec4& max, const glm::vec4& min)
	{
		for (const Plane& p : fustrem)
		{
			if (IsAABBIntersectWithePlane(p, max, min))
				return true;
		}
		return false;
	}


#pragma endregion	

	void Renderer::ExtractFrustum(const glm::mat4& viewProj, std::array<Plane, 6>& planes)
	{
		glm::mat4 viewProjTranspose = glm::transpose(viewProj);
		const glm::vec4& viewProjTranspose3Comume = viewProjTranspose[3];

		for (int i = 0; i < 3; i++)
		{
			int x = i * 2;
			int y = x + 1;


			const glm::vec4& mRow = viewProjTranspose[i];

			planes[x] = CaculateCorectViewFustremPlaneAdd(viewProjTranspose3Comume, mRow);
			planes[y] = CaculateCorectViewFustremPlaneSub(viewProjTranspose3Comume, mRow);
		}
	}

	Plane Renderer::CaculateCorectViewFustremPlaneAdd(const glm::vec4& matrixColum, const glm::vec4& matrixRow)
	{
		glm::vec4 planeVec4 = matrixColum + matrixRow;
		return CreateViewFustremPlane(planeVec4);
	}

	Plane Renderer::CaculateCorectViewFustremPlaneSub(const glm::vec4& matrixColum, const glm::vec4& matrixRow)
	{
		glm::vec4 planeVec4 = matrixColum - matrixRow;
		return CreateViewFustremPlane(planeVec4);
	}

	Plane Renderer::CreateViewFustremPlane(glm::vec4& planeVec4)
	{
		float aPow2 = std::powf(planeVec4.x, 2.0f);
		float bPow2 = std::powf(planeVec4.y, 2.0f);
		float cPow2 = std::powf(planeVec4.z, 2.0f);
		float sum = 0.0f;

		sum += aPow2;
		sum += bPow2;
		sum += cPow2;

		float length = std::sqrtf(sum);
		planeVec4 = planeVec4 / length;

		return Plane(planeVec4, planeVec4.w);
	}

	void Renderer::RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec)
	{
		for (ShaderDrawResource& shaderDraw : shaderDrawResourceVec)
		{

			uint32_t index = 0u;
#ifdef RY_SSBO_VARIENTS
			for (auto& resource : shaderDraw.GetBindeStorage())
			{
				std::visit([index](auto& ssbo)
					{
						if (nullptr == ssbo)
							return;
						if constexpr (std::is_same_v<decltype(ssbo), BindlesTextureArray>)
							ssbo->AktivateTextures();

						ssbo->Bind(index);
					}, resource);

				index++;
			}
#else

			for (Ref<StorageBuffer>& resource : shaderDraw.GetBindeStorage())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
#endif
			index = 0u;
#ifdef RY_TEXTURE_VARIENTS
			for (auto& resource : shaderDraw.GetBindeTextures())
			{
				std::visit([index](auto& texture)
					{
						if (nullptr == texture)
							return;
						// if(resource->IsTransferd())
						texture->Bind(index);

					}, resource);

				index++;
			}
#else

			for (Ref<Texture>& resource : shaderDraw.GetBindeTextures())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
#endif
			index = 0u;

			for (Ref<UniformBuffer>& resource : shaderDraw.GetBindeUniform())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
			index = 0u;
			shaderDraw.shaderProgramm->Bind();

			const Mesh::PerDrawObject& drawElement = shaderDraw.drawElement;
			RenderCommand::SetMode(shaderDraw.renderMode);

			RenderCommand::DrawElement(shaderDraw.vao, drawElement);

		}

	}

	void Renderer::RenderShaderDrawResourceVec(std::vector<ShaderDrawResource>& shaderDrawResourceVec, const int mode)
	{
		for (ShaderDrawResource& shaderDraw : shaderDrawResourceVec)
		{

			uint32_t index = 0u;
#ifdef RY_SSBO_VARIENTS
			for (auto& resource : shaderDraw.GetBindeStorage())
			{
				std::visit([index](auto& ssbo)
					{
						if (nullptr == ssbo)
							return;
						if constexpr (std::is_same_v<decltype(ssbo), BindlesTextureArray>)
							ssbo->AktivateTextures();

						ssbo->Bind(index);
					}, resource);

				index++;
			}
#else

			for (Ref<StorageBuffer>& resource : shaderDraw.GetBindeStorage())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
#endif
			index = 0u;
#ifdef RY_TEXTURE_VARIENTS
			for (auto& resource : shaderDraw.GetBindeTextures())
			{
				std::visit([index](auto& texture)
					{
						if (nullptr == texture)
							return;
						// if(resource->IsTransferd())
						texture->Bind(index);

					}, resource);

				index++;
			}
#else

			for (Ref<Texture>& resource : shaderDraw.GetBindeTextures())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
#endif
			index = 0u;

			for (Ref<UniformBuffer>& resource : shaderDraw.GetBindeUniform())
			{
				if (nullptr != resource)
				{
					// if(resource->IsTransferd())
					resource->Bind(index);
				}

				index++;
			}
			index = 0u;
			shaderDraw.shaderProgramm->Bind();

			const Mesh::PerDrawObject& drawElement = shaderDraw.drawElement;
			RenderCommand::SetMode(mode);

			RenderCommand::DrawElement(shaderDraw.vao, drawElement);

		}

	}

}