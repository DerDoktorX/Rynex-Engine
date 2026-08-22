#include "rypch.h"
#include "DrawCallsList.h"
namespace Rynex {
	DrawCallsList::DrawCallsList()
	{
		Set(0);
	}

	DrawCallsList::~DrawCallsList()
	{
	}

	void DrawCallsList::Set(uint32_t index)
	{
		RY_CORE_ASSERT(index < m_DrawCallsVecArray.size());
		m_DrawCallsVecPtr = &m_DrawCallsVecArray[index];
	}

	void DrawCallsList::Add(const ShaderDrawResource& drawlist)
	{
		m_DrawCallsVecPtr->emplace_back(drawlist);
	}

	bool DrawCallsList::Empty() const
	{
		return m_DrawCallsVecPtr->empty();
	}

	void DrawCallsList::Clear()
	{
		m_DrawCallsVecPtr->clear();
	}

	void DrawCallsList::SubmitToVec(std::vector<ShaderDrawResource>& vec)
	{
		vec.insert(vec.begin(), m_DrawCallsVecPtr->begin(), m_DrawCallsVecPtr->end());
	}
}