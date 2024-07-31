#include "rypch.h"
#include "Renderer2D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Scene/SceneGeomtry.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Import/ShaderImporter.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




#define OLD_GEOMTRY_HANDLE 1
namespace Rynex {

	// TODO: Add TailingFactor for texture

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex;

		//Editor-only
		int EntityID = -2;
	};

	struct Renderer2DStorage
	{
		const uint32_t MaxQuads = 5000;
		const uint32_t MaxVertices = 4 * MaxQuads;
		const uint32_t MaxIndecies = 6 * MaxQuads;
		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		int32_t Samplers[MaxTextureSlots];

		Ref<Shader> TextureShader;
		Ref<Texture2D> WhitheTexture;

		void* QuadVertexBufferBase_ = nullptr;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotsIndex = 1;
		glm::vec4 QuadVertexPos[4];
		
		Renderer2D::Statistics Stats;
	};


	static Renderer2DStorage s_Data;

	

	void Renderer2D::Init()
	{
		RY_PROFILE_FUNCTION();
		s_Data.QuadVertexArray = VertexArray::Create();

		float squareVertices[] = {
			-1.0f,  1.0f,		1.0f, 0.0f,
			 1.0f,  1.0f,		0.0f, 0.0f,
			 1.0f, -1.0f,		0.0f, 1.0f,
			-1.0f, -1.0f,		1.0f, 1.0f
		};

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Postion"	},
			{ ShaderDataType::Float4,	"a_Color"		},
			{ ShaderDataType::Float2,	"a_TexCoord"	},
			{ ShaderDataType::Int,		"a_TexIndex"	},
			{ ShaderDataType::Int,		"a_EntityID"	}
		});
		
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
		
		uint32_t* quadIndecies = new uint32_t[s_Data.MaxIndecies];
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data.MaxIndecies; i+=6 )
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
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(quadIndecies, s_Data.MaxIndecies);
		s_Data.QuadVertexArray->SetIndexBuffer(squareIB);
		
		
		delete[] quadIndecies;	
	
		
		s_Data.WhitheTexture = Texture2D::Create(1, 1);
		uint32_t whitheTexData = 0xffffffff;
		s_Data.WhitheTexture->SetData(&whitheTexData, sizeof(uint32_t));

		
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			s_Data.Samplers[i] = i;

		// TODO: make the check not nasesary

#ifdef RY_SANDBOX
		s_Data.TextureShader = AssetManager::GetAssetFromPath<Shader>("Assets/shaders/Texture.glsl");
#else
		s_Data.TextureShader = ShaderImporter::LoadShader("Assets/shaders/Texture.glsl", "Texture");
#endif // RY_SANDBOX

		
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", s_Data.Samplers, s_Data.MaxTextureSlots);
		
		
		s_Data.TextureSlots[0] = s_Data.WhitheTexture;

#if OLD_GEOMTRY_HANDLE		
		s_Data.QuadVertexPos[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPos[3] = { -0.5f,  0.5f, 0.0f, 1.0f };	
#endif
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
	}


	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		RY_PROFILE_FUNCTION();
		glm::mat4 viewProj = camera.GetViewProjection();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", s_Data.Samplers, s_Data.MaxTextureSlots);
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotsIndex = 1;
		
	}

	void Renderer2D::BeginScene(const OrthograficCamera& camera)
	{
		RY_PROFILE_FUNCTION();
		//m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		//std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlattColorShader)->Bind();
		//std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlattColorShader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotsIndex = 1;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		RY_PROFILE_FUNCTION();
		glm::mat4 viewProj = camera.GetProjektion() * glm::inverse(transform);

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotsIndex = 1;
	}


	void Renderer2D::EndScene()
	{
		RY_PROFILE_FUNCTION();
		Flush();
	}

	void Renderer2D::Flush()
	{
		RY_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount)
		{

			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.Stats.ChachSize = dataSize;


#if OLD_GEOMTRY_HANDLE
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
#else
			s_Data.Geomtrys->GetVertexArray()->GetVertexBuffers()->
#endif

			for (uint32_t i = 0; i < s_Data.TextureSlotsIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			
			RenderCommand::DrawIndexedMesh(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			RenderCommand::DrawIndexedLineLoop(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
			s_Data.QuadVertexArray->UnBind();
		}
	}

	void Renderer2D::FlushAndReset()
	{
		RY_PROFILE_FUNCTION();
		Flush();
		EndScene();
	}


	void Renderer2D::DrawSprite(const glm::mat4& tranform, SpriteRendererComponent& src, int entityID)
	{
		RY_PROFILE_FUNCTION();
		if (src.RenderSingle)
		{
#if 0
			if (src.Shader)
			{
				
				src.Shader->Bind();
				
			}
			else
			{

			}


			if (src.Texture)
				DrawQuadSingle(tranform, src.Texture, entityID);
			else
				DrawQuadSingle(tranform, src.Color, entityID);
#endif // TODO: Shader if runtim Sricpteble
		}
		else
		{
			if(src.Texture) 
				DrawQuad(tranform, src.Texture, entityID);
			else
				DrawQuad(tranform, src.Color, entityID);
		}
			
	}

	

	//Quad		|
	//			V
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		RY_PROFILE_FUNCTION();
		DrawQuad({ position.x, position.y, 0.0f },  size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		RY_PROFILE_FUNCTION();
		const float textureIndex = 0.0f;
		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(tranform, color);

#if 0
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.WhitheTexture->Bind();

		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Tranform", tranform);
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexedMesh(s_Data.QuadVertexArray);
#endif
	}

	//Textures	|
	//			V
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		RY_PROFILE_FUNCTION();
		DrawQuad({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> texture)
	{
		RY_PROFILE_FUNCTION();
		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(tranform, texture);

#if 0

#if 0
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->TextureShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(s_Data->TextureShader)->UploadUniformFloat4("u_Color", color);
#endif

		s_Data.TextureShader->SetFloat4("u_Color", {0.2f, 0.3f, 0.8f, 0.5f}); //glm::vec4(1.0));
		texture->Bind();

		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) * /*rotation*/ glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Tranform", tranform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexedMesh(s_Data.QuadVertexArray);
#endif
		
	}

	void Renderer2D::DrawQuadSingle(const glm::mat4& tranform, const glm::vec4& color, int entityID)
	{
		RY_PROFILE_FUNCTION();
	}

	void Renderer2D::DrawQuadSingle(const glm::mat4& tranform, const Ref<Texture2D> texture, int entityID)
	{
		RY_PROFILE_FUNCTION();
	}

	//Rotation	|
	//			V
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		RY_PROFILE_FUNCTION();
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		RY_PROFILE_FUNCTION();
		const float textureIndex = 0.0f;
		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0), rotation, { 0.0f, 0.0f, 1.0f }) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(tranform, color);
		
