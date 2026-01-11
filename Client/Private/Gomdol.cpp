#include "Gomdol.h"
#include "CharacterVirtual.h"
#include "Body_Gomdol.h"
#include "AI_Controller_Gomdol.h"


CGomdol::CGomdol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{pDevice,pContext}
{
}

CGomdol::CGomdol(const CGomdol& Prototype)
    :CMonster{Prototype}
{
}

_float4* CGomdol::Get_LockOnPosition()
{
    return nullptr;
}

void CGomdol::Set_InitPosition()
{
    m_pCharVirCom->Set_Position(XMVectorSet(-4.f, 0.f, 17.78f, 1.f));
}

HRESULT CGomdol::Initialize_Prototype()
{
 
    return S_OK;
}

HRESULT CGomdol::Initialize_Clone(void* pArg)
{

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_AnimEvent()))
        return E_FAIL;

    m_pController = CAI_Controller_Gomdol::Create(this);
    if (nullptr == m_pController)
        return E_FAIL;

    if (nullptr != m_pController)
    {
        m_pController->Get_BlackBoard()->Set_Value(m_strName, "Target", m_pTarget);
    }

    // HP 등록

    m_pTransformCom->Rotation(XMVectorSet(0.f,1.f,0.f,0.f),XMConvertToRadians(180.f));

    return S_OK;
}

void CGomdol::Priority_Update(_float fTimeDelta)
{
    CContainerObject::Priority_Update(fTimeDelta);
}

void CGomdol::Update(_float fTimeDelta)
{
    m_pController->Update(this, fTimeDelta);


    __super::Update(fTimeDelta);
}

void CGomdol::Late_Update(_float fTimeDelta)
{
    CContainerObject::Late_Update(fTimeDelta);
}

HRESULT CGomdol::Render()
{
    return S_OK;
}

void CGomdol::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CGomdol::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CGomdol::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
}

HRESULT CGomdol::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.9f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fHeight = 0.3f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 10.f;
    tCharVirDesc.fMaxStrength = 10.f;
    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.vWalkStairsStepDownExtra = _float3(0.f, 0.f, 0.f);

    m_tGomdolCharvir.pGameObject = this;
    m_tGomdolCharvir.strName = TEXT("tGomdolChaVir");
    m_tGomdolCharvir.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
    tCharVirDesc.pCollisionDesc = &m_tGomdolCharvir;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    m_pCharVirCom->Collision_Active(true);



    return S_OK;
}

HRESULT CGomdol::Ready_PartObjects()
{
    CBody_Gomdol::BODY_DESC BodyDesc{};
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;
    BodyDesc.pOwner = this;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::TRAINING), TEXT("Prototype_PartObject_Gomdol_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Gomdol*>(pBody);
    Safe_AddRef(m_pBody);

    return S_OK;
}

HRESULT CGomdol::Ready_AnimEvent()
{
    return S_OK;
}

CGomdol* CGomdol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGomdol* pInstance = new CGomdol(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CGomdol"));
    }
    return pInstance;
}

CGameObject* CGomdol::Clone(void* pArg)
{
    CGomdol* pInstance = new CGomdol(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CGomdol"));
    }
    return pInstance;
}

void CGomdol::Free()
{
    Safe_Release(m_pBody);
    __super::Free();
}
