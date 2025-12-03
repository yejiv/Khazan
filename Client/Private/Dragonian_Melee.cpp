#include "Dragonian_Melee.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Melee.h"
#include "Dragonian_Sword.h"
#include "AI_Controller_Dragonian_Melee.h"

#include "Mon_Hp.h"
#include "UI_Talk_Danjinjar.h"

#include "MeshTrail.h"
CDragonian_Melee::CDragonian_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Melee::CDragonian_Melee(const CDragonian_Melee& Prototype)
    :CMonster( Prototype )
{
}

CDragonian_Melee::MONDATA& CDragonian_Melee::Get_Data()
{
    return m_Data;
}

void CDragonian_Melee::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CDragonian_Melee::LockOn()
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

void CDragonian_Melee::Hp_Visivle(_bool isVisivle)
{
    m_pUI_HP->Update_Visible(isVisivle);
}

void CDragonian_Melee::Hp_Dead()
{
    m_pUI_HP->Set_IsDead(true);
    m_pUI_HP = nullptr;
}

void CDragonian_Melee::LookAt_Lerp(_float fTimeDelta)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDelta, 0.8f);

}

_bool CDragonian_Melee::Check_AttackRanage(string strKey)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));
    _float fAttackRanage = m_pBlackBoard->Get_Value<_float>(m_strName, strKey);

    if (fAttackRanage <= 0)
        return false;

    if (fDist <= fAttackRanage)
        return true;
    else
        return false;
}

_bool CDragonian_Melee::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CDragonian_Melee::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
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

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(),E_FAIL);
    m_pHeadMatrix = m_pBody->Get_BoneMatrix_Ptr("Bip001-Head");

    m_pLockOnSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("FX_Body_ExpGained");
    m_vLockOnPosition = &m_vLockOnPos;

    CHECK_FAILED(Ready_AnimEvent(),E_FAIL);
    CHECK_FAILED(Ready_Components(),E_FAIL);
    
    CMeshTrail::TRAIL_DESC MeshDesc{};
    MeshDesc.iTextureIdx = 11;
    MeshDesc.fLifeTime = 0.25f;
    MeshDesc.iDivisionCount = 10.f;
    MeshDesc.vColor = _float4(1.58f, 1.788f, 1.592f, 1.f);
    m_pMeshTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDesc));

    return S_OK;
}

void CDragonian_Melee::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);

   /* if (m_pGameInstance->Key_Down(DIK_M))
    {
        m_fCurrentHP = m_fMaxHP;
        m_Data.isSleep = true;
    }
    else if (m_pGameInstance->Key_Down(DIK_B) && m_pGameInstance->Key_Pressing(DIK_LCONTROL, 0.f))
        m_fCurrentStamina = 0;
    else if (m_pGameInstance->Key_Down(DIK_B))
        Take_Damage(10.f, HITREACTION::BRUTAL_ATTACK, m_pTarget);
        */
    m_pMeshTrail->Priority_Update(fTimeDelta);
}

void CDragonian_Melee::Update(_float fTimeDelta)
{
    m_fTimeDelta = fTimeDelta;

    if (m_Data.fAttackCool >= 0.f)
        m_Data.fAttackCool -= fTimeDelta;

    if (m_Data.fAnimDeley >= 0.f)
        m_Data.fAnimDeley -= fTimeDelta;

    _float4x4 m_vSwordMat = m_pWeapon->Get_CombindMat();
    _vector vClawLRight = XMVector3Normalize({ m_vSwordMat._11, m_vSwordMat._12, m_vSwordMat._13 });
    _vector vClawLUp = XMVector3Normalize({ m_vSwordMat._21, m_vSwordMat._22, m_vSwordMat._23 });
    _vector vClawLLook = XMVector3Normalize({ m_vSwordMat._31, m_vSwordMat._32, m_vSwordMat._33 });

    _vector vSwordPos = { m_vSwordMat._41, m_vSwordMat._42, m_vSwordMat._43 };
    _vector vSwordStart = vSwordPos - vClawLUp * 0.45f + vClawLRight * 0.25f;
    _vector vSwordEnd = vSwordPos + vClawLUp * 1.1f - vClawLRight * 0.2f;
    XMStoreFloat4(&m_vSword_Start, XMVectorSetW(vSwordStart, 1.f));
    XMStoreFloat4(&m_vSword_End, XMVectorSetW(vSwordEnd, 1.f));

    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);
    Update_UIHp();
    Update_WalkSpeed();

    m_pMeshTrail->Update(fTimeDelta);
}

