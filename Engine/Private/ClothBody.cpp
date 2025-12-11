#include "ClothBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "RootBody.h"
#include "ChildBody.h"
#include "Bone.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CClothBody::CClothBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CClothBody::CClothBody(const CClothBody& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CClothBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CClothBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    CLOTH_BODY_DESC* pDesc = static_cast<CLOTH_BODY_DESC*>(pArg);

    m_pModel = pDesc->pModel;
    m_RootBoneIndices = pDesc->RootBoneIndices;
    m_pOwnerTransform = pDesc->pOwnerTransform;
    m_iObjectLayer = pDesc->iObjectLayer;

    if (FAILED(Ready_Root(pArg)))
        return E_FAIL;

    return S_OK;
}

void CClothBody::Priority_Update(_float fTimeDelta)
{
    const float MIN_DT = 1.f / 240.f;
    const float MAX_DT = 1.f / 60.f;

    float clothDt = fTimeDelta;

    if (clothDt > MAX_DT)
        clothDt = MAX_DT;

    if (clothDt < MIN_DT)
        clothDt = 0.f;

    if (clothDt <= 0.f)
        return;


    for (auto Root : m_RootBodys)
    {
        Root->Priority_Update(fTimeDelta);
    }
}

void CClothBody::Update(_float fTimeDelta)
{
    const float MIN_DT = 1.f / 240.f;
    const float MAX_DT = 1.f / 60.f;

    float clothDt = fTimeDelta;

    if (clothDt > MAX_DT)
        clothDt = MAX_DT;

    if (clothDt < MIN_DT)
        clothDt = 0.f;

    if (clothDt <= 0.f)
        return;

    for (auto Root : m_RootBodys)
    {
        Root->Update(fTimeDelta);
    }
}

void CClothBody::Late_Update(_float fTimeDelta)
{
    const float MIN_DT = 1.f / 240.f;
    const float MAX_DT = 1.f / 60.f;

    float clothDt = fTimeDelta;

    if (clothDt > MAX_DT)
        clothDt = MAX_DT;

    if (clothDt < MIN_DT)
        clothDt = 0.f;

    if (clothDt <= 0.f)
        return;

    for (auto Root : m_RootBodys)
    {
        Root->Late_Update(fTimeDelta);
    }
}

HRESULT CClothBody::Ready_Root(void* pArg)
{    
    CLOTH_BODY_DESC* pDesc = static_cast<CLOTH_BODY_DESC*>(pArg);

    for (_int i = 0; i < m_RootBoneIndices.size(); i++)
    {
        CRootBody::ROOT_BODY_DESC desc;
        desc.pModel = m_pModel;        
        desc.iBoneIndex = m_RootBoneIndices[i];
        desc.pOwnerTransform = m_pOwnerTransform;
        desc.iObjectLayer = m_iObjectLayer;

        desc.fGravity = pDesc->fGravity;
        desc.fAngularDamping = pDesc->fAngularDamping;
        desc.fLinearDamping = pDesc->fLinearDamping;
        desc.fMass = pDesc->fMass;

        desc.fMinDistance = pDesc->fMinDistance;
        desc.fMaxDistance = pDesc->fMaxDistance;
        desc.fSpringFrequency = pDesc->fSpringFrequency;
        desc.fSpringDamping = pDesc->fSpringDamping;

        desc.eType = pDesc->eType;
        desc.pCollisionDesc = pDesc->pCollisionDesc;
        m_RootBodys.push_back(CRootBody::Create(m_pDevice, m_pContext, &desc));
    }

    return S_OK;
}

CClothBody* CClothBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CClothBody* pInstance = new CClothBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CClothBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CClothBody::Clone(void* pArg)
{
    CClothBody* pInstance = new CClothBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CClothBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CClothBody::Free()
{
    for (auto pRoot : m_RootBodys)
    {
        Safe_Release(pRoot);
    }
    m_RootBodys.clear();
    m_pModel = nullptr;
    m_pOwnerTransform = nullptr;
    __super::Free();

}
