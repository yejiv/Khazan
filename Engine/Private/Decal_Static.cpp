#include "Decal_Static.h"
#include "GameInstance.h"

CDecal_Static::CDecal_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CDecal_Static::CDecal_Static(const CDecal_Static& Prototype)
    : CGameObject { Prototype }
{
}

HRESULT CDecal_Static::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDecal_Static::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CDecal_Static::Priority_Update(_float fTimeDelta)
{
}

void CDecal_Static::Update(_float fTimeDelta)
{
}

void CDecal_Static::Late_Update(_float fTimeDelta)
{
}

HRESULT CDecal_Static::Render()
{
    return S_OK;
}

HRESULT CDecal_Static::Bind_ShaderResources(CShader* pShader, class CTexture** pTexture, CVIBuffer_Cube* pVIBuffer)
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(pShader, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4 WorldMatrixInv{};
    XMStoreFloat4x4(&WorldMatrixInv, m_pTransformCom->Get_WorldMatrix_Inverse());

    if (FAILED(pShader->Bind_Matrix("g_WorldMatrixInv", &WorldMatrixInv)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vDecalColor", &m_Desc.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fThreshold", &m_fThreshold, sizeof(_float))))
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
    }

    // 버퍼 렌더 및 텍스처 바인딩, 셰이더 비긴
    pShader->Begin(1);

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

_bool CDecal_Static::isCameraInDecalBox()
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

void CDecal_Static::Set_Desc(STATIC_DECAL_DESC Desc)
{
    m_Desc = Desc;

    m_pTransformCom->Scale(m_Desc.vScale);
    m_pTransformCom->Rotation(XMConvertToRadians(m_Desc.vAngle.x), XMConvertToRadians(m_Desc.vAngle.y), XMConvertToRadians(m_Desc.vAngle.z));
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_Desc.vPosition.x, m_Desc.vPosition.y, m_Desc.vPosition.z, 1.f));
}

CDecal_Static* CDecal_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDecal_Static* pInstance = new CDecal_Static(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CDecal_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDecal_Static::Clone(void* pArg)
{
    CDecal_Static* pInstance = new CDecal_Static(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CDecal_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDecal_Static::Free()
{
    __super::Free();
}