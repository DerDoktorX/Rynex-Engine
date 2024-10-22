#include "rypch.h"
#include "Renderer2D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Import/ShaderImporter.h"
#include "Rynex/Asset/Import/TextureImporter.h"




#define OLD_GEOMTRY_HANDLE 1

namespace Rynex {

	// TODO: Add TailingFactor for texture
	struct IconVertex
	{
		glm::vec4 Position;
		int TexCoord;
		int TexIndex;
		int EntityID = -2;
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex;

		//Editor-only
		int EntityID = -2;
	};

	struct Renderer2DScene
	{
		glm::vec4 VertexPos[4];
		glm::mat4 ViewPorj;
		glm::mat4 View;
		glm::mat4 Porj;
		glm::uvec2 ViewPortSize;
		Ref<Texture> WhitheTexture;
	};

	template<typename T, uint32_t MaxVert, uint32_t MaxTex>
	struct Renderer2DStorage
	{
		const uint32_t MaxQuads = MaxVert;
		const uint32_t MaxVertices = 4 * MaxQuads;
		const uint32_t MaxIndecies = 6 * MaxQuads;
		static const uint32_t MaxTextureSlots = MaxTex;

		Ref<VertexArray> VertexArray;
		Ref<VertexBuffer> VertexBuffer;
		int32_t Samplers[MaxTextureSlots];

		Ref<Shader> TextureShader;
		

		uint32_t IndexCount = 0;
		T* VertexBufferBase = nullptr;
		T* VertexBufferPtr = nullptr;

		std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotsIndex = 1;
		

		Renderer2D::Statistics Stats;
	};

	static Renderer2DStorage<IconVertex, 16, 8> s_DatatIcons;
	static Renderer2DStorage<QuadVertex, 500, 32> s_DatatQauds;
	static Renderer2DScene s_DatatScene;

	void Renderer2D::InitEditor()
	{
		RY_PROFILE_FUNCTION();

#pragma region SceneInit	

		s_DatatScene.WhitheTexture = Texture::Create(1, 1);
		uint32_t whitheTexData = 0xffffffff;
		s_DatatScene.WhitheTexture->SetData(&whitheTexData, sizeof(uint32_t));


		s_DatatScene.VertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_DatatScene.ViewPortSize = { 1u,1u };

#pragma endregion

#pragma region QuadeInit


		s_DatatQauds.VertexArray = VertexArray::Create();


		s_DatatQauds.VertexBuffer = VertexBuffer::Create(s_DatatQauds.MaxVertices * sizeof(QuadVertex));

		s_DatatQauds.VertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Postion"	},
			{ ShaderDataType::Float4,	"a_Color"		},
			{ ShaderDataType::Float2,	"a_TexCoord"	},
			{ ShaderDataType::Int,		"a_TexIndex"	},
			{ ShaderDataType::Int,		"a_EntityID"	}
		});

		s_DatatQauds.VertexArray->AddVertexBuffer(s_DatatQauds.VertexBuffer);
		s_DatatQauds.VertexBufferBase = new QuadVertex[s_DatatQauds.MaxVertices];

		uint32_t* quadIndecies = new uint32_t[s_DatatQauds.MaxIndecies];
		uint32_t quadOffset = 0;

		for (uint32_t i = 0; i < s_DatatQauds.MaxIndecies; i += 6)
		{
			quadIndecies[i + 0] = quadOffset + 0;
			quadIndecies[i + 1] = quadOffset + 1;
			quadIndecies[i + 2] = quadOffset + 2;

			quadIndecies[i + 3] = quadOffset + 2;
			quadIndecies[i + 4] = quadOffset + 3;
			quadIndecies[i + 5] = quadOffset + 0;

			quadOffset += 4;
		}

		
		Ref<IndexBuffer> squareQaudIB = IndexBuffer::Create(quadIndecies, s_DatatQauds.MaxIndecies);
		s_DatatQauds.VertexArray->SetIndexBuffer(squareQaudIB);
		RY_CORE_MULTY_FREE_ALICATION("quadIndecies", "Renderer2D::Init");
		delete[] quadIndecies;
		
		

		for (uint32_t i = 0; i < s_DatatQauds.MaxTextureSlots; i++)
			s_DatatQauds.Samplers[i] = i;

		s_DatatQauds.TextureShader = AssetManager::GetAsset<Shader>("Assets/shaders/Texture.glsl");
		
		s_DatatQauds.TextureShader->Bind();
		s_DatatQauds.TextureShader->SetIntArray("u_Textures", s_DatatQauds.Samplers, s_DatatQauds.MaxTextureSlots);
		
		
		s_DatatQauds.TextureSlots[0] = s_DatatScene.WhitheTexture;

