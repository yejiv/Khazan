#include "UI_ComBatSpirit_Slot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_ComBatSpirit_Gauge.h"

CUI_ComBatSpirit_Slot::CUI_ComBatSpirit_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Slot{ pDevice, pContext }
{
}

CUI_ComBatSpirit_Slot::CUI_ComBatSpirit_Slot(const CUI_ComBatSpirit_Slot& Prototype)
	: CUI_Slot(Prototype)
{
}

void CUI_ComBatSpirit_Slot::Update_Pos(_float2 vPos, _float fOffSetX, _int iMaxIndex)
{
	m_vWorldPos.x = vPos.x + m_iIndex * fOffSetX - (iMaxIndex -1) * fOffSetX / 2 ;
	m_vWorldPos.y = vPos.y;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
	__super::Update_Transform(nullptr, m_vWorldPos);
}

void CUI_ComBatSpirit_Slot::Update_Gauge(_float fValue)
{
	m_fCulGauge = fValue;
}

HRESULT CUI_ComBatSpirit_Slot::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT CUI_ComBatSpirit_Slot::Initialize_Clone(void* pArg)
{
	UISLOTDESC* pDesc = static_cast<UISLOTDESC*>(pArg);

	m_iIndex = pDesc->iIndex;
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;
	m_iState = ENUM_CLASS(UISTATE::ENABLE);
	return S_OK;
}

void CUI_ComBatSpirit_Slot::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_ComBatSpirit_Slot::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_ComBatSpirit_Slot::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_ComBatSpirit_Slot::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI_ComBatSpirit_Slot::Bubble_EventCall()
{
}

HRESULT CUI_ComBatSpirit_Slot::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	static_cast<CUI_CombatSpirit_Gauge*>(m_Children[0])->Setting_Gauge(&m_fCulGauge);
	return S_OK;
}

HRESULT CUI_ComBatSpirit_Slot::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(m_iLevel, TEXT("Prototype_Component_Hud_ComBatSpirit"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_ComBatSpirit_Slot* CUI_ComBatSpirit_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_ComBatSpirit_Slot* pInstance = new CUI_ComBatSpirit_Slot(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_ComBatSpirit_Slot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_ComBatSpirit_Slot::Clone(void* pArg)
{
	CUI_ComBatSpirit_Slot* pInstance = new CUI_ComBatSpirit_Slot(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_ComBatSpirit_Slot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ComBatSpirit_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}
