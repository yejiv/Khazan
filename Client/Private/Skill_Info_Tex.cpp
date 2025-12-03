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

void CSkill_Info_Tex::Setting_Tex(_int iSkillIndex)
{
    m_fDeltaTime = 0.f;
    m_isMovie = true;
    m_iTexPass = 0;
    m_pIcon->Update_Visible(false);
#ifndef _DEBUG
    if (iSkillIndex == 110)
        m_eMovieType = MOVIE_TYPE::Assault;
    else if (iSkillIndex == 104)
        m_eMovieType = MOVIE_TYPE::FullMoon;
    else if (iSkillIndex == 101)
        m_eMovieType = MOVIE_TYPE::MoonLight;
    else if (iSkillIndex == 102)
        m_eMovieType = MOVIE_TYPE::MoonlightStance;
    else if (iSkillIndex == 111)
        m_eMovieType = MOVIE_TYPE::QuickSlash;
    else if (iSkillIndex == 105)
        m_eMovieType = MOVIE_TYPE::ShadowSlash;
    else if (iSkillIndex == 114)
        m_eMovieType = MOVIE_TYPE::ShadowStrike;
    else if (iSkillIndex == 106)
        m_eMovieType = MOVIE_TYPE::SpiralThrust;
    else if (iSkillIndex == 113)
        m_eMovieType = MOVIE_TYPE::VitalStrike;
    else if (iSkillIndex == 107)
        m_eMovieType = MOVIE_TYPE::Thrust_Vortex;
    else if (iSkillIndex == 211)
        m_eMovieType = MOVIE_TYPE::Breakthrough;
    else if (iSkillIndex == 202)
        m_eMovieType = MOVIE_TYPE::BreathTaking;
    else if (iSkillIndex == 204)
        m_eMovieType = MOVIE_TYPE::Bloodshed;
    else if (iSkillIndex == 203)
        m_eMovieType = MOVIE_TYPE::Embryonic;
    else if (iSkillIndex == 214)
        m_eMovieType = MOVIE_TYPE::BridleOfBattle;
    else if (iSkillIndex == 206)
        m_eMovieType = MOVIE_TYPE::GiantHunt;
    else if (iSkillIndex == 213)
        m_eMovieType = MOVIE_TYPE::InnerFury;
    else if (iSkillIndex == 209)
        m_eMovieType = MOVIE_TYPE::LimitBreak;
    else if (iSkillIndex == 205)
        m_eMovieType = MOVIE_TYPE::ManifestStrength;
    else if (iSkillIndex == 201)
        m_eMovieType = MOVIE_TYPE::Momentum;
    else if (iSkillIndex == 208)
        m_eMovieType = MOVIE_TYPE::Phantom;
    else if (iSkillIndex == 212)
        m_eMovieType = MOVIE_TYPE::WarCry;
    else
#endif
    {
        m_pIcon->Update_Visible(true);
        m_isMovie = false;
        return;
    }

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
    if (m_isMovie)
    {
        m_fDeltaTime += fTimeDelta * 55.f;
        if (m_fDeltaTime >= 1.f)
        {
            m_fDeltaTime = 0.f;
            ++m_iTexPass;
        }
        if (m_iTexPass >= (_int)m_pMovieCom[m_eMovieType]->Get_NumTextures())
            m_iTexPass = 0;
    }
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

	if (m_isMovie)
		CHECK_FAILED(m_pMovieCom[m_eMovieType]->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);
	else
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

#ifndef _DEBUG
    m_pMovieCom.resize(MOVIE_TYPE::END);
    m_pMovieCom[MOVIE_TYPE::Assault] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_Assault/MV_Spear_Assault_%d.dds"), 133);
    m_pMovieCom[MOVIE_TYPE::FullMoon] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_FullMoon/MV_Spear_FullMoon_%d.dds"), 159);
    m_pMovieCom[MOVIE_TYPE::MoonLight] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_MoonLight/MV_Spear_MoonLight_%d.dds"), 142);
    m_pMovieCom[MOVIE_TYPE::MoonlightStance] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_MoonlightStance/MV_Spear_MoonlightStance_%d.dds"), 276);
    m_pMovieCom[MOVIE_TYPE::QuickSlash] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_QuickSlash/MV_Spear_QuickSlash_%d.dds"), 78);
    m_pMovieCom[MOVIE_TYPE::ShadowSlash] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_ShadowSlash/MV_Spear_ShadowSlash_%d.dds"), 166);
    m_pMovieCom[MOVIE_TYPE::ShadowStrike] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_ShadowStrike/MV_Spear_ShadowStrike_%d.dds"), 151);
    m_pMovieCom[MOVIE_TYPE::SpiralThrust] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_SpiralThrust/MV_Spear_SpiralThrust_%d.dds"), 128);
    m_pMovieCom[MOVIE_TYPE::VitalStrike] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/MV_Spear_VitalStrike/MV_Spear_VitalStrike_%d.dds"), 119);
    m_pMovieCom[MOVIE_TYPE::Thrust_Vortex] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/Spear/Thrust_Vortex/Thrust_Vortex_%d.dds"), 165);

    m_pMovieCom[MOVIE_TYPE::Breakthrough] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_Breakthrough/MV_Kazan_GSword_Breakthrough_%d.dds"), 187);
    m_pMovieCom[MOVIE_TYPE::BreathTaking] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_BreathTaking/MV_Kazan_GSword_BreathTaking_%d.dds"), 135);
    m_pMovieCom[MOVIE_TYPE::Bloodshed] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_BreathTaking_Bloodshed/MV_Kazan_GSword_BreathTaking_Bloodshed_%d.dds"), 219);
    m_pMovieCom[MOVIE_TYPE::Embryonic] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_BreathTaking_Embryonic/MV_Kazan_GSword_BreathTaking_Bloodshed_%d.dds"), 150);
    m_pMovieCom[MOVIE_TYPE::BridleOfBattle] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_BridleOfBattle/MV_Kazan_GSword_BridleOfBattle_%d.dds"), 184);
    m_pMovieCom[MOVIE_TYPE::GiantHunt] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_GiantHunt/MV_Kazan_GSword_GiantHunt_%d.dds"), 184);
    m_pMovieCom[MOVIE_TYPE::InnerFury] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_InnerFury/MV_Kazan_GSword_InnerFury_%d.dds"), 142);
    m_pMovieCom[MOVIE_TYPE::LimitBreak] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_LimitBreak/MV_Kazan_GSword_LimitBreak_%d.dds"), 213);
    m_pMovieCom[MOVIE_TYPE::ManifestStrength] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_ManifestStrength/MV_Kazan_GSword_ManifestStrength_%d.dds"), 208);
    m_pMovieCom[MOVIE_TYPE::Momentum] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_Momentum/MV_Kazan_GSword_Momentum_%d.dds"), 195);
    m_pMovieCom[MOVIE_TYPE::Phantom] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_Phantom/MV_Kazan_GSword_Phantom_%d.dds"), 343);
    m_pMovieCom[MOVIE_TYPE::WarCry] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Movie/Skill/GS/MV_Kazan_GSword_WarCry/MV_Kazan_GSword_WarCry_%d.dds"), 101);
#endif

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
    Safe_Release(m_pIcon);

	for (auto Movie : m_pMovieCom)
		Safe_Release(Movie);
	m_pMovieCom.clear();

}
