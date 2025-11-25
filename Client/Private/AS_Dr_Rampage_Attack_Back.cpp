#include "AS_Dr_Rampage_Attack_Back.h"
#include "GameInstance.h"

CAS_Dr_Rampage_Attack_Back::CAS_Dr_Rampage_Attack_Back()
{
}

void CAS_Dr_Rampage_Attack_Back::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CDragonian_Rampage*>(pOwner)->Get_Data();

    m_pMonData->iAnimIndex = 45;
}

void CAS_Dr_Rampage_Attack_Back::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pMonData->isAnimFinash)
    {
        m_pMonData->eAttack_State = CDragonian_Rampage::ATTACKSTATE::END;
    }
}

void CAS_Dr_Rampage_Attack_Back::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->fAttackCool = m_pGameInstance->Rand(5.f, 9.f);
}

CAS_Dr_Rampage_Attack_Back* CAS_Dr_Rampage_Attack_Back::Create()
{
    return new CAS_Dr_Rampage_Attack_Back();
}

void CAS_Dr_Rampage_Attack_Back::Free()
{
    __super::Free();
}
