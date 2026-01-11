#include "AS_Groggy_Viper.h"
#include "AI_Controller.h"
#include "Viper.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Viper.h"
#include "Body_Viper.h"
#include "ClientInstance.h"
#include "Target_BrutalAttack.h"
#include "Body_Phase2_Viper.h"

CAS_Groggy_Viper::CAS_Groggy_Viper()
    :m_fGroggyTime{ 7.f }
{
}

void CAS_Groggy_Viper::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_fCurrentTime = 0.f;

    CViper* pViper = static_cast<CViper*>(pOwner);

    if (CViper::PHASE::PHASE1 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));
        pModel->Set_Animation(33);
        m_pGameInstance->PlaySoundOnce(TEXT("Mon_vo_viper_gro_start_01 (SFX).wav"), 1.f);

    }

    else if (CViper::PHASE::PHASE2 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));
        pModel->Set_Animation(29);
    }

    m_eState = GROGGY::START;
    m_vBonePosition = pViper->Get_LockOnPosition();
    CClientInstance::GetInstance()->Set_PlayerInput(true);
    m_fBrutalAttackDelayTime = 0.5f;

}

void CAS_Groggy_Viper::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CViper* pViper = static_cast<CViper*>(pOwner);

    if (m_isCheckBrutalCnt)
        m_fBrutalAcc += fTimeDelta;


    if (CViper::PHASE::PHASE1 == pViper->Get_Phase())
    {
        CModel* pModel = static_cast<CModel*>(pViper->Get_Body()->Get_Component(TEXT("Com_Model")));

        switch (m_eState)
        {
        case GROGGY::START:
            if (pModel->Play_Animation(fTimeDelta))
            {
                m_eState = GROGGY::LOOP;
                pModel->Set_Animation(32);
                m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
                if (nullptr == m_pBrutalAttack)
                    return;
                Safe_AddRef(m_pBrutalAttack);

                m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_UI"), m_pBrutalAttack);
                m_pBrutalAttack->Setting_BrutalAttack(m_vBonePosition, m_fGroggyTime);
                m_pGameInstance->PlaySoundLoop(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);

            }
            break;

        case GROGGY::LOOP:
        {
            m_fCurrentTime += fTimeDelta;
            pModel->Play_Animation(fTimeDelta);

            CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", true);

            if (m_fCurrentTime >= m_fGroggyTime)
            {
                m_eState = GROGGY::RECOVERY;
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);
                pModel->Set_Animation(31);
                m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);


                if (m_isBrutalAttackSuccess)
                {
                    HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

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
                    static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

                if (HITREACTION::BRUTAL_ATTACK == eHitreaction)
                {
                    m_pBrutalAttack->Off_BrutalAttack();
                    m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);
                    m_isBrutalAttackSuccess = false;
                    pModel->Set_Animation(23);
                    pViper->Set_RequestRecoveryStamina(true);
                    pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);
                    m_eState = GROGGY::BRUTALATTACK;
                }
            }

        }
        break;


        case GROGGY::BRUTALATTACK:
        {

            if (m_fBrutalAttackDelayTime <= m_fBrutalAcc && !m_isBlockAnimState)
            {
                m_isBlockAnimState = true;
                pModel->Set_Animation(24);
            }

            if (pModel->Play_Animation(fTimeDelta))
            {
                if (m_isBlockAnimState)
                {
                    pModel->Set_Animation(31);
                    m_isBlockAnimState = false;
                    m_fBrutalAcc = 0.f;
                    m_isCheckBrutalCnt = false;
                    m_eState = GROGGY::RECOVERY;
                }

            }
        }
        break;


        case GROGGY::RECOVERY:
            if (pModel->Play_Animation(fTimeDelta))
            {
                m_eState = GROGGY::END;
                pModel->Set_Animation(42);
                pViper->Set_RequestRecoveryStamina(true);
                CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isSuperArmor", true);
            }
            break;

        case GROGGY::END:
            pViper->Recovery_Stamina(fTimeDelta * 150.f);

            if (pModel->Play_Animation(fTimeDelta))
            {
                pViper->Set_RequestRecoveryStamina(false);
                CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);

                pViper->Set_RequestRecoveryStamina(false);
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isGroggyFinished", true);
            }
            break;
        }
    }


    else if (CViper::PHASE::PHASE2 == pViper->Get_Phase())
    {

        CModel* pModel = static_cast<CModel*>(pViper->Get_P2Body()->Get_Component(TEXT("Com_Model")));

        switch (m_eState)
        {
        case GROGGY::START:
            if (pModel->Play_Animation(fTimeDelta))
            {
                m_eState = GROGGY::LOOP;
                pModel->Set_Animation(28);
                m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
                if (nullptr == m_pBrutalAttack)
                    return;
                Safe_AddRef(m_pBrutalAttack);

                m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::VIPER), TEXT("Layer_UI"), m_pBrutalAttack);
                m_pBrutalAttack->Setting_BrutalAttack(m_vBonePosition, m_fGroggyTime);
                m_pGameInstance->PlaySoundLoop(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);

            }
            break;

        case GROGGY::LOOP:
        {
            m_fCurrentTime += fTimeDelta;
            pModel->Play_Animation(fTimeDelta);

            CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
            pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", true);

            if (m_fCurrentTime >= m_fGroggyTime)
            {
                m_eState = GROGGY::RECOVERY;
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);
                pModel->Set_Animation(27);
                m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);


                if (m_isBrutalAttackSuccess)
                {
                    HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

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
                    static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

                if (HITREACTION::BRUTAL_ATTACK == eHitreaction)
                {
                    m_pBrutalAttack->Off_BrutalAttack();
                    m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);
                    m_isBrutalAttackSuccess = false;
                    pModel->Set_Animation(4);
                    pViper->Set_RequestRecoveryStamina(true);
                    pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);
                    m_eState = GROGGY::BRUTALATTACK;
                }
            }

        }
        break;


        case GROGGY::BRUTALATTACK:
        {

            if (m_fBrutalAttackDelayTime <= m_fBrutalAcc && !m_isBlockAnimState)
            {
                m_isBlockAnimState = true;
                pModel->Set_Animation(5);
            }

            if (pModel->Play_Animation(fTimeDelta))
            {
                if (m_isBlockAnimState)
                {
                    pModel->Set_Animation(31);
                    m_isBlockAnimState = false;
                    m_fBrutalAcc = 0.f;
                    m_isCheckBrutalCnt = false;
                    m_eState = GROGGY::RECOVERY;
                }

            }
        }
        break;


        case GROGGY::RECOVERY:

            if (pModel->Play_Animation(fTimeDelta))
            {
                m_eState = GROGGY::END;
                pModel->Set_Animation(42);
                pViper->Set_RequestRecoveryStamina(true);
                CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isSuperArmor", true);
            }
            break;

        case GROGGY::END:
            pViper->Recovery_Stamina(fTimeDelta * 150.f);

            if (pModel->Play_Animation(fTimeDelta))
            {
                pViper->Set_RequestRecoveryStamina(false);
                CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);

                pViper->Set_RequestRecoveryStamina(false);
                pBB->Set_Value<_bool>(pViper->Get_Name(), "isGroggyFinished", true);
            }
            break;
        }
    }
}

