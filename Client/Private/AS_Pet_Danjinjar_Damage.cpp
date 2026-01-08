#include "AS_Pet_Danjinjar_Damage.h"
#include "GameInstance.h"

CAS_Pet_Danjinjar_Damage::CAS_Pet_Danjinjar_Damage()
{
}

void CAS_Pet_Danjinjar_Damage::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CPet_Danjinjar*>(pOwner)->Get_Data();

    m_pMonData->isTPDanjin = true;
    m_pMonData->iAnimIndex = 18;
    m_fAccTime = 2.f;
    
    switch (m_pGameInstance->Rand(0, 2))
    {
    case 0:    m_pMonData->pOwner->isTalk(true, 1003); break;
    case 1:    m_pMonData->pOwner->isTalk(true, 1004); break;
    case 2:    m_pMonData->pOwner->isTalk(true, 1005); break;
    }
}

void CAS_Pet_Danjinjar_Damage::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    m_fAccTime -= fTimeDelta;
    m_pMonData->pOwner->LockOnLerp(fTimeDelta, 1.5f);
    if (m_fAccTime <= 0.f)
    {
        m_pMonData->isDamage = false;
        m_pMonData->isTPDanjin = false;
    }
}

void CAS_Pet_Danjinjar_Damage::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isDamage = false;
    m_pMonData->isTPDanjin = false;
}

CAS_Pet_Danjinjar_Damage* CAS_Pet_Danjinjar_Damage::Create()
{
    return new CAS_Pet_Danjinjar_Damage();
}

void CAS_Pet_Danjinjar_Damage::Free()
{
    __super::Free();
}
