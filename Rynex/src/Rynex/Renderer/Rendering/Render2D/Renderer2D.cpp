#include "rypch.h"
#include "Renderer2D.h"

#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/Rendering/Render2D/VertexQuadeBatching.h>
#include <Rynex/Renderer/Rendering/Render2D/VertexQuadeTransformBatche.h>
#include <Rynex/Renderer/RenderCommand.h>
#include <Rynex/Renderer/Text/MSDFData.h>
#include <Rynex/Asset/Import/TextureImporter.h>

#define TEST_RENDERER_QUADE_001 1
#define TEST_RENDERER_TEXT_001 1
#define TEST_RENDERER_ICON_001 1
#define RY_RENDER2D_TEST_BATCHING_SBO 1
#define RY_RENDER2D_TEST_BATCHING_SBO_UPDATE 0
#define RY_RENDER2D_TEST_BATCHING_SBO_PARICLE 0

namespace Rynex {

#if 1
	namespace IconTexture {
		enum
		{
			None = -1,
			Dafult = 0,
			PointLigthe = 1,
			SpotLigth = 2,
			DirectionelLigthe = 3,
			Camera = 4
		};
	}

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex;

		//Editor-only
		int EntityID = -2;
	};

	struct ObjectData
	{
		glm::mat4 ModelMatrix;
		glm::vec4 Color;
		int TexIndex;
		int EntityID = -2;
		int Empty[2] = {-30, -40 };
	};

	struct ParticleData
	{
		glm::vec3 Position;
		float Scale;
		glm::vec4 Color;
	};

	struct QuadVertexTransfom
	{
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};

	struct IconVertex
	{
		glm::vec4 Position;
		int TexCoord;
		int TexIndex;
		int EntityID = -2;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		int TexIndex = 0;
		int EntityID = -2;
	
	};

	constexpr bool operator==(const ::Rynex::TextVertex& a, const ::Rynex::TextVertex& b)
	{
		return  a.Position == b.Position && a.Color == b.Color && a.TexCoord == b.TexCoord && a.TexIndex == b.TexIndex && a.EntityID == b.EntityID;
	};
	constexpr bool operator!=(const ::Rynex::TextVertex& a, const ::Rynex::TextVertex& b)
	{
		return a.Position != b.Position || a.Color != b.Color || a.TexCoord != b.TexCoord || a.TexIndex != b.TexIndex || a.EntityID != b.EntityID;
	};

	constexpr bool operator==(const ::Rynex::QuadVertex& a, const ::Rynex::QuadVertex& b)
	{
		return a.Position == b.Position && a.Color == b.Color && a.TexCoord == b.TexCoord && a.TexIndex == b.TexIndex && a.EntityID == b.EntityID;
	};
	constexpr bool operator!=(const ::Rynex::QuadVertex& a, const ::Rynex::QuadVertex& b)
	{
		return a.Position != b.Position || a.Color == b.Color || a.TexCoord != b.TexCoord || a.TexIndex != b.TexIndex || a.EntityID != b.EntityID;
	};

	constexpr bool operator==(const ::Rynex::IconVertex& a, const ::Rynex::IconVertex& b)
	{
		return a.Position == b.Position && a.TexCoord == b.TexCoord && a.TexIndex == b.TexIndex && a.EntityID == b.EntityID;
	};
	constexpr bool operator!=(const ::Rynex::IconVertex& a, const ::Rynex::IconVertex& b)
	{
		return a.Position != b.Position || a.TexCoord != b.TexCoord || a.TexIndex != b.TexIndex || a.EntityID != b.EntityID;
	};

	constexpr bool operator==(const ::Rynex::ParticleData& a, const ::Rynex::ParticleData& b)
	{
		return a.Position == b.Position && a.Scale == b.Scale && a.Color == b.Color;
	};
	constexpr bool operator!=(const ::Rynex::ParticleData& a, const ::Rynex::ParticleData& b)
	{
		return a.Position != b.Position || a.Scale != b.Scale || a.Color != b.Color;
	};

	constexpr bool operator==(const ::Rynex::ObjectData& a, const ::Rynex::ObjectData& b)
	{
		return a.ModelMatrix == b.ModelMatrix && a.EntityID == b.EntityID && a.Color == b.Color && a.TexIndex == b.TexIndex;
	};
	constexpr bool operator!=(const ::Rynex::ObjectData& a, const ::Rynex::ObjectData& b)
	{
		return a.ModelMatrix != b.ModelMatrix || a.EntityID != b.EntityID || a.Color != b.Color && a.TexIndex == b.TexIndex;
	};

	struct Renderer2DStorage
	{
		
		VertexQuadeBatching<QuadVertex, 16> Quades;
		VertexQuadeTransformBatche<ObjectData, QuadVertexTransfom, 16, 4> QuadesT;
		VertexQuadeTransformBatche<ObjectData, QuadVertexTransfom, 16, 4> QuadesSingle;
		VertexQuadeTransformBatche<ParticleData, QuadVertexTransfom, 16, 4> Particle;
		VertexQuadeBatching<TextVertex, 8> Text;
		VertexQuadeBatching<TextVertex, 8> TextSingle;

		VertexQuadeBatching<IconVertex, 8> Icon;
		VertexQuadeBatching<IconVertex, 8> IconSingle;

		std::array<Ref<Texture>, 8> IconTexture;
		bool Aktiv = false;
	};


	static Renderer2DStorage s_Storarage2D;
