#pragma once
#include "Rynex/Core/UUID.h"


namespace Rynex {
#if 1
	using RenderResouce = UUID;

	struct DrawObject
	{
		RenderResouce Pipline;
		std::vector<RenderResouce> VertexBuffers;
		std::vector<RenderResouce> BindGroup;
		RenderResouce IndexBuffer;
		uint32_t DrawVerticiesCount = 0;
	};

	class Pipline
	{
	public:
		static void Init();
		static void Shutdown();

		static RenderResouce CreateBuffer();
		static RenderResouce CreateVertex();
		static RenderResouce CreateIndex();
		static RenderResouce CreateTexture();
		static RenderResouce CreateFrambuffer();
		static RenderResouce CreateShader();

		static RenderResouce CreateRenderPiline();
		static RenderResouce CreateComputePiline();

		static RenderResouce CreateComander();

		static void BeginRender(RenderResouce commander);
		static void EndeRender();

		static void DrawPipline(const DrawObject& drawObject);
	};
#endif
}

