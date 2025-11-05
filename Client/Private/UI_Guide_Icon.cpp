#include "UI_Guide_Icon.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Guide_Icon::CUI_Guide_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Text(pDevice, pContext)
{
}

CUI_Guide_Icon::CUI_Guide_Icon(const CUI_Guide_Icon& Prototype)
	: CUI_Text(Prototype)
{
}

void CUI_Guide_Icon::Set_LocalPos(_float2 vPos, CUIObject* pParent)
{
	m_vLocalPos = vPos;
	Update_Transform(pParent, vPos);
}

void CUI_Guide_Icon::Set_Pos(_float2 vPos)
{
	m_vWorldPos = vPos;
	Update_Transform(nullptr, m_vWorldPos);
}

void CUI_Guide_Icon::Update_Color(_float4 vColor)
{
	m_vColor = vColor;
}

void CUI_Guide_Icon::Set_Texture(_float4 vUV, _uint iTexPass)
{
	m_vUV[0] = vUV;
	m_iTexPass = iTexPass;
}

void CUI_Guide_Icon::Set_Shader(_uint iShaderPass)
{
	m_iShaderPass = iShaderPass;
}

HRESULT CUI_Guide_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Guide_Icon::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CUI_Guide_Icon::Priority_Update(_float fTimeDelta)
{
}

void CUI_Guide_Icon::Update(_float fTimeDelta)
{
}

void CUI_Guide_Icon::Late_Update(_float fTimeDelta)
{
	if (!m_isVisible)
		return;
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Guide_Icon::Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	m_pShaderCom->Begin(m_iShaderPass);

	m_pGameInstance->Draw_Text(m_wstrTexttag, m_wstrText, m_vWorldPos.x + m_iPivotX, m_vWorldPos.y + m_iPivotY, m_vColor, m_eTextAlign);

	return S_OK;
}

HRESULT CUI_Guide_Icon::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_Guide_Icon* CUI_Guide_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Guide_Icon* pInstance = new CUI_Guide_Icon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Guide_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Guide_Icon::Clone(void* pArg)
{
	CUI_Guide_Icon* pInstance = new CUI_Guide_Icon(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Guide_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Guide_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
