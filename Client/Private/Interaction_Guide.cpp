#include "Interaction_Guide.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Guide_Gauge.h"
#include "Interaction_Icon.h"

#include "UI_TextBox.h"

CInteraction_Guide::CInteraction_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel(pDevice, pContext)
{
}

CInteraction_Guide::CInteraction_Guide(const CInteraction_Guide& Prototype)
	: CUI_Panel(Prototype)
{
}

void CInteraction_Guide::Setting_Guide(GUIDE_TYPE eType, const _float4x4* pTagetMat, _float2 vOffset, _wstring strText, _float fDelayTime)
{
	m_eGuideType = eType;
	m_fDelayTime = fDelayTime;

	m_pTagetMat = pTagetMat;
	m_vLocalPos = { vOffset.x, -vOffset.y };

	m_pTextBox->Set_Text(strText);
}

_bool CInteraction_Guide::IsPressing()
{
	if (m_eGuideType == GUIDE_TYPE::DEFAULT)
		return true;

	m_bIsFiash = false;
	m_isPressing = true;
	m_bPrePressingState = m_isPressing;
	m_pIcon->Anim_Start(true);

	if (m_fAccTime >= m_fDelayTime)
		m_bIsFiash = true;

	return m_bIsFiash;
}

HRESULT CInteraction_Guide::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CInteraction_Guide::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	m_vUV.resize(1);
	m_iTexPass = 3;
	m_iShaderPass = 0;
	m_vColor = { 1.f, 1.f, 1.f, 1.f };
	m_isPool = true;

	m_fAlpha = 1.f;
	m_vUV[0] = CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3);

	m_fAccTime = 0.f;
	m_fDelayTime = 3.f;
	CHECK_FAILED(Ready_Children(), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);

	return S_OK;
}

void CInteraction_Guide::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_Guide::Update(_float fTimeDelta)
{
	if (m_eGuideType == GUIDE_TYPE::DEFAULT)
		return;

	if (m_bPrePressingState != m_isPressing)
	{
		m_fAccTime = 0.f;
		m_bPrePressingState = m_isPressing;
		m_pIcon->Anim_Start(false);
	}

	if (m_isPressing)
		m_fAccTime += fTimeDelta;

	m_isPressing = false;

	__super::Update(fTimeDelta);
    
}

void CInteraction_Guide::Late_Update(_float fTimeDelta)
{
    Update_WorldPos();

	if (!m_isVisible || !m_isActive)
		return;
	
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    if (m_eGuideType == GUIDE_TYPE::DEFAULT)
		m_pTextBox->Late_Update(fTimeDelta);
	else
		__super::Late_Update(fTimeDelta);

}

HRESULT CInteraction_Guide::Render()
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

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

	return S_OK;
}

void CInteraction_Guide::Reset()
{
	m_fAlpha = 1.f;
	m_vLocalPos = {};
}

