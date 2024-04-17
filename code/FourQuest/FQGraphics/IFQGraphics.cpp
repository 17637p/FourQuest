#include "FQGraphics.h"

fq_graphics::IFQGraphics* fq_graphics::EngineExporter::GetEngine()
{
	return new fq_graphics::FQGraphics;
}

void fq_graphics::EngineExporter::DeleteEngine(IFQGraphics* engine)
{
	delete engine;
}
