#include "rypch.h"
#include <Rynex/Core/Instrumentor.h>
#include <Platform/Windows/WinowsInstromenter.h>

namespace Rynex {

    Ref<PlatformTimer> PlatformTimer::Create(int64_t* timeElaps)
    {
        return CreateRef<WinowsInstromenterTimer>(timeElaps);
    }
}

