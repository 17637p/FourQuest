#include "FQGraphics.h"

#include "Renderer.h"

using namespace fq::graphics;

FQGraphics::~FQGraphics()
{

}

FQGraphics::FQGraphics()
	:mRenderer(nullptr)
{
	mRenderer = std::make_shared<Renderer>();
}

bool fq::graphics::FQGraphics::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	mRenderer->Initialize(hWnd, width, height);

	return true;
}

bool fq::graphics::FQGraphics::Update(float deltaTime)
{
	return true;
}

bool FQGraphics::BeginRender()
{
	mRenderer->BeginRender();

	return true;
}

bool FQGraphics::Render()
{
	mRenderer->Render();

	return true;
}

bool FQGraphics::EndRender()
{
	mRenderer->EndRender();

	return true;
}

bool FQGraphics::Finalize()
{
	return true;
}


bool FQGraphics::SetViewportSize(const unsigned short width, const unsigned short height)
{
	return true;
}

bool FQGraphics::SetWindowSize(const unsigned short width, const unsigned short height)
{
	return true;
}

