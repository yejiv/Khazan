// AnimationStateManager.h
#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CKhazan_Spear_ASManager final : public CBase
{
public:

    typedef struct tagKhazanSpearAnimationStateInfo
    {
        kHAZAN_ANIM_FIND animFind;
        _bool   isInterruptible; //중단 가능 여부 <- 이벤트로 넘겨주기!
        _int    iAnimIndex;
        _float  fStateTime; // 상태 지속 시간
        _float  fMinDuration = 0.2f ; //최소 지속 시간...할지 고민 중  <- 툴에서 작업

    }SPEAR_ANIMSTATE;


    typedef struct tagKhazanSpearAnimationTransitionRule
    {
        _uint               iFromCategory;
        kHAZAN_ANIM_FIND    toAnimFind;
        function<_bool()>   checkCondition;
        _uint               iPriority;  //  특수 상황에서 이걸 사용하자

        tagKhazanSpearAnimationTransitionRule(_uint from, kHAZAN_ANIM_FIND to, function<_bool()> condition, _uint priority )
            : iFromCategory(from), toAnimFind(to), checkCondition(condition), iPriority(priority) {
        }

    }SPEAR_ANIMTRANSITION;


private:
    CKhazan_Spear_ASManager();
    virtual ~CKhazan_Spear_ASManager() = default;

public:
    HRESULT Initialize_Prototype(class CKhazan_Spear_ASMachine* pASM);

    // 전환 규칙 추가
    void Add_Transition(_uint iFrom, kHAZAN_ANIM_FIND toAnimFind, function<_bool()> condition, _uint priority = 99);

    // 강제 상태 변경 (우선순위 무시)
    void Force_ChangeState(kHAZAN_ANIM_FIND animFind);

    // 업데이트
    void Update(_float fTimeDelta);

    // 현재 상태 업데이트 및 전환 체크
    _bool Update_Transition(_uint& iCurrentState);

    // 현재 상태로 애니메이션 인덱스 가져오기
    _int Get_AnimationIndex(_uint iState, _uint iDirection = 0);

    // 상태 정보
    const SPEAR_ANIMSTATE& Get_CurrentState() const { return m_eCurState; }
    _uint Get_CurrentCategory() const { return m_eCurState.animFind.iCategory; }
    _int Get_CurrentAnimIndex() const { return m_eCurState.iAnimIndex; }


private:
    class CKhazan_Spear_ASMachine*  m_pASM = { nullptr };
    vector<SPEAR_ANIMTRANSITION>    m_Transitions;

    SPEAR_ANIMSTATE                 m_eCurState;
    SPEAR_ANIMSTATE                 m_ePrevState;


private:
    // 상태 변경
    void Change_State(kHAZAN_ANIM_FIND animFind);

    // 전환 가능 여부 체크
    _bool Can_Transition(_uint iNewCategory, _uint iNewSubType);

    // 애니메이션 인덱스 찾기
    _int Find_AnimationIndex(kHAZAN_ANIM_FIND animFind);

public:
    static CKhazan_Spear_ASManager* Create(class CKhazan_Spear_ASMachine* pASM);
    virtual void Free() override;

};

NS_END
