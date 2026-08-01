#pragma once
#include <Rynex/Renderer/Rendering/RenderTarget.h>

namespace Rynex {
	class DrawCallsList
	{
	public:
		DrawCallsList();
		~DrawCallsList();

		void Set(uint32_t index);

		void Add(const ShaderDrawResource& drawlist);
		bool Empty() const;
		void Clear();

		void SubmitToVec(std::vector<ShaderDrawResource>& vec);
	private:
		std::vector<ShaderDrawResource>* m_DrawCallsVecPtr;
		std::array<std::vector<ShaderDrawResource>, 1> m_DrawCallsVecArray;


	};
}
