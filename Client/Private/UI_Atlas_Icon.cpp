#include "UI_Atlas_Icon.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Atlas_Icon::CUI_Atlas_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{pDevice, pContext}
{
}

CUI_Atlas_Icon::CUI_Atlas_Icon(const CUI_Atlas_Icon& Prototype)
	: CUI_Texture( Prototype )
{
}

void CUI_Atlas_Icon::Update_Color(_float4 vColor)
{
	m_vColor = vColor;
}

void CUI_Atlas_Icon::Set_Texture(_float4 vUV, _uint iTexPass)
{
	m_vUV[0] = vUV;
	m_iTexPass = iTexPass;
}

void CUI_Atlas_Icon::Set_Shader(_uint iShaderPass)
{
	m_iShaderPass = iShaderPass;
}

HRESULT CUI_Atlas_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Atlas_Icon::Initialize_Clone(void* pArg)
{
	UIATLASICON_DESC* pDesc = static_cast<UIATLASICON_DESC*>(pArg);

	m_vUV.push_back(pDesc->vUV);
	m_iShaderPass = pDesc->iShaderPass;
	m_iTexPass = pDesc->iTexPass;
	m_vColor = pDesc->vColor;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}
void CUI_Atlas_Icon::Priority_Update(_float fTimeDelta)
{
}

void CUI_Atlas_Icon::Update(_float fTimeDelta)
{
}

void CUI_Atlas_Icon::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
}

HRESULT CUI_Atlas_Icon::Render()
{
	return S_OK;
}

CUI_Atlas_Icon* CUI_Atlas_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Atlas_Icon* pInstance = new CUI_Atlas_Icon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Atlas_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Atlas_Icon::Clone(void* pArg)
{
	CUI_Atlas_Icon* pInstance = new CUI_Atlas_Icon(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Atlas_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Atlas_Icon::Free()
{
	__super::Free();
}
