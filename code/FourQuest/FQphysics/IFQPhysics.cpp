#include "FQPhysics.h"

fq::physics::IFQPhysics* fq::physics::EngineExporter::GetEngine()
{
    return new fq::physics::FQPhysics;
}

void fq::physics::EngineExporter::DeleteEngine(IFQPhysics* engine)
{
    delete engine;
}
