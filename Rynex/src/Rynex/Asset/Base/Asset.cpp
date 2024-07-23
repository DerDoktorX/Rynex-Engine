#include "rypch.h"
#include "Asset.h"

namespace Rynex {



    std::string_view AssetTypeToString(AssetType type)
    {
        RY_PROFILE_FUNCTION();
        switch (type)
        {
            case AssetType::None:        return "AssetType::None";
            case AssetType::Scene:       return "AssetType::Scene";
            case AssetType::Shader:      return "AssetType::Shader";
            case AssetType::Texture:     return "AssetType::Texture";
            case AssetType::Texture2D:   return "AssetType::Texture2D";
            default:
                break;
        }
        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return nullptr;
    }

    AssetType AssetTypeFromString(std::string_view assetType)
    {
        RY_PROFILE_FUNCTION();
        if (assetType == "AssetType::Texture2D")    return AssetType::Texture2D;
        if (assetType == "AssetType::Texture")      return AssetType::Texture;
        if (assetType == "AssetType::Shader")       return AssetType::Shader;   
        if (assetType == "AssetType::Scene")        return AssetType::Scene;
        if (assetType == "AssetType::None")         return AssetType::None;

        RY_CORE_ASSERT(false, "Error: Unkowne AssetType");
        return AssetType::None;
    }

    AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath)
    {
        RY_PROFILE_FUNCTION();
        std::filesystem::path extension = filePath.extension();

        if (extension == ".png")		return AssetType::Texture2D;
        if (extension == ".glsl")		return AssetType::Shader;
        if (extension == ".rynexscene") return AssetType::Scene;

        RY_CORE_ERROR("Error: AssetType GetAssetTypeFromFilePath! Unkowne AssetType");

        return AssetType::None;

    }

}