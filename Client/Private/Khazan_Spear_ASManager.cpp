
#include "Khazan_Spear_ASManager.h"
#include "Khazan_Spear_ASMachine.h"


CKhazan_Spear_ASManager::CKhazan_Spear_ASManager()
{
}

HRESULT CKhazan_Spear_ASManager::Initialize_Prototype(CKhazan_Spear_ASMachine* pASM)
{
    m_pASM = pASM;
    Safe_AddRef(m_pASM);

    /* 초기 애니메이션 값 */
    kHAZAN_ANIM_INFO initAnim =
        tagKhazanAnimationFindInfo(
            CKhazan_Spear_ASMachine::WEAPON::NONE,
            CKhazan_Spear_ASMachine::CATEGORY::M_MOVE,
            //CKhazan_Spear_ASMachine::MOVE::MOVE_IDLE,
            ENUM_CLASS(DIRECTION::F)
        );
    m_eCurState.animInfo = initAnim;
    m_eCurState.isInterruptible = true;
    m_eCurState.iAnimIndex = Find_AnimationIndex(initAnim);
    m_eCurState.fStateTime = 0.f;
    m_eCurState.fMinDuration = 0.f;

    return S_OK;
}

void CKhazan_Spear_ASManager::Add_Transition(_uint iFrom, kHAZAN_ANIM_INFO toAnimInfo, function<_bool()> condition, _uint priority)
{
    m_Transitions.emplace_back(iFrom, toAnimInfo, condition, priority);

    // 우선순위로 정렬
    sort(m_Transitions.begin(), m_Transitions.end(), [](const SPEAR_ANIMTRANSITION& a, const SPEAR_ANIMTRANSITION& b) {
        return a.iPriority > b.iPriority; });
}

void CKhazan_Spear_ASManager::Force_ChangeState(kHAZAN_ANIM_INFO animInfo)
{
    Change_State(animInfo);
}

kHAZAN_ANIM_INFO CKhazan_Spear_ASManager::Detect_State(const vector<kHAZAN_ANIM_INFO>& candidates)
{
    kHAZAN_ANIM_INFO bestAnim;
    _uint bestPriority = 9999;

    for (const auto& candidate : candidates)
    {
        _uint priority = m_pASM->Get_CategoryPriority(candidate.iCategory, candidate.iSubType);
        if (priority < bestPriority)  // 낮은 숫자가 높은 우선순위
        {
            bestPriority = priority;
            bestAnim = candidate;
        }
    }

    // 아무것도 없으면 IDLE 반환
    if (bestPriority == 9999)
    {
        return kHAZAN_ANIM_INFO(
            CKhazan_Spear_ASMachine::WEAPON::SPEAR,
            CKhazan_Spear_ASMachine::CATEGORY::M_MOVE,
            //CKhazan_Spear_ASMachine::MOVE::MOVE_IDLE,
            ENUM_CLASS(DIRECTION::F)
        );
    }

    return bestAnim;
}


void CKhazan_Spear_ASManager::Update(_float fTimeDelta, kHAZAN_ANIM_INFO animInfo)
{
    m_eCurState.fStateTime += fTimeDelta;


    /* 최소 시간 체크 */
    if (m_eCurState.fStateTime < m_eCurState.fMinDuration)
        return;

    /* 중단 불가 체크*/
    if (!m_eCurState.isInterruptible)
        return;

    /* 우선순위 체크 */
    for (const auto& rule : m_Transitions)
    {
        /* 카테고리 일치 체크 */
        if (!(m_eCurState.animInfo.iCategory & rule.iFromCategory))
            continue;

        /* 컨디션 체크 */
        if (!rule.checkCondition || rule.checkCondition())
            continue;

        /*  */
        if (!Can_Transition(rule.toAnimInfo.iCategory, rule.toAnimInfo.iSubType))
            continue;

        /* 전환  */
        Change_State(rule.toAnimInfo);
        return;
    }
    

}

_bool CKhazan_Spear_ASManager::Update_Transition(_uint& iCurrentState)
{
    // 우선순위 순으로 전환 조건 체크
    for (const auto& transition : m_Transitions)
    {
        // 현재 상태와 일치하는 전환만 체크
        if ((iCurrentState & transition.iFromCategory) && transition.checkCondition())
        {
            // 이전 상태 제거하고 새 상태 추가
            iCurrentState &= ~transition.iFromCategory;
            iCurrentState |= transition.toAnimInfo.iCategory;
            return true;  // 전환 발생
        }
    }

    return false;  // 전환 없음
}

