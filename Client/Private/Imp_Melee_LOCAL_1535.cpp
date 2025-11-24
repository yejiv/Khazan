#include "Imp_Melee.h"
#include "CharacterVirtual.h"
#include "Body_Imp_Melee.h"
#include "Imp_Sword.h"
#include "AI_Controller_Imp_Melee.h"
#include "Mon_HP.h"
#include "ClientInstance.h"
#include "Amount.h"
#include "Model.h"
#include "GameInstance.h"

CImp_Melee::CImp_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CImp_Melee::CImp_Melee(const CImp_Melee& Prototype)
    :CMonster{ Prototype }
{
}

_float4* CImp_Melee::Get_LockOnPosition()
{
    return nullptr;
}

HRESULT CImp_Melee::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CImp_Melee::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    m_pController = CAI_Controller_Imp_Melee::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if (nullptr != m_pController)
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);

    return S_OK;
}

void CImp_Melee::Priority_Update(_float fTimeDelta)
{
   CContainerObject::Priority_Update(fTimeDelta);

    if (m_fCurrentHP <= 0.f && !m_isDeadFlag)
    {
        CClientInstance::GetInstance()->Add_SkillExp(10.f);
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::GOLD, 100);
        m_isDeadFlag = true;
    }
}

void CImp_Melee::Update(_float fTimeDelta)
{
   m_pController->Update(this, fTimeDelta);


    if (m_isLookAt)
    {
        CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
        if (nullptr == pModel)
            return;
        _float fRatio = pModel->MakeRatio();
        Look_Target_Lerp(fTimeDelta, fRatio, m_fTurnSpeed);
    }


    __super::Update(fTimeDelta);

    m_vLockOnPosition = m_pBody->Get_BonePointEX("FX_Body_ExpGained");
}

void CImp_Melee::Late_Update(_float fTimeDelta)
{
    if (!m_isDetected)
    {

        CBlackBoard* pBB = m_pController->Get_BlackBoard();
        if (pBB->Get_Value<_bool>(m_strName, "isDetected"))
        {
            m_isDetected = true;

            m_pUI_HP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));

            if (m_pUI_HP != nullptr)
            {
                Safe_AddRef(m_pUI_HP);

                m_pUI_HP->Setting_HP(m_vLockOnPosition, { 0.f, 50.f }, &m_fCurrentHP, &m_fMaxHP, &m_fCurrentStamina, &m_fMaxStamina);
                m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pUI_HP);
            }
        }
    }


    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CImp_Melee::Render()
{
    return S_OK;
}

void CImp_Melee::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CImp_Melee::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CImp_Melee::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

void CImp_Melee::HPUI_Dead()
{
    m_pUI_HP->Update_Visible(false);
    Safe_Release(m_pUI_HP);
    m_pUI_HP->Set_IsDead(true);
}

HRESULT CImp_Melee::Ready_Components()
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

HRESULT CImp_Melee::Ready_PartObjects()
{
    CBody_Imp_Melee::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Imp_Melee*>(pBody);
    Safe_AddRef(m_pBody);


    CImp_Sword::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pOwner = this;
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");


    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Sword"), &WeaponDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CImp_Sword*>(pWeapon);
    Safe_AddRef(m_pWeapon);
    if (nullptr == pWeapon)
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Melee::Ready_AnimEvent()
{
    CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
    if (nullptr == pModel)
        return E_FAIL;

#pragma region NonStopAttack

    pModel->Register_Event("NontStopAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        m_fTurnSpeed = 10.f;
        m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("NontStopAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        m_fTurnSpeed = 10.f;
        m_pWeapon->Set_OnAttackCollision(false);
        });


    pModel->Register_Event("NontStopAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("NontStopAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        m_pWeapon->Set_OnAttackCollision(false);
        });


#pragma endregion




#pragma region ChainAttack

    pModel->Register_Event("ChainAttack1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("ChainAttack1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        m_pWeapon->Set_OnAttackCollision(false);
        });


    pModel->Register_Event("ChainAttack2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        m_pWeapon->Set_OnAttackCollision(true);
        });

    pModel->Register_Event("ChainAttack2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        m_pWeapon->Set_OnAttackCollision(false);
        });


    pModel->Register_Event("ChainAttack3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        m_pWeapon->Set_OnAttackCollision(true);

        });

    pModel->Register_Event("ChainAttack3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        m_pWeapon->Set_OnAttackCollision(false);

        });


    pModel->Register_Event("ChainAttack_After", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_isLookAt = true;
        });

    pModel->Register_Event("ChainAttack_After", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isLookAt = false;
        });


#pragma endregion


    return S_OK;
}



CImp_Melee* CImp_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImp_Melee* pInstance = new CImp_Melee(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CImp_Melee"));
    }
    return pInstance;
}

CGameObject* CImp_Melee::Clone(void* pArg)
{
    CImp_Melee* pInstance = new CImp_Melee(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CImp_Melee"));
    }
    return pInstance;
}

void CImp_Melee::Free()
{
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pUI_HP);

    __super::Free();
}
