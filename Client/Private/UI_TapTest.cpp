#include "UI_TapTest.h"
#include "GameInstance.h"
#include "ClientInstance.h"
CUI_TapTest::CUI_TapTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CUI_TapTest::CUI_TapTest(const CUI_TapTest& Prototype)
	: CUI_Tap ( Prototype )
{
}

HRESULT CUI_TapTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TapTest::Initialize_Clone(void* pArg)
{
	FAILED_CHECK(__super::Initialize_Clone(pArg), E_FAIL);

	return S_OK;
}

void CUI_TapTest::Priority_Update(_float fTimeDelta)
{
}

void CUI_TapTest::Update(_float fTimeDelta)
{
}

void CUI_TapTest::Late_Update(_float fTimeDelta)
{
	m_eState = STATE::DISABLE;
	if (m_pGameInstance->Key_Pressing(DIK_P, fTimeDelta, nullptr))
	{
		m_eState = STATE::ENABLE;
	}
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_TapTest::Render()
{
	return S_OK;
}

CUI_TapTest* CUI_TapTest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_TapTest* pInstance = new CUI_TapTest(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_TapTest"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_TapTest::Clone(void* pArg)
{
	CUI_TapTest* pInstance = new CUI_TapTest(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_TapTest"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_TapTest::Free()
{
	__super::Free();
}