#if 0
		s_Data.TextureShader->SetFloat4("u_Color", color);
		s_Data.WhitheTexture->Bind();
		
		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		s_Data.TextureShader->SetMat4("u_Tranform", tranform);
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexedMesh(s_Data.QuadVertexArray);
#endif
	}

	//Matrix	|
	//			V
	void Renderer2D::DrawQuad(const glm::mat4& transfrom, const glm::vec4& color, int entityID)
	{
		RY_PROFILE_FUNCTION();
		constexpr size_t quadVertexCount = 4;
		const int textureIndex = 0;
		constexpr glm::vec2 TexCoord[] = {
			{0.0f, 0.0},
			{1.0f, 0.0},
			{1.0f, 1.0},
			{0.0f, 1.0}
		};

		if (s_Data.QuadIndexCount >= s_Data.MaxIndecies)
			StartNewBatch();

		for (size_t i = 0; i < quadVertexCount; i++) 
		{
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->Position = transfrom * s_Data.QuadVertexPos[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transfrom, const Ref<Texture2D> texture, int entityID)
	{
		RY_PROFILE_FUNCTION();
		constexpr glm::vec4 color = { 1.0f , 1.0f, 1.0f, 1.0f };
		
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 TexCoord[] = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};
		int textureIndex = 0;

		if (s_Data.QuadIndexCount >= s_Data.MaxIndecies)
			StartNewBatch();

		for (uint32_t i = 1; i < s_Data.TextureSlotsIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (int)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotsIndex;
			s_Data.TextureSlots[s_Data.TextureSlotsIndex] = texture;
			s_Data.TextureSlotsIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++) 
		{
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->Position = transfrom * s_Data.QuadVertexPos[i];
			s_Data.QuadVertexBufferPtr->TexCoord = TexCoord[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	// Rotation + Texture	|
	//						V
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D> texture)
	{
		RY_PROFILE_FUNCTION();
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D> texture)
	{
		RY_PROFILE_FUNCTION();
		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) 
			* glm::rotate(glm::mat4(1.0), rotation, { 0.0f, 0.0f, 1.0f }) 
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(tranform, texture);

#if 0
		s_Data.TextureShader->SetFloat4("u_Color", { 0.2f, 0.3f, 0.8f, 0.5f }); //glm::vec4(1.0));
		texture->Bind();

		glm::mat4 tranform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0), rotation, { 0.0f, 0.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
		s_Data.TextureShader->SetMat4("u_Tranform", tranform);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexedMesh(s_Data.QuadVertexArray);
#endif
	}

	// RenderWithout Batching	|
	//							V
	


	void Renderer2D::ResetStats()
	{
		RY_PROFILE_FUNCTION();
		uint32_t chach = s_Data.Stats.ChachSize;
		memset(&s_Data.Stats, 0, sizeof(Statistics));
		s_Data.Stats.ChachSize = chach;
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		RY_PROFILE_FUNCTION();
		return s_Data.Stats;
	}

	void Renderer2D::StartNewBatch()
	{
		RY_PROFILE_FUNCTION();
		EndScene();
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotsIndex = 1;
	}

}