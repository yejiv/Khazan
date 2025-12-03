#include "UI_Default_Tex.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Default_Tex::CUI_Default_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CUI_Default_Tex::CUI_Default_Tex(const CUI_Default_Tex& Prototype)
	: CUI_Texture(Prototype)
{
}

HRESULT CUI_Default_Tex::Set_Texture(const _wstring& strPrototypeTag, _int iTexPass)
{
	m_iTexPass = iTexPass;
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), strPrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return E_NOTIMPL;
}

void CUI_Default_Tex::Tex_Scaling(_float fSizeX, _float fSizeY)
{
    m_pTransformCom->Scale(_float3{ m_vLocalSize.x * fSizeX, m_vLocalSize.y * fSizeY, 1.f });
}

void CUI_Default_Tex::Set_PosX(_float fPosX, CUIObject* pParent)
{
    m_vLocalPos.x = fPosX;

    Update_Transform(pParent, m_vLocalPos);
}

HRESULT CUI_Default_Tex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Default_Tex::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}
void CUI_Default_Tex::Priority_Update(_float fTimeDelta)
{
}

void CUI_Default_Tex::Update(_float fTimeDelta)
{
}

void CUI_Default_Tex::Late_Update(_float fTimeDelta)
{
	if(m_isVisible)
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Default_Tex::Render()
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
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Default_Tex::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{

	string strTexTag = pInData.value("TexTag", "");
	wstring wstrTexTag = AnsiToWString(strTexTag);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Default_Tex::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CUI_Default_Tex* CUI_Default_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Default_Tex* pInstance = new CUI_Default_Tex(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Default_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Default_Tex::Clone(void* pArg)
{
	CUI_Default_Tex* pInstance = new CUI_Default_Tex(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Default_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Default_Tex::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

}
