#include "AS_Elamein_Sleep.h"
#include "GameInstance.h"

CAS_Elamein_Sleep::CAS_Elamein_Sleep()
{
}

void CAS_Elamein_Sleep::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    if (m_pMonData == nullptr)
        m_pMonData = &static_cast<CElamein*>(pOwner)->Get_Data();

    m_pMonData->pOwner->Hp_Visivle(false);
    m_pMonData->iAnimIndex = 101;

}

void CAS_Elamein_Sleep::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_BACKSPACE))
    {
        m_isChange ? m_isChange = false : m_isChange = true;
        m_isChange ? m_pMonData->iAnimIndex = 101 : m_pMonData->iAnimIndex = 83;
    }
 
    if (!m_pMonData->isSleep)
    {
        m_pMonData->pOwner->Hp_Visivle(true);
        m_pMonData->isStateFiash = true;
        m_pMonData->fGuardCool = 30.f;
        m_pMonData->fSpecial_AttackCool = 40.f;
    }
}

void CAS_Elamein_Sleep::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pMonData->isSleep = false;
    m_pMonData->isStateFiash = true;
}

CAS_Elamein_Sleep* CAS_Elamein_Sleep::Create()
{
    return new CAS_Elamein_Sleep();
}

void CAS_Elamein_Sleep::Free()
{
    __super::Free();
}
