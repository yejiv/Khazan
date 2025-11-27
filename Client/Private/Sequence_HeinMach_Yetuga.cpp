#include "Sequence_HeinMach_Yetuga.h"
#include "Camera_Compre.h"
#include "UI_Announce_MapName.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Creature.h"
#include "Transform.h"
#include "GameObject.h"

CSequence_HeinMach_Yetuga::CSequence_HeinMach_Yetuga(CCamera_Compre* pCamera, class CCreature* pPlayer)
	: m_pCamera_Compre{ pCamera }
	, m_pPlayer { pPlayer }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_HeinMach_Yetuga::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
	m_Id = tDesc.tId;
	m_fTime = tDesc.fStartTime;

	m_pPlayer = dynamic_cast<CCreature*>(m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_Creature_Player")));

	return S_OK;
}

void CSequence_HeinMach_Yetuga::Update(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (!m_isFadeOut)
	{
		CClientInstance::GetInstance()->Fade_Out();

		m_isFadeOut = true;
	}

	if (m_fTime > 2.f && !m_isPlayerMove)
	{
		PlayerMove(fTimeDelta);

		m_isPlayerMove = true;
	}

	if (m_fTime > 3.f && !m_isFadeIn)
	{
		CClientInstance::GetInstance()->Fade_In();

		m_isFadeIn = true;
	}

	if (m_fTime >= 5.f)
	{
		m_isEnd = true;
	}


}

void CSequence_HeinMach_Yetuga::Pause()
{

}

void CSequence_HeinMach_Yetuga::Resume()
{

}

void CSequence_HeinMach_Yetuga::StopImmediate()
{

}

void CSequence_HeinMach_Yetuga::Jump(_float fTime)
{
	m_fTime = fTime;
}

_bool CSequence_HeinMach_Yetuga::IsEnd() const
{
	return m_isEnd;
}

void CSequence_HeinMach_Yetuga::PlayerMove(_float fTimeDelta)
{
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->CGameObject::Get_Component(TEXT("Com_Transform")));
	CCharacterVirtual* pPlayerCharVir = dynamic_cast<CCharacterVirtual*>(m_pPlayer->CGameObject::Get_Component(TEXT("Com_CharacterVirtual")));

	pPlayerTransform->Set_State(STATE::POSITION, XMVectorSet(510.32f, -9.72f, 256.11f, 1.f));
    pPlayerTransform->LookAt(XMVectorSet(0.47f, -0.04f, -0.88f, 0.f));
	pPlayerCharVir->Sync_Update(pPlayerTransform);
	pPlayerCharVir->Update(fTimeDelta, pPlayerTransform);
}

CSequence_HeinMach_Yetuga* CSequence_HeinMach_Yetuga::Create(CCamera_Compre* pCamera, class CCreature* pPlayer)
{
	return new CSequence_HeinMach_Yetuga(pCamera, pPlayer);
}

void CSequence_HeinMach_Yetuga::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	m_pCamera_Compre = nullptr;
}
