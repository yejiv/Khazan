#include "AS_Devour_Viper.h"
#include "Viper.h"
#include "Body_Viper.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "GameInstance.h"
#include "ClientInstance.h"


CAS_Devour_Viper::CAS_Devour_Viper()
{

}

void CAS_Devour_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    m_fMinRange = pBB->Get_Value<_float>(pViper->Get_Name(), "AttackRange") - 10.f;
    m_fMaxRange = m_fMinRange + 400.f;
    m_fMinSpeed = 5.f;
    m_fMaxSpeed = 30.f;

    pModel->Set_Animation(64);
}

void CAS_Devour_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{

    m_fDevourAcc += fTimeDelta;

    CViper* pViper = static_cast<CViper*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();

    if (pBB->Get_Value<_bool>(pViper->Get_Name(), "P1_SpinStart"))
    {
        CGameObject* pTarget = pBB->Get_Value<CGameObject*>(pViper->Get_Name(), "Target");
        CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));

        _vector vOwnerPos = pOwnerTransform->Get_State(STATE::POSITION);
        _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

        _vector vDirection = XMVector3Normalize(vOwnerPos - vTargetPos);
        _float fDist = pBB->Get_Value<_float>(pViper->Get_Name(), "TargetDist");

        _float fSpeed = MakeDevourSpeed(fDist);

        if (m_fDevourAcc >= 0.5f)
        {
            if (fDist < m_fMinRange)
            {
                CCreature* pDamagedTarget = static_cast<CCreature*>(pTarget);
                
               
                CClientInstance::GetInstance()->Set_PlayerInput(false);
                pDamagedTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_WEAK);
                m_fDevourAcc = 0.f;
                
            }
        }


        _vector vNewPos = vTargetPos + (vDirection * fSpeed * fTimeDelta);
        pTargetTransform->Set_State(STATE::POSITION, vNewPos);

    }
    if (pModel->Play_Animation(fTimeDelta))
    {
        pViper->Get_Controller()->Get_BlackBoard()->Set_Value<_bool>(pViper->Get_Name(), "isP1_DevourFinished", true);
        CClientInstance::GetInstance()->Set_PlayerInput(true);
    }

}

void CAS_Devour_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CClientInstance::GetInstance()->Set_PlayerInput(true);

}

void CAS_Devour_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    COLLISION_LAYER eLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (COLLISION_LAYER::PLAYER == eLayer)
    {

        CCreature* pTarget = static_cast<CCreature*>(pDesc->pGameObject);
        CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
        pTarget->Take_Damage(10.f, HITREACTION::KNOCKBACK_WEAK);
        _vector vLook = pOwnerTransform->Get_State(STATE::LOOK);
        //pTarget->KnockBack(vLook, 20.f, 30.f);
    }
}

_float CAS_Devour_Viper::MakeDevourSpeed(_float fDist)
{
    fDist = clamp(fDist, m_fMinRange, m_fMaxRange);
    _float t = (fDist - m_fMinRange) / (m_fMaxRange - m_fMinRange);
    return m_fMinSpeed + t * (m_fMaxSpeed - m_fMinSpeed);

}

CAS_Devour_Viper* CAS_Devour_Viper::Create()
{
    return new CAS_Devour_Viper();
}

void CAS_Devour_Viper::Free()
{
    __super::Free();
}
