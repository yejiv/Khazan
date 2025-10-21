#include "Inven_Panel.h"
#include "GameInstance.h"

CInven_Panel::CInven_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CInven_Panel::CInven_Panel(const CInven_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CInven_Panel::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CInven_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInven_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInven_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CInven_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CInven_Panel::Render()
{
	return S_OK;
}

CInven_Panel* CInven_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInven_Panel* pInstance = new CInven_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CInven_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInven_Panel::Clone(void* pArg)
{
	CInven_Panel* pInstance = new CInven_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInven_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInven_Panel::Free()
{
	__super::Free();
}
