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

#include "Statue.h"                     // 조각상 퍼즐 ( 렌더 X ) ( 회전 애니메이션만 )
#include "Statue_Plate.h"               // 조각상 퍼즐 발판
#include "Statue_Deco.h"                // 조각상 퍼즐 석상

#include "VerticalGate.h"               // 수직 차단봉

#include "IronGate.h"                   // 철문 ( 렌더 X )
#include "IronGate_Lock.h"              // 철문 잠금장치
#include "IronGate_Part_L.h"            // 철문 왼쪽
#include "IronGate_Part_R.h"            // 철문 오른쪽

#include "Ladder.h"                     // 사다리 ( 렌더 X ) ( 뼈 없슴 )
#include "Ladder_Top.h"                 // 사다리 위쪽 ( 뼈 O )
#include "Ladder_Middle.h"              // 사다리 중간 다리들 ( 1개씩 )
#include "Ladder_Bottom.h"              // 사다리 아래쪽 ( 뼈 O )
#include "Ladder_Support.h"             // 사다리 아래쪽 발판

#include "GearGate.h"                   // 문쪽 기어 작동 완료하면 열리는 문
#include "Door_Gear.h"                  // 문쪽 기어, 레버랑 기어 완료되면 해당 기어 가동

#include "UnLockGear.h"                 // 엘리베이터 잠금 해제하는 기어

#include "ElevatorL.h"                  // 엘리베이터 큰거 ( 렌더 X )
#include "Elevator_Inner.h"             // 엘리베이터 큰거 ( 안쪽꺼 )
#include "Elevator_Mid.h"               // 엘리베이터 큰거 ( 중간꺼 )
#include "Elevator_Outer.h"             // 엘리베이터 큰거 ( 바깥꺼 )

#include "GiantGate.h"                   // 바이퍼 가는 문 ( 렌더 X )
#include "GiantGate_Part_L.h"            // 바이퍼 가는 문 왼쪽
#include "GiantGate_Part_R.h"            // 바이퍼 가는 문 오른쪽

// Interactive ( NPC )
#include "NPC_Daphrona.h"                // 다프로나 ( 사실 서나 ) ( 귀석 )
#include "NPC_Duimuk.h"                  // 두이무크 ( 상인 )
#include "Duimuk_Part.h"                 // 두이무크 파츠
#include "NPC_Danjin.h"                  // 단진 ( 랜턴 상인 )

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
