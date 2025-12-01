#include "Decal.h"
#include "GameInstance.h"

CDecal::CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CDecal::CDecal(const CDecal& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CDecal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDecal::Priority_Update(_float fTimeDelta)
{
}

void CDecal::Update(_float fTimeDelta)
{
    if (false == m_isDecoration)
    {
        m_fTimeAcc += fTimeDelta;

        // 수명이 끝나면 사망 처리 -> 풀
        if (m_fTimeAcc >= m_Desc.fLifeTime)
        {
            m_isDead = true;
            m_fTimeAcc = 0.f;
            m_fOpacity = 0.f;
            return;
        }

        // 페이드 아웃 계산
        if (m_fTimeAcc > m_Desc.vFadeTime.y)
        {
            _float fFadeDuration = m_Desc.fLifeTime - m_Desc.vFadeTime.y;	// 페이드 아웃 총 시간
            _float fFadeTimeAcc = m_fTimeAcc - m_Desc.vFadeTime.y;			// 페이드 아웃 시작 후 누적 시간
            _float fRatio = (fFadeTimeAcc / fFadeDuration);					// 페이드 아웃 비율
            m_fOpacity = 1.f - fRatio;
            m_fOpacity = max(0.f, m_fOpacity);								// 비율 0 -> 불투명, 비율 1 -> 투명
        }

        // Fade In
        if (m_fTimeAcc < m_Desc.vFadeTime.x)
        {
            m_fOpacity = m_fTimeAcc / m_Desc.vFadeTime.x;		// 페이드 인 총 시간
            m_fOpacity = min(1.f, m_fOpacity);
        }
    }
}

void CDecal::Late_Update(_float fTimeDelta)
{
}

HRESULT CDecal::Render()
{
	return S_OK;
}

void CDecal::Reset()
{
}

HRESULT CDecal::Bind_ShaderResources(CShader* pShader, class CTexture** pTexture, CVIBuffer_Cube* pVIBuffer)
{
	if (FAILED(m_pTransformCom->Bind_Shader_Resource(pShader, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4 WorldMatrixInv{};
	XMStoreFloat4x4(&WorldMatrixInv, m_pTransformCom->Get_WorldMatrix_Inverse());
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrixInv", &WorldMatrixInv)))
		return E_FAIL;

    if (DECALTYPE::EMISSIVE == m_Desc.eType)
    {
        if (FAILED(pShader->Bind_RawValue("g_vBaseColor", &m_Desc.EmissiveDesc.vBaseColor, sizeof(_float3))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_vEmissiveColor", &m_Desc.EmissiveDesc.vEmissiveColor, sizeof(_float3))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_vBorderColor", &m_Desc.EmissiveDesc.vBorderColor, sizeof(_float3))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_fEmissiveMaskPower", &m_Desc.EmissiveDesc.fEmissiveMaskPower, sizeof(_float))))
            return E_FAIL;

        if (FAILED(pShader->Bind_RawValue("g_fEmissiveIntensity", &m_Desc.EmissiveDesc.fEmissiveIntensity, sizeof(_float))))
            return E_FAIL;
    }
    else
    {
	    if (FAILED(pShader->Bind_RawValue("g_vDecalColor", &m_Desc.vColor, sizeof(_float3))))
		    return E_FAIL;

        if (true == m_isDecoration)             // 데코용일 경우 고정 쓰레스 홀드
        {
            if (FAILED(pShader->Bind_RawValue("g_fThreshold", &m_fThreshold, sizeof(_float))))
                return E_FAIL;
        }
        else
        {
            if (FAILED(pShader->Bind_RawValue("g_iRandSeed", &m_iRandSeed, sizeof(_uint))))
                return E_FAIL;
        }
    }

	if (FAILED(pShader->Bind_RawValue("g_fOpacity", &m_fOpacity, sizeof(_float))))
		return E_FAIL;

	switch (m_Desc.eType)
	{
	case DECALTYPE::LINEAR:
		if (FAILED(pTexture[ENUM_CLASS(DECALTYPE::LINEAR)]->Bind_Shader_Resource(pShader, "g_DecalTexture", m_Desc.iTextureIndex)))
			return E_FAIL;
		break;
	case DECALTYPE::CIRCLE:
		if (FAILED(pTexture[ENUM_CLASS(DECALTYPE::CIRCLE)]->Bind_Shader_Resource(pShader, "g_DecalTexture", m_Desc.iTextureIndex)))
			return E_FAIL;
		break;
	case DECALTYPE::CURVE:
		if (FAILED(pTexture[ENUM_CLASS(DECALTYPE::CURVE)]->Bind_Shader_Resource(pShader, "g_DecalTexture", m_Desc.iTextureIndex)))
			return E_FAIL;
        break;
    case DECALTYPE::EMISSIVE:
        if (FAILED(pTexture[ENUM_CLASS(DECALTYPE::EMISSIVE)]->Bind_Shader_Resource(pShader, "g_DecalTexture", m_Desc.iTextureIndex)))
            return E_FAIL;
		break;
	}

	// 버퍼 렌더 및 텍스처 바인딩, 셰이더 비긴
    if (true == m_isDecoration)             // 데코용일 경우 패스 1번 ( 감사합니다 )
    {
        pShader->Begin(1);
    }
    else
    {
        if (DECALTYPE::EMISSIVE == m_Desc.eType)
            pShader->Begin(3);
        else
            pShader->Begin(0);
    }

	pVIBuffer->Bind_Resources();
	pVIBuffer->Render();

    // 맵 에디터에서 큐브 사이즈 확인 위한 . . .
    if (true == m_isWireFrame)
    {
        pShader->Begin(2);
        pVIBuffer->Render();
    }

	return S_OK;
}

_bool CDecal::isCameraInDecalBox()
{
    // 데칼 로컬로 카메라 포지션 변환
    _float4 vCamPos = *m_pGameInstance->Get_CamPosition();

    _float4 vLocalCamPos = {};
    XMStoreFloat4(&vLocalCamPos, XMVector3TransformCoord(XMLoadFloat4(&vCamPos), m_pTransformCom->Get_WorldMatrix_Inverse()));

    if (vLocalCamPos.x <= m_Desc.vScale.x * 0.5f && vLocalCamPos.x >= m_Desc.vScale.x * -0.5f &&
        vLocalCamPos.y <= m_Desc.vScale.y * 0.5f && vLocalCamPos.y >= m_Desc.vScale.y * -0.5f &&
        vLocalCamPos.z <= m_Desc.vScale.z * 0.5f && vLocalCamPos.z >= m_Desc.vScale.z * -0.5f)
        return true;

    return false;
}

void CDecal::Set_Desc(DECAL_DESC Desc)
{
	m_Desc = Desc;

	m_Desc.vFadeTime.y = Desc.fLifeTime - Desc.vFadeTime.y;

	m_pTransformCom->Scale(Desc.vScale);
    m_pTransformCom->Rotation(XMConvertToRadians(Desc.vAngle.x), XMConvertToRadians(Desc.vAngle.y), XMConvertToRadians(Desc.vAngle.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(Desc.vPosition.x, Desc.vPosition.y, Desc.vPosition.z, 1.f));
}

CDecal* CDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal* pInstance = new CDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDecal::Clone(void* pArg)
{
	CDecal* pInstance = new CDecal(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CDecal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal::Free()
{
	__super::Free();
}