#pragma endregion

#pragma region IconInit


		s_DatatIcons.VertexArray = VertexArray::Create();


		s_DatatIcons.VertexBuffer = VertexBuffer::Create(s_DatatIcons.MaxVertices * sizeof(IconVertex));

		s_DatatIcons.VertexBuffer->SetLayout({
			{ ShaderDataType::Float4,	"a_Postion"	},
			{ ShaderDataType::Int,		"a_TexCoord"	},
			{ ShaderDataType::Int,		"a_TexIndex"	},
			{ ShaderDataType::Int,		"a_EntityID"	}
			});

		s_DatatIcons.VertexArray->AddVertexBuffer(s_DatatIcons.VertexBuffer);
		s_DatatIcons.VertexBufferBase = new IconVertex[s_DatatIcons.MaxVertices];

		uint32_t* iconIndecies = new uint32_t[s_DatatIcons.MaxIndecies];
		uint32_t iconOffset = 0;

		for (uint32_t i = 0; i < s_DatatIcons.MaxIndecies; i += 6)
		{
			iconIndecies[i + 0] = iconOffset + 0;
			iconIndecies[i + 1] = iconOffset + 1;
			iconIndecies[i + 2] = iconOffset + 2;

			iconIndecies[i + 3] = iconOffset + 2;
			iconIndecies[i + 4] = iconOffset + 3;
			iconIndecies[i + 5] = iconOffset + 0;

			iconOffset += 4;
		}

		
		Ref<IndexBuffer> squareIconIB = IndexBuffer::Create(iconIndecies, s_DatatIcons.MaxIndecies);
		s_DatatIcons.VertexArray->SetIndexBuffer(squareIconIB);
		RY_CORE_MULTY_FREE_ALICATION("quadIndecies", "Renderer2D::Init");
		delete[] iconIndecies;



		for (uint32_t i = 0; i < s_DatatIcons.MaxTextureSlots; i++)
			s_DatatIcons.Samplers[i] = i;

		s_DatatIcons.TextureShader = AssetManager::GetAsset<Shader>("Assets/shaders/IconTexture.glsl");

		s_DatatIcons.TextureShader->Bind();
		s_DatatIcons.TextureShader->SetIntArray("u_Textures", s_DatatIcons.Samplers, s_DatatIcons.MaxTextureSlots);


		s_DatatIcons.TextureSlots[0] = s_DatatScene.WhitheTexture;
		s_DatatIcons.TextureSlots[1] = TextureImporter::LoadTexture("Resources/Icons/ViewPort/PointLigtheIcon.png");
		s_DatatIcons.TextureSlots[2] = TextureImporter::LoadTexture("Resources/Icons/ViewPort/SpotLigthIcon.png");
		s_DatatIcons.TextureSlots[3] = TextureImporter::LoadTexture("Resources/Icons/ViewPort/DirectionelLigtheIcon.png");
		s_DatatIcons.TextureSlots[4] = TextureImporter::LoadTexture("Resources/Icons/ViewPort/CameraIcon.png");
