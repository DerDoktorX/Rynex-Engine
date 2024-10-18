#include "rypch.h"
#include "RendererIcons.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"

#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Renderer/RenderCommand.h"

namespace Rynex {

	struct QuadVertex
	{
		glm::vec3 PositionIcon;
		glm::vec2 TexCoordIcon;
		int TexIndexIcon;
		int EntityIDIcon = -2;

	};

	struct RendererIconsStorage
	{
		const uint32_t MaxIconQuads = 16;
		const uint32_t MaxIconVertices = 4 * MaxIconQuads;
		const uint32_t MaxIconIndecies = 6 * MaxIconQuads;
		static const uint32_t MaxIconTextureSlots = 8;

		Ref<VertexArray> QuadIconVertexArray = nullptr;
		Ref<VertexBuffer> QuadIconVertexBuffer = nullptr;
		int32_t IconSamplers[MaxIconTextureSlots];

		Ref<Shader> IconShader = nullptr;

		Ref<Texture> LigthPointIcon = nullptr;
		Ref<Texture> LigthSpotIcon = nullptr;
		Ref<Texture> LigthDirctionelIcon = nullptr;
		Ref<Texture> CameraIcon = nullptr;
		Ref<Texture> IconWhitheTexture = nullptr;

		uint32_t QuadIconIndexCount = 0;
		QuadVertex* QuadIconVertexBufferPtr = nullptr;
		QuadVertex* QuadIconVertexBufferBase = nullptr;
		glm::vec4 QuadIconVertexPos[4];
		std::array<Ref<Texture>, MaxIconTextureSlots> TextureSlots;
		uint32_t IconTextureSlotsIndex = 1;
		glm::uvec2 ViewPortSizeIcon;
		glm::mat4 ViewPorjIcon;
		glm::mat4 PorjIcon;
		glm::mat4 ViewIcon;
	};
	static RendererIconsStorage s_Data;

