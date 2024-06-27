#include "rypch.h"
#include "RenderCommmand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Rynex {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}