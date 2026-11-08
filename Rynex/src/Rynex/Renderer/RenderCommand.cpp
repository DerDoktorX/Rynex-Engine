#include <rypch.h>
#include "RenderCommand.h"

#include <Platform/OpenGL/OpenGLRendererAPI.h>

namespace Rynex {


    void RenderCommand::Create()
    {
        const RendererAPI::API api = RendererAPI::GetAPI();
        switch (api)
        {
        case RendererAPI::API::OpenGL:
            s_RendererAPI = CreateRef<OpenGLRendererAPI>();
            s_RendererAPI->Init();
            break;
        case RendererAPI::API::None:
        default:
            RY_CORE_ASSERT(false, "None/Unknown RendererAPI API!");
            s_RendererAPI = nullptr;
            break;
        }
    }

    void RenderCommand::Destroy()
    {
        if (nullptr == s_RendererAPI)
            return;


        RY_DESTROY_REF(s_RendererAPI);
    }
}
