#include "rypch.h"
#include "Layer.h"

namespace Rynex {

	Layer::Layer(const std::string& debugeName)
		: m_DebugeName(debugeName)
	{
		RY_PROFILE_FUNCTION();
	}
	
	Layer::~Layer()
	{
		RY_PROFILE_FUNCTION();
	}

}