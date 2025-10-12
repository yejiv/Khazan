#include "ParticleSystem.h"
#include "GameInstance.h"

CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject { pDevice, pContext }
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticleSystem::Initialize_Clone(void* pArg)
{
    PARTICLE_SYSTEM_DESC* pDesc = static_cast<PARTICLE_SYSTEM_DESC*>(pArg);

    m_strName = pDesc->strName;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CParticleSystem::Priority_Update(_float fTimeDelta)
{
    for (auto& pEmitter : m_Emitters)
        pEmitter->Priority_Update(fTimeDelta);
}

void CParticleSystem::Update(_float fTimeDelta)
{
    for (auto& pEmitter : m_Emitters)
        pEmitter->Update(fTimeDelta);
}

void CParticleSystem::Late_Update(_float fTimeDelta)
{
    for (auto& pEmitter : m_Emitters)
        pEmitter->Late_Update(fTimeDelta);
}

HRESULT CParticleSystem::Render()
{
    return S_OK;
}

CParticleEmitter* CParticleSystem::Get_Emitter(_uint iIndex)
{
    if (iIndex >= m_iNumEmitters)
        return nullptr;

    return m_Emitters[iIndex];
}

HRESULT CParticleSystem::Add_Emitter(CParticleEmitter::PARTICLE_EMITTER_DESC Desc)
{
    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::EFFECT),
        TEXT("Prototype_GameObject_ParticleEmitter"), &Desc));
    if (nullptr == pGameObject)
        return E_FAIL;

    CParticleEmitter* pEmitter = dynamic_cast<CParticleEmitter*>(pGameObject);
    if (nullptr == pEmitter)
        return E_FAIL;

    m_Emitters.push_back(pEmitter);

    m_iNumEmitters = static_cast<_uint>(m_Emitters.size());

    return S_OK;
}

HRESULT CParticleSystem::Remove_Emitter(_uint iIndex)
{
    if (iIndex < 0 || iIndex >= m_iNumEmitters)
        return E_FAIL;

    Safe_Release(m_Emitters[iIndex]);
    m_Emitters.erase(m_Emitters.begin() + iIndex);

    m_iNumEmitters = static_cast<_uint>(m_Emitters.size());

    return S_OK;
}

void CParticleSystem::Recreate_Emitter()
{
}

CParticleSystem* CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticleSystem* pInstance = new CParticleSystem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CParticleSystem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticleSystem::Clone(void* pArg)
{
    CParticleSystem* pInstance = new CParticleSystem(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CParticleSystem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticleSystem::Free()
{
    __super::Free();

    for (auto& pEmitter : m_Emitters)
        Safe_Release(pEmitter);
    m_Emitters.clear();
}
