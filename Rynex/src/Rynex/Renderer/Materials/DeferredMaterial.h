#pragma once
#include "Rynex/Renderer/Materials/Material.h"


namespace Rynex {

    class DeferredMaterial : public Material
    {
    public:
        DeferredMaterial();
        virtual ~DeferredMaterial();

        virtual AssetType GetType() const override { return AssetType::DeferredMaterial; }
    };


}