#pragma once
#include <Rynex/Renderer/Rendering/Draw/ProtypeDrawCalls.h>

namespace Rynex {

	class DrawCallBatchBuilder
	{
	public:
		DrawCallBatchBuilder();
		~DrawCallBatchBuilder();

		void Process(const std::vector<RenderProxy*>& renderProxyVec, std::vector<ShaderDrawResource>& outShaderDrawResourceVec);
	private:
		DrawCallKey Classify(const RenderProxy* renderProxy, std::vector<ShaderDrawResource>& outShaderDrawResourceVec) const;
		void Build(const DrawCallBucket& bucket, std::vector<ShaderDrawResource>& outShaderDrawResourceVec);
	};
}