HRESULT CInteraction_Guide::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_F_Key_Icon"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Atlas/T_Icon_KB_F.png"), 1))))
        return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Tex_Guide_Press"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Common/Guide_Press.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Guide_Gauge"),
		CUI_Guide_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Interaction_Icon"),
		CInteraction_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteraction_Guide::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Tex_F_Key_Icon"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInteraction_Guide::Ready_Children()
{
	CUIObject::UIOBJECT_DESC TextDesc = {};
	TextDesc.fDepth = m_fDepth;
	TextDesc.iUIType = ENUM_CLASS(UITYPE::TEXT);
	TextDesc.szName = "Text";
	TextDesc.vLocalPos = _float2{ 26.f, 20.f };
	TextDesc.vLocalSize = { m_vLocalSize.x, m_vLocalSize.y };
	TextDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_pTextBox = static_cast<CUI_TextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_TextBox"), &TextDesc));

	if (m_pTextBox == nullptr)
		return E_FAIL;
	m_Children.push_back(m_pTextBox);
	Safe_AddRef(m_pTextBox);

	CUI_TextBox::TEXTBOX_DESC TextSet = {};
	TextSet.bIsTextBox = false;
	TextSet.eTextAlign = TEXT_ALIGN::LEFT_CENTER;
	TextSet.fMaxWidth = 0;
	TextSet.fOffsetHeight = 0;
	TextSet.iPivotX = 0;
	TextSet.iPivotY = 0;
	TextSet.wstrTexttag = TEXT("Blade_Medium_22");
	TextSet.wstrText = TEXT("");
	TextSet.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_pTextBox->Setting_Text(TextSet);
	
	//가이드 아이콘

	CUIObject::UIOBJECT_DESC IconDesc = {};
	IconDesc.fDepth = m_fDepth;
	IconDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	IconDesc.szName = "Icon";
	IconDesc.vLocalPos = _float2{ 0.f, -m_vLocalSize.y * 0.5f - 7.f };
	IconDesc.vLocalSize = { 16.f, 16.f };
	IconDesc.vColor = { 1.f, 1.f, 1.f, 1.f };

	m_pIcon = static_cast<CInteraction_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_Interaction_Icon"), &IconDesc));

	if (m_pIcon == nullptr)
		return E_FAIL;

	m_Children.push_back(m_pIcon);
	Safe_AddRef(m_pIcon);

	//가이드 게이지
	CUI_Guide_Gauge::GUIDEGAUGE_DESC GuideDesc = {};
	GuideDesc.fDepth = m_fDepth;
	GuideDesc.iUIType = ENUM_CLASS(UITYPE::PROGRESSBAR);
	GuideDesc.szName = "Gauge";
	GuideDesc.vLocalPos = _float2{ 0.f, 0.f };
	GuideDesc.vLocalSize = { m_vLocalSize.x + 4.f, m_vLocalSize.y + 4.f };
	GuideDesc.vColor = { 1.f, 1.f, 1.f, 1.f };
	GuideDesc.pCulValue = &m_fAccTime;
	GuideDesc.pMaxValue = &m_fDelayTime;

	m_pGauge = static_cast<CUI_Guide_Gauge*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_Guide_Gauge"), &GuideDesc));

	if (m_pGauge == nullptr)
		return E_FAIL;
	m_Children.push_back(m_pGauge);
	Safe_AddRef(m_pGauge);

	return S_OK;
}

void CInteraction_Guide::Update_WorldPos()
{
	_float4 vTemp = _float4{ m_pTagetMat->m[3] };
	_vector vTargetPos = XMLoadFloat4(&vTemp);

	vTemp = CClientInstance::GetInstance()->Get_ActiveCameraLook();
	_vector vCamLook = XMVector3Normalize(XMLoadFloat4(&vTemp));

	_float3 vDest = CClientInstance::GetInstance()->Get_ActiveCameraPos();
	_vector vCamPos = XMLoadFloat3(&vDest);

	_vector vDir = XMVector3Normalize(vTargetPos - vCamPos);

	_float fDot = XMVectorGetX(XMVector3Dot(vCamLook, vDir));


	if (fDot <= 0)
	{
		m_isActive = false;
		return;
	}
	else
		m_isActive = true;

	_matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	_matrix vVPMatrix = OldVeiw * OldProj;
	_vector vWinPos = XMVector3TransformCoord(vTargetPos, vVPMatrix);

	_float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
	_float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

	m_vWorldPos.x = fWinPosX + m_vLocalPos.x;
	m_vWorldPos.y = fWinPosY + m_vLocalPos.y;

	if (m_vWorldPos.x < 0)
		m_vWorldPos.x = 0;
	else if (m_vWorldPos.x > g_iWinSizeX)
		m_vWorldPos.x = g_iWinSizeX;

	if (m_vWorldPos.y < 0)
		m_vWorldPos.y = 0;
	else if (m_vWorldPos.y > g_iWinSizeY)
		m_vWorldPos.y = g_iWinSizeY;


	Update_Transform(nullptr, m_vWorldPos);
}

CInteraction_Guide* CInteraction_Guide::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInteraction_Guide* pInstance = new CInteraction_Guide(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CInteraction_Guide"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteraction_Guide::Clone(void* pArg)
{
	CInteraction_Guide* pInstance = new CInteraction_Guide(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInteraction_Guide"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInteraction_Guide::Free()
{
	__super::Free();
	Safe_Release(m_pGauge);
	Safe_Release(m_pTextBox);
	Safe_Release(m_pIcon);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
