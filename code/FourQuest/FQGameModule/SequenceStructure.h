#pragma once

#include <set>
#include <vector>
#include <string>

#include "../FQCommon/IFQRenderResource.h"
#include "InputEnum.h"

namespace fq::game_module
{
	struct TrackKey
	{
		float time = 0.f;
		DirectX::SimpleMath::Vector3 position = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 rotation = { 0.f, 0.f, 0.f };
		DirectX::SimpleMath::Vector3 scale = { 1.f, 1.f, 1.f };
	};

	struct AnimationTrackKey
	{
		float time = 0.f;
		float speed = 1.f;
		std::string animationPath = {};
	};

	struct CameraChangeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetCameraName = {};
		std::string prevCameraName = {};
		std::vector<TrackKey> keys;
	};

	struct ObjectMoveTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<TrackKey> keys;
	};

	struct ObjectTeleportTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<TrackKey> keys;
	};

	struct PlayerMoveTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		unsigned int playerID = 0;
		std::vector<TrackKey> keys;
	};

	struct PlayerTeleportTrackInfo
	{
		bool isObjectReturnToStartTransform = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		unsigned int playerID = 0;
		std::vector<TrackKey> keys;
	};

	struct ObjectAnimationInfo
	{
		bool isLoop = true;
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string targetObjectName = {};
		std::vector<AnimationTrackKey> animationTrackKeys = {};
	};

	struct EffectTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string effectPath = {};
		std::vector<TrackKey> keys;
	};

	struct SoundTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string keyName = {};
		std::string soundObjectName = {};
		bool bIsLoop = false;
	};

	struct TextPrintTrackInfo
	{
		std::string fontPath = {};// "던파 연단된 칼날";
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		float nameFontCenterY = 200.f;
		float nameFontSize = 30.f;
		DirectX::SimpleMath::Color nameFontColor = { 0.f, 0.f, 1.f, 1.f };
		std::string name = {};

		float textFontCenterY = 150.f;
		float textFontSize = 20.f;
		DirectX::SimpleMath::Color textFontColor = { 1.f, 1.f, 1.f, 1.f };
		std::string text = {};
	};

	struct CameraShakeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string cameraObjectName = {};
		DirectX::SimpleMath::Vector3 magnitude = {};
		int shakeCount = 100;
		int originInitSpacing = 5;
	};

	struct VibrationTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		EVibrationMode mode = EVibrationMode::Left;
		float Intensity = 0.f;
	};

	struct FadeTrackInfo
	{
		float startTime = 0.f;
		float totalPlayTime = 1.f;

		std::string postProcessObjectName = {};
		float blendTime = 1.f;
		DirectX::SimpleMath::Color finalColor = {0.f, 0.f, 0.f, 1.f};
	};
}