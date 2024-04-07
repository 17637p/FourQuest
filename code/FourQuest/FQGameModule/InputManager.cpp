#include "GameModulePCH.h"
#include "InputManager.h"

fq::game_module::InputManager::InputManager()
	:mHWND{}
	,mDeltaMousePosition{}
	,mCurrentMousePosition{}
	,mPrevMousePosition{}
{}

fq::game_module::InputManager::~InputManager()
{

}

void fq::game_module::InputManager::Update()
{


}

void fq::game_module::InputManager::Initialize(HWND hwnd)
{
	mHWND = hwnd;

}

void fq::game_module::InputManager::updateMouse()
{
	mPrevMousePosition = mCurrentMousePosition;

	GetCursorPos(&mCurrentMousePosition);
	ScreenToClient(mHWND, &mCurrentMousePosition);

	mDeltaMousePosition.x = mCurrentMousePosition.x - mPrevMousePosition.x;
	mDeltaMousePosition.y = mCurrentMousePosition.y - mPrevMousePosition.y;
}