void CDragonian_Melee::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);

    m_pMeshTrail->Late_Update(fTimeDelta);
}

void CDragonian_Melee::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (m_Data.eHitType == HITREACTION::BRUTAL_ATTACK)
        ++m_Data.iBrutalHit;

    __super::Take_Damage(fDamage, eHitreaction, pGameObject);
}

const TRAIL_CONFIG& CDragonian_Melee::Get_TrailConfig() const
{
    return m_pMeshTrail->Get_TrailConfig();
}

void CDragonian_Melee::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pMeshTrail->Set_TrailConfig(Config);
}

_uint CDragonian_Melee::Get_NumTrailTextures()
{
    return m_pMeshTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CDragonian_Melee::Get_TrailTexture(_uint iIndex)
{
    return m_pMeshTrail->Get_TrailTexture(iIndex);
}

void CDragonian_Melee::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CDragonian_Melee::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CDragonian_Melee::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
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

HRESULT CDragonian_Melee::Ready_ETC()
{
    m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    CHECK_NULLPTR(m_pUI_HP, E_FAIL);
    m_pUI_HP->Setting_HP(&m_vHpPos, { 0.f, 0.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
    m_pGameInstance->Push_PoolObject_ToLayer(m_iPrototypeIndex, TEXT("Layer_UI"), m_pUI_HP);

    m_pController = CAI_Controller_Dragonian_Melee::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);

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
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.35f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;
    
    m_tCollisionDesc.pGameObject = this;
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    //캡슐 Desc
    tCharVirDesc.fRadius = 1.2f;
    tCharVirDesc.fHeight = 0.4f;
    
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);


    return S_OK;
}

HRESULT CDragonian_Melee::Ready_PartObjects()
{
    CBody_Dragonian_Melee::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
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
    WeaponDesc.pData = &m_Data;

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
    CModel* pModel = m_pBody->Get_Model();

    pModel->Register_Event("Walk1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });
    pModel->Register_Event("Walk4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isSlowWalk = false; });

    pModel->Register_Event("Walk1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });
    pModel->Register_Event("Walk4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isSlowWalk = true; });

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });

    pModel->Register_Event("DoubleSwing_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("DoubleSwing_4", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {this->m_Data.isAttack_Collision = true; });

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {this->Attack_Move(); Update_MeshTrail(); });

    pModel->Register_Event("HardSmash_1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });
    pModel->Register_Event("HardSmash_2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {this->m_Data.isAttack_Collision = false; });

    return S_OK;
}

HRESULT CDragonian_Melee::Ready_MonData()
{
    m_Data.pOwner = this;
    m_Data.fGloggyTime = 3.f;
    m_Data.pCulHp = &m_fCurrentHP;
    m_Data.pMaxHp = &m_fMaxHP;

    m_Data.pCulStamina = &m_fCurrentStamina;
    m_Data.pMaxStamina = &m_fMaxStamina;

    m_Data.fEdgeWidth = 0.1f;
    m_Data.fEdgeColor = { 4.2f, 1.6f, 0.2f, 1.f };

    return S_OK;
}

void CDragonian_Melee::Update_UIHp()
{
    m_vHpPos = { m_pHeadMatrix->m[3][0], m_pHeadMatrix->m[3][1], m_pHeadMatrix->m[3][2], 1.f };
    XMStoreFloat4(&m_vHpPos, XMVector4Transform(XMLoadFloat4(&m_vHpPos), m_pTransformCom->Get_WorldMatrix()));
    m_vHpPos.y += 0.5f;
}

void CDragonian_Melee::Update_WalkSpeed()
{
    if (m_Data.isSlowWalk)
        m_Data.fWarkSpeed = 0.7f;
    else
        m_Data.fWarkSpeed = 1.6f;
}

void CDragonian_Melee::Update_MeshTrail()
{
    _vector vSwordStart = XMLoadFloat4(&m_vSword_Start);
    _vector vSwordEnd = XMLoadFloat4(&m_vSword_End);
    m_pMeshTrail->Add_ControlPoint(vSwordEnd, vSwordStart);
}

void CDragonian_Melee::Attack_Move()
{
    LockOnLerp(m_fTimeDelta, 3.f);
    Get_Transform()->Go_Straight(m_fTimeDelta);
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
    Safe_Release(m_pMeshTrail);
    m_Data.pOwner = nullptr;
    if (m_pUI_HP != nullptr)
    {
        m_pUI_HP->Set_IsDead(true);
    }
    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pBlackBoard);
}