	void RendererIcons::Init()
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_WARN("Begine Init RendererIcons!");
		s_Data.QuadIconVertexArray = VertexArray::Create();
		s_Data.QuadIconVertexBuffer = VertexBuffer::Create(s_Data.MaxIconVertices * sizeof(QuadVertex));
		s_Data.QuadIconVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Postion"	},
			{ ShaderDataType::Float2,	"a_TexCoord"	},
			{ ShaderDataType::Int,		"a_TexIndex"	},
			{ ShaderDataType::Int,		"a_EntityID"	}
		});
		s_Data.QuadIconVertexArray->AddVertexBuffer(s_Data.QuadIconVertexBuffer);

		RY_CORE_MULTY_MEMORY_ALICATION("s_Data.QuadVertexBufferBase", "Renderer2D::Init", s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadIconVertexBufferBase = new QuadVertex[s_Data.MaxIconVertices];
		
		RY_CORE_MULTY_MEMORY_ALICATION("quadIndecies", "Renderer2D::Init", s_Data.MaxIconIndecies * sizeof(uint32_t));
		uint32_t size = s_Data.MaxIconIndecies;
		uint32_t* quadIndecies = new uint32_t[size];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data.MaxIconIndecies; i += 6)
		{
			quadIndecies[i + 0] = offset + 0;
			quadIndecies[i + 1] = offset + 1;
			quadIndecies[i + 2] = offset + 2;

			quadIndecies[i + 3] = offset + 2;
			quadIndecies[i + 4] = offset + 3;
			quadIndecies[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndecies, s_Data.MaxIconIndecies);
		s_Data.QuadIconVertexArray->SetIndexBuffer(squareIB);
		delete[] quadIndecies;
		for (uint32_t i = 0; i < s_Data.MaxIconTextureSlots; i++)
			s_Data.IconSamplers[i] = i;

		 s_Data.IconShader = AssetManager::GetAsset<Shader>("Assets/shaders/IconTexture.glsl");
		 s_Data.IconShader->Bind();
		 s_Data.IconShader->SetIntArray("u_Textures", s_Data.IconSamplers, s_Data.MaxIconTextureSlots);


		 s_Data.LigthPointIcon = TextureImporter::LoadTexture("Resources/Icons/ViewPort/PointLigtheIcon.png");
		 s_Data.LigthSpotIcon = TextureImporter::LoadTexture("Resources/Icons/ViewPort/SpotLigthIcon.png");
		 s_Data.LigthDirctionelIcon = TextureImporter::LoadTexture("Resources/Icons/ViewPort/DirectionelLigtheIcon.png");
		 s_Data.CameraIcon = TextureImporter::LoadTexture("Resources/Icons/ViewPort/KameraIcon.png");

		s_Data.IconWhitheTexture = Texture::Create(1, 1);
		uint32_t whitheTexData = 0xffffffff;
		s_Data.IconWhitheTexture->SetData(&whitheTexData, sizeof(uint32_t));


		s_Data.TextureSlots[0] = s_Data.IconWhitheTexture;
		
		s_Data.QuadIconVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadIconVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadIconVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadIconVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		RY_CORE_MULTY_FREE_ALICATION("quadIndecies", "Renderer2D::Init");
		RY_CORE_WARN("Finished Init RendererIcons!");
	}

	void RendererIcons::Shutdown()
	{
		RY_CORE_MULTY_FREE_ALICATION("s_Data.QuadVertexBufferBase", "Renderer2D::Shutdown");
		delete[] s_Data.QuadIconVertexBufferBase;
	}

	void RendererIcons::BeginScene(const Camera& camera, const glm::mat4& transform, const glm::uvec2& sreenSize)
	{
		s_Data.ViewPorjIcon = camera.GetProjektion() * transform;
		s_Data.PorjIcon = camera.GetProjektion();
		s_Data.ViewIcon = transform;
		s_Data.QuadIconIndexCount = 0;
		s_Data.QuadIconVertexBufferPtr = s_Data.QuadIconVertexBufferBase;
		s_Data.ViewPortSizeIcon = sreenSize;
		s_Data.IconTextureSlotsIndex = 1;
	}

	void RendererIcons::EndScene()
	{
		Flush();
	}

	void RendererIcons::Flush()
	{
		if (s_Data.QuadIconIndexCount)
		{		
			s_Data.IconShader->Bind();
			s_Data.IconShader->SetMat4("u_ViewProjection", s_Data.ViewPorjIcon);
			s_Data.IconShader->SetMat4("u_Projection", s_Data.PorjIcon);
			s_Data.IconShader->SetMat4("u_View", s_Data.ViewIcon);
			s_Data.IconShader->SetUint2("u_SreenSize", s_Data.ViewPortSizeIcon);
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadIconVertexBufferPtr - (uint8_t*)s_Data.QuadIconVertexBufferBase);
			s_Data.QuadIconVertexArray->Bind();
			s_Data.QuadIconVertexBuffer->SetData(s_Data.QuadIconVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Data.IconTextureSlotsIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			RenderCommand::DrawIndexedMesh(s_Data.QuadIconVertexArray, s_Data.QuadIconIndexCount);
			s_Data.QuadIconVertexArray->UnBind();
			s_Data.IconShader->UnBind();
		}
	}

	void RendererIcons::StartNewBatch()
	{
		EndScene();
		s_Data.QuadIconIndexCount = 0;
		s_Data.QuadIconVertexBufferPtr = s_Data.QuadIconVertexBufferBase;
		s_Data.IconTextureSlotsIndex = 1;
		
	}


	void RendererIcons::DrawQuad(const glm::mat4& tranform, const Ref<Texture>& texture, int entityID)
	{
		constexpr glm::vec4 color = { 1.0f , 1.0f, 1.0f, 1.0f };
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 TexCoord[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};
		int textureIndex = 0;

		if (s_Data.QuadIconIndexCount >= s_Data.MaxIconIndecies)
			StartNewBatch();
		
		for (uint32_t i = 1; i < s_Data.IconTextureSlotsIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.IconTextureSlotsIndex;
			s_Data.TextureSlots[s_Data.IconTextureSlotsIndex] = texture;
			s_Data.IconTextureSlotsIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
#if 0
			s_Data.QuadVertexBufferPtr->Position = (tranform * s_Data.QuadVertexPos[i]);
#else
			s_Data.QuadIconVertexBufferPtr->PositionIcon = (tranform * glm::vec4(0.,0.,0.0,1.));
#endif
			s_Data.QuadIconVertexBufferPtr->TexCoordIcon = TexCoord[i];
			s_Data.QuadIconVertexBufferPtr->TexIndexIcon = textureIndex;
			s_Data.QuadIconVertexBufferPtr->EntityIDIcon = entityID;
			s_Data.QuadIconVertexBufferPtr++;
		}

		
		s_Data.QuadIconIndexCount+=6;

	}

	void RendererIcons::DrawLigthPointIcon(const glm::mat4& tranform, int entityID)
	{
		DrawQuad(tranform, s_Data.LigthPointIcon, entityID);
	}

	void RendererIcons::DrawLigthSpotIcon(const glm::mat4& tranform, int entityID)
	{
		DrawQuad(tranform, s_Data.LigthSpotIcon, entityID);
	}

	void RendererIcons::DrawLigthDirctionelIcon(const glm::mat4& tranform, int entityID)
	{
		DrawQuad(tranform, s_Data.LigthDirctionelIcon, entityID);
	}

	void RendererIcons::DrawCameraIcon(const glm::mat4& tranform, int entityID)
	{
		DrawQuad(tranform, s_Data.CameraIcon, entityID);
	}

}