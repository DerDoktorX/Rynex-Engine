#pragma once

#include <Rynex/Renderer/Rendering/Draw/ProtypeDrawCalls.h>

namespace Rynex {
	class DrawCallProcess
	{
	public:
		virtual ~DrawCallProcess() { }

		
		virtual void Build(const DrawCallBucket& drawCallBucket);
	protected:
		template<typename ShadersPass, typename BindData>
		void BuildDrawCommand(
			const DrawCallBucket& drawCallBucket,
			ShadersPass shadersPass,
			int renderMode,
			MaterielStragie materielStragie,
			const Ref<Material>& materiel,
			BindData bindData
		);
	private:
		std::vector<ShaderDrawResource> m_ShaderDrawResourceVec;
	};
}