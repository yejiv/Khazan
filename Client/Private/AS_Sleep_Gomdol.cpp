#include "AS_Sleep_Gomdol.h"
#include "AI_Controller.h"
#include "Gomdol.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Body_Gomdol.h"

CAS_Sleep_Gomdol::CAS_Sleep_Gomdol()
{
}

void CAS_Sleep_Gomdol::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{

    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));

    pModel->Set_Animation(36);                
    m_eState = GOMDOL_SLEEP_STATE::SLEEP;
    m_isChanged = false;
}

void CAS_Sleep_Gomdol::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{

    CGomdol* pGomdol = static_cast<CGomdol*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pGomdol->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pGomdol->Get_Controller()->Get_BlackBoard();

    switch (m_eState)
    {
    case GOMDOL_SLEEP_STATE::SLEEP:
        if (pBB->Get_Value<_bool>(pGomdol->Get_Name(), "isDetected"))
        {
            if (!m_isChanged)
            {
                pModel->Set_Animation(37);                
                m_isChanged = true;
            }

            m_eState = GOMDOL_SLEEP_STATE::WAKEUP;
            m_isChanged = false;                           
        }
        break;

    case GOMDOL_SLEEP_STATE::WAKEUP:
        if (!m_isChanged)
        {
            pModel->Set_Animation(38);               
            m_isChanged = true;
        }
        break;

    case GOMDOL_SLEEP_STATE::END:
        break;
    }

    if (pModel->Play_Animation(fTimeDelta))
    {
        if (m_eState == GOMDOL_SLEEP_STATE::WAKEUP)
        {
            m_eState = GOMDOL_SLEEP_STATE::END;
            m_isChanged = false;
        }
        else if (m_eState == GOMDOL_SLEEP_STATE::END)
        {
            pBB->Set_Value<_bool>(pGomdol->Get_Name(), "isSleepFinished", true);
        }
    }

}

void CAS_Sleep_Gomdol::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Sleep_Gomdol* CAS_Sleep_Gomdol::Create()
{
    return new CAS_Sleep_Gomdol();
}

void CAS_Sleep_Gomdol::Free()
{
    __super::Free();
}
