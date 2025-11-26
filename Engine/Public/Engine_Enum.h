#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
    enum class TARGET_DIR { F, B, L, R, FR, FL, BR, BL, END };
	enum class MODELTYPE { NONANIM, ANIM };
	enum class COLLIDER { AABB, OBB, SPHERE, END };
	enum class SHAPE { CAPSULE, SPHERE, BOX, MESH, CONVEX, END };
	enum class JOLT_BP_LAYER { MOVING, NON_MOVING, TRIGGER, END};
	enum class JOLT_COLLSION_TYPE { BODY, CHAR, CHARVIR, END };
	enum class NAV_POINT { A, B, C, END };
	enum class NAV_LINE { AB, BC, CA, END };
	enum class D3DTS { VIEW, PROJ, END };
	enum class STATE { RIGHT, UP, LOOK, POSITION };
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class RENDERGROUP { PRIORITY, SHADOW, STATIC, DYNAMIC, MOTIONVECTOR, OUTLINE, NONLIGHT, MOTIONTRAIL, WEIGHT_BLEND, BLEND, UI, END };
	enum class WINMODE { FULL, WIN, END };
	enum class MOUSEKEYSTATE { LB, RB, WB, END };
	enum class MOUSEMOVESTATE {	X, Y, WHEEL, END };
	enum class POOLSTATUS { WAIT, RESET, END};
	enum class COMPUTEJOB { UPDATE, POSTPROCESS, END };
    enum class INPUT_TYPE { GAMEPLAY, UI, POPUP, WORLD_UI, END };
	enum class TEXT_ALIGN { LEFT_TOP, CENTER_TOP, RIGHT_TOP, LEFT_CENTER, CENTER, RIGHT_CENTER, LEFT_BOTTOM, CENTER_BOTTOM, RIGHT_BOTTOM, END };
	enum class TIME_CHANNEL : unsigned int { WORLD, PLAYER, ENEMY, EFFECT, MAP, END };
	enum class DEFERRED_CONTEXT { MAP, END }; 
	enum class DECALTYPE { LINEAR, CIRCLE, CURVE, END };

	//enum class ANIM_DIRECTION { 
	//	NONE = 0,
	//	F, B, L, R, U, D,
	//	FL, FR, BL, BR,
	//	UL, UR, DL, DR,
	//	U_F, U_B, U_L, U_R,
	//	D_F, D_B, D_L, D_R,
	//	BBL, BLL,
	//	ALL, CC, CD, CU, LC, LD, LU, RC, RD, RU,
	//	END,
	//};
	enum class DIRECTION : uint32_t {
		NONE = 0,
		F = 1 << 0,
		B = 1 << 1,
		L = 1 << 2,
		R = 1 << 3,
		U = 1 << 4,
		D = 1 << 5,
		C = 1 << 6,
		CC = 1 << 7,
		ALL = 1 << 8, //안씀

		BBL = 1 << 9, //안씀
		BLL	= 1 << 10,//안씀

		R180 = 1 << 11,
		L180 = 1 << 12,

		END,
	};

	enum class ANIM_ROTATIONTYPE {
		NONE = 0 ,
		ROT_L45 = 1 << 0,
		ROT_L90 = 1 << 1,
		ROT_L135 = 1 << 2,
		ROT_L180 = 1 << 3,
		ROT_L225 = 1 << 4,
	
		ROT_R45	= 1 << 5,
		ROT_R90 = 1 << 6,
		ROT_R135 = 1 << 7,
		ROT_R180 = 1 << 8,
		ROT_R225 = 1 << 9,
	};


	enum class ANIM_TRANSITIONTYPE { AUTO, FLAG, INPUT, MANUAL, END };	//애니메이션 전환 조건
	enum class ANIM_EVENT_TRIGGERTYPE { ONCE , ENTER, EXIT, CONTINUE }; //애니메이션 이벤트 트리거 종류 
	enum class MAPOBJECT_TYPE : unsigned short { OBJECT, STATIC_INST, ANIMATED_INST, INTERACTIVE, DYNAMIC, END };
	enum class BTNODESTATE { RUNNING, SUCCESS, FAILURE, END };

	enum class INTERACTIVE_TYPE {
        CHECKPOINT, CHEST, TOMBSTONE, TRIGGER, SPAWN, LADDER, ELEVATOR, LEVER, GEAR1, GEAR2,
        STATUE, VERTICALGATE, IRONGATE, UNLOCKGEAR, LARGEELEVATOR, GIANTGATE, DAPHRONA, DUIMUK, DANJIN,
        END };

    enum MATERIAL_FLAG_BIT : unsigned int
    {
        M_DIFFUSE            = 1 << 0,
        M_NORMAL             = 1 << 1,
        M_EMISSIVE           = 1 << 2,
        M_SPECULAR           = 1 << 3,
        M_METALIC            = 1 << 4,
        M_ROUGHNESS          = 1 << 5
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
