#include "rypch.h"
#include "Asset.h"

#include <regex>

namespace Rynex {



    std::string_view AssetTypeToString(AssetType type)
    {
        switch (type)
        {
            case AssetType::None:           return "AssetType::None";
            case AssetType::Scene:          return "AssetType::Scene";
            case AssetType::Shader:         return "AssetType::Shader";
            case AssetType::Texture:        return "AssetType::Texture";
            case AssetType::Texture2D:      return "AssetType::Texture2D";
            case AssetType::FrameBuffer:    return "AssetType::FrameBuffer";
            case AssetType::VertexArray:    return "AssetType::VertexArray";
            case AssetType::VertexBuffer:   return "AssetType::VertexBuffer";
            case AssetType::IndexBuffer:    return "AssetType::IndexBuffer";
            case AssetType::Mesh:           return "AssetType::Mesh";
            case AssetType::Model:          return "AssetType::Model";
            case AssetType::Value:          return "AssetType::Value";
            default:
                break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return nullptr;
    }

    AssetType AssetTypeFromString(std::string_view assetType)
    {
        if (assetType == "AssetType::Texture2D")    return AssetType::Texture2D;
        if (assetType == "AssetType::Texture")      return AssetType::Texture;
        if (assetType == "AssetType::Shader")       return AssetType::Shader;   
        if (assetType == "AssetType::Scene")        return AssetType::Scene;
        if (assetType == "AssetType::FrameBuffer")  return AssetType::FrameBuffer;
        if (assetType == "AssetType::VertexArray")  return AssetType::VertexArray;
        if (assetType == "AssetType::VertexBuffer") return AssetType::VertexBuffer;
        if (assetType == "AssetType::IndexBuffer")  return AssetType::IndexBuffer;
        if (assetType == "AssetType::Model")        return AssetType::Model;
        if (assetType == "AssetType::Mesh")         return AssetType::Mesh;
        if (assetType == "AssetType::Value")        return AssetType::Value;
        if (assetType == "AssetType::None")         return AssetType::None;


        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return AssetType::None;
    }

    AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath)
    {
        std::filesystem::path extension = filePath.extension();
        if (extension == ".png" || extension == ".rytex2d" || extension == ".jpeg")	return AssetType::Texture2D;
        if (extension.string().rfind(".ryframe-", 0) == 0) 
            return AssetType::Texture2D;
        if (extension == ".glsl")		    return AssetType::Shader;
        if (extension == ".gltf")           return AssetType::Model;
        if (extension == ".rymesh")         return AssetType::Mesh;
        if (extension == ".rynexscene")     return AssetType::Scene;
        if (extension == ".ryframe")        return AssetType::FrameBuffer;
        if (extension == ".ryarray")        return AssetType::VertexArray;
        if (extension == ".ryarray-i")      return AssetType::IndexBuffer;
        if (extension.string().rfind(".ryarray-", 0) == 0)
            return AssetType::VertexBuffer;
        RY_CORE_ERROR("Error: AssetType GetAssetTypeFromFilePath! Unkowne AssetType");

        return AssetType::None;

    }

    std::string GetAssetTypeDragAndDropName(AssetType type)
    {
        switch (type)
        {
        case AssetType::None:           return "ASSET_BROWSER_NONE";
        case AssetType::Scene:          return "ASSET_BROWSER_SCENE";
        case AssetType::Shader:         return "ASSET_BROWSER_SHADER";
        case AssetType::Texture:        return "ASSET_BROWSER_TEXTURE";
        case AssetType::Texture2D:      return "ASSET_BROWSER_TEXTURE2D";
        case AssetType::FrameBuffer:    return "ASSET_BROWSER_FRAMBUFFER";
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

    std::string GetAssetTypeMoveAssetInfosName(AssetType type)
    {
        switch (type)
        {
            case AssetType::None:           return "ASSET_BROWSER_OVERIDE_NONE";
            case AssetType::Scene:          return "ASSET_BROWSER_OVERIDE_SCENE";
            case AssetType::Shader:         return "ASSET_BROWSER_OVERIDE_SHADER";
            case AssetType::Texture:        return "ASSET_BROWSER_OVERIDE_TEXTURE";
            case AssetType::Texture2D:      return "ASSET_BROWSER_OVERIDE_TEXTURE2D";
            case AssetType::FrameBuffer:    return "ASSET_BROWSER_OVERIDE_FRAMBUFFER";
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