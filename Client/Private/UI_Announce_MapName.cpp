#include "UI_Announce_MapName.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Announce_MapName::CUI_Announce_MapName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Text(pDevice, pContext)
{
}

CUI_Announce_MapName::CUI_Announce_MapName(const CUI_Announce_MapName& Prototype)
	: CUI_Text(Prototype)
{
}

HRESULT CUI_Announce_MapName::Initialize_Prototype(_int iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Announce_MapName::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	CHECK_FAILED(Ready_Component(), E_FAIL);

	Update_Transform(nullptr, m_vLocalPos);
	m_eTextAlign = TEXT_ALIGN::CENTER;
	m_fDepth = 10.f;
	m_vColor = { 1.f, 1.f, 1.f, 1.f };
  	m_pGameInstance->Subscribe_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), [&](const EVENT_ANNOUNCE_MAPNAME& e)
		{ Setting_Text(e);});

	return S_OK;
}

void CUI_Announce_MapName::Priority_Update(_float fTimeDelta)
{
}

void CUI_Announce_MapName::Update(_float fTimeDelta)
{
	EVENT_ANNOUNCE_MAPNAME Desc = {};
	Desc.fTime = 2.f;
	Desc.iMapType = 0;
	Desc.fFadeOutTime = 2.5f;
	if(m_pGameInstance->Key_Down(DIK_N))
		m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);
}

void CUI_Announce_MapName::Late_Update(_float fTimeDelta)
{
	m_iPivotY = 180.f;
	if (m_fAccTime >= 0.f)
	{
		m_fAccTime -= fTimeDelta;
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	}
	else if (m_fFadeTime >= 0.f)
	{
		m_fFadeTime -= fTimeDelta;
		_float fTime = (m_fFadeTime / m_fFadeDuration);
		fTime = clamp(fTime, 0.0f, 1.0f);
		if (m_isDissovle)
			m_fDissolveTime = fTime;
		else
			m_fAlpha = fTime;
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	}

}

HRESULT CUI_Announce_MapName::Render()
{
	if (m_isDissovle)
	{
		CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissovleTexture", 3), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDissovle", &m_fDissolveTime, sizeof(_float)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDissovleTexAspect", &m_fDissovleAspect, sizeof(_float)), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	}
	else
		CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fTexAspect", &m_fTexAspect, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", m_iTexPass), E_FAIL);

	if (m_isDissovle)
		m_pShaderCom->Begin(2);
	else
		m_pShaderCom->Begin(m_iShaderPass);
	
	m_pGameInstance->Draw_Text(m_wstrTexttag, m_wstrText, m_vWorldPos.x + m_iPivotX, m_vWorldPos.y + m_iPivotY, m_vColor, m_eTextAlign);
	return S_OK;
}

HRESULT CUI_Announce_MapName::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Font_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Font/Mask/T_Img_TextGrungeTex_%d.png"), 3)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Font_Dissovle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Font/Mask/T_Fx_%d.png"), 5)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Announce_MapName::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);
		
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Font_Mask"),
		TEXT("Com_MaskTex"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr), E_FAIL);
	
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Font_Dissovle"),
		TEXT("Com_DissoveTex"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);


	return S_OK;
}

void CUI_Announce_MapName::Setting_Text(const EVENT_ANNOUNCE_MAPNAME& e)
{
	MAP_TYPE eType = static_cast<MAP_TYPE>(e.iMapType);

	if (MAP_TYPE::HEINMACH == eType)
	{
		m_fFadeTime = e.fFadeOutTime;
		m_fFadeDuration = e.fFadeOutTime;
		m_fAccTime = e.fTime;

		m_iTexPass = 1;
		m_iShaderPass = 1;
		m_fDissolveTime = 1.f;
		m_fAlpha = 1.f;
		m_wstrText = TEXT("HEINMACH");
		m_wstrTexttag = TEXT("Blade_Bold_250");
		m_fTexAspect = { 720.f / 100.f };
		m_fDissovleAspect = { 256.f/ 68.f };

		m_isDissovle = true;
	}
}

CUI_Announce_MapName* CUI_Announce_MapName::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
	CUI_Announce_MapName* pInstance = new CUI_Announce_MapName(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Announce_MapName"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Announce_MapName::Clone(void* pArg)
{
	CUI_Announce_MapName* pInstance = new CUI_Announce_MapName(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Announce_MapName"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Announce_MapName::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
}
