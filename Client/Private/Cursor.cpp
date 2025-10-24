#include "Cursor.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Cursor_FX.h"

CCursor::CCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{pDevice, pContext}
{
}

CCursor::CCursor(const CCursor& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CCursor::Initialize_Prototype()
{
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CCursor::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_eMode = MOUSE_MODE::UI_MODE;

	CUIObject::UIOBJECT_DESC Desc = {};
	Desc.vLocalSize = { 20.f, 20.f };
	Desc.vLocalPos = { 0.f, 0.f };
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "Cursor_FX";
	Desc.fDepth = 0.5f;

	if (FAILED(m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor_FX"),
		TEXT("Pool_Cursor_FX"), &Desc, 20)))
		return E_FAIL;

	return S_OK;
}

void CCursor::Priority_Update(_float fTimeDelta)
{
	if (m_eMode == MOUSE_MODE::PLAY_MODE)
		Play_Mode();
	else if (m_eMode == MOUSE_MODE::UI_MODE)
		UI_Mode();
}

void CCursor::Update(_float fTimeDelta)
{
	m_IsPressing = false;
	if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB, INPUT_TYPE::GAMEPLAY))
		m_IsPressing = true;

	if (m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::LB, INPUT_TYPE::GAMEPLAY))
	{
		CGameObject* pFX = m_pGameInstance->Pop_PoolObject(TEXT("Pool_Cursor_FX"));
		if (pFX != nullptr)
		{
			m_pGameInstance->Reset_PoolObject(pFX);
			m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pFX);
		}
	}
	if(!m_IsDefalutMouse)
		ShowCursor(FALSE);
}

void CCursor::Late_Update(_float fTimeDelta)
{
	if (m_eMode == MOUSE_MODE::UI_MODE)
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CCursor::Render()
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


	m_IsPressing ? m_pShaderCom->Begin(1) : m_pShaderCom->Begin(0);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCursor::Ready_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Cursor/UI_Cursor.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Cursor_FX"),
		CCursor_FX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCursor::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Cursor"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Cursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CCursor::Play_Mode()
{
	RECT clientRect;
	POINT centerPoint;

	GetClientRect(g_hWnd, &clientRect);
	centerPoint.x = (clientRect.right - clientRect.left) / 2;
	centerPoint.y = (clientRect.bottom - clientRect.top) / 2;

	ClientToScreen(g_hWnd, &centerPoint);

	SetCursorPos(centerPoint.x, centerPoint.y);
}

void CCursor::UI_Mode()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	__super::Update_Transform(nullptr, { ptMouse.x + m_vLocalSize.x * 0.5f -7.f, ptMouse.y + m_vLocalSize.y * 0.5f });
}

CCursor* CCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCursor* pInstance = new CCursor(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CCursor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCursor::Clone(void* pArg)
{
	CCursor* pInstance = new CCursor(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CCursor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCursor::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
