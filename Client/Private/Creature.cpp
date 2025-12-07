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


FMOD_CHANNEL** CCreature::Get_SoundChannel(_int iIndex)
{
    if (m_pChannel.size() <= iIndex)
        m_pChannel.resize(iIndex + 1, nullptr);

    return &m_pChannel[iIndex];
}

void CCreature::KnockBack(_vector vDir, _float fPower, _float fLoss)
{
   // m_pCharVirCom->Set_VelocityPower(vDir,fPower,fLoss);

    if (m_strName == "Yetuga")
        return;

    if (m_strName == "Viper")
        return;


    m_isKnockBack = true;
    m_fKnockBackDir = vDir;
    m_fKnockBackPower = fPower;
    m_fKnockBackLoss = fLoss;

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

    Compute_KnockBack(fTimeDelta);

    if (m_pCharVirCom != nullptr)
    {
        //m_pCharVirCom->Sync_Update(m_pTransformCom);
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


void CCreature::Creature_Release()
{
    m_pCharVirCom->Fake_Release();
    m_isGhost = true;
    m_isDead = true;
    m_isActive = false;
}

void CCreature::Compute_KnockBack(_float fTimeDelta)
{
    if (!m_isKnockBack)
        return; 

    m_fKnockBackPower -= m_fKnockBackLoss * fTimeDelta;

    if(m_fKnockBackPower  <= 0.01f )
    { 
        m_fKnockBackPower = 0.f;
        m_isKnockBack = false;
        return; 
    }

    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);

    m_pTransformCom->Set_State(STATE::POSITION, vPos + m_fKnockBackDir * m_fKnockBackPower * fTimeDelta);

}

void CCreature::Free()
{
    __super::Free();

    Safe_Release(m_pCharVirCom);
}
