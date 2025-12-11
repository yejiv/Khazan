#pragma once

#pragma warning(disable : 4251)
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>
#include <SimpleMath.h>


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "d3dx11effect.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

#include "SpriteBatch.h"
#include "SpriteFont.h"

#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "Effects.h"
#include "ScreenGrab.h"

#include "scene.h"
#include "postprocess.h"
#include "Importer.hpp"

using namespace DirectX;

#include <Jolt.h>
#include <Core/JobSystemThreadPool.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/Body/BodyInterface.h>
#include <Physics/Character/CharacterVirtual.h>
#include <RegisterTypes.h>
#include <Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Core/Factory.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics//Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/GroupFilterTable.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Geometry/AABox.h>
using namespace JPH;

#include <ft2build.h>
#include FT_FREETYPE_H

#include "fmod.h"
#include "fmod.hpp"

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <random>
#include <io.h>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include <any>
#include <deque>
#include <filesystem>
#include <strsafe.h>
#include <iostream>
#include <typeindex>
#include <typeinfo>

using namespace std;
using namespace std::chrono_literals;
namespace Engine
{
	static const unsigned int g_iMaxNumBones = 512;

	//const unsigned int g_iMaxWidth = 16384;
	//const unsigned int g_iMaxHeight = 9216;	

	const unsigned int g_iMaxWidth = 8192;
	const unsigned int g_iMaxHeight = 4608;

	const float g_fGravity = -9.81f;

}

#define PI 3.14159f

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Json.h"
#include "Engine_ConstantBuffer.h"
#include "Engine_Sequence.h"
#include "Imgui_Header.h"
#include "Debug_Header.h"

using namespace Engine;


