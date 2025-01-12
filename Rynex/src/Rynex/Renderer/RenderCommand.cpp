#include "rypch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

#if 0
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#endif 
namespace Rynex {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}