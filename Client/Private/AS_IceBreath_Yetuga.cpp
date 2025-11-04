#include "AS_IceBreath_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_IceBreath_Yetuga::CAS_IceBreath_Yetuga()
{

}

void CAS_IceBreath_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_eState = PHASE::START;

    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (PHASE::START == m_eState)
    {
        pModel->Set_Animation(21);
        pYetuga->Look_Target();
    }
    

}

void CAS_IceBreath_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = m_pGameInstance->Get_BlackBoard();

    switch (m_eState)
    {
    case PHASE::START:
        if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath"))
        {
            BB->Set_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath", false);
            pModel->Set_Animation(49);
            m_eState = PHASE::ROOP;
        }
        break;
    case PHASE::ROOP:
        break;
    }

    if (pModel->Play_Animation(fTimeDelta))
    {

        if (PHASE::START == m_eState)
            BB->Set_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath", true);

        if (PHASE::ROOP == m_eState)
            m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isIceBreathFinished", true);
        
    }


}

void CAS_IceBreath_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_IceBreath_Yetuga* CAS_IceBreath_Yetuga::Create()
{
    return new CAS_IceBreath_Yetuga();
}

void CAS_IceBreath_Yetuga::Free()
{
    __super::Free();
}
