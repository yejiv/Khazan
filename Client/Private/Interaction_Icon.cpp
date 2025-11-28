#include "Interaction_Icon.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CInteraction_Icon::CInteraction_Icon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CInteraction_Icon::CInteraction_Icon(const CInteraction_Icon& Prototype)
	: CUI_Texture(Prototype)
{
}

void CInteraction_Icon::Anim_Start(_bool isStart)
{
	m_isAnim = isStart;

	if (m_isAnim)
		return;

	m_vLocalPos = m_vDefaultPos;
	m_fAlpha = 1.f;
	
}

HRESULT CInteraction_Icon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Icon::Initialize_Clone(void* pArg)
{
	m_iShaderPass = 1;

	m_vColor = { 1.f,1.f,1.f,1.f };
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vDefaultPos = m_vLocalPos;
	return S_OK;
}

void CInteraction_Icon::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_Icon::Update(_float fTimeDelta)
{
}

void CInteraction_Icon::Late_Update(_float fTimeDelta)
{
    if (m_isAnim)
    {
        m_vLocalPos.y = m_vLocalPos.y + fTimeDelta * 20.f;
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
        m_fAlpha -= fTimeDelta * 2.f;
    }
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CInteraction_Icon::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CInteraction_Icon::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Guide_Press"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CInteraction_Icon* CInteraction_Icon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInteraction_Icon* pInstance = new CInteraction_Icon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CInteraction_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteraction_Icon::Clone(void* pArg)
{
	CInteraction_Icon* pInstance = new CInteraction_Icon(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInteraction_Icon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInteraction_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
