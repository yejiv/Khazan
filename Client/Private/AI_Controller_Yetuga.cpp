#include "AI_Controller_Yetuga.h"
#include "Yetuga.h"
#include "BT_Yetuga.h"
#include "FSM_Yetuga.h"
#include "BlackBoard.h"
#include "GameInstance.h"
#include "Perception.h"


CAI_Controller_Yetuga::CAI_Controller_Yetuga()
{
}

HRESULT CAI_Controller_Yetuga::Initialize(void* pArg)
{
	SIGHT_DESC SightDesc = {};
	SightDesc.fRadius = 600.f;
	SightDesc.fLoseSightTime = 0.1f;
	SightDesc.fFov = 90.f;
	SightDesc.fFovCos = cosf(XMConvertToRadians(SightDesc.fFov * 0.5f));
	m_pPerception = CPerception::Create("Yetuga", SightDesc, ENUM_CLASS(TEAM::YETI));


	m_pFSM = CFSM_Yetuga::Create();
	if (nullptr == m_pFSM)
		return E_FAIL;


	m_pBB = m_pGameInstance->Get_BlackBoard();
	if (nullptr == m_pBB)
		return E_FAIL;

	m_pBT = CBT_Yetuga::Create(pArg);
	if (nullptr == m_pBT)
		return E_FAIL;

	if (FAILED(Ready_BlackBoard()))
		return E_FAIL;

	if (FAILED(Ready_Perception()))
		return E_FAIL;

	if (FAILED(Ready_BehaviorTree()))
		return E_FAIL;

	return S_OK;
}

void CAI_Controller_Yetuga::Update(CGameObject* pOwner, _float fTimeDelta)
{
	m_pPerception->Update(pOwner, fTimeDelta);

	m_pBT->Update();

	m_pFSM->Update(pOwner,fTimeDelta);

}


HRESULT CAI_Controller_Yetuga::Ready_BlackBoard()
{
	m_pBB->Set_Value("Yetuga", "isIdle", true);
	m_pBB->Set_Value("Yetuga", "isDetected", false);
	m_pBB->Set_Value("Yetuga", "isMove", false);
	m_pBB->Set_Value("Yetuga", "isAttack", false);
	m_pBB->Set_Value("Yetuga", "isDead", false);
	m_pBB->Set_Value("Yetuga", "isAttackAnimFinish", false);
	m_pBB->Set_Value("Yetuga", "AttackCoolDown", 8.f);
	m_pBB->Set_Value("Yetuga", "AttackRange", 150.f);
	m_pBB->Set_Value("Yetuga", "ChaseRange", 2000.f);

	return S_OK;
}

HRESULT CAI_Controller_Yetuga::Ready_Perception()
{
	m_pPerception->Set_PerceptionCallBack([this](CGameObject* pTarget, const STIMULUS& Stim)
		{
			if (Stim.eType == SENSETYPE::SIGHT)
			{
				if (Stim.bSensed)
				{
					m_pBB->Set_Value("Yetuga", "Target", pTarget);
					m_pBB->Set_Value("Yetuga", "isDetected", true);
				}
				else
				{
					m_pBB->Set_Value("Yetuga", "isDetected", false);
				}
			}
		});

	return S_OK;
}

HRESULT CAI_Controller_Yetuga::Ready_BehaviorTree()
{
	if (nullptr == m_pBB)
		return E_FAIL;

	m_pBT->Set_BlackBoard(m_pBB);

	if (nullptr == m_pBT)
		return E_FAIL;

	return S_OK;
}

CAI_Controller_Yetuga* CAI_Controller_Yetuga::Create(void* pArg)
{
	CAI_Controller_Yetuga* pInstance = new CAI_Controller_Yetuga();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX(TEXT("Failed Create : CAI_Controller_Yetuga"));
	}

	return pInstance;
}

void CAI_Controller_Yetuga::Free()
{
	__super::Free();
}
