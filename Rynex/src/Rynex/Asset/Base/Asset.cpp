#include "rypch.h"
#include "Asset.h"
#include <magic_enum.hpp>

namespace Rynex {

#define RY_TRANSFORM_TYPE_STRING(x) #x

    std::string_view Asset::AssetTypeToString(AssetType type)
    {
       
#if 0
        switch (type)
        {
            case AssetType::None:           return RY_TRANSFORM_TYPE_STRING(AssetType::None);
            case AssetType::Scene:          return RY_TRANSFORM_TYPE_STRING(AssetType::Scene);
            case AssetType::Script:         return RY_TRANSFORM_TYPE_STRING(AssetType::Script);
            case AssetType::Shader:         return RY_TRANSFORM_TYPE_STRING(AssetType::Shader);
            case AssetType::TextFont:       return RY_TRANSFORM_TYPE_STRING(AssetType::TextFont);
            case AssetType::Texture:        return RY_TRANSFORM_TYPE_STRING(AssetType::Texture);
            case AssetType::Texture2D:      return RY_TRANSFORM_TYPE_STRING(AssetType::Texture2D);
            case AssetType::Framebuffer:    return RY_TRANSFORM_TYPE_STRING(AssetType::FrameBuffer);
            case AssetType::VertexArray:    return RY_TRANSFORM_TYPE_STRING(AssetType::VertexArray);
            case AssetType::VertexBuffer:   return RY_TRANSFORM_TYPE_STRING(AssetType::VertexBuffer);
            case AssetType::IndexBuffer:    return RY_TRANSFORM_TYPE_STRING(AssetType::IndexBuffer);
            case AssetType::Mesh:           return RY_TRANSFORM_TYPE_STRING(AssetType::Mesh);
            case AssetType::Model:          return RY_TRANSFORM_TYPE_STRING(AssetType::Model);
            case AssetType::Value:          return RY_TRANSFORM_TYPE_STRING(AssetType::Value);
            default:
                break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return nullptr; 
#else
        return magic_enum::enum_name(type);
#endif
    }

    AssetType Asset::AssetTypeFromString(std::string_view assetType)
    {
#if 0

        if (assetType == "AssetType::Texture2D")    return AssetType::Texture2D;
        if (assetType == "AssetType::Texture")      return AssetType::Texture;
        if (assetType == "AssetType::Shader")       return AssetType::Shader;   
        if (assetType == "AssetType::Scene")        return AssetType::Scene;
        if (assetType == "AssetType::Script")       return AssetType::Script;
        if (assetType == "AssetType::TextFont")     return AssetType::TextFont;
        if (assetType == "AssetType::FrameBuffer")  return AssetType::Framebuffer;
        if (assetType == "AssetType::VertexArray")  return AssetType::VertexArray;
        if (assetType == "AssetType::VertexBuffer") return AssetType::VertexBuffer;
        if (assetType == "AssetType::IndexBuffer")  return AssetType::IndexBuffer;
        if (assetType == "AssetType::Model")        return AssetType::Model;
        if (assetType == "AssetType::MeshSource")   return AssetType::MeshSource;
        if (assetType == "AssetType::MeshSource")   return AssetType::MeshSource;
        if (assetType == "AssetType::Mesh")         return AssetType::Mesh;
        if (assetType == "AssetType::Value")        return AssetType::Value;
        if (assetType == "AssetType::None")         return AssetType::None;


        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return AssetType::None;
#else
        std::optional<AssetType> typeAsset = magic_enum::enum_cast<AssetType>(assetType, magic_enum::case_insensitive);
        if (typeAsset.has_value())
            return typeAsset.value();
        return AssetType::None;
#endif

    }

    AssetType Asset::GetAssetTypeFromFilePath(const std::filesystem::path& filePath)
    {
        std::filesystem::path extension = filePath.extension();
        if (extension == ".png" || extension == ".rytex2d" || extension == ".jpeg" || extension == ".jpg")	return AssetType::Texture2D;
        if (extension.string().rfind(".ryframe-", 0) == 0) return AssetType::Texture2D;
        if (extension == ".cs")		        return AssetType::Script;
        if (extension == ".glsl")		    return AssetType::Shader;
        if (extension == ".gltf")           return AssetType::Model;
        if (extension == ".rymesh")         return AssetType::Mesh;
        if (extension == ".rynexscene")     return AssetType::Scene;
        if (extension == ".ryframe")        return AssetType::Framebuffer;
        if (extension == ".ttf")            return AssetType::TextFont;
        if (extension == ".ryarray")        return AssetType::VertexArray;
        if (extension == ".ryarray-i")      return AssetType::IndexBuffer;
        if (extension.string().rfind(".ryarray-", 0) == 0)
            return AssetType::VertexBuffer;
        RY_CORE_ERROR("Error: AssetType GetAssetTypeFromFilePath! Unkowne AssetType");

        return AssetType::None;
    }

    std::string Asset::GetAssetTypeDragAndDropName(AssetType type)
    {

        switch (type)
        {
        case AssetType::None:           return "ASSET_BROWSER_NONE";
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
            case AssetType::None:           return "ASSET_BROWSER_OVERIDE_NONE";
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
            case AssetType::Value:          return "ASSET_BROWSER_OVERIDE_VALUE";
            
            default:
                break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return "ASSET_BROWSER_ITEM";
    }

}