#include "UI_Tutorial.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Tutorial_Panel.h"
#include "Tutorial_Tex.h"

#include "UI_BackGround.h"
CUI_Tutorial::CUI_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_Tutorial::CUI_Tutorial(const CUI_Tutorial& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_Tutorial::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);
	return S_OK;
}

HRESULT CUI_Tutorial::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	CHECK_FAILED(Ready_Component(), E_FAIL);
	return S_OK;
}

void CUI_Tutorial::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Tutorial::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Tutorial::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Tutorial::Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
	CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
	CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CUI_Tutorial::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
	__super::Load_UI(pInData, iPrototypeLevelID, pArg);

	m_iShaderPass = 15;
	return S_OK;
}

void CUI_Tutorial::Bubble_EventCall(BUBBLEEVENT* pArg)
{

}

HRESULT CUI_Tutorial::Update_Switch(void* pArg)
{
	return S_OK;
}

HRESULT CUI_Tutorial::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Tutorial_Panel"),
		CTutorial_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Tutorial_Tex"),
		CTutorial_Tex::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_GuidePage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/GuidePage_%d.dds"), 6)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_GuideTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/GuideTex_%d.dds"), 3)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_BrutalAttack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/BrutalAttack/BrutalAttack_%d.dds"), 162)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_CountAttack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/CountAttack/CountAttack_%d.dds"), 152)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_Dodge_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Dodge_01/Dodge_01_%d.dds"), 58)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_Dodge_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Dodge_02/Dodge_02_%d.dds"), 90)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_Guard_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Guard_01/Guard_01_%d.dds"), 372)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_Guard_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Guard_02/Guard_02_%d.dds"), 138)), E_FAIL);


	return S_OK;
}

HRESULT CUI_Tutorial::Ready_Object()
{
	UIOBJECT_DESC Desc = {};
	Desc.fDepth = 5.5f;
	Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	Desc.szName = "BackGround";
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
	if (m_pBackGround == nullptr)
		return E_FAIL;
	m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::LOADING);
	m_Children.push_back(m_pBackGround);
	Safe_AddRef(m_pBackGround);

	return S_OK;

}

HRESULT CUI_Tutorial::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuideTex"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	return S_OK;
}

CUI_Tutorial* CUI_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Tutorial* pInstance = new CUI_Tutorial(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Tutorial"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Tutorial::Clone(void* pArg)
{
	CUI_Tutorial* pInstance = new CUI_Tutorial(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Tutorial"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Tutorial::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBackGround);
}
