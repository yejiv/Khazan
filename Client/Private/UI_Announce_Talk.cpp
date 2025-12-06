#include "UI_Announce_Talk.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Announce_Talk::CUI_Announce_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Text(pDevice, pContext)
{
}

CUI_Announce_Talk::CUI_Announce_Talk(const CUI_Announce_Talk& Prototype)
	: CUI_Text(Prototype)
{
}

void CUI_Announce_Talk::ShowUI(_int iTalkIndex)
{
	m_eState = UIAnimeStae::ON;
	m_fAlpha = 0.f;

	const ANNOUNCE_TALK_DB* TalkDB = CClientInstance::GetInstance()->Get_Data<ANNOUNCE_TALK_DB>(iTalkIndex);
	m_wstrTalkName = TalkDB->wstrName;
    m_wstrSoundName = TalkDB->wstrSoundName;
    m_iNextIndex = TalkDB->iNextIndex;

    if (m_iNextIndex == iTalkIndex)
        m_iNextIndex = 0;

	NameColor();

	m_wstrText_1 = TalkDB->wstrText_1;
	if (TalkDB->wstrText_2 != TEXT("-"))
	{
		m_wstrText_2 = TalkDB->wstrText_2;
		m_isOneLine = false;
	}
	else
		m_isOneLine = true;
}

HRESULT CUI_Announce_Talk::Initialize_Prototype(_int iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Announce_Talk::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	CHECK_FAILED(Ready_Component(), E_FAIL);

	Update_Transform(nullptr, m_vLocalPos);
	m_eTextAlign = TEXT_ALIGN::CENTER;

    m_pGameInstance->Subscribe_Event<EVENT_ANNOUNCE_TALK>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_TALK), [&](const EVENT_ANNOUNCE_TALK& e)
        { ShowUI(e.iTalkIndex); });

	m_vColor.w = 0.7f;

	m_wstrText_1 = TEXT("명계의 기운이 깃들었다.");
	m_wstrText_2 = TEXT("명계의 기운이 깃들었다.");

	return S_OK;
}

void CUI_Announce_Talk::Priority_Update(_float fTimeDelta)
{
}

void CUI_Announce_Talk::Update(_float fTimeDelta)
{
}

void CUI_Announce_Talk::Late_Update(_float fTimeDelta)
{
	if (m_eState == UIAnimeStae::END)
		return;

	if (m_eState == UIAnimeStae::ON)
	{
		m_fAlpha += fTimeDelta * 2.f;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
			m_eState = UIAnimeStae::SHOW;
            m_pGameInstance->PlaySoundOnce(m_wstrSoundName.c_str());
		}
	}
	else if (m_eState == UIAnimeStae::SHOW)
	{
		if (!m_pGameInstance->IsPlayingByKey(m_wstrSoundName.c_str()))
			m_eState = UIAnimeStae::OFF;
	}
	else if (m_eState == UIAnimeStae::OFF)
	{
		m_fAlpha -= fTimeDelta * 2.f;
		if (m_fAlpha <= 0.f)
		{
			m_fAlpha = 0.f;
			m_eState = UIAnimeStae::END;

            if (m_iNextIndex > 0)
                ShowUI(m_iNextIndex);
		}
	}

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CUI_Announce_Talk::Render()
{
	//m_isOneLine = true;
	//m_isOneLine = false;
	BG_RenderSetting();
	BG_Render();
	
	if (m_isOneLine)
		Font_TextOneLine();
	else
		Font_TextTwoLine();
	
	return S_OK;
}

void CUI_Announce_Talk::NameColor()
{
	if (m_wstrTalkName == TEXT("???") || m_wstrTalkName == TEXT("블레이드 팬텀"))
		m_vNameColor = { 0.686f, 0.280f, 0.373f, 1.f };

	if (m_wstrTalkName == TEXT("카잔"))
		m_vNameColor = { 0.863f, 0.780f, 0.638f, 1.f };
}

HRESULT CUI_Announce_Talk::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Announce_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/BG/T_AlertMessage_BG.png"), 1)), E_FAIL);

	return S_OK;
}

HRESULT CUI_Announce_Talk::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Font"),
		TEXT("Com_TextShader"), reinterpret_cast<CComponent**>(&m_pTextShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_Announce_BG"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	return S_OK;
}

void CUI_Announce_Talk::Setting_Text(const EVENT_ANNOUNCE_MAPNAME& e)
{

}

void CUI_Announce_Talk::BG_RenderSetting()
{
	if (m_isOneLine)
	{
		_int iLength = (_int)m_wstrText_1.length();
		Update_Transform(nullptr, { g_iWinSizeX >> 1, 800.f });
		m_vLocalSize = { 150.f + iLength * 25.f, 90.f };
		m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	}
	else
	{
		_int iLength = 0;
		m_wstrText_1.length() > m_wstrText_2.length() ? iLength = (_int)m_wstrText_1.length() : iLength = (_int)m_wstrText_2.length();
		Update_Transform(nullptr, { g_iWinSizeX >> 1, 790.f });
		m_vLocalSize = { 100.f + iLength * 35.f, 110.f };
		m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	}
}

HRESULT CUI_Announce_Talk::Font_TextOneLine()
{
	CHECK_FAILED(m_pTextShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	m_pTextShaderCom->Begin(0);

	m_pGameInstance->Draw_Text(TEXT("Blade_Medium_22"), m_wstrTalkName, m_vWorldPos.x, m_vWorldPos.y - 5, m_vNameColor, m_eTextAlign);
	m_pGameInstance->Draw_Text(TEXT("Blade_Medium_24"), m_wstrText_1, m_vWorldPos.x, m_vWorldPos.y + 35, { 1.f, 1.f, 1.f, 1.f }, m_eTextAlign);


	return E_NOTIMPL;
}

HRESULT CUI_Announce_Talk::Font_TextTwoLine()
{
	CHECK_FAILED(m_pTextShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	m_pTextShaderCom->Begin(0);

	m_pGameInstance->Draw_Text(TEXT("Blade_Medium_22"), m_wstrTalkName, m_vWorldPos.x, m_vWorldPos.y - 15, m_vNameColor, m_eTextAlign);
	m_pGameInstance->Draw_Text(TEXT("Blade_Medium_24"), m_wstrText_1, m_vWorldPos.x, m_vWorldPos.y + 25, { 1.f, 1.f, 1.f, 1.f }, m_eTextAlign);
	m_pGameInstance->Draw_Text(TEXT("Blade_Medium_24"), m_wstrText_2, m_vWorldPos.x, m_vWorldPos.y + 52, { 1.f, 1.f, 1.f, 1.f }, m_eTextAlign);

	return S_OK;
}

HRESULT CUI_Announce_Talk::BG_Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);

	CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(14);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

CUI_Announce_Talk* CUI_Announce_Talk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
	CUI_Announce_Talk* pInstance = new CUI_Announce_Talk(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Announce_Talk"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Announce_Talk::Clone(void* pArg)
{
	CUI_Announce_Talk* pInstance = new CUI_Announce_Talk(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Announce_Talk"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Announce_Talk::Free()
{
	__super::Free();

	Safe_Release(m_pTextShaderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

}
