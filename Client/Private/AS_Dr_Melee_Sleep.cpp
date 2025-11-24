#include "AS_Dr_Melee_Sleep.h"
#include "Dragonian_Melee.h"
#include "BlackBoard.h"
#include "StateMachine.h"

CAS_Dr_Melee_Sleep::CAS_Dr_Melee_Sleep()
{
}

void CAS_Dr_Melee_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Melee*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 60;
    m_eState = SLEEP;
}

void CAS_Dr_Melee_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_eState == SLEEP && !m_pMonData->isSleep)
    {
        m_pMonData->iAnimIndex = 59;
        m_eState = GETUP;
    }
    else if (m_eState == GETUP)
    {
        if (m_pMonData->isAnimFinash)
            m_pMonData->isStateFiash = true;
    }
}

void CAS_Dr_Melee_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isStateFiash = false;
    m_eState = END;
}

CAS_Dr_Melee_Sleep* CAS_Dr_Melee_Sleep::Create()
{
    return new CAS_Dr_Melee_Sleep();
}

void CAS_Dr_Melee_Sleep::Free()
{
    __super::Free();
}
