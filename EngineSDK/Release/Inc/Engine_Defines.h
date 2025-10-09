#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXCollision.h>


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

#pragma once

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Jolt.h>
#include <Core/JobSystemThreadPool.h>
#include <Physics/PhysicsSystem.h>
#include <Physics/Body/BodyInterface.h>
#include <RegisterTypes.h>
#include <Core/TempAllocator.h>

using namespace JPH;

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

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
using namespace std;


namespace Engine
{
	static const unsigned int g_iMaxNumBones = 512;

	//const unsigned int g_iMaxWidth = 16384;
	//const unsigned int g_iMaxHeight = 9216;	

	const unsigned int g_iMaxWidth = 8192;
	const unsigned int g_iMaxHeight = 4608;
}

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"



#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//struct LeakDumpAtEnd {
//	~LeakDumpAtEnd() { _CrtDumpMemoryLeaks(); }
//} g_LeakDumpAtEnd;
//#endif
//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//struct LeakWatch {
//    LeakWatch() {
//        // 자동 누수 덤프
//        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//        // 필요하면 보고 형식/대상 설정
//        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
//        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
//        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
//    }
//} gLeakWatch;
//#endif


using namespace Engine;


