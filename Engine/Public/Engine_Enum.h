#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class MODELTYPE { NONANIM, ANIM };
	enum class COLLIDER { AABB, OBB, SPHERE, END };
	enum class SHAPE { CAPSULE, SPHERE, BOX, MESH, CONVEX, END };
	enum class JOLT_BP_LAYER { MOVING, NON_MOVING, TRIGGER, END};
	enum class NAV_POINT { A, B, C, END };
	enum class NAV_LINE { AB, BC, CA, END };
	enum class D3DTS { VIEW, PROJ, END };
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class RENDERGROUP { PRIORITY, SHADOW, NONBLEND, NONLIGHT, BLEND, UI, END };
	enum class WINMODE { FULL, WIN, END };	
	enum class MOUSEKEYSTATE { LB, RB, WB, END };
	enum class MOUSEMOVESTATE {	X, Y, WHEEL, END };
	enum class POOLSTATUS { WAIT, RESET, END};
	enum class COMPUTEJOB { UPDATE, POSTPROCESS, END };
	enum class ANIM_DIRECTION { 
		F, B, L, R, U, D, 
		LF, LB, RF, RB, 
		F_RF, F_LF, B_RB, B_LB,
		U_F, U_B, U_L, U_R, 
		D_F, D_B, D_L, D_R,
		BBL, BLL,
		END,
	};
}

namespace JoltLayers
{
	// 너의 용도에 맞게 늘려 써도 됨
	static constexpr ObjectLayer MAP = 0;
	static constexpr ObjectLayer DYNAMIC = 1;
	static constexpr ObjectLayer CHARACTER = 2;
	static constexpr ObjectLayer TRIGGER = 3;
	static constexpr uint32      NUM_LAYERS = 4;
}

// 브로드페이즈 레이어 (간단 매핑: 0=정적, 1=동적)
namespace JoltBPLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint32          NUM_LAYERS = 2;
}

#endif // Engine_Enum_h__
