#include "rypch.h"
#include "PiplineVec.h"

#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

namespace Rynex{
	PiplineRefBaseVec::PiplineRefBaseVec()
		: m_OutPutLayout()
		, m_PiplineIndex(0u)
		, m_PiplineVec()
	{
	}


	void PiplineRefBaseVec::ResetFramePipline()
	{
		const uint32_t count = m_PiplineVec.size();
		RY_CORE_ASSERT(m_PiplineIndex <= count, "PiplineIndex should be not be greater then the stack at max only equal!");

		for (uint32_t i = 0; i < m_PiplineIndex; i++)
		{
			const Ref<PiplineRenderBase>& piplineBase = m_PiplineVec.at(i);
			piplineBase->Clear();
		}
		m_PiplineIndex = 0u;
	}

	void PiplineRefBaseVec::ResetExpectedIndex()
	{
		const uint32_t count = m_PiplineVec.size();
		RY_CORE_ASSERT(m_PiplineIndex <= count, "PiplineIndex should be not be greater then the stack at max only equal but equal is also not expected!");

		m_PiplineIndex = 0u;
	}

	void PiplineRefBaseVec::SetValidOutPut(const BufferLayout& layout)
	{
		m_OutPutLayout = layout;
	}

	const BufferLayout& PiplineRefBaseVec::GetValidOutPut() const
	{
		return m_OutPutLayout;
	}

}