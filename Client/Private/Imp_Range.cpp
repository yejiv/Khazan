#include "Imp_Range.h"
#include "CharacterVirtual.h"
#include "Body_Imp_Range.h"
#include "Imp_Wand.h"
#include "AI_Controller_Imp_Range.h"
#include "Projectile_Imp_MagicBall.h"

CImp_Range::CImp_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{pDevice,pContext}
{
}

CImp_Range::CImp_Range(const CImp_Range& Prototype)
    :CMonster{Prototype}
{
}

_float4* CImp_Range::Get_LockOnPosition()
{
    return nullptr;
}

HRESULT CImp_Range::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CImp_Range::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    //-4 0 27
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(513.f, -11.f, 225.f, 1.f));

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Projectiles()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

      m_pController = CAI_Controller_Imp_Range::Create(this);
      if (nullptr == m_pController)
          return E_FAIL;

      return S_OK;
}

void CImp_Range::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CImp_Range::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);

    __super::Update(fTimeDelta);
}

void CImp_Range::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CImp_Range::Render()
{
    return S_OK;
}

void CImp_Range::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CImp_Range::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CImp_Range::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{
}

HRESULT CImp_Range::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 2.1f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 2.f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;

    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Range::Ready_PartObjects()
{
    CBody_Imp_Range::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Imp_Range*>(pBody);
    Safe_AddRef(m_pBody);


    CImp_Wand::WEAPON_DESC WeaponDesc{};
    WeaponDesc.pOwner = this;
    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    WeaponDesc.pOwnerTransform = m_pTransformCom;
    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Range_Wand"), &WeaponDesc)))
        return E_FAIL;

    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pWeapon = dynamic_cast<CImp_Wand*>(pWeapon);
    if (nullptr == pWeapon)
        return E_FAIL;

    return S_OK;
}

HRESULT CImp_Range::Ready_Projectiles()
{
    CProjectile_Imp_MagicBall::PROJECTILE_DESC Desc{};
    Desc.fDamage = 10.f;
    Desc.fSpeedPerSec = 30.f;
    Desc.fLifeTime = 6.f;
    Desc.fRotationPerSec = 180.f;

    m_pGameInstance->Add_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_Projectile_Imp_Magic_Ball"),
        ENUM_CLASS(LEVEL::HEINMACH), TEXT("Imp_MagicBall"), &Desc, 18);

    return S_OK;
}

HRESULT CImp_Range::Ready_AnimEvent()
{
    return S_OK;
}

void CImp_Range::Make_MagicBall()
{
    _float4 vTempSpawnPoint = *m_pWeapon->Get_BonePointEX("Weapon_L");
    _float3 vSpawnPoint = _float3(vTempSpawnPoint.x, vTempSpawnPoint.y, vTempSpawnPoint.z);
    CGameObject* pGameObject = m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Stone"));
    if (nullptr == pGameObject)
        return;

    m_pMagicBall = static_cast<CProjectile_Imp_MagicBall*>(pGameObject);
    if (m_pMagicBall == nullptr)
        return;
    Safe_AddRef(m_pMagicBall);

    _float3 vTargetDir = m_pGameInstance->Get_BlackBoard()->Get_Value<_float3>(m_strName, "TargetDir");
    _vector vTempVec = XMVector3Normalize(XMLoadFloat3(&vTargetDir));
    _float3 vNormalize{};
    XMStoreFloat3(&vNormalize, vTempVec);
    m_pMagicBall->Set_SpawnDir(vNormalize);
    m_pMagicBall->Set_IsActive(false);   
    m_pMagicBall->Set_Visible(true);  
    m_pMagicBall->Set_SpanwPoint(vSpawnPoint);
    m_pMagicBall->Reset();

    m_pGameInstance->Push_PoolObject_ToLayer(
        ENUM_CLASS(LEVEL::HEINMACH),
        TEXT("Layer_Imp_MagicBall"),
        m_pMagicBall
    );
}

void CImp_Range::Cast_MagicBall()
{

}

void CImp_Range::Shoot_MagicBall()
{

}

CImp_Range* CImp_Range::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImp_Range* pInstance = new CImp_Range(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CImp_Range"));
    }
    return pInstance;
}

CGameObject* CImp_Range::Clone(void* pArg)
{
    CImp_Range* pInstance = new CImp_Range(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CImp_Range"));
    }
    return pInstance;
}

void CImp_Range::Free()
{
    Safe_Release(m_pBody);
    Safe_Release(m_pWeapon);
    Safe_Release(m_pMagicBall);

    __super::Free();
}
