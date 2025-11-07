#include "Tutorial_Tex.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CTutorial_Tex::CTutorial_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{ pDevice, pContext }
{
}

CTutorial_Tex::CTutorial_Tex(const CTutorial_Tex& Prototype)
	: CUI_Texture(Prototype)
{
}

void CTutorial_Tex::Setting_Tex(GUIDE_TYPE eType, _int iPage)
{
	m_isMovie = false;
	m_iTexPass = 0;

	if (eType == GUIDE_TYPE::LOCKON)
		m_iTexPass = 4;
	else if (eType == GUIDE_TYPE::GUARD )
	{
		m_isMovie = true;
		if (iPage == 0)
			m_eMovieType = MOVIE_TYPE::GUARD_1;
		else
			m_eMovieType = MOVIE_TYPE::GUARD_2;
	}
	else if (eType == GUIDE_TYPE::UNDERWORLD)
		m_iTexPass = 5;
	else if (eType == GUIDE_TYPE::DODGE)
	{
		m_isMovie = true;
		if (iPage == 0)
			m_eMovieType = MOVIE_TYPE::DODGE_1;
		else
			m_eMovieType = MOVIE_TYPE::DODGE_2;
	}
	else if (eType == GUIDE_TYPE::BURTALATTACK)
	{
		if (iPage == 0)
			m_iTexPass = 0;
		else
		{
			m_isMovie = true;
			m_eMovieType = MOVIE_TYPE::DODGE_2;
		}
	}
	else if (eType == GUIDE_TYPE::FALLATTACK)
		m_iTexPass = 2;
	else if (eType == GUIDE_TYPE::IMPULSE)
		m_iTexPass = 3;

}

HRESULT CTutorial_Tex::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTutorial_Tex::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	m_vColor = { 1.f, 1.f, 1.f, 1.f };
	return S_OK;
}

void CTutorial_Tex::Priority_Update(_float fTimeDelta)
{
}

void CTutorial_Tex::Update(_float fTimeDelta)
{
	
}

void CTutorial_Tex::Late_Update(_float fTimeDelta)
{
	if (m_isMovie)
	{
		++m_iTexPass;

		if (m_iTexPass >= m_pMovieCom[m_eMovieType]->Get_NumTextures())
			m_iTexPass = 0;
	}

	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
}

HRESULT CTutorial_Tex::Render()
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

HRESULT CTutorial_Tex::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuidePage"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	m_pMovieCom.resize(MOVIE_TYPE::END);

	m_pMovieCom[MOVIE_TYPE::BRUTALATTACK] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/BrutalAttack/BrutalAttack_%d.dds"), 162);
	m_pMovieCom[MOVIE_TYPE::DODGE_1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Dodge_01/Dodge_01_%d.dds"), 58);
	m_pMovieCom[MOVIE_TYPE::DODGE_2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Dodge_02/Dodge_02_%d.dds"), 90);
	m_pMovieCom[MOVIE_TYPE::GUARD_1] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Guard_01/Guard_01_%d.dds"), 372);
	m_pMovieCom[MOVIE_TYPE::GUARD_2] = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/Guard_02/Guard_02_%d.dds"), 138);

//CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Movie_CountAttack"),
//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/TutorialGuide/CountAttack/CountAttack_%d.dds"), 152)), E_FAIL);

	return S_OK;
}

CTutorial_Tex* CTutorial_Tex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTutorial_Tex* pInstance = new CTutorial_Tex(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTutorial_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTutorial_Tex::Clone(void* pArg)
{
	CTutorial_Tex* pInstance = new CTutorial_Tex(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTutorial_Tex"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTutorial_Tex::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	for (auto Movie : m_pMovieCom)
		Safe_Release(Movie);
	m_pMovieCom.clear();
}
