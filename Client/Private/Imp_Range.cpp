#include "Imp_Range.h"
#include "CharacterVirtual.h"
#include "Body_Imp_Range.h"
#include "AI_Controller_Imp_Range.h"

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

    return S_OK;
}

HRESULT CImp_Range::Ready_AnimEvent()
{
    return S_OK;
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

    __super::Free();
}
