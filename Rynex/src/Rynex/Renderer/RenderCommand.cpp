#include "rypch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Rynex {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}