#pragma endregion
	}

	void Renderer2D::Init()
	{
		RY_PROFILE_FUNCTION();
#pragma region SceneInit	

		s_DatatScene.WhitheTexture = Texture::Create(1, 1);
		uint32_t whitheTexData = 0xffffffff;
		s_DatatScene.WhitheTexture->SetData(&whitheTexData, sizeof(uint32_t));


		s_DatatScene.VertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_DatatScene.VertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

#pragma endregion

#pragma region QuadeInit


		s_DatatQauds.VertexArray = VertexArray::Create();


		s_DatatQauds.VertexBuffer = VertexBuffer::Create(s_DatatQauds.MaxVertices * sizeof(QuadVertex));

		s_DatatQauds.VertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Postion"	},
			{ ShaderDataType::Float2,	"a_TexCoord"	},
			{ ShaderDataType::Int,		"a_TexIndex"	},
			{ ShaderDataType::Int,		"a_EntityID"	}
			});

		s_DatatQauds.VertexArray->AddVertexBuffer(s_DatatQauds.VertexBuffer);
		s_DatatQauds.VertexBufferBase = new QuadVertex[s_DatatQauds.MaxVertices];

		uint32_t* quadIndecies = new uint32_t[s_DatatQauds.MaxIndecies];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_DatatQauds.MaxIndecies; i += 6)
		{
			quadIndecies[i + 0] = offset + 0;
			quadIndecies[i + 1] = offset + 1;
			quadIndecies[i + 2] = offset + 2;

			quadIndecies[i + 3] = offset + 2;
			quadIndecies[i + 4] = offset + 3;
			quadIndecies[i + 5] = offset + 0;

			offset += 4;
		}

		uint32_t squareIndices[] = {
			0, 1, 2,
			2, 3, 0
		};
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndecies, s_DatatQauds.MaxIndecies);
		s_DatatQauds.VertexArray->SetIndexBuffer(squareIB);
		RY_CORE_MULTY_FREE_ALICATION("quadIndecies", "Renderer2D::Init");
		delete[] quadIndecies;



		for (uint32_t i = 0; i < s_DatatQauds.MaxTextureSlots; i++)
			s_DatatQauds.Samplers[i] = i;

		s_DatatQauds.TextureShader = AssetManager::GetAsset<Shader>("Assets/shaders/Texture.glsl");

		s_DatatQauds.TextureShader->Bind();
		s_DatatQauds.TextureShader->SetIntArray("u_Textures", s_DatatQauds.Samplers, s_DatatQauds.MaxTextureSlots);


		s_DatatQauds.TextureSlots[0] = s_DatatScene.WhitheTexture;

#pragma endregion

	}

	void Renderer2D::Shutdown()
	{
		RY_CORE_MULTY_FREE_ALICATION("s_DatatQauds.VertexBufferBase", "Renderer2D::Shutdown");
#pragma region QuadeShutdown	

		delete[] s_DatatQauds.VertexBufferBase;

		//delete s_Data.QuadVertexBufferPtr;
		s_DatatQauds.VertexArray.reset();
		s_DatatQauds.VertexBuffer.reset();
		s_DatatQauds.TextureShader.reset();

#pragma endregion
		s_DatatIcons.VertexArray.reset();
		s_DatatIcons.VertexBuffer.reset();
		s_DatatIcons.TextureShader.reset();
#pragma endregion

#pragma region QuadeShutdown

#pragma region SceneShutdown
		s_DatatScene.WhitheTexture.reset();
#pragma endregion


	}

