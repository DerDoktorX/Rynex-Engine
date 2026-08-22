#pragma once
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/API/Shader.h>

#include <Rynex/Renderer/SceneRender/DrawCommands.h>
#include <Rynex/Renderer/SceneRender/SceneLightData.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>
#include  <Rynex/Renderer/SceneRender/SceneRenderPipeline.h>

namespace Rynex {

    // Scene / ECS
    class Scene;

    // Mesh types (defined in Rynex/Renderer/Mesh/)
    class MeshStatic;
    class MeshDynamic;
    class MeshSingle;

    // Material / font
    class Material;
    class Font;


    struct RYNEX_API SceneRenderContext
    {
        SceneRenderCameraData camera;
        glm::uvec2  viewportSize    = { 1280u, 720u };
        glm::vec4 backgroundColor   = { 0.1f, 0.1f, 0.1f, 1.0f };

        // Editor-specific context
        bool isEditorContext = false;
        entt::entity selectedEntity = entt::null;
        bool showGrid = false;
        bool showBoundingBoxes = false;
        bool showLightIcons = false;
        bool showCameraIcons = false;

        // Optimisation hints
        bool enableFrustumCulling = true;
        FrustumCullMode cullMode = FrustumCullMode::CPU;
    };




	class SceneRenderer
	{
	public:
		SceneRenderer();
		~SceneRenderer();

		void Init();
		void Shutdown();

        void BeginScene(const SceneRenderContext& context);
        void EndScene();
        void FlushScene(); // execute the pipeline without waiting for EndScene()


        void SubmitScene(const Ref<Scene>& scene);

        void Submit3DStaticMeshesFromScene(const Ref<Scene>& scene);
        void Submit3DSingleStaticMeshesFromScene(const Ref<Scene>& scene);
        void Submit3DDynamicMeshesFromScene(const Ref<Scene>& scene);
        void Submit2DSpritesFromScene(const Ref<Scene>& scene);
        void SubmitTextFromScene(const Ref<Scene>& scene);
        void SubmitLightsFromScene(const Ref<Scene>& scene);


        void Submit3DStaticMesh(
            entt::entity entity,
            const glm::mat4& transform,
            const Ref<MeshStatic>& mesh
        );
        
     

        void Submit3DMesh(
            entt::entity entity,
            const glm::mat4& transform,
            const Ref<VertexArray>& geometry,
            const Ref<Material>& material,
            uint32_t indexCount = 0u,
            int renderMode = 0,
            bool castShadow = true
        );

        void Submit3DDynamicMesh(
            entt::entity entity,
            const glm::mat4& transform,
            const Ref<MeshDynamic>& mesh,
            const Ref<Material>& material
        );

        void Submit3DSingleMesh(
            entt::entity entity,
            const glm::mat4& transform,
            const Ref<MeshSingle>& mesh,
            const Ref<Material>& material
        );
        
        // 2d quads
        void SubmitQuad(
            entt::entity entity,
            const glm::mat4& transform,
            const glm::vec4& color
        );

        void SubmitQuad(
            entt::entity entity,
            const glm::mat4& transform,
            const glm::vec4& color,
            const Ref<Texture>& texture,
            float tilingFactor = 1.0f
        );
        
        // Text
        void SubmitText(
            entt::entity entity,
            const glm::mat4& transform,
            const std::string& text,
            const Ref<Font>& font,
            const glm::vec4& color,
            float kerning = 0.0f,
            float lineSpacing = 0.0f
        );

        // Lights
        void SubmitDirectionalLight(
            entt::entity entity,
            const glm::vec3& direction,
            const glm::vec3& color,
            float intensity,
            bool castShadow = false
        );


        void SubmitPointLight(
            entt::entity entity,
            const glm::vec3& position,
            const glm::vec3& color,
            float intensity,
            float range,
            float constant = 1.0f,
            float linear = 0.022f,
            float quadratic = 0.0019f,
            bool castShadow = false
        );

        void SubmitSpotLight(
            entt::entity entity,
            const glm::vec3& position,
            const glm::vec3& direction,
            const glm::vec3& color,
            float intensity,
            float range,
            float innerConeAngle,
            float outerConeAngle,
            bool castShadow = false
        );


        // -- Pipeline Management ---------------------------------------------------------------------------------------------
        void SetPipeline(Ref<SceneRenderPipeline> pipeline);

        Ref<SceneRenderPipeline>& GetPipeline();
        const Ref<SceneRenderPipeline>& GetPipeline() const;

        Ref<RenderPass> GetPass(const std::string& name) const;

        template<typename PassType>
        Ref<PassType> GetPassAs(const std::string& name) const;


        // -- Entity Picking --------------------------------------------------------------------------------------------------
        // Requires EntityPickingPass to be present and enabled in the pipeline.
        int32_t ReadEntityIDAt(int32_t x, int32_t y) const;
        entt::entity GetEntityAt(int32_t x, int32_t y) const;


        // -- Selection / Highlighting ----------------------------------------------------------------------------------------
        void SetSelectedEntity(entt::entity entity);
        entt::entity GetSelectedEntity() const;
        bool IsEntitySelected(entt::entity entity) const;


        // -- Viewport --------------------------------------------------------------------------------------------------------
        void OnViewportResize(uint32_t width, uint32_t height);
        glm::uvec2 GetViewportSize() const;


        // -- Output Framebuffer ----------------------------------------------------------------------------------------------
        const Ref<Framebuffer>& GetOutputFramebuffer() const;
        void SetOutputFramebuffer(const Ref<Framebuffer>& framebuffer);


        // -- Shared Resources ------------------------------------------------------------------------------------------------
        const Ref<SceneRenderResources>& GetRenderResources() const;


        // -- Statistics ------------------------------------------------------------------------------------------------------
        const SceneRendererStats& GetStatistics() const;
        void ResetStatistics();

        // -- Factories -------------------------------------------------------------------------------------------------------
        static Ref<SceneRenderer> Create();
        static Ref<SceneRenderer> Create(Ref<SceneRenderPipeline> pipeline);


        // Convenience: construct renderer + standard pipeline in one call.
        static Ref<SceneRenderer> CreateEditor(const Ref<Framebuffer>& outputFB);

        static Ref<SceneRenderer> CreateRuntime(
            const Ref<Framebuffer>& outputFB,
            SceneRenderMode mode = SceneRenderMode::Forward
        );

    private:
        void UploadCameraData();
        void UploadLightData();
        void BuildShadowCameras();
        void ResizeLightBuffers();
        void ExecutePipeline();
        void UpdateOutlinePass();
        bool FrustumCull(const DrawCommand3D& cmd) const;

    // -- private member varibles ---------------------------------------------------------------------------------------------
        Ref<SceneRenderPipeline>    m_Pipeline;
        Ref<SceneRenderResources>   m_Resources;

        SceneRenderContext          m_CurrentContext;
        DrawCommandQueue            m_CommandQueue;
        SceneLightEnvironment       m_LightEnvironment;

        entt::entity                m_SelectedEntity = entt::null;

        Ref<Framebuffer>            m_OutputFramebuffer;
        glm::uvec2                  m_ViewportSize = { 1280u, 720u };

        SceneRendererStats          m_Statistics;
        bool                        m_SceneActive = false;

	};

}