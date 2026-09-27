#pragma once
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Core/UnorderDoubleMap.h>
#include <Rynex/Renderer/Materials/MaterialTypes.h>
#include <Rynex/Renderer/API/BindlesTextureArray.h>
#include <Rynex/Renderer/Objects/BufferDataPack.h>

namespace Rynex {
	namespace RenderMode {
		enum RenderMode : uint16_t;
	}

	struct ShaderMaterialDefaultNames;
	struct CameraData;
	struct MeshTexture;

	namespace DrawSpecification {
		using BatchConfig = int;

		enum ResourceType : int
		{
			MaterielParameter = 1,
			MaterielTexture = 2,
			RenderObject = 3,
			Geometry = 4
		};
		constexpr int s_ResurceTypeBitMove = BinaryPresentionCount(4);


		enum BatchingType : BatchConfig
		{
			Uniform = 1 << s_ResurceTypeBitMove,
			Vertex = 2 << s_ResurceTypeBitMove,
			ShaderStorageBufferObject = 3 << s_ResurceTypeBitMove,
			Texture = 4 << s_ResurceTypeBitMove
		};
		constexpr int s_BatchingTypeBitMove = BinaryPresentionCount(4) + s_ResurceTypeBitMove;


		enum Distribution : BatchConfig
		{
			DistMultyBind = 1 <<  s_BatchingTypeBitMove,
			DistArray = 2 << s_BatchingTypeBitMove,
			DistInterleaved = 3 << s_BatchingTypeBitMove
		};
		constexpr int s_DistributionBitMove = BinaryPresentionCount(3) + s_BatchingTypeBitMove;


		enum TextureMode : BatchConfig
		{
			TextureNone = 0 << s_DistributionBitMove,
			TextureSparse = 1 << s_DistributionBitMove,
			TextureAtlas = 2 << s_DistributionBitMove,
			TextureBindless = 3 << s_DistributionBitMove,
			TextureArray = 4 << s_DistributionBitMove
		};

	    constexpr int s_TextureModeBitMove = BinaryPresentionCount(4) + s_DistributionBitMove;
	    constexpr int s_DrawSpecificationCount = s_TextureModeBitMove;
	    

	    constexpr BatchConfig ConfigFlag(BatchingType type, Distribution dist)
	    {
	        BatchConfig batchConfig = type | dist;
	        return batchConfig;
	    };

	    constexpr BatchConfig ConfigFlag(BatchingType type, TextureMode mode, Distribution dist)
	    {
	        BatchConfig batchConfig = ConfigFlag(type, dist) | mode;
	        return batchConfig;
	    };

	    constexpr BatchConfig ConfigFlagResource(ResourceType res, BatchConfig batchConfig)
	    {
	        BatchConfig spec = res | batchConfig;
	        return spec;
	    };

	    enum {
	        None = 0,

            RenderObject_Vertex_Array = ConfigFlagResource(ResourceType::RenderObject, ConfigFlag(BatchingType::Vertex, Distribution::DistArray)),
            RenderObject_ShaderStorageBufferObject_Array = ConfigFlagResource(ResourceType::RenderObject, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::DistArray)),
            RenderObject_Uniform_MultyBind = ConfigFlagResource(ResourceType::RenderObject, ConfigFlag(BatchingType::Uniform, Distribution::DistMultyBind)),

