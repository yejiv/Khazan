#include "Dragonian_Melee.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Melee.h"
#include "Dragonian_Sword.h"
#include "AI_Controller_Dragonian_Melee.h"

CDragonian_Melee::CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Melee::CDragonian_Melee(const CDragonian_Melee& Prototype)
    :CMonster( Prototype )
{
}

HRESULT CDragonian_Melee::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDragonian_Melee::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Components(),E_FAIL);
    CHECK_FAILED(Ready_PartObjects(),E_FAIL);
    CHECK_FAILED(Ready_AnimEvent(),E_FAIL);

    m_pController = CAI_Controller_Dragonian_Melee::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pController->Get_BlackBoard()->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);
    return S_OK;
}

void CDragonian_Melee::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CDragonian_Melee::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
}

void CDragonian_Melee::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
}

void CDragonian_Melee::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Melee::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Melee::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

HRESULT CDragonian_Melee::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Melee"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Melee/Dragonian_Melee/Dragonian_Melee.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_Component_Dragonian_Sword"),
        CModel::Create(m_pDevice, m_pContext, "../Bin/Data/Monster/Model/Dragonian_Melee/DragonSword/DragonSword.dat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Melee_Body"),
        CBody_Dragonian_Melee::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Sword"),
        CDragonian_Sword::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.6f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 0.3f;
    tCharVirDesc.fHeight = 0.7f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;

    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_PartObjects()
{
    CBody_Dragonian_Melee::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Melee_Body"), &BodyDesc)))
        return E_FAIL;
    
    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Dragonian_Melee*>(pBody);
    Safe_AddRef(m_pBody);

    CDragonian_Sword::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Dragonian_Sword"), &WeaponDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CDragonian_Sword*>(pWeapon);
    Safe_AddRef(m_pWeapon);

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_AnimEvent()
{
    return S_OK;
}



CDragonian_Melee* CDragonian_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CDragonian_Melee* pInstance = new CDragonian_Melee(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CDragonian_Melee"));
    }
    return pInstance;
}

CGameObject* CDragonian_Melee::Clone(void* pArg)
{
    CDragonian_Melee* pInstance = new CDragonian_Melee(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CDragonian_Melee"));
    }
    return pInstance;
}

void CDragonian_Melee::Free()
{
    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
}
