#include "IFQPhysics.h"

fq::physics::IFQPhysics* fq::physics::EngineExporter::GetEngine()
{
    return new fq::physics::IFQPhysics;
}

void fq::physics::EngineExporter::DeleteEngine(IFQPhysics* engine)
{
    delete engine;
}
