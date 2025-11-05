#include "AS_Groggy_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"
#include "ClientInstance.h"
#include "Target_BrutalAttack.h"

CAS_Groggy_Yetuga::CAS_Groggy_Yetuga()
	:m_fGroggyTime{ 5.f }
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
}

void CAS_Groggy_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
	CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
	CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

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
			Safe_AddRef(m_pBrutalAttack);
			

			m_pGameInstance->Push_GameObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);
			m_pBrutalAttack->Setting_BrutalAttack(m_vBonePosition, m_fGroggyTime);
		}
		break;

	case GROGGY::LOOP:
		m_fCurrentTime += fTimeDelta;
		pModel->Play_Animation(fTimeDelta); 


		if (m_fCurrentTime >= m_fGroggyTime)
		{
			m_eState = GROGGY::END;
			pModel->Set_Animation(91);

			if (m_isBrutalAttackSuccess)
			{
				HITREACTION eHitreaction = 
					static_cast<HITREACTION>(m_pGameInstance->Get_BlackBoard()->Get_Value<_uint>(pYetuga->Get_Name(), "DamageType"));

				if (HITREACTION::GROGGY == eHitreaction)
				{
					m_pBrutalAttack->Off_BrutalAttack();
					Safe_Release(m_pBrutalAttack);
					m_isBrutalAttackSuccess = false;
				}
			}
		}
		break;

	case GROGGY::END:
		if (pModel->Play_Animation(fTimeDelta))
		{
			m_pGameInstance->Get_BlackBoard()->Set_Value<_bool>(pYetuga->Get_Name(), "isGroggyFinished", true);
		}
		break;
	}
}

void CAS_Groggy_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{

}

CAS_Groggy_Yetuga* CAS_Groggy_Yetuga::Create()
{
	return new CAS_Groggy_Yetuga();
}

void CAS_Groggy_Yetuga::Free()
{
	__super::Free();

	Safe_Release(m_pBrutalAttack);

}