#endif

	void Renderer2D::Init()
	{
		s_Storarage2D.Quades.Create(
			"../Rynex-Editor/Editor-Assets/shaders/Texture2.glsl", 
			1u, 
			{
				{ ShaderDataType::Float3,	"a_Postion"	},
				{ ShaderDataType::Float4,	"a_Color"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
				{ ShaderDataType::Int,		"a_TexIndex"},
				{ ShaderDataType::Int,		"a_Entity"	},
			}, 
			Texture::White(),
			"u_Textures"
			);


		constexpr glm::vec2 texCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};
		constexpr glm::vec2 vertexPos[] = {
				{ -0.5f, -0.5f },
				{  0.5f, -0.5f },
				{  0.5f,  0.5f },
				{ -0.5f,  0.5f }
		};

		QuadVertexTransfom quadVerticesLayout[4] = {
			QuadVertexTransfom{
				vertexPos[0],
				texCoord[0]
			},
			QuadVertexTransfom{
				vertexPos[1],
				texCoord[1]
			},
			QuadVertexTransfom{
				vertexPos[2],
				texCoord[2]
			},
			QuadVertexTransfom{
				vertexPos[3],
				texCoord[3]
			},
		};
		void* dataPtr = quadVerticesLayout;

		s_Storarage2D.QuadesT.Create(
			"../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl",
			1u,
			quadVerticesLayout,
			{
				{ ShaderDataType::Float2,	"a_Postion"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
			},
			Texture::White(),
			"u_Textures"
		);
		s_Storarage2D.QuadesT.SetCameraSlot(0u);
#if RY_RENDER2D_TEST_BATCHING_SBO_PARICLE
		s_Storarage2D.Particle.Create(
			// "../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl",
			100u,
			quadVerticesLayout,
			{
				{ ShaderDataType::Float2,	"a_Postion"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
			},
			Texture::White(),
			"u_Textures"
			);
#endif
		s_Storarage2D.Text.Create(
			"../Rynex-Editor/Editor-Assets/shaders/TextShader.glsl",
			1u,
			{
				{ ShaderDataType::Float3,	"a_Postion"	},
				{ ShaderDataType::Float4,	"a_Color"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
				{ ShaderDataType::Int,		"a_TexIndex"},
				{ ShaderDataType::Int,		"a_Entity"	},
			},
			Font::GetDefault()->GetAtlasTexture(),
			"u_Textures"
			);
		s_Storarage2D.Text.SetCameraSlot(0u);


		
	}

	void Renderer2D::InitEditor()
	{
		constexpr glm::vec2 texCoord[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};
		constexpr glm::vec2 vertexPos[] = {
				{ -0.5f, -0.5f },
				{  0.5f, -0.5f },
				{  0.5f,  0.5f },
				{ -0.5f,  0.5f }
		};

		QuadVertexTransfom quadVerticesLayout[4] = {
			QuadVertexTransfom{
				vertexPos[0],
				texCoord[0]
			},
			QuadVertexTransfom{
				vertexPos[1],
				texCoord[1]
			},
			QuadVertexTransfom{
				vertexPos[2],
				texCoord[2]
			},
			QuadVertexTransfom{
				vertexPos[3],
				texCoord[3]
			},
		};
		void* dataPtr = quadVerticesLayout;

		s_Storarage2D.QuadesSingle.Create(
			"../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl",
			1u,
			quadVerticesLayout,
			{
				{ ShaderDataType::Float2,	"a_Postion"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
			},
			Texture::White(),
			"u_Textures"
		);
		s_Storarage2D.QuadesSingle.SetCameraSlot(0u);

		s_Storarage2D.TextSingle.Create(
			"../Rynex-Editor/Editor-Assets/shaders/TextShader.glsl",
			1u,
			{
				{ ShaderDataType::Float3,	"a_Postion"	},
				{ ShaderDataType::Float4,	"a_Color"	},
				{ ShaderDataType::Float2,	"a_TexCoord"},
				{ ShaderDataType::Int,		"a_TexIndex"},
				{ ShaderDataType::Int,		"a_Entity"	},
			},
			Font::GetDefault()->GetAtlasTexture(),
			"u_Textures"
			);
		s_Storarage2D.TextSingle.SetCameraSlot(0u);

		s_Storarage2D.Icon.Create(
			"../Rynex-Editor/Editor-Assets/shaders/IconTexture.glsl",
			16u,
			{
				{ ShaderDataType::Float4,	"a_Postion"	},
				{ ShaderDataType::Int,		"a_TexCoord"	},
				{ ShaderDataType::Int,		"a_TexIndex"	},
				{ ShaderDataType::Int,		"a_EntityID"	}
			},
			Texture::Default(),
			"u_Textures"
		);
		s_Storarage2D.Icon.SetCameraSlot(0u);
		s_Storarage2D.Icon.SetDisplaySlot(1u);

		s_Storarage2D.IconSingle.Create(
			"../Rynex-Editor/Editor-Assets/shaders/IconTexture.glsl",
			16u,
			{
				{ ShaderDataType::Float4,	"a_Postion"	},
				{ ShaderDataType::Int,		"a_TexCoord"	},
				{ ShaderDataType::Int,		"a_TexIndex"	},
				{ ShaderDataType::Int,		"a_EntityID"	}
			},
			Texture::Default(),
			"u_Textures"
		);
		s_Storarage2D.IconSingle.SetCameraSlot(0u);
		s_Storarage2D.IconSingle.SetDisplaySlot(1u);

		s_Storarage2D.IconTexture[IconTexture::Dafult] = Texture::Default();
		s_Storarage2D.IconTexture[IconTexture::PointLigthe] = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ViewPort/PointLigtheIcon.png");
		s_Storarage2D.IconTexture[IconTexture::SpotLigth] = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ViewPort/SpotLigthIcon.png");
		s_Storarage2D.IconTexture[IconTexture::DirectionelLigthe] = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ViewPort/DirectionelLigtheIcon.png");
		s_Storarage2D.IconTexture[IconTexture::Camera] = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ViewPort/CameraIcon.png");
		for (Ref<Texture> tex : s_Storarage2D.IconTexture)
		{
			if (tex)
			{
				s_Storarage2D.Icon.AddTexture(tex);
			}
		}
	}

	void Renderer2D::Shutdown()
	{
		s_Storarage2D.Quades.Destroy();
		s_Storarage2D.QuadesT.Destroy();
		s_Storarage2D.Text.Destroy();
	}

	void Renderer2D::ShutdownEditor()
	{
		for (Ref<Texture>& tex : s_Storarage2D.IconTexture)
		{
			if (tex)
			{
				RY_DESTROY_REF(tex);
			}
		}
		s_Storarage2D.Icon.Destroy();
		
		s_Storarage2D.QuadesSingle.Destroy();
		s_Storarage2D.TextSingle.Destroy();

		s_Storarage2D.IconSingle.Destroy();
	}

	uint32_t Renderer2D::SubmitPass(const ViewPassData& viewPass)
	{
		return 0;
	}

	void Renderer2D::SubmitSpriteSingle(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		Ref<Texture> tex = src.Texture.lock();
		SubmitQuadSingle(transform, tex, entityID);
		
	}

	

	void Renderer2D::SubmitQuadSingle(const glm::mat4& transform, const Ref<Texture>& texture, int entityID)
	{
		int textureIndex = nullptr == texture ? 0 : s_Storarage2D.QuadesSingle.AddTexture(texture);

		ObjectData objectData{
			transform,
			glm::vec4{1.0f, 1.0f, 1.0f, 1.0f},
			textureIndex,
			entityID
		};
		s_Storarage2D.QuadesSingle.Emplace_Back_Push(objectData);
	}
	
	void Renderer2D::SubmitRenderDrawList()
	{
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();
		const Ref<UniformBuffer>& camerPackedUB = Renderer::GetMainPassViewCamerUniform();
		const Ref<UniformBuffer>& displayUB = Renderer::GetMainPassViewDisblayUniform();


#else
		Ref<RenderTarget>& target = Renderer::GetRenderTargetMain();
		RenderTarget& targetMain = *target;
		Ref<UniformBuffer>& camerPackedUB = Renderer::GetPackegeCamerUniformMain();
		Ref<UniformBuffer>& displayUB = Renderer::GetPackegeCamerUniformMain();

#endif		
		
		SubmitRenderDrawListQuad(targetMain, camerPackedUB, displayUB);
		SubmitRenderDrawListText(targetMain, camerPackedUB, displayUB);
		SubmitRenderDrawListIcon(targetMain, camerPackedUB, displayUB);

	}

	void Renderer2D::SubmitRenderDrawListViewPortSelected()
	{
		

	}


#pragma region Quade


	void Renderer2D::SubmitSprite(const glm::mat4& transfrom, SpriteRendererComponent& src, int entityID)
	{
#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE
		if(src.Signel == RenderSginale::NotInit || src.Signel == RenderSginale::UpdateData)
		{
			if (Ref<Texture> tex = src.Texture.lock())
				SubmitQuad(transfrom, tex, entityID);
			else
				SubmitQuad(transfrom, src.Color, entityID);

			src.Signel = RenderSginale::Darw;
		}
		else if (src.Signel == RenderSginale::Darw)
		{
			s_Storarage2D.QuadesT.MoveElementPtr();
		}
#else
		if (Ref<Texture> tex = src.Texture.lock())
			SubmitQuad(transfrom, tex, entityID);
		else
			SubmitQuad(transfrom, src.Color, entityID);
#endif
	}

	void Renderer2D::SubmitQuad(const glm::mat4& transfrom, const glm::vec4& color, int entityID)
	{
		const int textureIndex = 0;
#if RY_RENDER2D_TEST_BATCHING_SBO
		ObjectData objectData{
				transfrom,
				color,
				textureIndex,
				entityID
		};
		s_Storarage2D.QuadesT.Emplace_Back_Push(objectData);
#else
		constexpr size_t quadVertexCount = 4;

		constexpr glm::vec2 texCoord[] = {
			{0.0f, 0.0},
			{1.0f, 0.0},
			{1.0f, 1.0},
			{0.0f, 1.0}
		};
		constexpr glm::vec4 vertexPos[] = {
				{ -0.5f, -0.5f, 0.0f, 1.0f },
				{  0.5f, -0.5f, 0.0f, 1.0f },
				{  0.5f,  0.5f, 0.0f, 1.0f },
				{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

	
	
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			glm::vec3 position = transfrom * vertexPos[i];
			s_Storarage2D.Quades.Emplace_Back_Push(QuadVertex{
					position,
					color,
					texCoord[i],
					(int)textureIndex,
					entityID
				});
		}
#endif
	}

	void Renderer2D::SubmitQuad(const glm::mat4& transfrom, const Ref<Texture>& texture, int entityID)
	{

		
#if RY_RENDER2D_TEST_BATCHING_SBO
		int textureIndex = s_Storarage2D.QuadesT.AddTexture(texture);
		ObjectData objectData{
				transfrom,
				glm::vec4{1.,1.,1.,1.},
				textureIndex,
				entityID
		};
		s_Storarage2D.QuadesT.Emplace_Back_Push(objectData);

#else
		int textureIndex = s_Storarage2D.Quades.AddTexture(texture);
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 texCoord[] = {
			{0.0f, 0.0},
			{1.0f, 0.0},
			{1.0f, 1.0},
			{0.0f, 1.0}
		};
		constexpr glm::vec4 vertexPos[] = {
				{ -0.5f, -0.5f, 0.0f, 1.0f },
				{  0.5f, -0.5f, 0.0f, 1.0f },
				{  0.5f,  0.5f, 0.0f, 1.0f },
				{ -0.5f,  0.5f, 0.0f, 1.0f }
		};
		
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Storarage2D.Quades.Emplace_Back_Push(QuadVertex{
					glm::vec3{transfrom * vertexPos[i]},
					glm::vec4{1.0f,1.0f, 1.0f, 1.0f },
					texCoord[i],
					textureIndex,
					entityID
				});
		}

#endif
	}

	



	void Renderer2D::SubmitRenderDrawListQuad(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
	{


		if(s_Storarage2D.QuadesT.GenartateShaderDrawList(camerbuffer, displaybuffer))
		{
			std::vector<ShaderDrawResource>& shaderDrawResourceVec = Renderer::GetShaderDrawResourceMain();
			const ShaderDrawResource& shaderDrawResource = s_Storarage2D.QuadesT.GetShaderDrawList();
			shaderDrawResourceVec.emplace_back(shaderDrawResource);
		}

		s_Storarage2D.QuadesT.Clear();
		s_Storarage2D.QuadesT.RestTextur();
	}

	

	void Renderer2D::DrawQuads()
	{
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<UniformBuffer>& camerB = Renderer::GetMainPassViewCamerUniform();
#else
		const Ref<UniformBuffer>& camerB = Renderer::GetPackegeCamerUniformMain();
#endif
#if RY_RENDER2D_TEST_BATCHING_SBO
		
		uint32_t count;
#if 0
		static uint32_t s_First = 1;
		if(s_First)
		{
			count = s_Storarage2D.QuadesT.FlushData();
			s_First--;
		}
		else
		{
			count = s_Storarage2D.QuadesT.Size() * 6u;
		}
#else
		count = s_Storarage2D.QuadesT.FlushData();
#endif
		if (count > 0)
		{
			camerB->Bind(0);
			s_Storarage2D.QuadesT.Bind("u_Textures", 1u);
			Ref<VertexArray> vertexArray = s_Storarage2D.QuadesT.GetVertexArray();
			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.QuadesT.UnBind(1u);

#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE - 1
			s_Storarage2D.QuadesT.Clear();
			s_Storarage2D.QuadesT.RestTextur();
#else
			s_Storarage2D.QuadesT.Clear();
#endif
#if 0

			
			camerB->Bind(0);
			s_Storarage2D.QuadesT.Bind("u_Textures", 1);

			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.QuadesT.UnBind();

#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE - 1
			s_Storarage2D.QuadesT.Clear();
			s_Storarage2D.QuadesT.RestTextur();
#else
			s_Storarage2D.QuadesT.Clear();
#endif
			camerB->Bind(0);
			s_Storarage2D.QuadesT.Bind("u_Textures", 1);

			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.QuadesT.UnBind();

#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE - 1
			s_Storarage2D.QuadesT.Clear();
			s_Storarage2D.QuadesT.RestTextur();
#else
			s_Storarage2D.QuadesT.Clear();
#endif
			camerB->Bind(0);
			s_Storarage2D.QuadesT.Bind("u_Textures", 1);

			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.QuadesT.UnBind();

#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE - 1
			s_Storarage2D.QuadesT.Clear();
			s_Storarage2D.QuadesT.RestTextur();
#else
			s_Storarage2D.QuadesT.Clear();
#endif
			camerB->Bind(0);
			s_Storarage2D.QuadesT.Bind("u_Textures", 1);

			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.QuadesT.UnBind();

#if RY_RENDER2D_TEST_BATCHING_SBO_UPDATE - 1
			s_Storarage2D.QuadesT.Clear();
			s_Storarage2D.QuadesT.RestTextur();
#else
			s_Storarage2D.QuadesT.Clear();
#endif
#endif
		}


#else
#if TEST_RENDERER_QUADE_001
		
#endif
		uint32_t count = s_Storarage2D.Quades.Bind("u_Textures");
		if (count > 0)
		{
			Ref<VertexArray> vertexArray = s_Storarage2D.Quades.GetShadeVertexArray();
			RenderCommand::DrawIndexedMesh(vertexArray, count);
			s_Storarage2D.Quades.UnBind();
			s_Storarage2D.Quades.Clear();
			s_Storarage2D.Quades.RestTextur();
		}

#if TEST_RENDERER_QUADE_001

		
#endif
#endif
	}


#pragma endregion

#pragma region Text

	void Renderer2D::SubmitString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
	{
		const MSDFData* data = font->GetMSDFData();
		const msdf_atlas::FontGeometry& fontGeometry = data->FontGeometry;
		const msdfgen::FontMetrics& metrics = fontGeometry.getMetrics();
		Ref<Texture> fontAtlas = font->GetAtlasTexture();
		int textureIndex = s_Storarage2D.Text.AddTexture(fontAtlas);

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				// NOTE(Yan): is this right?
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
				continue;
			}

			const msdf_atlas::GlyphGeometry* glyph = fontGeometry.getGlyph(character);
			if (!glyph)
			{
				glyph = fontGeometry.getGlyph('?');
			}
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			s_Storarage2D.Text.Emplace_Back_Push(TextVertex{
					transform * glm::vec4(quadMax, 0.0f, 1.0f),
					textParams.Color,
					texCoordMax,
					textureIndex,
					entityID
				});

			s_Storarage2D.Text.Emplace_Back_Push(TextVertex{
					transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f),
					textParams.Color,
					{ texCoordMin.x, texCoordMax.y },
					textureIndex,
					entityID
				});
			
			s_Storarage2D.Text.Emplace_Back_Push(TextVertex{
					transform * glm::vec4(quadMin, 0.0f, 1.0f),
					textParams.Color,
					texCoordMin,
					textureIndex,
					entityID
				});
	

			s_Storarage2D.Text.Emplace_Back_Push(TextVertex{
					transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f),
					textParams.Color,
					{ texCoordMax.x, texCoordMin.y },
					textureIndex,
					entityID
				});

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void Renderer2D::SubmitStringCom(const glm::mat4& transform, TextComponent& textC, int entityID)
	{
		SubmitString(textC.TextString, textC.FontAsset, transform, { textC.Color, textC.Kerning, textC.LineSpacing }, entityID);
	}

	void Renderer2D::SubmitRenderDrawListText(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
	{


		s_Storarage2D.Text.AddShadeDrawListToRenderTarget(target, camerbuffer, displaybuffer);
		s_Storarage2D.Text.Clear();
		s_Storarage2D.Text.RestTextur();
	}

	
	void Renderer2D::RenderText()
	{
#if TEST_RENDERER_TEXT_001
		

#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<UniformBuffer>& camerB = Renderer::GetMainPassViewCamerUniform();
		const Ref<Framebuffer>& framneB = Renderer::GetMainPassViewFramebuffer();
#else
		const Ref<UniformBuffer>& camerB = Renderer::GetPackegeCamerUniformMain();
		Ref<Framebuffer> framneB = Renderer::GetFramebufferMain();
#endif
		framneB->Bind();
		camerB->Bind(0);
#endif
		uint32_t count = s_Storarage2D.Text.Bind("u_Textures");
		if (count > 0)
		{
			Ref<VertexArray> vertexArray = s_Storarage2D.Text.GetVertexArray();
			RenderCommand::DrawIndexedMesh(vertexArray, count);
			// s_Storarage2D.Text.UnBind();
			s_Storarage2D.Text.Clear();
			s_Storarage2D.Text.RestTextur();
		}

#if TEST_RENDERER_TEXT_001
		// framneB->Unbind();

#endif
	}


#pragma endregion

#pragma region Icons

	void Renderer2D::SubmitLigthPointIcon(const glm::mat4& transform, int entityID)
	{
		int textureIndex = IconTexture::PointLigthe;
		SubmitIcon(transform, textureIndex, entityID);
	}

	void Renderer2D::SubmitLigthSpotIcon(const glm::mat4& transform, int entityID)
	{
		int textureIndex = IconTexture::SpotLigth;
		SubmitIcon(transform, textureIndex, entityID);
	}

	void Renderer2D::SubmitLigthDirctionelIcon(const glm::mat4& transform, int entityID)
	{
		int textureIndex = IconTexture::DirectionelLigthe;
		SubmitIcon(transform, textureIndex, entityID);
	}

	void Renderer2D::SubmitCameraIcon(const glm::mat4& transform, int entityID)
	{
		int textureIndex = IconTexture::Camera;
		SubmitIcon(transform, textureIndex, entityID);
	}

	void Renderer2D::SubmitIcon(const glm::mat4& transform, int index, int entityID)
	{
		const Ref<Texture>& texture = s_Storarage2D.IconTexture[index];
		int textureIndex = s_Storarage2D.Icon.AddTexture(texture);
		
		constexpr size_t iconVertexCount = 4;
		for (size_t i = 0; i < iconVertexCount; i++)
		{
			s_Storarage2D.Icon.Emplace_Back_Push(
				IconVertex{
					glm::vec4{ transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) },
					(int)BIT(i),
					textureIndex,
					entityID 
				});
			
		}


	}
	
	void Renderer2D::SubmitRenderDrawListIcon(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer)
	{

		s_Storarage2D.Icon.AddShadeDrawListToRenderTarget(target, camerbuffer, displaybuffer);
		s_Storarage2D.Icon.Clear();
		s_Storarage2D.Icon.RestTextur();

	}

	

	
	void Renderer2D::RenderIcons()
	{

#if TEST_RENDERER_ICON_001
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<UniformBuffer>& camerB = Renderer::GetMainPassViewCamerUniform();
		const Ref<UniformBuffer>& displayB = Renderer::GetMainPassViewDisblayUniform();
#else
		const Ref<UniformBuffer>& camerB = Renderer::GetPackegeCamerUniformMain();
		Ref<UniformBuffer>& displayB = Renderer::GetViewUniformMain();
#endif
		camerB->Bind(0);
		displayB->Bind(1);
#endif
		uint32_t count = s_Storarage2D.Icon.Bind("u_Textures");
		if (count > 0)
		{
			Ref<VertexArray> vertexArray = s_Storarage2D.Icon.GetVertexArray();
			RenderCommand::DrawIndexedMesh(vertexArray, count);
			// s_Storarage2D.Icon.UnBind();
			s_Storarage2D.Icon.Clear();
		}

#if TEST_RENDERER_ICON_001


#endif
	}	
	
	

#pragma endregion



}