#pragma region Quade

	void Renderer2D::BeginSceneQuade(const EditorCamera& camera)
	{
		RY_PROFILE_FUNCTION();
		s_DatatScene.ViewPorj = camera.GetViewProjection();

		s_DatatQauds.IndexCount = 0;
		s_DatatQauds.VertexBufferPtr = s_DatatQauds.VertexBufferBase;
		s_DatatQauds.TextureSlotsIndex = 1;
	}

	void Renderer2D::BeginSceneQuade(const OrthograficCamera& camera)
	{
		RY_PROFILE_FUNCTION();
		
		s_DatatScene.ViewPorj = camera.GetViewProjectionMatrix();

		s_DatatQauds.IndexCount = 0;
		s_DatatQauds.VertexBufferPtr = s_DatatQauds.VertexBufferBase;
		s_DatatQauds.TextureSlotsIndex = 1;
	}

	void Renderer2D::BeginSceneQuade(const Camera& camera, const glm::mat4& transform)
	{
		RY_PROFILE_FUNCTION();
		s_DatatScene.Porj = camera.GetProjektion();
		s_DatatScene.View = transform;
		s_DatatScene.ViewPorj = s_DatatScene.Porj * s_DatatScene.View;
		
		
		s_DatatQauds.IndexCount = 0;
		s_DatatQauds.VertexBufferPtr = s_DatatQauds.VertexBufferBase;
		s_DatatQauds.TextureSlotsIndex = 1;
	}


	void Renderer2D::EndSceneQuade()
	{
		FlushQuade();
	}

	void Renderer2D::FlushQuade()
	{
		if (s_DatatQauds.IndexCount)
		{
			
			s_DatatQauds.TextureShader->Bind();
			s_DatatQauds.TextureShader->SetMat4("u_ViewProjection", s_DatatScene.ViewPorj);
			s_DatatQauds.TextureShader->SetIntArray("u_Textures", s_DatatIcons.Samplers, s_DatatIcons.MaxTextureSlots);
			uint32_t dataSize = (uint32_t)((uint8_t*)s_DatatQauds.VertexBufferPtr - (uint8_t*)s_DatatQauds.VertexBufferBase);
			s_DatatQauds.Stats.ChachSize = dataSize;

			s_DatatQauds.VertexBuffer->SetData(s_DatatQauds.VertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_DatatQauds.TextureSlotsIndex; i++)
				s_DatatQauds.TextureSlots[i]->Bind(i);

			
			RenderCommand::DrawIndexedMesh(s_DatatQauds.VertexArray, s_DatatQauds.IndexCount);
			s_DatatQauds.Stats.DrawCalls++;
			s_DatatQauds.VertexArray->UnBind();
			s_DatatQauds.TextureShader->UnBind();
		}
	}


	void Renderer2D::DrawSprite(const glm::mat4& transfrom, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transfrom, src.Texture, entityID);
		else
			DrawQuad(transfrom, src.Color, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transfrom, const glm::vec4& color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		const int textureIndex = 0;
		constexpr glm::vec2 TexCoord[] = {
			{0.0f, 0.0},
			{1.0f, 0.0},
			{1.0f, 1.0},
			{0.0f, 1.0}
		};

		if (s_DatatQauds.IndexCount >= s_DatatQauds.MaxIndecies)
			StartNewBatchQuade();

		for (size_t i = 0; i < quadVertexCount; i++) 
		{
			s_DatatQauds.VertexBufferPtr->Color = color;
			s_DatatQauds.VertexBufferPtr->Position = transfrom * s_DatatScene.VertexPos[i];
			s_DatatQauds.VertexBufferPtr->TexCoord = TexCoord[i];
			s_DatatQauds.VertexBufferPtr->TexIndex = textureIndex;
			s_DatatQauds.VertexBufferPtr->EntityID = entityID;
			s_DatatQauds.VertexBufferPtr++;
		}

		s_DatatQauds.IndexCount += 6;
		s_DatatQauds.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transfrom, const Ref<Texture> texture, int entityID)
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

		if (s_DatatQauds.IndexCount >= s_DatatQauds.MaxIndecies)
			StartNewBatchQuade();

		for (uint32_t i = 1; i < s_DatatQauds.TextureSlotsIndex; i++)
		{
			if (*s_DatatQauds.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			textureIndex = (float)s_DatatQauds.TextureSlotsIndex;
			s_DatatQauds.TextureSlots[s_DatatQauds.TextureSlotsIndex] = texture;
			s_DatatQauds.TextureSlotsIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++) 
		{
			s_DatatQauds.VertexBufferPtr->Color = color;
			s_DatatQauds.VertexBufferPtr->Position = transfrom * s_DatatScene.VertexPos[i];
			s_DatatQauds.VertexBufferPtr->TexCoord = TexCoord[i];
			s_DatatQauds.VertexBufferPtr->TexIndex = textureIndex;
			s_DatatQauds.VertexBufferPtr->EntityID = entityID;
			s_DatatQauds.VertexBufferPtr++;
		}

		s_DatatQauds.IndexCount += 6;

		s_DatatQauds.Stats.QuadCount++;
	}

	void Renderer2D::StartNewBatchQuade()
	{
		EndSceneQuade();
		s_DatatQauds.IndexCount = 0;
		s_DatatQauds.VertexBufferPtr = s_DatatQauds.VertexBufferBase;

		s_DatatQauds.TextureSlotsIndex = 1;
	}

	void Renderer2D::ResetQuadeStats()
	{
		uint32_t chach = s_DatatQauds.Stats.ChachSize;
		memset(&s_DatatQauds.Stats, 0, sizeof(Statistics));
		s_DatatQauds.Stats.ChachSize = chach;
	}

	void Renderer2D::ResetIconStats()
	{
	}
	
	Renderer2D::Statistics Renderer2D::GetQuadeStats()
	{
		return s_DatatQauds.Stats;
	}

#pragma endregion