_int CKhazan_Spear_ASManager::Get_AnimationIndex(_uint iState, _uint iDirection)
{
    // 우선 공격 체크
    if (iState & CKhazan_Spear_ASMachine::CATEGORY::M_ATTACK)
    {
        // 어떤 공격 타입인지 확인
        const auto* pASM = m_pASM->Get_ASM(0);  // 임시

        // 예시: FAST 공격
        if (iState & (1 << 3))  // ATTACK_FAST
        {
            return m_pASM->Get_AnimIndexByName(L"CA_P_Kazan_Spear_Com_FastAtk01");
        }
    }

    // 이동 체크
    if (iState & CKhazan_Spear_ASMachine::CATEGORY::M_MOVE)
    {
        if (iState & (1 << 2))  // RUN
        {
            return m_pASM->Get_AnimIndexByName(L"CA_P_Kazan_Spear_Run_F");
        }
        else if (iState & (1 << 1))  // WALK
        {
            return m_pASM->Get_AnimIndexByName(L"CA_P_Kazan_Spear_Walk_F");
        }
        else if (iState & (1 << 0))  // IDLE
        {
            return m_pASM->Get_AnimIndexByName(L"CA_P_Kazan_Spear_Stand");
        }
    }

    return -1;
}

void CKhazan_Spear_ASManager::Change_State(kHAZAN_ANIM_INFO animInfo)
{
    m_ePrevState = m_eCurState;

    m_eCurState.animInfo = animInfo;
    m_eCurState.iAnimIndex = Find_AnimationIndex(animInfo);
    m_eCurState.fStateTime = 0.f;

    /* 상태별 속성 설정하기  */
    /* todo  */
    m_eCurState.isInterruptible = true;
}

_bool CKhazan_Spear_ASManager::Can_Transition(_uint iNewCategory, _uint iNewSubType)
{
    return m_pASM->Can_Interrupt(m_eCurState.animInfo.iCategory, m_eCurState.animInfo.iSubType, iNewCategory, iNewSubType);
}

_int CKhazan_Spear_ASManager::Find_AnimationIndex(kHAZAN_ANIM_INFO animInfo)
{
    const vector<CKhazan_Spear_ASMachine::SPEAR_ASM>& asms = m_pASM->Get_ASMs();

    for (_uint i = 0; i < static_cast<_uint>(asms.size()); ++i)
    {
         const CKhazan_Spear_ASMachine::SPEAR_ASM&  c_asm = asms[i];
         // 무기 체크 
         if (!(c_asm.iWeapon & animInfo.iWeapon)) continue;

         // 카테고리 체크
         if (!(c_asm.iCategory & animInfo.iCategory)) continue;

         // 서브타입 체크
         // MOVE
         if ((CKhazan_Spear_ASMachine::CATEGORY::M_MOVE & animInfo.iCategory))
         {
             if (!(c_asm.iMove & animInfo.iSubType)) continue;
             if (animInfo.iCycle && !(c_asm.iCycle & animInfo.iCycle)) continue;
             if (animInfo.iDirection && !(c_asm.iDirection & animInfo.iDirection)) continue;
             return i;
         }

         //ATTACK 
         else if (animInfo.iCategory & CKhazan_Spear_ASMachine::CATEGORY::M_ATTACK)
         {
             if (!(c_asm.iAttack & animInfo.iSubType)) continue;
             if (animInfo.iDirection && !(c_asm.iDirection & animInfo.iDirection)) continue;
             return i;
         }

         //SKILL
         else if (animInfo.iCategory & CKhazan_Spear_ASMachine::CATEGORY::M_SKILL)
         {
             if (c_asm.iSkill != animInfo.iSubType) continue;
             return i;
         }




    }

    return -1;
}

CKhazan_Spear_ASManager* CKhazan_Spear_ASManager::Create(CKhazan_Spear_ASMachine* pASM)
{
    CKhazan_Spear_ASManager* pInstance = new CKhazan_Spear_ASManager();

    if (FAILED(pInstance->Initialize_Prototype(pASM)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CKhazan_Spear_ASManager::Free()
{
    Safe_Release(m_pASM);
    __super::Free();
}
