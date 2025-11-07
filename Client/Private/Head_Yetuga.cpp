#include "Head_Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "Yetuga.h"
#include "AI_Controller.h"

CHead_Yetuga::CHead_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPartObject{pDevice,pContext}
{
}

CHead_Yetuga::CHead_Yetuga(const CHead_Yetuga& Prototype)
    :CPartObject{Prototype}
{
}

HRESULT CHead_Yetuga::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHead_Yetuga::Initialize_Clone(void* pArg)
{

    HEAD_DESC* pDesc = static_cast<HEAD_DESC*>(pArg);

    m_pOwnerTransform = pDesc->pOwnerTransform;
    if (nullptr == m_pOwnerTransform)
        return E_FAIL;
    Safe_AddRef(m_pOwnerTransform);

    m_pOwner = pDesc->pOwner;
    if (nullptr == m_pOwner)
        return E_FAIL;


    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;


    if (FAILED(Ready_Colliders()))
        return E_FAIL;
;

    m_pHeadBodyCom->Activate(false);
    m_isOnAttackCollision = false;
    return S_OK;
}

void CHead_Yetuga::Priority_Update(_float fTimeDelta)
{
}

void CHead_Yetuga::Update(_float fTimeDelta)
{
    if (m_isOnAttackCollision)
    {
        m_pHeadBodyCom->Activate(true);
        m_pGameInstance->Set_DrawFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK));

    }
    else
    {
        m_pHeadBodyCom->Activate(false);
        m_pGameInstance->Remove_DrawFilter(ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK));
    }


    Update_CombinedMatrix();

    Carculate_Matrix(fTimeDelta);


}

void CHead_Yetuga::Late_Update(_float fTimeDelta)
{
}

HRESULT CHead_Yetuga::Render()
{
    return S_OK;
}

void CHead_Yetuga::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    COLLISION_LAYER eType = static_cast<COLLISION_LAYER>(iOtherObjectLayer);
         m_pOwner->Get_Controller()->AI_React_Collision(pDesc,m_pOwner);
}

void CHead_Yetuga::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{

}

void CHead_Yetuga::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
{

}



void CHead_Yetuga::Carculate_Matrix(_float fTimeDelta)
{

    CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")));
    if (pModel == nullptr)
        return;

    _float4x4 BoneMatrix = *pModel->Get_BoneMatrix("Bip001-Head");
    _vector vOutQuat, vOutPos;
    XMStoreFloat4x4(&m_HeadMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
    m_pHeadBodyCom->Sync_Update(XMLoadFloat4x4(&m_HeadMatrix));
    m_pHeadBodyCom->Update(fTimeDelta, XMLoadFloat4x4(&m_HeadMatrix), vOutQuat, vOutPos);

    m_HeadMatrix._41 = vOutPos.m128_f32[0];
    m_HeadMatrix._42 = vOutPos.m128_f32[1];
    m_HeadMatrix._43 = vOutPos.m128_f32[2];
    m_HeadMatrix._44 = vOutPos.m128_f32[3];

}

HRESULT CHead_Yetuga::Ready_Colliders()
{
    // ¸Ó¸®

    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};

    BodyDesc.fRadius = 2.f;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::LinearCast;
    BodyDesc.eShapeType = SHAPE::SPHERE;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK);
    CModel* pModel = static_cast<CModel*>(m_pOwner->Get_Component(TEXT("Part_Body"), TEXT("Com_Model")));
    if (pModel == nullptr)
        return E_FAIL;

    _float4x4 BoneMatrix = *pModel->Get_BoneMatrix("Bip001-Head");
    XMStoreFloat4x4(&m_HeadMatrix, m_pTransformCom->Get_WorldMatrix() *
        XMLoadFloat4x4(&BoneMatrix) * XMLoadFloat4x4(m_pParentMatrix));
     _vector vScale, vQuat, vTrans;
     // ÂÉ°µ´Ù.
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_HeadMatrix));
    // À§Ä¡°ª
    BodyDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
    // ÄõÅÍ´Ï¾ð
    BodyDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;
    BodyDesc.bIsTrigger = true;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body_Head"), reinterpret_cast<CComponent**>(&m_pHeadBodyCom), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

CHead_Yetuga* CHead_Yetuga::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHead_Yetuga* pInstance = new CHead_Yetuga(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CHead_Yetuga"));
    }

    return pInstance;
}

CGameObject* CHead_Yetuga::Clone(void* pArg)
{
    CHead_Yetuga* pInstance = new CHead_Yetuga(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CHead_Yetuga"));
    }

    return pInstance;
}

void CHead_Yetuga::Free()
{
    Safe_Release(m_pOwnerTransform);
    Safe_Release(m_pHeadBodyCom);

    __super::Free();
}
