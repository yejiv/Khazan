#pragma once

// 엔진 DLL 빌드 시(EngineDLL 프로젝트)에는 ENGINE_EXPORTS를 정의한다고 가정
#ifdef ENGINE_EXPORTS
#ifndef IMGUI_API
#define IMGUI_API __declspec(dllexport)
#endif
#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API __declspec(dllexport)
#endif
#else
#ifndef IMGUI_API
#define IMGUI_API __declspec(dllimport)
#endif
#ifndef IMGUI_IMPL_API
#define IMGUI_IMPL_API __declspec(dllimport)
#endif
#endif