void CAS_Groggy_Viper::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_pGameInstance->StopByKey_FadeOut(TEXT("Mon_vo_viper_part_gro_loop_04 (SFX).wav"), 1.f);
    CViper* pViper = static_cast<CViper*>(pOwner);
    CBlackBoard* pBB = pViper->Get_Controller()->Get_BlackBoard();
    pBB->Set_Value<_bool>(pViper->Get_Name(), "isGroggyFinished", true);
    pBB->Set_Value<_bool>(pViper->Get_Name(), "isCanBrutalAttack", false);
    m_pBrutalAttack->Off_BrutalAttack();
    m_pBrutalAttack->Set_IsDead(true);
    m_isBrutalAttackSuccess = false;
    m_isCheckBrutalCnt = false;

}

void CAS_Groggy_Viper::OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, CGameObject* pOwner)
{
    CViper* pViper = static_cast<CViper*>(pOwner);
    COLLISION_LAYER eCollisionLayer = static_cast<COLLISION_LAYER>(iCollisionLayer);

    if (eCollisionLayer == COLLISION_LAYER::PLAYER_ATTACK /*&& pDesc->strName == TEXT("AttackCollisionDesc")*/)
    {
        CBlackBoard* pBB = static_cast<CBlackBoard*>(pViper->Get_Controller()->Get_BlackBoard());
        HITREACTION eHitreaction = static_cast<HITREACTION>(pBB->Get_Value<_uint>(pViper->Get_Name(), "DamageType"));

        if (eHitreaction == HITREACTION::BRUTAL_ATTACK)
            m_isCheckBrutalCnt = true;

    }
}

CAS_Groggy_Viper* CAS_Groggy_Viper::Create()
{
    return new CAS_Groggy_Viper();
}

void CAS_Groggy_Viper::Free()
{
    __super::Free();

    if (m_pBrutalAttack)
        m_pBrutalAttack->Set_IsDead(true);

}
