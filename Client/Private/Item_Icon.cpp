#include "Item_Icon.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Item_Slot.h"

CItem_Icon::CItem_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CItem_Icon::CItem_Icon(const CItem_Icon& Prototype)
    : CUI_Panel( Prototype )
{
}

HRESULT CItem_Icon::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CItem_Icon::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;
}

void CItem_Icon::Priority_Update(_float fTimeDelta)
{
}

void CItem_Icon::Update(_float fTimeDelta)
{
}

void CItem_Icon::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
}

HRESULT CItem_Icon::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Item_Slot"),
		CItem_Slot::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

	return S_OK;
}

HRESULT CItem_Icon::Ready_Childer()
{
	CUIObject* pObject = { nullptr };
	CUIObject::UIOBJECT_DESC Desc;
	Desc.fDepth = m_fDepth;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "Item_Slot";
	Desc.vLocalPos = _float2{ 0.f, 0.f };
	Desc.vLocalSize = m_vLocalSize;

	pObject = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Item_Slot"), &Desc));

	if (pObject == nullptr)
		return E_FAIL;

	m_Children.push_back(pObject);
	return S_OK;
}

CItem_Icon* CItem_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CItem_Icon* pInstance = new CItem_Icon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CItem_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItem_Icon::Clone(void* pArg)
{
	CItem_Icon* pInstance = new CItem_Icon(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CItem_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_Icon::Free()
{
    __super::Free();
}
