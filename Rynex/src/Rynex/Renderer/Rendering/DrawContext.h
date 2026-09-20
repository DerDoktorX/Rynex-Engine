#pragma once



namespace Rynex {
    struct ShaderComputeList;
    struct ShaderDrawList;
    class Camera;

    struct SourceLight
    {

    };


    class DrawContext
    {
    public:
        ~DrawContext();

        void SetCamera(const glm::mat4& model, const Camera& camera);
        void AddDrawCall(const ShaderDrawList& shaderDrawList);
        void AddDrawCall(const ShaderComputeList& shaderDrawList);
        void AddLight();
    public:


    };
} // Rynex

