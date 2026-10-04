#pragma once
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Scene/ScenePrototyps.h>
#include <Rynex/Renderer/Materials/Material.h>
namespace Rynex {
    struct ShaderComputeList;
    struct ShaderDrawList;
    class Camera;

    using TierPassKindUnit = uint8_t;
    enum class TierPassKind : TierPassKindUnit;

    class DrawContext
    {
    public:
        DrawContext();
        ~DrawContext();

        void NextFrame();
        void SetPass(const BufferLayout& layoutOuPut, TierPassKind pass);
        void SetLod(int levelOffDetails);
        void SetFeatureMask(FeatureMask featureMask);

        void SetOutColorTexture(const Ref<Texture>& texture, uint32_t index = 0u);
        void SetOutDeathTexture(const Ref<Texture>& texture);
        void SetCamera(const glm::mat4& model, const Camera& camera);
        void SetViewPortSize(const glm::uvec4& viewPortSize);


        void AddLight(const glm::mat4& model, const DirectionLightComponent& direction);
        void AddLight(const glm::mat4& model, const PointLightComponent& point);
        void AddLight(const glm::mat4& model, const SpotLightComponent& spot);

        void AddLight(const glm::mat4& model, const DirectionLightComponent& direction, const Ref<Texture>& outTexture);
        void AddLight(const glm::mat4& model, const PointLightComponent& point, const Ref<Texture>& outTexture);
        void AddLight(const glm::mat4& model, const SpotLightComponent& spot, const Ref<Texture>& outTexture);


        void AddCompute(const ShaderDrawList& shaderDrawList);
        void AddDraw(const ShaderComputeList& shaderComputeList);


        TierPassKind GetPass() const;
        int GetLod() const;
        const BufferLayout& GetLayoutOutPut() const;

        uint64_t GetFrameIndex() const;
    // ------------------------------------------------------------------------------------------------------------------------
    };
} // Rynex

