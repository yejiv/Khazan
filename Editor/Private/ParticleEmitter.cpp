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

    return S_OK;
}

void CParticleEmitter::Priority_Update(_float fTimeDelta)
{
}

void CParticleEmitter::Update(_float fTimeDelta)
{
    if (nullptr != m_pVIBufferCom)
    {
        if (m_isSpread)
            m_pVIBufferCom->Spread(fTimeDelta);

        else if (m_isDrop)
            m_pVIBufferCom->Drop(fTimeDelta);
    }
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

    if (nullptr != m_pVIBufferCom)
    {
        m_pVIBufferCom->Bind_Resources();
        m_pVIBufferCom->Render();
    }

    return S_OK;
}

HRESULT CParticleEmitter::Recreate_Particle(CVIBuffer_Point_Instance::POINT_INSTANCE_DESC PointDesc)
{
    if (nullptr != m_pVIBufferCom)
        Safe_Release(m_pVIBufferCom);

    m_pVIBufferCom = CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, &PointDesc);
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;
    
    if (FAILED(m_pVIBufferCom->Initialize_Clone(nullptr)))
        return E_FAIL;
    
    m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);
    
    Safe_AddRef(m_pVIBufferCom);

    return S_OK;
}

HRESULT CParticleEmitter::Ready_Components()
{
    // Com_Shader
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParticle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

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
