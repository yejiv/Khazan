#include "SoftBody.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Model.h"
#include "SoftBody_Creator.h"

static inline bool IsFiniteVec3(const JPH::Vec3& v) {
    return std::isfinite(v.GetX()) && std::isfinite(v.GetY()) && std::isfinite(v.GetZ());
}

CSoftBody::CSoftBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CSoftBody::CSoftBody(const CSoftBody& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CSoftBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSoftBody::Initialize_Clone(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    SOFTBODY_DESC* pDesc = static_cast<SOFTBODY_DESC*>(pArg);

    m_pModel = pDesc->pModel;

    vector<_float3> Vertices = m_pModel->Get_VerticesPos(pDesc->iMeshIndex);
    vector<_uint> Indices = m_pModel->Get_Indices(pDesc->iMeshIndex);

    m_FixBoneMatrix = pDesc->FixBoneMatrix;

    Ref<SoftBodySharedSettings> pSkirtSettings =
        /*CSoftBody_Creator::CreateClothSoftBodySettings(pDesc->pClothData, pDesc->pSkeletonData);*/
        //CSoftBody_Creator::CreateClothSoftBody_Settings_independent(pDesc->pClothData);
        CSoftBody_Creator::CreateClothSoftBody_Settings_independentV2(Vertices, Indices);
      
    SoftBodyCreationSettings SoftBody(pSkirtSettings, LoadVec3(pDesc->vPos), LoadQuat(pDesc->vQuat), pDesc->iObjectLayer);
    SoftBody.mUpdatePosition = true;
    SoftBody.mMakeRotationIdentity = false;
    
    m_pBody = m_pGameInstance->CreateAndAdd_SoftBody(SoftBody, &m_pBodyInterface);
    m_BodyID = m_pBody->GetID();

    return S_OK;
}

void CSoftBody::Update_SoftBody_Pelvis(_float fTimeDelta, _matrix ParentMatrix)
{
    _matrix BoneMatrix = XMLoadFloat4x4(m_FixBoneMatrix);

    for (uint32_t i = 0; i < 3; i++)
        BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);

    m_WolrdBoneMatrix = BoneMatrix * ParentMatrix;

    Mat44 jPelvis = LoadMat44(m_WolrdBoneMatrix);
    Vec3  pos = jPelvis.GetTranslation();    
    Quat  rot = jPelvis.GetQuaternion();;

    RMat44 com = RMat44::sRotationTranslation(rot, pos);
   
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, com.GetTranslation(), com.GetQuaternion(), EActivation::Activate);
}

void CSoftBody::Set_PosRot(_vector vPos, _vector vRot)
{
    m_pBodyInterface->SetPositionAndRotation(m_BodyID, LoadVec3(vPos), LoadQuat(vRot), EActivation::Activate);
}

void CSoftBody::Set_Pos(_vector vPos)
{
    m_pBodyInterface->SetPosition(m_BodyID, LoadVec3(vPos), EActivation::Activate);
}

_vector CSoftBody::Get_Pos()
{
    Vec3 vPostion = m_pBodyInterface->GetPosition(m_BodyID);

    return XMVectorSet(vPostion.GetX(), vPostion.GetY(), vPostion.GetZ(), 1.f);
}

_vector CSoftBody::Get_Rot()
{
    Quat vRotation = m_pBodyInterface->GetRotation(m_BodyID);

    return XMVectorSet(vRotation.GetX(), vRotation.GetY(), vRotation.GetZ(), vRotation.GetW());
}

void CSoftBody::Collision_Active(_bool isActive)
{
    if (isActive)
    {
        if (!m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->AddBody(m_BodyID, EActivation::Activate);
        }
    }
    else
    {
        if (m_pBodyInterface->IsAdded(m_BodyID))
        {
            m_pBodyInterface->RemoveBody(m_BodyID);
        }
    }
}


const JPH::Array<Vec3> CSoftBody::ConvertToArrayVec3(CModel* pModel)
{
    JPH::Array<Vec3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(0);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(LoadVec3(ModelVertices[i]));

    return Vertices;
}

const JPH::Array<Vec3> CSoftBody::ConvertToArrayVec3(CModel* pModel, _uint iMeshIndex, const Vec3& vScale)
{
    JPH::Array<Vec3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(iMeshIndex);

    Vertices.reserve(ModelVertices.size());

    for (size_t i = 0; i < ModelVertices.size(); ++i)
    {
        _float3 v = ModelVertices[i];

        // Transform 스케일 적용 (ConvexHull은 ScaledShape 써도 되지만
        // 여기서 그냥 정점에 직접 반영하는 방식)
        v.x *= vScale.GetX();
        v.y *= vScale.GetY();
        v.z *= vScale.GetZ();

        Vertices.push_back(LoadVec3(v));
    }

    return Vertices;
}

