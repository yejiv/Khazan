#include "Projectile.h"
#include "GameInstance.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{pDevice,pContext}
{
}

CProjectile::CProjectile(const CProjectile& Prototype)
    :CGameObject{Prototype}
{
}

FMOD_CHANNEL** CProjectile::Get_SoundChannel(_int iIndex)
{
    if (m_pChannel.size() <= iIndex)
        m_pChannel.resize(iIndex + 1, nullptr);

    return &m_pChannel[iIndex];
}

HRESULT CProjectile::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProjectile::Initialize_Clone(void* pArg)
{
    PROJECTILE_DESC* pDesc = static_cast<PROJECTILE_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_fLifeTime = pDesc->fLifeTime;
    m_fDamage = pDesc->fDamage;


    return S_OK;
}

void CProjectile::Priority_Update(_float fTimeDelta)
{

}

void CProjectile::Update(_float fTimeDelta)
{
    
}

void CProjectile::Late_Update(_float fTimeDelta)
{
    
}

HRESULT CProjectile::Render()
{
    return S_OK;
}



void CProjectile::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    __super::Free();
}
