#include "rypch.h"
#include "Layer.h"

namespace Rynex {

	Layer::Layer(const std::string& debugeName)
		: m_DebugeName(debugeName)
	{
	}
	
	Layer::~Layer()
	{
		m_DebugeName.empty();
		m_DebugeName.shrink_to_fit();
	}

}