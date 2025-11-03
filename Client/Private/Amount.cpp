#include "Amount.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Amount_Panel.h"

CAmount::CAmount(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CAmount::CAmount(const CAmount& Prototype)
	: CUI_Panel(Prototype)
{
}

void CAmount::Add_Value(AMOUNT_TYPE eType, _int IValue)
{
	m_pAmount[ENUM_CLASS(eType)]->Add_Value(IValue);
	m_isAddValue = true;
	On_Panel();
}

void CAmount::On_Panel()
{
	if (m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::ON;
	m_fAccTime = 0.5f;
	m_IsUpdate = true;
}

void CAmount::Off_Panel()
{
	if (!m_IsUpdate)
		return;

	m_eAnimState = UIANIMSTATE::OFF;
	m_fAccTime = 1.f;
	
}

HRESULT CAmount::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CAmount::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CAmount::Priority_Update(_float fTimeDelta)
{
	UI_Animation(fTimeDelta);
	__super::Priority_Update(fTimeDelta);
}

void CAmount::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_Z))
		Add_Value(AMOUNT_TYPE::LACHRYMA, 1000);

	__super::Update(fTimeDelta);

}

void CAmount::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	__super::Late_Update(fTimeDelta);

}

HRESULT CAmount::Render()
{
	return S_OK;
}

HRESULT CAmount::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	m_pAmount.resize(ENUM_CLASS(AMOUNT_TYPE::END));

	for (auto child : m_Children)
	{
		string strName = child->Get_Name();

		if (strName == "Gold")
		{
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::GOLD)] = static_cast<CAmount_Panel*>(child);
			Safe_AddRef(child);
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::GOLD)]->Setting_Index(AMOUNT_TYPE::GOLD, &m_iGold);
		}
		else if (strName == "Lachryma")
		{
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::LACHRYMA)] = static_cast<CAmount_Panel*>(child);
			Safe_AddRef(child);
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::LACHRYMA)]->Setting_Index(AMOUNT_TYPE::LACHRYMA, &m_iLachryma);
		}
		else if (strName == "Stone")
		{
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::STONE)] = static_cast<CAmount_Panel*>(child);
			Safe_AddRef(child);
			m_pAmount[ENUM_CLASS(AMOUNT_TYPE::STONE)]->Setting_Index(AMOUNT_TYPE::STONE, &m_iStone);
		}
		child->Set_ShaderPass(12);
	}
	__super::Update_Transform(nullptr, m_vLocalPos);

	return S_OK;
}

void CAmount::Bubble_EventCall(BUBBLEEVENT* pArg)
{
	Off_Panel();
}

HRESULT CAmount::Update_Switch(void* pArg)
{
	return S_OK;
}


HRESULT CAmount::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Amount_Panel"),
		CAmount_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

void CAmount::UI_Animation(_float fTimeDelta)
{
	if (m_eAnimState == UIANIMSTATE::ON)
	{
		m_fAccTime += fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 1.f;
			m_eAnimState = UIANIMSTATE::END;
		}
	}
	else if (m_eAnimState == UIANIMSTATE::OFF)
	{
		m_fAccTime -= fTimeDelta * 3.f;
		__super::Update_Alpha(m_fAccTime);

		if (m_fAccTime <= 0.f)
		{
			m_fAccTime = 0.f;
			m_eAnimState = UIANIMSTATE::END;
			m_IsUpdate = false;
		}
	}
}

CAmount* CAmount::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CAmount* pInstance = new CAmount(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CAmount"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAmount::Clone(void* pArg)
{
	CAmount* pInstance = new CAmount(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CAmount"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAmount::Free()
{
	__super::Free();

	for (auto Amount : m_pAmount)
		Safe_Release(Amount);
	m_pAmount.clear();

}
