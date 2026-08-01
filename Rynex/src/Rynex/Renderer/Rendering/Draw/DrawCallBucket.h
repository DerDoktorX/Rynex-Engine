#pragma once
#include <rypch.h>
#include <Rynex/Renderer/Rendering/Draw/DrawCallKey.h>
#include <Rynex/Renderer/Rendering/Draw/ProtypeDrawCalls.h>
#include <Rynex/Renderer/RenderProxy/RenderProxyPrototype.h>

namespace Rynex {
	struct DrawCallBucket
	{
		DrawCallKey key;
		std::vector<RenderProxy*> proxyVec;
		
		Ref<IndirectBuffer> indirectBuffer;
		std::vector<Ref<StorageBuffer>> storageBufferVec;
		std::vector<Ref<UniformBuffer>> uniformBufferVec;
		std::vector<Ref<VertexBuffer>> vertexBufferVec;
		std::vector<Ref<IndexBuffer>> indexBufferVec;
		uint32_t proxyIndex;

		void operator+=(RenderProxy* renderProxy)
		{
			Add(renderProxy);
		}

		void Add(RenderProxy* renderProxy)
		{
			CheckPorxyVecIndex();
			proxyVec.at(proxyIndex) = renderProxy;
			proxyIndex++;
		}

		void CheckPorxyVecIndex()
		{
			uint32_t count = static_cast<uint32_t>(proxyVec.size());
			if (count <= proxyIndex)
			{
				uint32_t resizeStep = (proxyIndex / 3);
				resizeStep += 1;
				uint32_t resizeCount = proxyIndex + resizeStep;
				proxyVec.resize(resizeCount);
			}
		}
	};
}