#ifndef MapEditor_Header_h__
#define MapEditor_Header_h__

// Map Editor에 필요한 헤더 모아놓는 용

// Camera
#include "Camera_Map.h"

// Sky
#include "SkySphere.h"
#include "CloudSphere.h"

// Properties
#include "Prop_Object.h"
#include "Prop_Static.h"
#include "Prop_Animated.h"
#include "Prop_Dynamic.h"
#include "Prop_Preview.h"

// Interactive Properties
#include "Prop_Interactive.h"

#include "BladeNexus.h"					// 귀검
#include "BigChest.h"					// 상자
#include "TombStone.h"					// 경계의 틈 툼스톤

#include "ElevatorS.h"                  // 엘리베이터
#include "Elevator_Gear.h"              // 엘리베이터 옆 기어
#include "Slate_Switch.h"               // 엘리베이터 발판 스위치

#include "Lever.h"                      // 레버 옆 기어 돌리는 레버
#include "Lever_Gear.h"                 // 레버 옆 기어, 다 돌면 문쪽 기어 돌림
#include "Door_Gear.h"                  // 레버 옆 기어, 다 돌면 문쪽 기어 돌림

// Properties Test
#include "Prop_Test.h"

// 플레이어 모델 테스트
#include "Map_TestPlayer.h"

// 맵 트리거 ( 좀 야매 )
#include "Trigger.h"

// 몬스터 스폰 ( 많이 야매 )
#include "Map_Spawn.h"

// 점 조명 라이트 메쉬
#include "Map_Light.h"

// Terrain
#include "Map_Terrain.h"

// Model 관련
#include "Editor_Model.h"
#include "Editor_Model_Instance.h"
#include "Editor_ModelMesh_Instance.h"

#endif // MapEditor_Header_h__
