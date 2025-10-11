#include "EditorPch.h"
#include "ParticleEmitter.h"
#include "GameInstance.h"

CParticleEmitter::CParticleEmitter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CParticleEmitter::CParticleEmitter(const CParticleEmitter& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CParticleEmitter::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticleEmitter::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    PARTICLE_EMITTER_DESC* pDesc = static_cast<PARTICLE_EMITTER_DESC*>(pArg);

    m_strName = pDesc->strName;

    CVIBuffer_Point_Instance::POINT_INSTANCE_DESC Desc{};
    Desc.iNumInstance = 300;
    Desc.vCenter = _float3(0.f, 0.f, 0.f);
    Desc.vRange = _float3(0.2f, 0.2f, 0.2f);
    Desc.vSize = _float2(0.05f, 0.1f);
    Desc.vLifeTime = _float2(0.5f, 2.f);
    Desc.vPivot = _float3(0.f, 0.f, 0.f);
    Desc.vSpeed = _float2(0.5f, 1.f);
    Desc.isLoop = true;

    m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &Desc);
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Initialize_Clone(pArg)))
        return E_FAIL;

    m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

    Safe_AddRef(m_pVIBufferCom);

    return S_OK;
}

void CParticleEmitter::Priority_Update(_float fTimeDelta)
{
}

void CParticleEmitter::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Spread(fTimeDelta);
}

void CParticleEmitter::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this)))
        return;
}

HRESULT CParticleEmitter::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CParticleEmitter::Recreate_Particle()
{
}

HRESULT CParticleEmitter::Ready_Components()
{
    // Com_Shader
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    // 파티클 버퍼 생성 Initialize_Clone으로 옮기기
    //  if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Particle_Snow"),
    //      TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
    //      return E_FAIL;

    // Com_Texture
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Component_Texture_TestParticle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticleEmitter::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DiffuseTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CParticleEmitter* CParticleEmitter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleEmitter* pInstance = new CParticleEmitter(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CParticleEmitter"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleEmitter::Clone(void* pArg)
{
    CParticleEmitter* pInstance = new CParticleEmitter(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CParticleEmitter"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticleEmitter::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
