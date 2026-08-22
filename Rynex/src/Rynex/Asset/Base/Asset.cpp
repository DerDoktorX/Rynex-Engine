#include "rypch.h"
#include "Asset.h"
#include <magic_enum.hpp>

namespace Rynex {

#define RY_TRANSFORM_TYPE_STRING(x) #x

    std::thread::id Asset::s_MainThreadId = std::this_thread::get_id();


    Asset::~Asset()
    {
     
        uint64_t id = Handle;
        RY_ASSET_WARN("Destroy Asset: {0} ", id);
    }

    std::string_view Asset::AssetTypeToString(AssetType type)
    {
        return magic_enum::enum_name(type);
    }

    AssetType Asset::AssetTypeFromString(std::string_view assetType)
    {
        std::optional<AssetType> typeAsset = magic_enum::enum_cast<AssetType>(assetType, magic_enum::case_insensitive);
        if (typeAsset.has_value())
            return typeAsset.value();
        return AssetType::None;
    }

    AssetType Asset::GetAssetTypeFromFilePath(const std::filesystem::path& filePath)
    {
        
        std::filesystem::path extension = filePath.extension();
        std::string extensionStr = extension.string();
        if (extensionStr == ".png" 
            || extensionStr == ".rytex2d" 
            || extension == ".jpeg" 
            || extension == ".jpg"
            || extension == ".hdr")	return AssetType::Texture2D;
        if (extensionStr.rfind(".ryframe-", 0) == 0) return AssetType::Texture2D;
        if (extensionStr == ".cs")		        return AssetType::Script;
        if (extensionStr == ".glsl")		    return AssetType::Shader;
        if (extensionStr == ".gltf")            return AssetType::MeshSource;
        if (extensionStr == ".rymesh")          return AssetType::MeshSource;
        if (extensionStr == ".fbx")             return AssetType::MeshSource;
        if (extensionStr == ".usda")            return AssetType::MeshSource;
        if (extensionStr == ".rystmesh")        return AssetType::MeshStatic;

        if (extensionStr == ".rynexscene")     return AssetType::Scene;
        if (extensionStr == ".ryframe")        return AssetType::Framebuffer;
        if (extensionStr == ".ttf")            return AssetType::TextFont;
        if (extensionStr == ".bin")            return AssetType::BinaryFile;
        if (extensionStr == ".max")            return AssetType::BinaryFile;
        if (extensionStr == ".txt")            return AssetType::TextFile;
        if (extensionStr == ".ryarray")        return AssetType::VertexArray;
        if (extensionStr == ".ryarray-i")      return AssetType::IndexBuffer;
        if (extensionStr.rfind(".ryarray-", 0) == 0)
            return AssetType::VertexBuffer;
        
        std::filesystem::path filename = filePath.filename();
        RY_ASSET_ERROR("Error: AssetType GetAssetTypeFromFilePath! Unkowne AssetType: ({} on {})", extensionStr, filename);

        return AssetType::None;
    }

    std::string Asset::GetAssetTypeDragAndDropName(AssetType type)
    {

        switch (type)
        {
        case AssetType::None:
        case AssetType::BinaryFile:
        case AssetType::TextFile:
            return "ASSET_BROWSER_NONE";
        case AssetType::Scene:          return "ASSET_BROWSER_SCENE";
        case AssetType::Script:         return "ASSET_BROWSER_SCRIPT";
        case AssetType::Shader:         return "ASSET_BROWSER_SHADER";
        case AssetType::Texture:        return "ASSET_BROWSER_TEXTURE";
        case AssetType::Texture2D:      return "ASSET_BROWSER_TEXTURE2D";
        case AssetType::Framebuffer:    return "ASSET_BROWSER_FRAMBUFFER";
        case AssetType::VertexArray:    return "ASSET_BROWSER_VERTEXARRAY";
        case AssetType::VertexBuffer:   return "ASSET_BROWSER_VERTEXBUFFER";
        case AssetType::IndexBuffer:    return "ASSET_BROWSER_INDEXBUFFER";
        case AssetType::Model:          return "ASSET_BROWSER_MODEL";
        case AssetType::Mesh:           return "ASSET_BROWSER_MESH";

        case AssetType::MeshSource:     return "ASSET_BROWSER_MESH_SOURCE";
        case AssetType::MeshStatic:     return "ASSET_BROWSER_MESH_STAITIC";

        case AssetType::Value:          return "ASSET_BROWSER_VALUE";
        default:
            break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return "ASSET_BROWSER_ITEM";
    }

    std::string Asset::GetAssetTypeMoveAssetInfosName(AssetType type)
    {
        switch (type)
        {
            case AssetType::None:
            case AssetType::BinaryFile:
            case AssetType::TextFile:
                return "ASSET_BROWSER_OVERIDE_NONE";
            case AssetType::Scene:          return "ASSET_BROWSER_OVERIDE_SCENE";
            case AssetType::Shader:         return "ASSET_BROWSER_OVERIDE_SHADER";
            case AssetType::Script:         return "ASSET_BROWSER_OVERIDE_SRCIPT";
            case AssetType::Texture:        return "ASSET_BROWSER_OVERIDE_TEXTURE";
            case AssetType::Texture2D:      return "ASSET_BROWSER_OVERIDE_TEXTURE2D";
            case AssetType::Framebuffer:    return "ASSET_BROWSER_OVERIDE_FRAMBUFFER";
            case AssetType::VertexArray:    return "ASSET_BROWSER_OVERIDE_VERTEXARRAY";
            case AssetType::VertexBuffer:   return "ASSET_BROWSER_OVERIDE_VERTEXBUFFER";
            case AssetType::IndexBuffer:    return "ASSET_BROWSER_OVERIDE_INDEXBUFFER";
            case AssetType::Model:          return "ASSET_BROWSER_OVERIDE_MODEL";
            case AssetType::Mesh:           return "ASSET_BROWSER_OVERIDE_MESH";
            case AssetType::MeshSource:     return "ASSET_BROWSER_OVERIDE_MESH_SOURCE";
            case AssetType::MeshStatic:     return "ASSET_BROWSER_OVERIDE_MESH_STAITIC";

            case AssetType::Value:          return "ASSET_BROWSER_OVERIDE_VALUE";
            
            default:
                break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return "ASSET_BROWSER_ITEM";
    }

    bool Asset::CurrentOnMainThread()
    {
        return std::this_thread::get_id() == s_MainThreadId;
    }

}