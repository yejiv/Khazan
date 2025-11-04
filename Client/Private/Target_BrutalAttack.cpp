#include "Target_BrutalAttack.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Default_Tex.h"
#include "BrutalAttack_Progress.h"
#include "BrutalAttack_Point.h"

CTarget_BrutalAttack::CTarget_BrutalAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CTarget_BrutalAttack::CTarget_BrutalAttack(const CTarget_BrutalAttack& Prototype)
	: CUI_Panel(Prototype)
{
}

void CTarget_BrutalAttack::Setting_BrutalAttack(const _float4* pTargetPos, _float fDelayTime, _float2 vOffset)
{
	m_pTagetPos = pTargetPos;
	m_vLocalPos = { vOffset.x, -vOffset.y };
	m_fTime = fDelayTime;
	m_fMaxTime = fDelayTime;
}

void CTarget_BrutalAttack::Off_BrutalAttack()
{
	m_pTagetPos = nullptr;
	m_isDead = true;
}

HRESULT CTarget_BrutalAttack::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTarget_BrutalAttack::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);
	CHECK_FAILED(Ready_GameObject(), E_FAIL);
	m_vColor = { 0.f, 0.f, 0.f, 0.5f };
	m_isPool = true;
	return S_OK;
}

void CTarget_BrutalAttack::Priority_Update(_float fTimeDelta)
{
}

void CTarget_BrutalAttack::Update(_float fTimeDelta)
{
	m_pProgress->Update(fTimeDelta);
}

void CTarget_BrutalAttack::Late_Update(_float fTimeDelta)
{
	m_fTime -= fTimeDelta;
	if (m_fTime <= 0 && m_fMaxTime > 0)
		m_isDead = true;


	Update_WorldPos();
	
	if (!m_isActive && m_isDead)
		return;
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
	m_pProgress->Late_Update(fTimeDelta);
	m_pPointBg->Late_Update(fTimeDelta);
	m_pPoint->Late_Update(fTimeDelta);
	m_pPointfront->Late_Update(fTimeDelta);
}

HRESULT CTarget_BrutalAttack::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

void CTarget_BrutalAttack::Reset()
{
	m_pTagetPos = nullptr;
	m_fMaxTime = 0;
	m_fTime = 0;
}

HRESULT CTarget_BrutalAttack::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_BrutalAttack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/World/BrutalAttack_%d.png"), 4)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Progress"),
		CBrutalAttack_Progress::Create(m_pDevice, m_pContext)), E_FAIL);

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Point"),
		CBrutalAttack_Point::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

HRESULT CTarget_BrutalAttack::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_BrutalAttack"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

	return S_OK;
}

void CTarget_BrutalAttack::Update_WorldPos()
{
	if (nullptr == m_pTagetPos)
		return;

	_vector vTargetPos = XMLoadFloat4(m_pTagetPos);

	_float4 vTemp = CClientInstance::GetInstance()->Get_ActiveCameraLook();
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

HRESULT CTarget_BrutalAttack::Ready_GameObject()
{
	CBrutalAttack_Progress::GUIDEGAUGE_DESC Desc = {};
	Desc.pCulValue = &m_fTime;
	Desc.pMaxValue = &m_fMaxTime;
	Desc.iUIType = ENUM_CLASS(UITYPE::PROGRESSBAR);
	Desc.szName = "Progress";
	Desc.vLocalSize = { 36.f, 36.f };

	Desc.vLocalPos = { 0.f, 0.f };

	m_pProgress = static_cast<CBrutalAttack_Progress*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Progress"), &Desc));
	
	Add_Child(m_pProgress);
	Safe_AddRef(m_pProgress);

	CUIObject::UIOBJECT_DESC PointDesc = {};
	PointDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	PointDesc.szName = "Point";
	PointDesc.vLocalSize = { 80.f, 40.f };
	PointDesc.vLocalPos = { 0.f, 0.f };

	m_pPoint = static_cast<CBrutalAttack_Point*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Point"), &PointDesc));
	m_pPoint->Set_TexPass(2);
	m_pPoint->Set_ShaderPass(3);

	Add_Child(m_pPoint);
	Safe_AddRef(m_pPoint);

	PointDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	PointDesc.szName = "PointBG";
	PointDesc.vLocalSize = { 14.f, 14.f };
	PointDesc.vLocalPos = { 0.f, 0.f };

	m_pPointBg = static_cast<CBrutalAttack_Point*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Point"), &PointDesc));
	m_pPointBg->Set_TexPass(1);
	m_pPointBg->Set_ShaderPass(4);
	m_pPointBg->Set_Color({ 1.f, 0.f, 0.f, 0.5f });
	Add_Child(m_pPointBg);
	Safe_AddRef(m_pPointBg);

	PointDesc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
	PointDesc.szName = "PointFront";
	PointDesc.vLocalSize = { 9.f, 9.f };
	PointDesc.vLocalPos = { 0.f, 0.f };

	m_pPointfront = static_cast<CBrutalAttack_Point*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BrutalAttack_Point"), &PointDesc));
	m_pPointfront->Set_TexPass(1);
	m_pPointfront->Set_ShaderPass(4);
	m_pPointfront->Set_Color({ 1.f, 1.f, 1.f, 1.f });
	Add_Child(m_pPointfront);
	Safe_AddRef(m_pPointfront);
	return S_OK;
}

CTarget_BrutalAttack* CTarget_BrutalAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CTarget_BrutalAttack* pInstance = new CTarget_BrutalAttack(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CTarget_BrutalAttack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTarget_BrutalAttack::Clone(void* pArg)
{
	CTarget_BrutalAttack* pInstance = new CTarget_BrutalAttack(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTarget_BrutalAttack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTarget_BrutalAttack::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pProgress);

	Safe_Release(m_pPointBg);
	Safe_Release(m_pPoint);
	Safe_Release(m_pPointfront);
}
