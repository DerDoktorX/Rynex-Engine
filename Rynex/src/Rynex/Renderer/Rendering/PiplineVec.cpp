#include "rypch.h"
#include "PiplineVec.h"

#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

namespace Rynex{
	PiplineRefBaseVec::PiplineRefBaseVec()
		: m_PiplineIndex(0u)
		, m_PiplineVec()
		, m_OutPutLayout()
	{
	}


	void PiplineRefBaseVec::ResetFramePipline()
	{
		uint32_t count = m_PiplineVec.size();
		RY_CORE_ASSERT(m_PiplineIndex <= count, "PilineIndex shoud be not be greater then the stack at max only equel!");

		for (uint32_t i = 0; i < m_PiplineIndex; i++)
		{
			Ref<PiplineRenderBase>& piplineBase = m_PiplineVec.at(i);
			piplineBase->Clear();
		}
		m_PiplineIndex = 0u;
	}

	void PiplineRefBaseVec::ResetExpextedIndex()
	{
		uint32_t count = m_PiplineVec.size();
		RY_CORE_ASSERT(m_PiplineIndex <= count, "PilineIndex shoud be not be greater then the stack at max only equel but equal is also not expeted!");

		m_PiplineIndex = 0u;
	}

	void PiplineRefBaseVec::SetVaildOutPut(const BufferLayout& layout)
	{
		m_OutPutLayout = layout;
	}

	const BufferLayout& PiplineRefBaseVec::GetVaildOutPut() const
	{
		return m_OutPutLayout;
	}

}