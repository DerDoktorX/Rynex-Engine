#pragma once
#include <Rynex/Core/UUID.h>

namespace Rynex {

	using AssetHandle = UUID;

	enum class AssetType
	{
		None = 0,
		Scene,
		Shader,
		Texture, LinkedTextureArray, StoreTextureArray,
		Texture2D,
		Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
		Entity, SceneCamera,
		Framebuffer,
		VertexBuffer, IndexBuffer, StorageBuffer, IndirectBuffer, BindBuffer, UniformBuffer,
		VertexArray, StorageArray,
		Mesh, Model,
		MeshSource, MeshStatic, MeshDynamic,
		Script, 
		TextFont, 
		TextFile,
		BinaryFile,
		Material, ShaderMaterial, BasicMaterial, DeferredMaterial, PhysicalMaterial, PhongMaterial,
		MaterielArray,
		BindlesArray,
		RenderPipline,
	};

	

	class Asset : public std::enable_shared_from_this<Asset>
	{
	public:
		Asset() = default;
		Asset(const UUID& handle)
			: Handle(handle)
		{ }
		AssetHandle Handle;
#ifdef RY_ASSET_VAILDE_BOOL
		void Invalidate() { m_Valid = false; };
		void Validate() { m_Valid = true; };
		bool IsValid() const{ return m_Valid; };
#endif
		virtual AssetType GetType() const = 0;
		virtual ~Asset();

		static std::string_view AssetTypeToString(AssetType type);
		static AssetType AssetTypeFromString(std::string_view assetType);
		static AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath);
		static std::string GetAssetTypeDragAndDropName(AssetType type);
		static std::string GetAssetTypeMoveAssetInfosName(AssetType type);

		static bool CurrentOnMainThread();



		static Ref<Asset> GetRefInPlace(Asset* assetPtr)
		{
			RY_CORE_ASSERT(nullptr != assetPtr);
			Ref<Asset> asset = assetPtr->shared_from_this();
			RY_CORE_ASSERT(nullptr != asset);
			
			
			return asset;
		}

		static Weak<Asset> GetWeakInPlace(Asset* assetPtr)
		{
			RY_CORE_ASSERT(nullptr != assetPtr);
			Weak<Asset> asset = assetPtr->weak_from_this();
			RY_CORE_ASSERT(!asset.expired());


			return asset;
		}

		template<typename T>
		static Ref<T> GetRefInPlaceType(T* resourcePtr)
		{
			Asset* assetPtr = reinterpret_cast<Asset*>(resourcePtr);
			Ref<Asset> assetRef = Asset::GetRefInPlace(assetPtr);
			Ref<T> resourceRef = std::static_pointer_cast<T, Asset>(assetRef);
			return resourceRef;
		}

		template<typename T, typename N>
		static Ref<N> GetRefInPlaceType(T* assetPtr)
		{
			Ref<T> resourceRefT = Asset::GetRefInPlaceType<T>(assetPtr);
			Ref<N> resourceRefN = Ref<N>(resourceRefT);
			RY_CORE_ASSERT(nullptr != resourceRefN);
			return resourceRefN;
		}


		template<typename T>
		static Weak<T> GetWeakInPlaceType(T* resourcePtr)
		{
			Asset* assetPtr = reinterpret_cast<Asset*>(resourcePtr);
			Weak<Asset> assetWeak = Asset::GetWeakInPlace(assetPtr);
			Weak<T> assetWeakT;

			if (Ref<Asset> assetRef = assetWeak.lock())
			{
				Ref<T> assetRefT = std::static_pointer_cast<T, Asset>(assetRef);
				assetWeakT = assetRefT;
			}
			return assetWeakT;
		}

		template<typename T, typename N>
		static Weak<N> GetWeakInPlaceType(T* assetPtr)
		{
			Weak<T> assetWeakT = Asset::GetWeakInPlace<T>(assetPtr);
			Weak<N> assetWeakN = nullptr;
			if(Ref<T> assetRefT  = assetWeak.lock())
			{
				Ref<T> assetRefN = std::static_pointer_cast<N, T>(assetRefT);
				assetWeakN = assetRefN;
			}
			return assetWeakN;
		}

	private:
		static std::thread::id s_MainThreadId;
#ifdef RY_ASSET_VAILDE_BOOL
		bool m_Valid = false;
#endif
	};


	

}


