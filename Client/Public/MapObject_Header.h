#ifndef MapObject_Header_h__
#define MapObject_Header_h__

// Map 관련 객체 헤더 모아놓는 용도

// SkySphere
#include "SkySphere.h"
#include "CloudSphere.h"

// 로딩용 귀검
#include "LoadingObj_BN.h"

// Properties
#include "Prop_Object.h"
#include "Prop_Static.h"
#include "Prop_Animated.h"
#include "Prop_Dynamic.h"

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

// Trigger
#include "Trigger.h"
#include "HeinMach_Trigger.h"

// Destructible
#include "Prop_Destructible.h"
#include "Prop_Chunk.h"
#include "Fence.h"
#include "Pot.h"
#include "Barrel.h"
#include "Obelisk.h"

// Test
#include "Prop_Test.h"


// 맵의 서브레벨 개수 및 귀검 있는 SUB LV
#define HEINMACH_SUBLV 12
#define HEINMACH_1ST_BLADENEXUS 0			// 첫번째 귀검
#define HEINMACH_2ND_BLADENEXUS 5			// 두번째 귀검
#define HEINMACH_3RD_BLADENEXUS 10			// 세번째 귀검
#define HEINMACH_YETUGA			11			// 예투가 맵

#define CREVICE_SUBLV 2						// 경계의 틈 

#define EMBARS_SUBLV 7                      // 엠바스 서브 레벨

#define VIPER_SUBLV 1

#endif // MapObject_Header_h__
