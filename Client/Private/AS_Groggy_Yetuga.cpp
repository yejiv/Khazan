#include "AS_Groggy_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "Head_Yetuga.h"
#include "ClientInstance.h"
#include "Target_BrutalAttack.h"

CAS_Groggy_Yetuga::CAS_Groggy_Yetuga()
	:m_fGroggyTime{ 7.f }
{

}

void CAS_Groggy_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
	m_fCurrentTime = 0.f;

	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

	
	m_vBonePosition = pYetuga->Get_LockOnPosition();

	m_eState = GROGGY::START;
	pModel->Set_Animation(89);
    m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_yetuga_gro_strong_start_02 (SFX).wav"), 1.f);


    pYetuga->Get_Body()->Set_AttackCollision_Back(false);
    pYetuga->Get_Body()->Set_OnAttackCollision(false);
    pYetuga->Get_Head()->Set_OnAttackCollision(false);


    m_fBrutalAttackDelayTime = 1.5f;

}

void CAS_Groggy_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (m_isCheckBrutalCnt)
        m_fBrutalAcc += fTimeDelta;


	switch (m_eState)
	{
	case GROGGY::START:
		if (pModel->Play_Animation(fTimeDelta))
		{
			m_eState = GROGGY::LOOP;
			pModel->Set_Animation(90);
			m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
			if (nullptr == m_pBrutalAttack)
				return;

			m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
			m_pBrutalAttack->Setting_BrutalAttack(m_vBonePosition, m_fGroggyTime);
            m_pGameInstance->PlaySoundLoop(TEXT("Mon_vo_yetuga_gro_strong_loop_01 (SFX).wav"), 1.f);
		}
		break;

	case GROGGY::LOOP:
    {
        m_fCurrentTime += fTimeDelta;
        pModel->Play_Animation(fTimeDelta);

        CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
        pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isCanBrutalAttack", true);

        if (m_fCurrentTime >= m_fGroggyTime)
        {
            m_eState = GROGGY::RECOVERY;
            pModel->Set_Animation(91);
            m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_yetuga_gro_strong_loop_01 (SFX).wav"), 1.f);

            if (m_isBrutalAttackSuccess)
            {
                CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
                HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

                if (HITREACTION::GROGGY == eHitreaction)
                {
                    m_pBrutalAttack->Off_BrutalAttack();
                    m_isBrutalAttackSuccess = false;
                }
            }
        }

        else
        {
            HITREACTION eHitreaction =
                static_cast<HITREACTION>(pBB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

           if (HITREACTION::BRUTAL_ATTACK == eHitreaction)
            {
               m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_yetuga_gro_strong_loop_01 (SFX).wav"), 1.f);
                m_pBrutalAttack->Off_BrutalAttack();
                m_isBrutalAttackSuccess = false;
                pModel->Set_Animation(68);
                pYetuga->Set_RequestRecoveryStamina(true);
                m_eState = GROGGY::BRUTALATTACK;
            }
        }

    }

        break;
    
    case GROGGY::BRUTALATTACK:
    {
        if (m_fBrutalAttackDelayTime <= m_fBrutalAcc && !m_isBlockAnimSet)
        {
            m_isBlockAnimSet = true;
            CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
            _vector vLook = pYetuga->Get_Transform()->Get_State(STATE::LOOK);

            m_isSecondKnockback = true;
            pModel->Set_Animation(69);
        }
     
        if (pModel->Play_Animation(fTimeDelta))
        {
            if (m_isBlockAnimSet)
            {
                pModel->Set_Animation(93);
                pYetuga->Set_RequestRecoveryStamina(true);
                CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
                pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isSuperArmor", true);
                m_isBlockAnimSet = false;
                m_fBrutalAcc = 0.f;
                m_isCheckBrutalCnt = false;
                m_eState = GROGGY::END;
            }
        }
    }
    break;

    case GROGGY::RECOVERY:

        if (pModel->Play_Animation(fTimeDelta))
        {
            m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_yetuga_stamina_recover_roar_01 (SFX).wav"), 1.f);

            m_eState = GROGGY::END;              
            pModel->Set_Animation(93);   
            pYetuga->Set_RequestRecoveryStamina(true);
            CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
            pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isSuperArmor", true);
        }
        break;

    case GROGGY::END:

        pYetuga->Recovery_Stamina(fTimeDelta * 100.f);


        if (pModel->Play_Animation(fTimeDelta))
        {
            CBlackBoard* pBB = pYetuga->Get_Controller()->Get_BlackBoard();
            pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isCanBrutalAttack", false);

            pYetuga->Set_RequestRecoveryStamina(false);
            pBB->Set_Value<_bool>(pYetuga->Get_Name(), "isGroggyFinished", true);
        }
        break;
    }

}

void CAS_Groggy_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    pYetuga->Set_IsGhost(false);
    m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_yetuga_stamina_recover_roar_01 (SFX).wav"), 1.f);
    m_isCheckBrutalCnt = false;
}

void CAS_Groggy_Yetuga::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);

    COLLISION_LAYER eCollisionLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (eCollisionLayer == COLLISION_LAYER::PLAYER_ATTACK && pDesc->strName == TEXT("AttackCollisionDesc"))
    {
        CBlackBoard* pBB = static_cast<CBlackBoard*>(pYetuga->Get_Controller()->Get_BlackBoard());
        HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

        if (eHitreaction == HITREACTION::BRUTAL_ATTACK)
        {
            m_isCheckBrutalCnt = true;
        }

    }


}

CAS_Groggy_Yetuga* CAS_Groggy_Yetuga::Create()
{
	return new CAS_Groggy_Yetuga();
}

void CAS_Groggy_Yetuga::Free()
{
	__super::Free();

    if (m_pBrutalAttack)
        m_pBrutalAttack->Set_IsDead(true);

}