            MaterielPram_ShaderStorageBufferObject_Array = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::DistArray)),
            MaterielParameter_ShaderStorageBufferObject_Interleaved = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::DistInterleaved)),
            MaterielTexture_Texture_MultyBind = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, Distribution::DistMultyBind)),
            MaterielTexture_Texture_Bindles_Array = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, TextureMode::TextureBindless, Distribution::DistArray)),


	        Geometry_ShaderStorageBufferObject_Array = ConfigFlagResource(ResourceType::Geometry, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::DistArray)),
        };





	}


    namespace BatchPresets {

	    struct BatchProfile
	    {
	        DrawSpecification::BatchConfig m_Transform            = DrawSpecification::None; // ResourceType::RenderObject
	        DrawSpecification::BatchConfig m_MaterialParameter    = DrawSpecification::None; // ResourceType::MaterialParameter
	        DrawSpecification::BatchConfig m_MaterialTexture      = DrawSpecification::None; // ResourceType::MaterialTexture
	        DrawSpecification::BatchConfig m_Geometry             = DrawSpecification::None; // ResourceType::Geometry (no value defined!)
	    };

	    constexpr BatchProfile TIER_0_SIGNAL{
	        DrawSpecification::RenderObject_Uniform_MultyBind,
            DrawSpecification::None,
            DrawSpecification::MaterielTexture_Texture_MultyBind,
            DrawSpecification::None
        };
	    constexpr BatchProfile TIER_1_INSTANCED{
	        DrawSpecification::RenderObject_Vertex_Array,
            DrawSpecification::None,
            DrawSpecification::MaterielTexture_Texture_MultyBind,
            DrawSpecification::None
        };
	    constexpr BatchProfile TIER_2_INSTANCED_OPTIMIZED_MATERIAL{
	        DrawSpecification::RenderObject_Vertex_Array,
            DrawSpecification::MaterielPram_ShaderStorageBufferObject_Array,
	        DrawSpecification::MaterielTexture_Texture_Bindles_Array,
            DrawSpecification::None
        };
	    constexpr BatchProfile TIER_3_INDIRECT{
	        DrawSpecification::RenderObject_ShaderStorageBufferObject_Array,
            DrawSpecification::MaterielPram_ShaderStorageBufferObject_Array,
	        DrawSpecification::MaterielTexture_Texture_Bindles_Array,
	        DrawSpecification::Geometry_ShaderStorageBufferObject_Array
        };

    }


	struct Pass
	{
		Ref<Shader> shader;
		Ref<UniformBuffer> prame;

		std::vector<TextureTypes> texturesTypeVec;
		std::vector<Ref<Texture>> texturesVec;

		DrawSpecification::BatchConfig drawSpecification;
		int renderMode;
	};

	class Material : public Asset
	{
	private:
		enum DefaultValues
		{
			TextureStoreIndex = -1
		};
	public:
		virtual ~Material() {};

		virtual void SetColor(const glm::vec3& color) = 0;
		virtual glm::vec3 GetColor() const = 0;

		virtual void SetAlpha(float v) = 0;
		virtual float GetAlpha() const = 0;

		virtual int GetShadeRenderMode() const;
		virtual int GetDepthRenderMode() const;

		virtual bool IsRady() const;
		virtual bool UpdateMaterielData() const = 0;

		virtual void* GetMaterielDataPtr() = 0;
		virtual const void* GetMaterielDataPtr() const = 0;
		virtual uint32_t GetMaterielDataByteSize() const = 0;


		virtual void UpdateMaterielData(void* materielArrayData, uint32_t offset, uint32_t size) const = 0;

		virtual int AddMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const	{ return DefaultValues::TextureStoreIndex; }
		virtual int AddMaterielSpecularTextures(Ref<BindlesTextureArray>& bindlesTexures) const { return DefaultValues::TextureStoreIndex; }
		virtual int AddMaterielHeigthTextures(Ref<BindlesTextureArray>& bindlesTexures) const	{ return DefaultValues::TextureStoreIndex; }

		virtual int GetMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const { return DefaultValues::TextureStoreIndex; }
		virtual int GetMaterielSpecularTextures(Ref<BindlesTextureArray>& bindlesTexures) const { return DefaultValues::TextureStoreIndex; }
		virtual int GetMaterielHeigthTextures(Ref<BindlesTextureArray>& bindlesTexures) const { return DefaultValues::TextureStoreIndex; }

		virtual Ref<Texture> GetAlbedoTextures() const { return nullptr; }
		virtual Ref<Texture> GetSpecularTextures() const { return nullptr; }
		virtual Ref<Texture> GetHeigthTextures() const { return nullptr; }

		virtual bool HasSpecForDraw(const BufferLayout& layout, int lodTier) const = 0;
		virtual std::vector<Ref<Texture>> GetTextureForDraw(const BufferLayout& layout, int lodTier) const = 0;
		virtual const Ref<Shader>& GetShaderForDraw(const BufferLayout& layout, int lodTier) const = 0;
		virtual DrawSpecification::BatchConfig GetDrawSpecification(const BufferLayout& layout, int lodTier) const = 0;
		virtual int GetRenderMode(const BufferLayout& layout, int lodTier) const = 0;
		virtual int GetLayoutIndex(const BufferLayout& layout, int lodTier) const = 0;

		virtual bool HasSpecForDraw(const BufferLayout& layout, int lodTier) = 0;
		virtual std::vector<Ref<Texture>> GetTextureForDraw() = 0;
		virtual const Ref<Shader>& GetShaderForDraw() = 0;
		virtual DrawSpecification::BatchConfig GetDrawSpecification() = 0;
		virtual int GetRenderMode() = 0;
		virtual int GetLayoutIndex() = 0;

		virtual Ref<UniformBuffer> GetMaterielUniformBuffer() { return nullptr; }

		static Ref<Material> CreateImport(std::string&& name, std::vector<MeshTexture>&& textures) { return nullptr; };

	    virtual BatchPresets::BatchProfile GetPreferredBatchProfile(const BufferLayout& layout, int lodTier) const
	    {
	        return BatchPresets::TIER_1_INSTANCED; // default behaviour - no part Material breaks
	    }
	private:
		template<typename T>
		static void SetupMaterielObject(T& materielDataObject, int texureAlbedoIndex, int texureSpecularIndex, int texureHeigthIndex);

	public:
		template<typename _Key, typename N>
		static int SetupMaterielObjectMapVector(const Ref<Material>& materiel, MapVector<_Key, N>& mapVector, Ref<BindlesTextureArray>& bindlesTextureArray)
		{
			const _Key& key = GetMaterielKey(mapVector, materiel);
			if (!mapVector.HasKey(key))
			{
				void* ptr = materiel->GetMaterielDataPtr();
				N* msdOrigPtr = (N*)ptr;
				N msdOrigCopy = *msdOrigPtr;
				N& msdMap = mapVector.AddData(key, msdOrigCopy);

				int texureAlbedoIndex = materiel->AddMaterielAlbedoTextures(bindlesTextureArray);
				int texureSpecularIndex = materiel->AddMaterielSpecularTextures(bindlesTextureArray);
				int texureHeigthIndex = materiel->AddMaterielHeigthTextures(bindlesTextureArray);

				SetupMaterielObject<N>(msdMap, texureAlbedoIndex, texureSpecularIndex, texureHeigthIndex);
	
			}
			int index = mapVector.GetIndex(key);
			return index;
		}

		template<typename T>
		static T GetMaterielDataFromMateriel(const Ref<Material>& material);

	private:		
		template<typename T>
		static UUID GetMaterielKey(MapVector<UUID, T>& mapVector, const Ref<Material>& materiel)
		{
			const UUID& handle = materiel->m_Handle;
			return handle;
		}

		template<typename T>
		static int64_t GetMaterielKey(MapVector<int64_t, T>& mapVector, const Ref<Material>& materiel)
		{
			Material* materielPtr = materiel.get();
			int64_t materileKey = (int64_t)materielPtr;
			return materileKey;
		}

	};

	

	

}