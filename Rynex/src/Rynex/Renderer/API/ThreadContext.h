#pragma once
#include "Rynex/Asset/Base/Asset.h"

#include "Rynex/Renderer/RendererAPI.h"

namespace Rynex {
	class Window;

	class ThreadContext
	{
	public:
		virtual void Init() = 0;
		virtual void Destroy() = 0;
		virtual const RendererAPI::API GetRendererAPI() const = 0;

		virtual void Create(Window* parent) = 0;
	};

}