JPH::Array<Vec3> CSoftBody::ConvertToHullPoints(
    CModel* pModel,
    _uint iMeshIndex,
    const Vec3& vScaleModel, // 이제 Transform 스케일 말고 "모델 고정 스케일"만
    Vec3& outCenter,
    _float& outNormalizeScale) // 정규화 스케일도 같이 반환
{
    JPH::Array<Vec3> points;

    vector<_float3> modelVerts = pModel->Get_VerticesPos(iMeshIndex);
    if (modelVerts.empty())
    {
        outCenter = Vec3::sZero();
        outNormalizeScale = 1.0f;
        return points;
    }

    points.reserve(modelVerts.size());

    // 1) 스케일 적용 + AABB 계산
    Vec3 minV = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    Vec3 maxV = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    outCenter = Vec3::sZero();
    for (auto& v : modelVerts)
    {
        Vec3 p(
            v.x * vScaleModel.GetX(),
            v.y * vScaleModel.GetY(),
            v.z * vScaleModel.GetZ()
        );

        if (!IsFiniteVec3(p))
            continue;

        outCenter += p;

        minV = Vec3::sMin(minV, p);
        maxV = Vec3::sMax(maxV, p);

        points.push_back(p);
    }

    if (points.empty())
    {
        outCenter = Vec3::sZero();
        outNormalizeScale = 1.0f;
        return points;
    }

    outCenter /= (float)points.size();

    // 2) 크기 측정
    Vec3 extents = maxV - minV;
    float maxExtent = max(extents.GetX(), max(extents.GetY(), extents.GetZ()));

    // 3) 정규화 스케일 계산 (Hull이 너무 큰 공간에서 안 돌아가게)
    //   - 예: targetSize = 10.0f (대략 10m 정도 공간에 들어오게)
    const float targetSize = 10.0f;
    outNormalizeScale = 1.0f;
    if (maxExtent > targetSize)
        outNormalizeScale = targetSize / maxExtent; // ex) 1000 -> 0.01 로 줄임

    // 4) center 기준/정규화/중복점 제거
    const float weldEps = 0.0001f;
    JPH::Array<Vec3> cleaned;
    cleaned.reserve(points.size());

    for (auto& p : points)
    {
        // 중심 기준 이동
        p -= outCenter;
        // 정규화 스케일 적용
        p *= outNormalizeScale;

        bool duplicate = false;
        for (auto& c : cleaned)
        {
            if ((p - c).LengthSq() < weldEps * weldEps)
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            cleaned.push_back(p);
    }

    return cleaned;
}

const JPH::Array<Float3> CSoftBody::ConvertToArrayFloat3(CModel* pModel, _uint iIndex)
{
    JPH::Array<Float3> Vertices;

    vector<_float3> ModelVertices = pModel->Get_VerticesPos(iIndex);

    for (size_t i = 0; i < ModelVertices.size(); ++i)
        Vertices.push_back(Float3(ModelVertices[i].x, ModelVertices[i].y, ModelVertices[i].z));

    return Vertices;
}

const JPH::Array<IndexedTriangle> CSoftBody::ConvertToArrayTri(CModel* pModel, _uint iIndex)
{
    JPH::Array<IndexedTriangle> Indices;

    vector<_uint> ModelIndices = pModel->Get_Indices(iIndex);

    for (size_t i = 0; i < ModelIndices.size(); i += 3)
        Indices.push_back(IndexedTriangle(ModelIndices[i], ModelIndices[i + 1], ModelIndices[i + 2]));

    return Indices;
}

CSoftBody* CSoftBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSoftBody* pInstance = new CSoftBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSoftBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CSoftBody::Clone(void* pArg)
{
    CSoftBody* pInstance = new CSoftBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CSoftBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CSoftBody::Free()
{
    if (!m_BodyID.IsInvalid() && m_pBodyInterface)
    {
        if (!m_BodyID.IsInvalid())
        {
            m_pGameInstance->Destroy_Body(m_BodyID);
            m_BodyID = BodyID();
            m_pBody = nullptr;
        }

        for (BodyID id : m_MeshBodyIDs)
        {
            if (!id.IsInvalid())
                m_pGameInstance->Destroy_Body(id);
        }
        m_MeshBodyIDs.clear();
    }
    __super::Free();

}