#pragma region Icons

	void Renderer2D::BeginSceneIcon(const Camera& camera, const glm::mat4& transform, const glm::uvec2& viewPortSize)
	{
		RY_PROFILE_FUNCTION();
		s_DatatScene.Porj = camera.GetProjektion();
		s_DatatScene.View = transform;
		s_DatatScene.ViewPorj = s_DatatScene.Porj * s_DatatScene.View;
		s_DatatScene.ViewPortSize = viewPortSize;

		s_DatatIcons.IndexCount = 0;
		s_DatatIcons.VertexBufferPtr = s_DatatIcons.VertexBufferBase;
		s_DatatIcons.TextureSlotsIndex = 1;
	}

	void Renderer2D::EndSceneIcon()
	{
		FlushIcon();
	}

	void Renderer2D::FlushIcon()
	{
		if (s_DatatIcons.IndexCount)
		{
			s_DatatIcons.TextureShader->Bind();
			s_DatatIcons.TextureShader->SetMat4("u_ViewProjection", s_DatatScene.ViewPorj);
			s_DatatIcons.TextureShader->SetMat4("u_Projection", s_DatatScene.Porj);
			s_DatatIcons.TextureShader->SetMat4("u_View", s_DatatScene.View);
			s_DatatIcons.TextureShader->SetUint2("u_ViewPortSize", s_DatatScene.ViewPortSize);
			s_DatatIcons.TextureShader->SetIntArray("u_Textures", s_DatatIcons.Samplers, s_DatatIcons.MaxTextureSlots);
			uint32_t dataSize = (uint32_t)((uint8_t*)s_DatatIcons.VertexBufferPtr - (uint8_t*)s_DatatIcons.VertexBufferBase);
			s_DatatIcons.Stats.ChachSize = dataSize;

			s_DatatIcons.VertexBuffer->SetData(s_DatatIcons.VertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_DatatIcons.TextureSlotsIndex; i++)
				s_DatatIcons.TextureSlots[i]->Bind(i);


			RenderCommand::DrawIndexedMesh(s_DatatIcons.VertexArray, s_DatatIcons.IndexCount);
			s_DatatIcons.Stats.DrawCalls++;
			s_DatatIcons.VertexArray->UnBind();
			s_DatatIcons.TextureShader->UnBind();
		}
	}

	void Renderer2D::DrawLigthPointIcon(const glm::mat4& transfrom, int entityID)
	{
		DrawIcon(transfrom, 1, entityID);
	}

	void Renderer2D::DrawLigthSpotIcon(const glm::mat4& transfrom, int entityID)
	{
		DrawIcon(transfrom, 2, entityID);
	}

	void Renderer2D::DrawLigthDirctionelIcon(const glm::mat4& transfrom, int entityID)
	{
		DrawIcon(transfrom, 3, entityID);
	}

	void Renderer2D::DrawCameraIcon(const glm::mat4& transfrom, int entityID)
	{
		DrawIcon(transfrom, 4, entityID);
	}

	void Renderer2D::DrawIcon(const glm::mat4& transfrom, int index, int entityID)
	{
		constexpr size_t IconVertexCount = 4;
		

		if (s_DatatIcons.IndexCount >= s_DatatIcons.MaxIndecies)
			StartNewBatchQuade();


		s_DatatIcons.TextureSlotsIndex = s_DatatIcons.TextureSlotsIndex <= index ? index +1: s_DatatIcons.TextureSlotsIndex;

		for (size_t i = 0; i < IconVertexCount; i++)
		{
			s_DatatIcons.VertexBufferPtr->Position = transfrom * glm::vec4(0.0,0.0,0.0,1.0);
			s_DatatIcons.VertexBufferPtr->TexCoord = BIT(i);
			s_DatatIcons.VertexBufferPtr->TexIndex = index;
			s_DatatIcons.VertexBufferPtr->EntityID = entityID;
			s_DatatIcons.VertexBufferPtr++;
		}

		s_DatatIcons.IndexCount += 6;

		s_DatatIcons.Stats.QuadCount++;
	}

	void Renderer2D::StartNewBatchIcon()
	{
		EndSceneIcon();
		s_DatatIcons.IndexCount = 0;
		s_DatatIcons.VertexBufferPtr = s_DatatIcons.VertexBufferBase;

		s_DatatIcons.TextureSlotsIndex = 1;
	}

	Renderer2D::Statistics Renderer2D::GetIconStats()
	{
		return s_DatatIcons.Stats;
	}

#pragma endregion

}