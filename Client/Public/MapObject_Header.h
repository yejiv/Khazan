#ifndef MapObject_Header_h__
#define MapObject_Header_h__

// Map Editor에 필요한 헤더 모아놓는 용

// Properties
#include "Prop_Object.h"
#include "Prop_Static.h"
#include "Prop_Animated.h"
#include "Prop_Dynamic.h"

// Interactive Properties
#include "Prop_Interactive.h"

#include "BladeNexus.h"					// 귀검
#include "BigChest.h"					// 상자

// Test
#include "Prop_Test.h"

// 맵의 서브레벨 개수 및 귀검 있는 SUB LV
#define HEINMACH_SUBLV 12
#define HEINMACH_1ST_BLADENEXUS 0			// 첫번째 귀검
#define HEINMACH_2ND_BLADENEXUS 5			// 두번째 귀검
#define HEINMACH_3RD_BLADENEXUS 10			// 세번째 귀검
#define HEINMACH_YETUGA			11			// 예투가 맵

#define CREVICE_SUBLV 0
#define EMBARS_SUBLV 0
#define VIPER_SUBLV 0

#endif // MapObject_Header_h__
