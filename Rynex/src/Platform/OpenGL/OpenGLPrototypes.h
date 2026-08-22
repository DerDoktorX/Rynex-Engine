#pragma once

struct GLFWwindow;

namespace Rynex {
	
	class OpenGLContext;
	class OpenGLThreadContext;
	class OpenGLRendererAPI;

	class OpenGLFence;

	class OpenGLShader;
	class OpenGLVertexArray;
	class OpenGLFramebuffer;

	// OpenGL Buffers 
	class OpenGLArrayBuffer;
	class OpenGLShaderStorageBuffer;

	class OpenGLVertexBuffer;
	class OpenGLIndexBuffer;
	class OpenGLStorageBuffer;
	class OpenGLUniformBuffer;
	class OpenGLIndriectBuffer;

	// OpenGL Textures 
	class OpenGLTextureObject;
	class OpenGLTextureSampler;

	class OpenGLTextureStorageModern;
	class OpenGLLinkedTextureArray;
	class OpenGLBindlesTextureArray;
	
}