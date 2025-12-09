#include "AS_Pet_Danjinjar_Move.h"
#include "GameInstance.h"

CAS_Pet_Danjinjar_Move::CAS_Pet_Danjinjar_Move()
{
}

void CAS_Pet_Danjinjar_Move::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CPet_Danjinjar*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 14;
    if (m_iTalkCount <= 0)
    {
        m_pMonData->pOwner->isTalk(true, 1001);
        m_iTalkCount = 5;
    }
    --m_iTalkCount;
}

void CAS_Pet_Danjinjar_Move::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->iAnimIndex = 7;
    }
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 3.5f);
    if(m_pMonData->iAnimIndex == 7)
        pOwner->Get_Transform()->Go_Straight(fTimeDelta * 1.5f);
}

void CAS_Pet_Danjinjar_Move::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    
}

CAS_Pet_Danjinjar_Move* CAS_Pet_Danjinjar_Move::Create()
{
    return new CAS_Pet_Danjinjar_Move();
}

void CAS_Pet_Danjinjar_Move::Free()
{
    __super::Free();
}
