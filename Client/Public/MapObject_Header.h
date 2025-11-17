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

#include "ElevatorS.h"
#include "Elevator_Gear.h"
#include "Slate_Switch.h"

// Trigger
#include "Trigger.h"
#include "HeinMach_Trigger.h"

// Test
#include "Prop_Test.h"

// 맵의 서브레벨 개수 및 귀검 있는 SUB LV
#define HEINMACH_SUBLV 12
#define HEINMACH_1ST_BLADENEXUS 0			// 첫번째 귀검
#define HEINMACH_2ND_BLADENEXUS 5			// 두번째 귀검
#define HEINMACH_3RD_BLADENEXUS 10			// 세번째 귀검
#define HEINMACH_YETUGA			11			// 예투가 맵

#define CREVICE_SUBLV 2						// 경계의 틈 

#define EMBARS_SUBLV 2

#define VIPER_SUBLV 1

#endif // MapObject_Header_h__
