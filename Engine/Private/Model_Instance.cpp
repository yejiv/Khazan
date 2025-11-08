#include "Model_Instance.h"

#include "Bone.h"
#include "MeshMaterial.h"
#include "Animation.h"

CModel_Instance::CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice ,pContext }
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& Prototype)
    : CComponent { Prototype }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }

    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }

    , m_iNumAnimations{ Prototype.m_iNumAnimations }

    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_strModelName{ Prototype.m_strModelName }
    , m_strModelFilePath{ Prototype.m_strModelFilePath }
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations)
    {
        CAnimation* pAnimation = pPrototypeAnimation->Clone();
        m_Animations.push_back(pAnimation);
        pAnimation->Set_TrackPositionPtr(&m_fCurrentTrackPosition);
    }

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}

HRESULT CModel_Instance::Initialize_Prototype(const _char* pModelFilePath, const CModelMesh_Instance::INSTANCE_DESC* pDesc)
{
    MODEL_DATA Data = {};

    ifstream ifs(pModelFilePath, std::ios::binary);
    if (!ifs.is_open())
    {
        MSG_BOX(TEXT("파일 열기 실패"));
        return E_FAIL;
    }

    Data.LoadBinary(ifs);

    ifs.close();

    m_strModelName = AnsiToWString(Data.strModelName);
    m_strModelFilePath = AnsiToWString(Data.strModelFilePath);

    m_eModelType = static_cast<MODELTYPE>(Data.iModelType);
    memcpy(&m_PreTransformMatrix, &Data.vPreTransformMatrix, sizeof(_float4x4));

    m_iNumMeshes = Data.iNumMeshes;
    m_iNumMaterials = Data.iNumMaterials;
    m_iNumAnimations = Data.iNumAnimations;

    CHECK_FAILED(Ready_Bones(Data), E_FAIL);

    CHECK_FAILED(Ready_Meshes(Data, pDesc), E_FAIL);

    CHECK_FAILED(Ready_Materials(Data), E_FAIL);

    CHECK_FAILED(Ready_Animations(Data), E_FAIL);

    return S_OK;
}

HRESULT CModel_Instance::Initialize_Clone(void* pArg)
{
    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        m_Meshes[i]->Initialize_Clone(pArg);
    }

    return S_OK;
}

HRESULT CModel_Instance::Render(_uint iMeshIndex)
{
    CHECK_FAILED(m_Meshes[iMeshIndex]->Bind_Resources(), E_FAIL);

    CHECK_FAILED(m_Meshes[iMeshIndex]->Render(), E_FAIL);

    return S_OK;
}

HRESULT CModel_Instance::Deferred_Render(_uint iMeshIndex, ID3D11DeviceContext* pDeferredContext)
{
    CHECK_FAILED(m_Meshes[iMeshIndex]->Deferred_Bind_Resources(pDeferredContext), E_FAIL);

    CHECK_FAILED(m_Meshes[iMeshIndex]->Deferred_Render(pDeferredContext), E_FAIL);

    return S_OK;
}

_float4x4* CModel_Instance::Get_BoneMatrix(const _char* pBoneName)
{
    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
        if (true == pBone->Compare_Name(pBoneName))
            return true;
        return false;
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

const _uint CModel_Instance::Get_NumInstances() const
{
    return m_Meshes[0]->Get_NumInstances();
}

HRESULT CModel_Instance::Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_iNumMaterials <= iMaterialIndex)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iIndex);
}

HRESULT CModel_Instance::Bind_Materials(class CDeferredShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_iNumMaterials <= iMaterialIndex)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, eTextureType, iIndex);
}

HRESULT CModel_Instance::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

_bool CModel_Instance::Play_Animation(_float fTimeDelta)
{
    m_isFinished = false;

    /* 현재 시간에 맞는 뼈의 상태대로 특정 뼈들의 TransformationMatrix를 갱신해준다. */
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta);


    /* 바꿔야할 뼈들의 Transforemation행렬이 갱신되었다면, 정점들에게 직접 전달되야할 CombindTransformationMatrix를 만들어준다. */
    for (auto& pBone : m_Bones)
    {
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
    }

    return m_isFinished;
}

void CModel_Instance::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (iIndex >= m_iNumAnimations)
        return;

    m_isLoop = isLoop;
    m_iCurrentAnimIndex = iIndex;
}

HRESULT CModel_Instance::Ready_Meshes(MODEL_DATA& Data, const CModelMesh_Instance::INSTANCE_DESC* pDesc)
{
    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        CModelMesh_Instance* pMesh = CModelMesh_Instance::Create(m_pDevice, m_pContext, m_eModelType, XMLoadFloat4x4(&m_PreTransformMatrix), Data.vecMeshes[i], pDesc);
        if (pMesh == nullptr)
        {
            MSG_BOX(TEXT("비상 CMesh::Create() 실패!!!!!!"));
            return E_FAIL;
        }
        m_Meshes.push_back(pMesh);
    }

    return S_OK;
}

HRESULT CModel_Instance::Ready_Materials(MODEL_DATA& Data)
{
    for (_uint i = 0; i < m_iNumMaterials; ++i)
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, Data.vecMaterials[i]);
        CHECK_NULLPTR(pMeshMaterial, E_FAIL);

        m_Materials.push_back(pMeshMaterial);
    }

    return S_OK;
}

HRESULT CModel_Instance::Ready_Bones(MODEL_DATA& Data)
{
    for (auto bone : Data.vecBones)
    {
        CBone* pBone = CBone::Create(bone);
        CHECK_NULLPTR(pBone, E_FAIL);

        m_Bones.push_back(pBone);
    }

    return S_OK;
}

HRESULT CModel_Instance::Ready_Animations(MODEL_DATA& Data)
{
    if (m_eModelType == MODELTYPE::NONANIM)
        return S_OK;

    for (_uint i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_Bones, Data.vecAnimation[i], i);
        CHECK_NULLPTR(pAnimation, E_FAIL);

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}

CModel_Instance* CModel_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const CModelMesh_Instance::INSTANCE_DESC* pDesc)
{
    CModel_Instance* pInstance = new CModel_Instance(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pDesc)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel_Instance::Clone(void* pArg)
{
    CModel_Instance* pInstance = new CModel_Instance(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel_Instance"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel_Instance::Free()
{
    __super::Free();

    for (auto& pAnimation : m_Animations)
        Safe_Release(pAnimation);

    m_Animations.clear();

    for (auto& pBone : m_Bones)
        Safe_Release(pBone);

    m_Bones.clear();

    for (auto& pMesh : m_Meshes)
        Safe_Release(pMesh);

    m_Meshes.clear();

    for (auto& pMaterial : m_Materials)
        Safe_Release(pMaterial);

    m_Materials.clear();
}
