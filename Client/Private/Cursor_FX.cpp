#include "Cursor_FX.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CCursor_FX::CCursor_FX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CCursor_FX::CCursor_FX(const CCursor_FX& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CCursor_FX::Initialize_Prototype()
{
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CCursor_FX::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_isPool = true;
	m_isDead = true;
	m_vColor = { 1.f, 1.f, 0.f, 0.5f };
	return S_OK;
}

void CCursor_FX::Priority_Update(_float fTimeDelta)
{

}

void CCursor_FX::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	m_fAlpha = 1.f - m_fAccTime;
	
	__super::Update_Scaling(1.f + m_fAccTime * 3.f);

	m_pTransformCom->Rotation({ 0.f, 0.f, -1.f, 0.f }, XMConvertToRadians(m_fAccTime * 45.f));
	if (m_fAccTime > 1.f)
		m_isDead = true;
}

void CCursor_FX::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CCursor_FX::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	
	m_pShaderCom->Begin(2);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CCursor_FX::Reset()
{
	m_fAccTime = 0.f;
	m_fAlpha = 1.f;
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	//__super::Update_Transform(nullptr, { ptMouse.x + m_vLocalSize.x * 0.5f, ptMouse.y + m_vLocalSize.y * 0.5f });
	__super::Update_Transform(nullptr, { (_float)ptMouse.x , (_float)ptMouse.y  });
}

HRESULT CCursor_FX::Ready_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor_FX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Cursor/Cursor_FX_%d.png"), 4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursor_FX::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Cursor"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor_FX"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

CCursor_FX* CCursor_FX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCursor_FX* pInstance = new CCursor_FX(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CCursor_FX"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCursor_FX::Clone(void* pArg)
{
	CCursor_FX* pInstance = new CCursor_FX(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CCursor_FX"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCursor_FX::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
