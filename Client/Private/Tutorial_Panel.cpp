#include "Tutorial_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CTutorial_Panel::CTutorial_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CTutorial_Panel::CTutorial_Panel(const CTutorial_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CTutorial_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorial_Panel::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);

	return S_OK;
}

void CTutorial_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CTutorial_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CTutorial_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

CTutorial_Panel* CTutorial_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorial_Panel* pInstance = new CTutorial_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTutorial_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTutorial_Panel::Clone(void* pArg)
{
	CTutorial_Panel* pInstance = new CTutorial_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTutorial_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_Panel::Free()
{
	__super::Free();
}
