#include "AS_Dr_Melee_Walk.h"

CAS_Dr_Melee_Walk::CAS_Dr_Melee_Walk()
{
}

void CAS_Dr_Melee_Walk::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 58;
    m_eState = WALK;
    m_pMonData->fAnimDeley = 0.5f;
}

void CAS_Dr_Melee_Walk::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == WALK)
    {
        m_pMonData->pOwner->LockOnLerp(fTimeDelta);
        pOwner->Get_Transform()->Go_Straight(m_pMonData->fWarkSpeed * fTimeDelta);

        if (m_pMonData->fAttackCool > 0.f)
        {
            if (m_pMonData->pOwner->Check_AttackRanage("WalkRange"))
            {
                if (!m_isRange)
                {
                    m_pMonData->fAnimDeley = 0.5f;
                    m_isRange = true;
                }
                if (m_pMonData->fAnimDeley < 0.f)
                    m_pMonData->isChase = false;
            }
            else
            {
                m_isRange = false;
                m_pMonData->fAnimDeley = 0.5f;
            }
        }
        else
        {
            if(m_pMonData->pOwner->Check_AttackRanage("AttackRange"))
                m_pMonData->isChase = false;
        }
    }
}

void CAS_Dr_Melee_Walk::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_eState = END;
    m_pMonData->isChase = false;
}

CAS_Dr_Melee_Walk* CAS_Dr_Melee_Walk::Create()
{
    return new CAS_Dr_Melee_Walk();
}

void CAS_Dr_Melee_Walk::Free()
{
    __super::Free();
}
