#pragma once
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

namespace Rynex{


	class RenderPiplineManger
	{
	public:
		RenderPiplineManger();
		~RenderPiplineManger();
		void Destroy();
		void Clear();
		void ResetPiplines();

		Ref<PiplineRenderBase> GetRenderPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel);
		Ref<PiplineRenderBase> CreateRenderPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel);
		Ref<PiplineRenderBase> DestroyRenderPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel);


	private:
		std::vector<Weak<PiplineRenderBase>> m_PiplineRender;
		bool m_Sorted;
	};


}


