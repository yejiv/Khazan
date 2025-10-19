#include "UI_SlotTest.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_SlotTest::CUI_SlotTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Slot{ pDevice, pContext }
{
}

CUI_SlotTest::CUI_SlotTest(const CUI_SlotTest& Prototype)
	: CUI_Slot( Prototype )
{
}

HRESULT CUI_SlotTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_SlotTest::Initialize_Clone(void* pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg), E_FAIL);

	return S_OK;
}

void CUI_SlotTest::Priority_Update(_float fTimeDelta)
{
}

void CUI_SlotTest::Update(_float fTimeDelta)
{
}

void CUI_SlotTest::Late_Update(_float fTimeDelta)
{
	m_iState = ENUM_CLASS(STATE::DISABLE);
	if (m_pGameInstance->Key_Pressing(DIK_P, fTimeDelta, nullptr))
	{
		m_iState = ENUM_CLASS(STATE::ENABLE);
	}
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_SlotTest::Render()
{
	return S_OK;
}

CUI_SlotTest* CUI_SlotTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_SlotTest* pInstance = new CUI_SlotTest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_SlotTest"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_SlotTest::Clone(void* pArg)
{
	CUI_SlotTest* pInstance = new CUI_SlotTest(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_SlotTest"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_SlotTest::Free()
{
	__super::Free();
}
