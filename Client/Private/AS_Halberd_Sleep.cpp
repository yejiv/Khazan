#include "AS_Halberd_Sleep.h"
#include "GameInstance.h"

CAS_Halberd_Sleep::CAS_Halberd_Sleep()
{
}

void CAS_Halberd_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CHalberd*>(pOwner)->Get_Data();

    m_pMonData->pOwner->Hp_Visivle(false);
    m_pMonData->iAnimIndex = 50;
    m_eState = SLEEP;
}

void CAS_Halberd_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{ 
    if (m_eState == SLEEP && !m_pMonData->isSleep)
    {
        m_pMonData->iAnimIndex = 49;
        m_pMonData->pOwner->Hp_Visivle(true);
        m_eState = GETUP;
    }
    else if (m_eState == GETUP)
    {
        if (m_pMonData->isAnimFinash)
            m_pMonData->isStateFiash = true;
    }
}

void CAS_Halberd_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isSleep = false;
    m_pMonData->isStateFiash = true;
}

CAS_Halberd_Sleep* CAS_Halberd_Sleep::Create()
{
    return new CAS_Halberd_Sleep();
}

void CAS_Halberd_Sleep::Free()
{
    __super::Free();
}
