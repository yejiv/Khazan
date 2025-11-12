#include "Creature.h"
#include "PartObject.h"
#include "CharacterVirtual.h"

CCreature::CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CContainerObject{ pDevice, pContext }
{
}

CCreature::CCreature(const CCreature& Prototype)
    :CContainerObject{ Prototype }
{
}


void CCreature::KnockBack(_vector vDir, _float fPower, _float fLoss)
{
    m_pCharVirCom->Set_VelocityPower(vDir,fPower,fLoss);
}

HRESULT CCreature::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCreature::Initialize_Clone(void* pArg)
{
    CREATURE_DESC* pDesc = static_cast<CREATURE_DESC*>(pArg);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_fMaxHP = pDesc->fMaxHP;
    m_fCurrentHP = m_fMaxHP;
    m_fMaxStamina = pDesc->fMaxStamina;
    m_fCurrentStamina = m_fMaxStamina;
    m_fAttack = pDesc->fAttack;
    m_fMoveSpeed = pDesc->fMoveSpeed;


    return S_OK;
}

void CCreature::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CCreature::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    if (m_pCharVirCom != nullptr)
    {
        m_pCharVirCom->Sync_Update(m_pTransformCom);
        m_pCharVirCom->Update(fTimeDelta, m_pTransformCom, m_vGravity);
    }
}

void CCreature::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CCreature::Render()
{
    return S_OK;
}

void CCreature::Free()
{
    Safe_Release(m_pCharVirCom);

    __super::Free();
}
