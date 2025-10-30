#include "UI_PlayerHP_Gauge.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "UI_Gague_Tip.h"

CUI_PlayerHP_Gauge::CUI_PlayerHP_Gauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_ProgressBar { pDevice, pContext }
{
}

CUI_PlayerHP_Gauge::CUI_PlayerHP_Gauge(const CUI_PlayerHP_Gauge& Prototype)
    : CUI_ProgressBar ( Prototype )
{
}

HRESULT CUI_PlayerHP_Gauge::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
    return S_OK;
}

HRESULT CUI_PlayerHP_Gauge::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

	m_fMaxValue = 100;
	m_fCurrentValue = 100;
	m_fLerpSpeed = 0.5f;
	m_fLerpValue = 1.f;
	m_fDeleyTime = 0.5f;
	return S_OK;
}

void CUI_PlayerHP_Gauge::Priority_Update(_float fTimeDelta)
{
}

void CUI_PlayerHP_Gauge::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_O))
		m_fCurrentValue -=30.f;
	if (m_pGameInstance->Key_Pressing(DIK_P, 0))
		m_fCurrentValue += fTimeDelta * 100.f;

	if (m_fCurrentValue < 0)
		m_fCurrentValue = 0;
	if (m_fCurrentValue > m_fMaxValue)
		m_fCurrentValue = m_fMaxValue;

	Update_Deley(fTimeDelta);

	_float2 vPos = {};
	vPos.x = m_vWorldPos.x - (m_vLocalSize.x * 0.5f) + (m_vLocalSize.x * m_fProgress_Value);
	vPos.y = m_vWorldPos.y;

	m_pUITip->Update_Pos(vPos);
}

void CUI_PlayerHP_Gauge::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	if(m_fProgress_Value < 1.f && m_fProgress_Value > 0.05f)
		m_pUITip->Late_Update(fTimeDelta);
}

HRESULT CUI_PlayerHP_Gauge::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	_float2 vValue = { m_fProgress_Value , m_fLerpValue };
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fProgressValue", &vValue, sizeof(_float2)), E_FAIL);

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_PlayerHP_Gauge::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	string strTexType = pInData.value("TexType", "");

	if ("Tex" == strTexType)
	{
		string strTexTag = pInData.value("TexTag", "");
		wstring wstrTexTag = AnsiToWString(strTexTag);

		if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
			return E_FAIL;
	}


	if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerHP_Gauge::Update_Alpha(_float fAlpha)
{
	__super::Update_Alpha(fAlpha);
	m_pUITip->Update_Alpha(fAlpha);
}

HRESULT CUI_PlayerHP_Gauge::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_PlayerHP_Gauge::Ready_Children()
{
	CUIObject::UIOBJECT_DESC Desc;
	Desc.fDepth = m_fDepth;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "Tip";
	Desc.vLocalPos = _float2{ 0.f, 0.f };
	Desc.vLocalSize = _float2{ 90.f, m_vLocalSize.y };

	m_pUITip = static_cast<CUI_Gague_Tip*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_Gague_Tip"), &Desc));
	
	if (m_pUITip == nullptr)
		return E_FAIL;

	return S_OK;
}

void CUI_PlayerHP_Gauge::Update_Deley(_float fTimeDelta)
{
	if (m_fCurrentValue < m_fPreValue && !m_isDelay)
	{
		m_isDelay = true;
		m_fAccTime = m_fDeleyTime;
	}
	Progress_Update();
	if (m_fAccTime > 0.f)
	{
		m_fAccTime -= fTimeDelta;
	}
	else
	{
		__super::Update(fTimeDelta);
		if (m_fLerpValue <= m_fProgress_Value)
			m_isDelay = false;
	}
}

CUI_PlayerHP_Gauge* CUI_PlayerHP_Gauge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_PlayerHP_Gauge* pInstance = new CUI_PlayerHP_Gauge(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_PlayerHP_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_PlayerHP_Gauge::Clone(void* pArg)
{
	CUI_PlayerHP_Gauge* pInstance = new CUI_PlayerHP_Gauge(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_PlayerHP_Gauge"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_PlayerHP_Gauge::Free()
{
	Safe_Release(m_pUITip);
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
