#include "rypch.h"
#include <Rynex/Core/Instrumentor.h>
#include <Platform/GLFW/WinowsInstromenter.h>

namespace Rynex {

    Ref<PlatformTimer> PlatformTimer::Create(int64_t* timeElaps)
    {
        return CreateRef<WinowsInstromenterTimer>(timeElaps);
    }
}

