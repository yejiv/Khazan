#include "AS_Sleep_Imp_Range.h"
#include "Imp_Range.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Imp_Range.h"
#include "AI_Controller.h"

CAS_Sleep_Imp_Range::CAS_Sleep_Imp_Range()
{
}

void CAS_Sleep_Imp_Range::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);

    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(1);

    m_eState = IMP_SlEEP_STATE::SLEEP;
    m_isChanged = false;
}

void CAS_Sleep_Imp_Range::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Range* pImp = static_cast<CImp_Range*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    //CBlackBoard* pBB = m_pGameInstance->Get_BlackBoard();
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();

    switch (m_eState)
    {
    case IMP_SlEEP_STATE::SLEEP: 
        if (pBB->Get_Value<_bool>(pImp->Get_Name(), "isDetected"))
        {
            if (!m_isChanged)
            {
                pModel->Set_Animation(2);
                m_eState = IMP_SlEEP_STATE::WAKEUP;
            }
        }
        break;
    case IMP_SlEEP_STATE::WAKEUP:
        break;

    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        if (m_eState == IMP_SlEEP_STATE::WAKEUP)
        {
            pBB->Set_Value<_bool>(pImp->Get_Name(), "isSleepFinished", true);
        }
    }
}

void CAS_Sleep_Imp_Range::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Sleep_Imp_Range* CAS_Sleep_Imp_Range::Create()
{
    return new CAS_Sleep_Imp_Range();
}

void CAS_Sleep_Imp_Range::Free()
{
    __super::Free();
}
