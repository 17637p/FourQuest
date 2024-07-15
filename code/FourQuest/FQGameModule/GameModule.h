#pragma once

#ifndef FQ_GAME_MODULE_H
#define FQ_GAME_MODULE_H

#include "GameModuleEnum.h"
#include "GameModuleRegister.h"

#include "ThreadPool.h"
#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "PrefabResource.h"
#include "InstantiatePrefab.h"

#include "Transform.h"
#include "Camera.h"
#include "StaticMeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"
#include "Light.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"
#include "TerrainCollider.h"
#include "RigidBody.h"
#include "CharacterController.h"
#include "SoundClip.h"
#include "Particle.h"
#include "Decal.h"
#include "Trail.h"
#include "ImageUI.h"
#include "Socket.h"
#include "NavigationAgent.h"

#include "EventHandler.h"
#include "EventManager.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "SceneManager.h"
#include "PrefabManager.h"
#include "SoundManager.h"
#include "ScreenManager.h"

#include "ICollider.h"
#include "Collision.h"
#include "CollisionMatrix.h"
#include "Articulation.h"
#include "LinkData.h"
#include "ArticulationData.h"
#include "SkyBox.h"
#include "PostProcessing.h"

// Animation
#include "AnimatorControllerLoader.h"
#include "Animator.h"
#include "AnimatorController.h"
#include "AnimationStateNode.h"
#include "AnimationTransition.h"
#include "LogStateBehaviour.h"
#endif 