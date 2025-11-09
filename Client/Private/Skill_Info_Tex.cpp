#include "Skill_Info_Tex.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"

CSkill_Info_Tex::CSkill_Info_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CSkill_Info_Tex::CSkill_Info_Tex(const CSkill_Info_Tex& Prototype)
	: CUI_Texture(Prototype)
{
}

HRESULT CSkill_Info_Tex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Info_Tex::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_vColor = { 1.f, 1.f, 1.f, 1.f };
	return S_OK;
}

void CSkill_Info_Tex::Priority_Update(_float fTimeDelta)
{
}

void CSkill_Info_Tex::Update(_float fTimeDelta)
{
	
}

void CSkill_Info_Tex::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    m_pIcon->Late_Update(fTimeDelta);
}

HRESULT CSkill_Info_Tex::Render()
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
	CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

	//if (m_isMovie)
	//	CHECK_FAILED(m_pMovieCom[m_eMovieType]->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);
	//else
	CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CSkill_Info_Tex::Update_Transform(CUIObject* pParent, _float2 vPos)
{
    __super::Update_Transform(pParent, vPos);
    m_pIcon->Update_Transform(this, vPos);
}

HRESULT CSkill_Info_Tex::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/BG/Black_Bg.png"), 1);
	//CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuidePage"),
	//	TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);
    
    CUI_Atlas_Icon::UIATLASICON_DESC AtlasDesc;

    AtlasDesc.fDepth = 1.f;
    AtlasDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    AtlasDesc.szName = "Icon";
    AtlasDesc.vLocalPos = { 0.f, 0.f };
    AtlasDesc.vLocalSize = { 128.f, 128.f };
    AtlasDesc.vUV = CClientInstance::GetInstance()->Get_AtlasUV("Loading_2.png", 4);
    AtlasDesc.iShaderPass = 2;
    AtlasDesc.iTexPass = 4;
    AtlasDesc.vColor = { 0.9f,0.9f,0.9f,0.8f };
    m_pIcon = static_cast<CUI_Atlas_Icon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Atlas_Icon"), &AtlasDesc));

	return S_OK;
}

CSkill_Info_Tex* CSkill_Info_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Info_Tex* pInstance = new CSkill_Info_Tex(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSkill_Info_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkill_Info_Tex::Clone(void* pArg)
{
	CSkill_Info_Tex* pInstance = new CSkill_Info_Tex(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSkill_Info_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Info_Tex::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	for (auto Movie : m_pMovieCom)
		Safe_Release(Movie);
	m_pMovieCom.clear();

    Safe_Release(m_pIcon);
}
