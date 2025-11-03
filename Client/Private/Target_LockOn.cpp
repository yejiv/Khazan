#include "Target_LockOn.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CTarget_LockOn::CTarget_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Texture{pDevice, pContext}
{
}

CTarget_LockOn::CTarget_LockOn(const CTarget_LockOn& Prototype)
	: CUI_Texture(Prototype)
{
}

void CTarget_LockOn::LockOn(const _float4* pTargetPos, _float2 vOffset)
{
	m_IsUpdate = true;
	m_pTagetPos = pTargetPos;
	m_vLocalPos = { vOffset.x, -vOffset.y };
	m_fDelta = 2.f;
}

void CTarget_LockOn::LockOff()
{
	m_IsUpdate = false;
	m_pTagetPos = nullptr;
}


HRESULT CTarget_LockOn::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	CHECK_FAILED(Ready_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTarget_LockOn::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Component(), E_FAIL);

	CClientInstance::GetInstance()->Add_RootUI(TEXT("LockOn"), this);

	return S_OK;
}

void CTarget_LockOn::Priority_Update(_float fTimeDelta)
{
}

void CTarget_LockOn::Update(_float fTimeDelta)
{
}

void CTarget_LockOn::Late_Update(_float fTimeDelta)
{
	if (!m_IsUpdate)
		return;

	if (m_fDelta > 1.f)
	{
		m_fDelta -= fTimeDelta * 8.f;
		if (m_fDelta <= 1.f)
			m_fDelta = 1.f;
	}
	Update_Scaling(m_fDelta);
	Update_WorldPos();
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CTarget_LockOn::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 0)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTarget_LockOn::Ready_Prototype()
{
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_Roct_Fx"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/World/Rock_FX_%d.png"), 2)), E_FAIL);

	return S_OK;
}

HRESULT CTarget_LockOn::Ready_Component()
{
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI_Effect"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr), E_FAIL);
		
	CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_Roct_Fx"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);
		
	return S_OK;
}

void CTarget_LockOn::Update_WorldPos()
{
	if (nullptr == m_pTagetPos)
		return;

	_vector vTargetPos = XMLoadFloat4(m_pTagetPos);

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

CTarget_LockOn* CTarget_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CTarget_LockOn* pInstance = new CTarget_LockOn(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CTarget_LockOn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTarget_LockOn::Clone(void* pArg)
{
	CTarget_LockOn* pInstance = new CTarget_LockOn(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTarget_LockOn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTarget_LockOn::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
