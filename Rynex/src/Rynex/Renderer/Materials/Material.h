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

		using ResourceType = enum ResourceType : BatchConfig
		{
			MaterielParameter = 1,
			MaterielTexture = 2,
			Transform = 3,
			Geometry = 4
		};

		constexpr int RESURRECT_TYPE_BIT_MOVE = BinaryPresentionCount(4);


		using BatchingType = enum BatchingType : BatchConfig
		{
			Uniform = 1 << RESURRECT_TYPE_BIT_MOVE,
			Vertex = 2 << RESURRECT_TYPE_BIT_MOVE,
			ShaderStorageBufferObject = 3 << RESURRECT_TYPE_BIT_MOVE,
			Texture = 4 << RESURRECT_TYPE_BIT_MOVE
		};
		constexpr int BATCHING_TYPE_BIT_MOVE = BinaryPresentionCount(4) + RESURRECT_TYPE_BIT_MOVE;


		using Distribution = enum Distribution : BatchConfig
		{
		    Single = 1 <<  BATCHING_TYPE_BIT_MOVE,
			MultipleBind = 2 <<  BATCHING_TYPE_BIT_MOVE,    // Multiply on bind slot in use. Only [1 or n] elements per bind slot.
			Array = 3 << BATCHING_TYPE_BIT_MOVE,            // Only on bind slot in use. But multiply elements.
			Interleaved = 4 << BATCHING_TYPE_BIT_MOVE
		};
		constexpr int DISTRIBUTION_BIT_MOVE = BinaryPresentionCount(4) + BATCHING_TYPE_BIT_MOVE;


		using TextureMode = enum TextureMode : BatchConfig
		{
			TextureNone = 1 << DISTRIBUTION_BIT_MOVE,
			TextureSparse = 2 << DISTRIBUTION_BIT_MOVE,
			TextureAtlas = 3 << DISTRIBUTION_BIT_MOVE,
			TextureBindless = 4 << DISTRIBUTION_BIT_MOVE,
			TextureArray = 5 << DISTRIBUTION_BIT_MOVE
		};

	    constexpr int TEXTURE_MODE_BIT_MOVE = BinaryPresentionCount(5) + DISTRIBUTION_BIT_MOVE;
	    constexpr int DrawSpecificationCount = TEXTURE_MODE_BIT_MOVE;
	    

	    constexpr BatchConfig ConfigFlag(const BatchingType type, const Distribution dist)
	    {
	        const BatchConfig batchConfig = type | dist;
	        return batchConfig;
	    };

	    constexpr BatchConfig ConfigFlag(const BatchingType type, const TextureMode mode, const Distribution dist)
	    {
	        const BatchConfig batchConfig = ConfigFlag(type, dist) | mode;
	        return batchConfig;
	    };

	    constexpr BatchConfig ConfigFlagResource(const ResourceType resource, const BatchConfig batchConfig)
	    {
	        const BatchConfig spec = resource | batchConfig;
	        return spec;
	    };

	    enum
	    {
	        None = 0,

	        Transform_Uniform_Single                    = ConfigFlagResource(ResourceType::Transform, ConfigFlag(BatchingType::Uniform, Distribution::Single)),
	        Transform_Uniform_MultipleBind                 = ConfigFlagResource(ResourceType::Transform, ConfigFlag(BatchingType::Uniform, Distribution::MultipleBind)),
	        Transform_Uniform_Array                     = ConfigFlagResource(ResourceType::Transform, ConfigFlag(BatchingType::Uniform, Distribution::Array)), // Static uniform array size in shader set!
	        Transform_Vertex_Array                      = ConfigFlagResource(ResourceType::Transform, ConfigFlag(BatchingType::Vertex,  Distribution::Array)),
            Transform_ShaderStorageBufferObject_Array   = ConfigFlagResource(ResourceType::Transform, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::Array)),


	        MaterielParameter_Uniform_Single                        = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::Uniform, Distribution::Single)),
	        MaterielParameter_Uniform_Array                         = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::Uniform, Distribution::Array)),          // Static texture array size in shader set! max elements count.
	        MaterielParameter_Uniform_MultipleBind                  = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::Uniform, Distribution::MultipleBind)),   // Static texture binds size in shader set! max elements count.
	        MaterielParameter_ShaderStorageBufferObject_Array       = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::Array)),
            MaterielParameter_ShaderStorageBufferObject_Interleaved = ConfigFlagResource(ResourceType::MaterielParameter, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::Interleaved)),


	        MaterielTexture_Texture_Single              = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, Distribution::Single)),
            MaterielTexture_Texture_MultipleBind        = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, Distribution::MultipleBind)),                     // Static texture array/bindings size in shader set!
	        MaterielTexture_Texture_Atlas_Array         = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, TextureMode::TextureAtlas, Distribution::Array)), // Dynamic texture count and dynamic texture size. Downside large textures.
	        MaterielTexture_Texture_Array_Array         = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, TextureMode::TextureArray, Distribution::Array)), // Static texture dimension size.
	        MaterielTexture_Texture_Bindless_Array      = ConfigFlagResource(ResourceType::MaterielTexture, ConfigFlag(BatchingType::Texture, TextureMode::TextureBindless, Distribution::Array)),

	        Geometry_Vertex_Array                       = ConfigFlagResource(ResourceType::Geometry, ConfigFlag(BatchingType::Vertex, Distribution::Array)),
	        Geometry_ShaderStorageBufferObject_Array    = ConfigFlagResource(ResourceType::Geometry, ConfigFlag(BatchingType::ShaderStorageBufferObject, Distribution::Array)),
        };





	}


    namespace BatchPresets {

	    struct BatchProfile
	    {
	        DrawSpecification::BatchConfig m_Transform            = DrawSpecification::None; // ResourceType::RenderObject
	        DrawSpecification::BatchConfig m_MaterialParameter    = DrawSpecification::None; // ResourceType::MaterialParameter
	        DrawSpecification::BatchConfig m_MaterialTexture      = DrawSpecification::None; // ResourceType::MaterialTexture
	        DrawSpecification::BatchConfig m_Geometry             = DrawSpecification::None; // ResourceType::Geometry
	    };

	    constexpr BatchProfile TIER_0_SINGLE{
	        DrawSpecification::Transform_Uniform_Single,
            DrawSpecification::MaterielParameter_Uniform_Single,
            DrawSpecification::MaterielTexture_Texture_MultipleBind,
            DrawSpecification::Geometry_Vertex_Array
        };
	    constexpr BatchProfile TIER_1_INSTANCED[3]{
	        BatchProfile{
	            DrawSpecification::Transform_Uniform_Array,
                DrawSpecification::MaterielParameter_Uniform_Single,
                DrawSpecification::MaterielTexture_Texture_Single,
                DrawSpecification::Geometry_Vertex_Array
            },  //  Using in case off no shader storage buffer object available.
	        BatchProfile{
	            DrawSpecification::Transform_Vertex_Array,
                DrawSpecification::MaterielParameter_Uniform_Single,
                DrawSpecification::MaterielTexture_Texture_Single,
                DrawSpecification::Geometry_Vertex_Array
	        },  //  Using in case off no shader storage buffer object available.
	        BatchProfile{
	            DrawSpecification::Transform_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielParameter_Uniform_Single,
                DrawSpecification::MaterielTexture_Texture_Single,
                DrawSpecification::Geometry_Vertex_Array
            }
        };


        /**
         * Index off array 12 ellements
         * [0 to 3] =  Using in case off no shader storage buffer object, no multiple bind option (OpenGL specific) and no bindless texture available.
         *      - 0 = Combine only materiels withe equal textures.
         *      - 1 = Texture array.
         *      - 2 = ...
         */
        constexpr BatchProfile TIER_2_INSTANCED_OPTIMIZED_MATERIAL[12]{

	       BatchProfile{
	            DrawSpecification::Transform_Vertex_Array,
                DrawSpecification::MaterielParameter_Uniform_Array,
                DrawSpecification::MaterielTexture_Texture_Single,
                DrawSpecification::Geometry_Vertex_Array
	       },  // Using in case off no shader storage buffer object, no multiple bind option (OpenGL specific) and no bindless texture available. Combine only materiels withe equal textures.
	       BatchProfile{DrawSpecification::Transform_Vertex_Array,
	           DrawSpecification::MaterielParameter_Uniform_Array,
	           DrawSpecification::MaterielTexture_Texture_Array_Array,
	           DrawSpecification::Geometry_Vertex_Array
	       },  // Using in case off no shader storage buffer object, no multiple bind option (OpenGL specific) and no bindless texture available.
	       BatchProfile{
	           DrawSpecification::Transform_Vertex_Array,
	           DrawSpecification::MaterielParameter_Uniform_Array,
	           DrawSpecification::MaterielTexture_Texture_Atlas_Array,
	           DrawSpecification::Geometry_Vertex_Array
	       },  // Using in case off no shader storage buffer object, no multiple bind option (OpenGL specific) and no bindless texture available.
	       BatchProfile{
	           DrawSpecification::Transform_Uniform_MultipleBind,
	           DrawSpecification::MaterielParameter_Uniform_MultipleBind,
	           DrawSpecification::MaterielTexture_Texture_Single,
	           DrawSpecification::Geometry_Vertex_Array
	       },  // Using in case off no shader storage buffer object, not enough bind slots available (OpenGL specific) and bindless texture available.

	        // multiple bind option (OpenGL specific) now available.
	        BatchProfile{
	            DrawSpecification::Transform_Vertex_Array,
                DrawSpecification::MaterielParameter_Uniform_MultipleBind,
                DrawSpecification::MaterielTexture_Texture_Array_Array,
                DrawSpecification::Geometry_Vertex_Array
            },   // Using in case off no shader storage buffer object, not enough bind slots available (OpenGL specific) and no bindless texture available.
	        BatchProfile{
	            DrawSpecification::Transform_Vertex_Array,
                DrawSpecification::MaterielParameter_Uniform_MultipleBind,
                DrawSpecification::MaterielTexture_Texture_Atlas_Array,
                DrawSpecification::Geometry_Vertex_Array
            },  // Using in case off no shader storage buffer object, not enough bind slots available and no bindless texture available.

	        // shader storage buffer object now available.

	        BatchProfile{
	            DrawSpecification::Transform_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielTexture_Texture_Single,
                DrawSpecification::Geometry_Vertex_Array
            }, // Using in case off no vertex per instance possible and no Bindless shader storage buffer object available! Combine only materiels withe equal textures.
	        BatchProfile{
	            DrawSpecification::Transform_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielTexture_Texture_Atlas_Array,
                DrawSpecification::Geometry_Vertex_Array
            }, // Using in case off no vertex per instance possible, no Bindless shader storage buffer object and no multiple bind option (OpenGL specific) available!
	        BatchProfile{
	            DrawSpecification::Transform_Vertex_Array,
                DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
                DrawSpecification::MaterielTexture_Texture_Array_Array,
                DrawSpecification::Geometry_Vertex_Array
            }, // Using in case off no multiple bind option (OpenGL specific) and no Bindless shader storage buffer object available!
            BatchProfile{
	           DrawSpecification::Transform_ShaderStorageBufferObject_Array,
               DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
               DrawSpecification::MaterielTexture_Texture_Array_Array,
               DrawSpecification::Geometry_Vertex_Array
           }, // Using in case off no multiple bind option (OpenGL specific) and no Bindless shader storage buffer object available.

	        // Bindless shader storage buffer object now available.
            BatchProfile{
               DrawSpecification::Transform_ShaderStorageBufferObject_Array,
               DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
               DrawSpecification::MaterielTexture_Texture_Bindless_Array,
               DrawSpecification::Geometry_Vertex_Array
            }, // Using in case off no vertex per instance possible!
            BatchProfile{
	           DrawSpecification::Transform_Vertex_Array,
               DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
               DrawSpecification::MaterielTexture_Texture_Bindless_Array,
               DrawSpecification::Geometry_Vertex_Array
            },

        };

	    constexpr BatchProfile TIER_3_INDIRECT{
	        DrawSpecification::Transform_ShaderStorageBufferObject_Array,
            DrawSpecification::MaterielParameter_ShaderStorageBufferObject_Array,
	        DrawSpecification::MaterielTexture_Texture_Bindless_Array,
	        DrawSpecification::Geometry_Vertex_Array
        };


    }
    enum class TierPassKind : uint8_t { Shade = 0u, Depth = 1u };   // Main = Shade, Current = Depth (Shadow)


	struct Pass
	{
		Ref<Shader> m_Shader;
		Ref<UniformBuffer> m_Parameter;

		std::vector<TextureTypes> m_TexturesTypeVec;
		std::vector<Ref<Texture>> m_TexturesVec;

		DrawSpecification::BatchConfig m_DrawSpecification;
		int m_RenderMode;
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

		virtual int AddMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlessTextures) const	{ return DefaultValues::TextureStoreIndex; }
		virtual int AddMaterielSpecularTextures(Ref<BindlesTextureArray>& bindlessTextures) const { return DefaultValues::TextureStoreIndex; }
		virtual int AddMaterielHeightTextures(Ref<BindlesTextureArray>& bindlessTextures) const	{ return DefaultValues::TextureStoreIndex; }

		virtual int GetMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlessTextures) const { return DefaultValues::TextureStoreIndex; }
		virtual int GetMaterielSpecularTextures(Ref<BindlesTextureArray>& bindlessTextures) const { return DefaultValues::TextureStoreIndex; }
		virtual int GetMaterielHeightTextures(Ref<BindlesTextureArray>& bindlessTextures) const { return DefaultValues::TextureStoreIndex; }

		virtual Ref<Texture> GetAlbedoTextures() const { return nullptr; }
		virtual Ref<Texture> GetSpecularTextures() const { return nullptr; }
		virtual Ref<Texture> GetHeightTextures() const { return nullptr; }

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

	    virtual BatchPresets::BatchProfile GetPreferredBatchProfile(TierPassKind pass, int lodTier) const
	    {
	        return BatchPresets::TIER_1_INSTANCED[0]; // default behaviour - no part Material breaks
	    }
	private:
		template<typename T>
		static void SetupMaterielObject(T& materielDataObject, int texureAlbedoIndex, int texureSpecularIndex, int texureHeigthIndex);

	public:
		template<typename _Key, typename N>
		static int SetupMaterielObjectMapVector(const Ref<Material>& materiel, MapVector<_Key, N>& mapVector, Ref<BindlesTextureArray>& bindlessTextureArray)
		{
			const _Key& key = GetMaterielKey(mapVector, materiel);
			if (!mapVector.HasKey(key))
			{
				void* ptr = materiel->GetMaterielDataPtr();
				N* msdOrigPtr = static_cast<N*>(ptr);
				N msdOrigCopy = *msdOrigPtr;
				N& msdMap = mapVector.AddData(key, msdOrigCopy);

				const int textureAlbedoIndex = materiel->AddMaterielAlbedoTextures(bindlessTextureArray);
				const int textureSpecularIndex = materiel->AddMaterielSpecularTextures(bindlessTextureArray);
				const int textureHeightIndex = materiel->AddMaterielHeightTextures(bindlessTextureArray);

				SetupMaterielObject<N>(msdMap, textureAlbedoIndex, textureSpecularIndex, textureHeightIndex);
	
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