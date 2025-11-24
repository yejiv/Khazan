#include "Dragonian_Rampage.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Dragonian_Rampage.h"

CDragonian_Rampage::CDragonian_Rampage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CDragonian_Rampage::CDragonian_Rampage(const CDragonian_Rampage& Prototype)
    :CMonster(Prototype)
{
}

HRESULT CDragonian_Rampage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDragonian_Rampage::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    //m_pController = CAI_Controller_Imp_Melee::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    return S_OK;
}

void CDragonian_Rampage::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CDragonian_Rampage::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CDragonian_Rampage::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
}

void CDragonian_Rampage::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Rampage::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CDragonian_Rampage::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}

HRESULT CDragonian_Rampage::Ready_Components()
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

HRESULT CDragonian_Rampage::Ready_PartObjects()
{
    CBody_Dragonian_Rampage::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    return S_OK;
}

HRESULT CDragonian_Rampage::Ready_AnimEvent()
{
    return S_OK;
}



CDragonian_Rampage* CDragonian_Rampage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDragonian_Rampage* pInstance = new CDragonian_Rampage(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CDragonian_Rampage"));
    }
    return pInstance;
}

CGameObject* CDragonian_Rampage::Clone(void* pArg)
{
    CDragonian_Rampage* pInstance = new CDragonian_Rampage(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CDragonian_Rampage"));
    }
    return pInstance;
}

void CDragonian_Rampage::Free()
{
    __super::Free();
    Safe_Release(m_pBody);
}
