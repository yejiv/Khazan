#include "AS_Sleep_Imp_Melee.h"
#include "Imp_Melee.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Imp_Melee.h"
#include "AI_Controller.h"
#include "FSM_Imp_Melee.h"

CAS_Sleep_Imp_Melee::CAS_Sleep_Imp_Melee()
{
}

void CAS_Sleep_Imp_Melee::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    pModel->Set_Animation(28);

    m_eState = IMP_SlEEP_STATE::SLEEP;
    m_isChanged = false;

}

void CAS_Sleep_Imp_Melee::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CImp_Melee* pImp = static_cast<CImp_Melee*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pImp->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pImp->Get_Controller()->Get_BlackBoard();
    switch (m_eState)
    {
    case IMP_SlEEP_STATE::SLEEP:
        if (pBB->Get_Value<_bool>(pImp->Get_Name(), "isDetected"))
        {
            if (!m_isChanged)
            {
                m_isChanged = true;
                pModel->Set_Animation(27);
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
            m_eState = IMP_SlEEP_STATE::END;
            m_isChanged = false;

            pBB->Set_Value<_bool>(pImp->Get_Name(), "isSleepFinished", true);
            pFSM->Change_State(ENUM_CLASS(IMPMELEE_STATE::IDLE),pImp);
        }
    }


}

void CAS_Sleep_Imp_Melee::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
}

CAS_Sleep_Imp_Melee* CAS_Sleep_Imp_Melee::Create()
{
    return new CAS_Sleep_Imp_Melee();
}

void CAS_Sleep_Imp_Melee::Free()
{
    __super::Free();
}
