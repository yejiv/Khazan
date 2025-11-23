#pragma once
#include "Client_Defines.h"
#include "Base.h"

//#include "Khazan_GS_InputHandler.h"

NS_BEGIN(Client)

class CKhazan_GS_StateMachine final : public CBase
{
public:
    // ===== 메인 상태 =====
    //GSSM =GREAT SWORD STATE MACHINE
    enum  GSSM_MAIN: _uint
    {
        /* 1 순위 */
        GS_DIE                = 1 << 0,

        /* 강제 애니메이션  */
        GS_FALL               = 1 << 1,
        GS_DAMAGED            = 1 << 2,
        GS_CLIMB              = 1 << 3,
        GS_HOLD               = 1 << 4,
        GS_DOWN               = 1 << 5, //보스한테 잡히고 던져졌을 때 눕는 애니메이션 재생
        GS_UP                 = 1 << 6, //눕고 일정시간 지나고 키입력들어왔을 때 일어나기 +일정시간지나면 저절로 일어나기 ?
        GS_JUSTGUARD          = 1 << 7,
        GS_GROGGY             = 1 << 8,

        /* 우선순위 둬야하는 애니메이션 */
        GS_SKILL              = 1 << 9,
        GS_ATTACK             = 1 << 10,
        GS_MOVE               = 1 << 11,
        GS_GUARD              = 1 << 12,

        /* 별개 */
        GS_LOCKON             = 1 << 13,
        GS_INTERACTION        = 1 << 14,
        GS_IDLE               = 1 << 15,
    };

    // ===== 이동 서브 상태 =====
    enum  GSSM_MOVE : _uint
    {
        //GS_NONE            = 1 << 0,
        GS_WALK            = 1 << 0,
        GS_RUN             = 1 << 1,
        GS_SPRINT          = 1 << 2,
        GS_DODGE           = 1 << 3,
        GS_TURN_180        = 1 << 4,
    };

    // ===== 공격 서브 상태 =====
    enum  GSSM_ATTACK : _uint
    {
       // GS_NONE,
        GS_WEAK_COMBO,        // 빠른 콤보
        GS_STRONG_COMBO,        // 강한 콤보
        GS_CHARGING,           // 차징 중
        GS_CHARGED_ATTACK,     // 차징 공격
        GS_DODGE_ATTACK,       // 회피 공격
        GS_SPRINT_ATTACK,      // 스프린트 공격
    };

    // ===== 가드 서브 상태 =====
    enum  GSSM_GUARD : _uint
    {
        //GS_NONE,
        GS_IDLE_GUARD,         // 정지 가드
        GS_MOVE_GUARD,         // 이동 가드
        GS_PARRY_READY,        // 패리 준비
        GS_PARRY_SUCCESS,      // 패리 성공
        GS_GUARD_BREAK,        // 가드 브레이크
    };


    enum  GSSM_WEAPON : _uint
    {
        GS_BAREHAND = 1 << 0,
        GS_SPEAR  = 1 << 1,
        GS_GSWORD = 1 << 2,
    };

    enum GSSM_CYCLE : _uint
    {
         GS_CYCLE_START = 1 << 0,
         GS_CYCLE_LOOP = 1 << 1,
         GS_CYCLE_END = 1 << 2,
         GS_CYCLE_ENDSTART = 1 << 3,   //사다리 끝에서
         GS_CYCLE_ENDEND = 1 << 4,   //사다리 끝에서
         //GS_CYCLE_STARTSTART    = 1 << 5,   //사다리 에서
         //GS_CYCLE_STARTEND      = 1 << 6,   //사다리 에서
         GS_CYCLE_BREAK = 1 << 5,
         GS_CYCLE_SHOT = 1 << 6,
         GS_CYCLE_FAIL = 1 << 7,
    };

public:
    typedef struct tagGSwordKhazanCommandDesc
    {
        _uint               iMainType = 0;
        _uint               iSubType = 0;
        DIRECTION_INFO      iDirection ;          // DIRECTION_INFO
        _float              fChargeTime = 0.f;
        _uint               iSkillID = 0;
        _bool               isIsPressed = false;     // 눌림/뗌
        _float              fTimestamp = 0.f;        // 입력 시간

        tagGSwordKhazanCommandDesc() = default;
        tagGSwordKhazanCommandDesc(_uint mainType, _uint SubType = 0) : iMainType(mainType), iSubType(SubType) {}
    }GS_COMMAND;

private:
    CKhazan_GS_StateMachine();
    virtual ~CKhazan_GS_StateMachine() = default;

public:
    HRESULT     Initialize();
    void        Update(_float fTimeDelta);

    /* 상태 전환 */
    GS_COMMAND  Get_SelectedCommand() const { return m_eCurCommand; }

    _bool       Request_StateChange(const vector<GS_COMMAND>& commands);
    void        Force_StateChange(_uint eState, _uint iSubState = 0);

    /* 상태 쿼리 */
    //_uint       Get_MainState() const { return m_iCurMainState; }
    //_uint       Get_PreviousMainState() const { return m_iPrevMainState; }
    //_uint       Get_SubState() const { return m_iCurSubState; }
    //_uint       Get_PreviousSubState() const { return m_iPrevSubState; }

    //_bool       Is_State(_uint eState) const { return m_iCurMainState == eState; }
    //_bool       Is_SubState(_uint iSubState) const { return (m_iCurSubState & iSubState) != 0; }
    //_bool       Can_Interrupt() const;
    //_bool       Is_StateChanged() const { return m_isStateChanged; }

    /* 상태 초기화 */  
    //void        Clear_States();
    //void        Clear_SubState() { m_iCurMainState = 0; }

private:
    /* 전환 로직 */ 
    _uint       Get_StatePriority(_uint iState) const;


private:
    GS_COMMAND  m_eCurCommand;
    _bool       m_isStateChanged = false;

    //_uint       m_iCurMainState = GS_IDLE;
    //_uint       m_iPrevMainState = GS_IDLE;   이전상태는 애니메이션 컨트롤러에서 저장해서
    //_uint       m_iCurSubState = 0;
    //_uint       m_iPrevSubState = 0;
    //DIRECTION_INFO   m_eCurDir;
    //DIRECTION_INFO   m_ePrevDir;

    // ===== 상태별 우선순위 =====
    unordered_map<_uint, _uint> m_StatePriorityTable;

public:
    static CKhazan_GS_StateMachine* Create();
    virtual void Free() override;
};

NS_END