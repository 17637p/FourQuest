#include "FQGraphics.h"

fq::graphics::IFQGraphics* fq::graphics::EngineExporter::GetEngine()
{
	return new fq::graphics::FQGraphics;
}

void fq::graphics::EngineExporter::DeleteEngine(IFQGraphics* engine)
{
	delete engine;
}

