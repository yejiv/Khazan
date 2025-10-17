
#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "MeshMaterial.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice ,pContext }
{
}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
    , m_iNumMeshes { Prototype.m_iNumMeshes }
    , m_Meshes { Prototype.m_Meshes }

    , m_iNumMaterials { Prototype.m_iNumMaterials }
    , m_Materials { Prototype.m_Materials }

    , m_iNumAnimations{ Prototype.m_iNumAnimations }

    , m_eModelType { Prototype.m_eModelType} 
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_strModelName{ Prototype.m_strModelName }
    , m_strModelFilePath{ Prototype.m_strModelFilePath }
   // , m_iRootBoneIndex(Prototype.m_iRootBoneIndex)
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations) {
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


HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
{
    /* aiProcess_PreTransformVertices : 각각의 메시를 붙여야할 위치에 적절히 배치한다. */
    /* 배치 : 각 메시의 정점들을 배치를 위한 임의의 행렬과 곱하여 로드한다. */

    MODEL_DATA data;

    ifstream ifs(pModelFilePath, std::ios::binary);
    if (!ifs.is_open())
    {
        MSG_BOX(TEXT("파일 열기 실패"));
        return E_FAIL;
    }

    data.LoadBinary(ifs);

    ifs.close();
    
#ifdef _DEBUG
    OutputDebugStringA(("모델 이름 : " + data.strModelName + "\n").c_str());
#endif // _DEBUG

    m_strModelName = AnsiToWString(data.strModelName);
    m_strModelFilePath = AnsiToWString(data.strModelFilePath);
    m_eModelType = static_cast<MODELTYPE>(data.iModelType);
    memcpy(&m_PreTransformMatrix, &data.vPreTransformMatrix, sizeof(_float4x4));
    m_iNumMeshes = data.iNumMeshes;
    m_iNumMaterials = data.iNumMaterials;
    m_iNumAnimations = data.iNumAnimations;


    if (FAILED(Ready_Bones(data)))
        return E_FAIL;

    if (FAILED(Ready_Meshes(data)))
        return E_FAIL;

    if (FAILED(Ready_Materials(data)))
        return E_FAIL;

    if (FAILED(Ready_Animations(data)))
        return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
	return S_OK;
}

_float4x4* CModel::Get_BoneMatrix(const _char* pBoneName)
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

_float4x4* CModel::Get_ContainNameBoneMatrix(const _char* pBoneName)
{
    auto iter = find_if(m_Bones.begin(), m_Bones.end(),
        [&](CBone* pBone) {
            if (pBone->Contains_Name(pBoneName)) return true;
            return false;
        });

    if (iter == m_Bones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

vector<_float3> CModel::Get_VerticesPos(_uint iIndex)
{
    return m_Meshes[iIndex]->Get_VerticesPos();
}

vector<_uint> CModel::Get_Indices(_uint iIndex)
{
    return m_Meshes[iIndex]->Get_Indices();
}

HRESULT CModel::Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    _uint       iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

    if (m_iNumMaterials <= iMaterialIndex)
        return E_FAIL;

    return m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, iTextureType, iIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);
}

_bool CModel::Play_Animation(_float fTimeDelta)
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

HRESULT CModel::Render(_uint iMeshIndex)
{    
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;
  
    return S_OK;
}

void CModel::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (iIndex >= m_iNumAnimations)
        return;

    m_isLoop = isLoop;
    m_iCurrentAnimIndex = iIndex;
}

HRESULT CModel::Ready_Meshes(MODEL_DATA& data)
{

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, XMLoadFloat4x4(&m_PreTransformMatrix), data.vecMeshes[i]);
		if (pMesh == nullptr)
		{
			MSG_BOX(TEXT("비상 CMesh::Create() 실패!!!!!!"));
			return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(MODEL_DATA& data)
{

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, data.vecMaterials[i]);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(MODEL_DATA& data)
{
    for (auto bone : data.vecBones)
    {
        CBone* pBone = CBone::Create(bone);
        if (pBone == nullptr)
            return E_FAIL;

        m_Bones.push_back(pBone);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations(MODEL_DATA& data)
{
    if (m_eModelType == MODELTYPE::NONANIM)
        return S_OK;

    for (_uint i = 0; i < m_iNumAnimations; i++)
    {
        CAnimation* pAnimation = CAnimation::Create(m_Bones, data.vecAnimation[i], i);
        if (nullptr == pAnimation)
            return E_FAIL;

        m_Animations.push_back(pAnimation);
    }

    return S_OK;
}


CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype( pModelFilePath)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CModel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
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


   // m_Importer.FreeScene();

}
