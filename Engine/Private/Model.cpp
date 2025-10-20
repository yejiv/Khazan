
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
    , m_iRootBoneIndex{ Prototype.m_iRootBoneIndex }
    , m_AnimationsSetup{Prototype.m_AnimationsSetup }
    , m_AnimationSets {Prototype.m_AnimationSets }
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

    _char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());

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
    m_AnimationSets = data.vecAnimationSets;
 
    if (FAILED(Ready_Bones(data)))
        return E_FAIL;

    if (FAILED(Ready_Meshes(data)))
        return E_FAIL;

    if (FAILED(Ready_Materials(data)))
        return E_FAIL;

    if (FAILED(Ready_Animations(data)))
        return E_FAIL;

    if (m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CBone* pBone) {
                if (pBone->Compare_Name("Root"))
                    return true;
                return false;
            });

        if (it == m_Bones.end())
        {
            OutputDebugStringA(("!!!!!!!!!!!!!!! 루트본 못 찾음!!!!!!!!!!!!!!!!!!!!!!!!!"));
        }
        else
            m_iRootBoneIndex = static_cast<_uint>(distance(m_Bones.begin(), it));
    }


    /* 피오나용 */
#ifdef _DEBUG
    if (m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CBone* pBone) {
                if (pBone->Compare_Name("Bip001"))
                    return true;
                return false;
            });

        if (it == m_Bones.end())
        {
            OutputDebugStringA(("!!!!!!!!1!!!!!!! 루트본 못 찾음!!!!!!!!!!!!!!!!!!!!!!!!!"));
        }
        else
            m_iRootBoneIndex = static_cast<_uint>(distance(m_Bones.begin(), it));
    }
#endif // _DEBUG

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

    /* 애니메이션 세트  */
    if (Has_State(ANIMSET_NEXT))
    {
        Set_Animation(m_AnimationSets[m_iCurrentAnimSetsIndex].vecAnimIndices[m_iCurrentAnimSetIndex]);
        Remove_State(ANIMSET_NEXT);
    }

    /* 애니메이션 변경 */
    if (Has_State(CHANGE_ANIMATION))
	{
        /* 루트 모션 체크 */
		Check_RootMotion();

        /* 완료 대기 여부 체크*/
        Check_WaitForComplete();

        /* 애니메이션 블랜딩할 이전 애니메이션 뼈 넘겨주기 */
		m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));

		Remove_State(CHANGE_ANIMATION);
	}

    /* 애니메이션의 현재 시간에 맞는 뼈의 상태대로 특정 뼈들을 갱신*/
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, Has_State(USED_ANIM_LOOP), Has_State(ANIM_LOOP), &m_isFinished, fTimeDelta);

    /* 정점들에게 직접 전달되어야할 매트릭스 만들기 */
    for (auto& pBone : m_Bones)
        pBone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
    
    /* 루트모션 진행 */
    if (Has_State(ROOTMOTION))
        Update_RootMotion(fTimeDelta);

    /* 대기중인 애니메이션이 있으면 실행 */
    if (Has_State(WAITFORCOMPLETE) && m_isFinished)
    {
        Remove_State(WAITFORCOMPLETE);
        if (m_OnWaitForComplete != nullptr) {
            m_OnWaitForComplete();
            m_OnWaitForComplete = nullptr;
            return m_isFinished;
        }
    }

    /* 애니메이션 세트 다음동작 및 끝났는지 */
    if (Has_State(ANIMSET_PLAYING) && m_isFinished)
    {
        ++m_iCurrentAnimSetIndex;
        if (m_iCurrentAnimSetIndex == m_iCurrentAnimSetsMaxIndex)
        {
            Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT);
            return true;
        }

        Add_State(ANIMSET_NEXT);
        return false;
    }

    return m_isFinished;
}

void CModel::Set_Animation(_uint iIndex)
{
    if (iIndex >= m_iNumAnimations)
        return;
    
    if (Has_State(WAITFORCOMPLETE))
    {
        m_iReserveAnimIndex = iIndex;

        OnWaitForComplete([this]() {
            Set_Animation(m_iReserveAnimIndex);
            });

        return;
    }

    m_iPrevAnimIndex = m_iCurrentAnimIndex;
    m_iCurrentAnimIndex = iIndex;


    if (m_iPrevAnimIndex >= 0 && m_iCurrentAnimIndex != m_iPrevAnimIndex) {
        if(!Has_State(ANIMSET_PLAYING)) Clear_State();
        Add_State(CHANGE_ANIMATION);
    }
}

void CModel::Set_AnimationSet(const string& strKey)
{
    m_iCurrentAnimSetIndex = 0;

    vector<ANIMATION_SET_DATA>::iterator iter = find_if(m_AnimationSets.begin(), m_AnimationSets.end(), [&strKey](const ANIMATION_SET_DATA& set) {
        return set.strAnimSetName == strKey;
        });

    if (iter != m_AnimationSets.end())
    {
        Clear_State();
        Add_State(ANIMSET_PLAYING | ANIMSET_NEXT);

        m_iCurrentAnimSetsIndex = static_cast<_uint>(distance(m_AnimationSets.begin(), iter));
        m_iCurrentAnimSetsMaxIndex = static_cast<_uint>(iter->vecAnimIndices.size());
    }
    else
    {
        Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT );
        m_iCurrentAnimSetsIndex = m_iCurrentAnimSetsMaxIndex = { 0 };
        OutputDebugStringA(("[CModel::Set_AnimationSet() Error] Invalid Animation Set Key: " + strKey + "\n").c_str());
    }
}


void CModel::Set_AnimationLoop(_bool isLoop)
{
    Add_State(USED_ANIM_LOOP);

    if(isLoop)
        Add_State(ANIM_LOOP);
}


HRESULT CModel::Render(_uint iMeshIndex)
{    
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;
  
    return S_OK;
}

void CModel::Check_RootMotion()
{
	if (m_AnimationsSetup[m_iCurrentAnimIndex].isRootMotion) {

		Add_State(ROOTMOTION);

		if (m_AnimationsSetup[m_iCurrentAnimIndex].isApplyRootPosition)
			Add_State(ROOTMOTION_POSITION);

		if (m_AnimationsSetup[m_iCurrentAnimIndex].isApplyRootRotation)
			Add_State(ROOTMOTION_ROTATION);

		FLOAT3_DATA scale = m_AnimationsSetup[m_iCurrentAnimIndex].RootMitionScale;
        //m_vRootMotionScale = XMVectorSet(scale.x, scale.y, scale.z, 1.f);
        m_vRootMotionScale = XMVectorSet(1.f, 1.f, 1.f, 1.f);

		m_fCurrentRootMotionBlendTime = { 0.f };

		_float blendIn = m_AnimationsSetup[m_iCurrentAnimIndex].fBlendInTime;
		_float blendOut = m_AnimationsSetup[m_iCurrentAnimIndex].fBlendOutTime;

		if (blendIn > 0.f && blendOut > 0.f)
			m_fRootMotionBlendTime = (blendIn + blendOut) / 2.f;
		else if (blendIn > 0.f)
			m_fRootMotionBlendTime = blendIn;
		else if (blendOut > 0.f)
			m_fRootMotionBlendTime = blendOut;
		else
			m_fRootMotionBlendTime = m_fBaseRootMotionBlendTime;

		m_PreRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();
	}
    else {
        // 루트 모션을 사용하지 않는 경우 상태 제거
        Remove_State(ROOTMOTION | ROOTMOTION_POSITION | ROOTMOTION_ROTATION);
    }
}

void CModel::Update_RootMotion(_float fTimeDelta)
{
    _matrix CurrentRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    m_fCurrentRootMotionBlendTime += fTimeDelta;

    if (m_fCurrentRootMotionBlendTime >= m_fRootMotionBlendTime)
        Remove_State(ROOTMOTION | ROOTMOTION_POSITION | ROOTMOTION_ROTATION);
    else
    {
        _float fRatio = m_fCurrentRootMotionBlendTime / m_fRootMotionBlendTime;

        _vector vCurrentScale = XMVectorSet(
            XMVectorGetX(XMVector3Length(CurrentRootMatrix.r[0])),
            XMVectorGetX(XMVector3Length(CurrentRootMatrix.r[1])),
            XMVectorGetX(XMVector3Length(CurrentRootMatrix.r[2])),
            1.f
        );

        //위치 적용
        if (Has_State(ROOTMOTION_POSITION))
        {
            _vector vCurrentPos = CurrentRootMatrix.r[3];
            _vector vPrePos = m_PreRootMatrix.r[3];

            _vector vLerpedPos = XMVectorLerp(vPrePos, vCurrentPos, fRatio);
            _vector vDelta = XMVectorSubtract(vLerpedPos, vPrePos);
            vDelta = XMVectorMultiply(vDelta, m_vRootMotionScale);
            _vector vFinalPos = XMVectorAdd(vPrePos, vDelta);

            CurrentRootMatrix.r[3] = vFinalPos;
        }
        else
        {
            CurrentRootMatrix.r[3] = m_PreRootMatrix.r[3];
        }

        //회전 적용
        //if (Has_State(ROOTMOTION_ROTATION))
        //{
        //    _vector vCurrentQuat = XMQuaternionRotationMatrix(CurrentRootMatrix);
        //    _vector vPreQuat = XMQuaternionRotationMatrix(m_PreRootMatrix);
        //    _vector vLerpedQuat = XMQuaternionSlerp(vPreQuat, vCurrentQuat, fRatio);

        //    _matrix RotationMatrix = XMMatrixRotationQuaternion(vLerpedQuat);
        //    CurrentRootMatrix.r[0] = RotationMatrix.r[0];
        //    CurrentRootMatrix.r[1] = RotationMatrix.r[1];
        //    CurrentRootMatrix.r[2] = RotationMatrix.r[2];
        //}

         // 회전 적용
        if (Has_State(ROOTMOTION_ROTATION))
        {
            _matrix NormalizedCurrent;
            NormalizedCurrent.r[0] = XMVector3Normalize(CurrentRootMatrix.r[0]);
            NormalizedCurrent.r[1] = XMVector3Normalize(CurrentRootMatrix.r[1]);
            NormalizedCurrent.r[2] = XMVector3Normalize(CurrentRootMatrix.r[2]);
            NormalizedCurrent.r[3] = g_XMIdentityR3;

            _matrix NormalizedPrev;
            NormalizedPrev.r[0] = XMVector3Normalize(m_PreRootMatrix.r[0]);
            NormalizedPrev.r[1] = XMVector3Normalize(m_PreRootMatrix.r[1]);
            NormalizedPrev.r[2] = XMVector3Normalize(m_PreRootMatrix.r[2]);
            NormalizedPrev.r[3] = g_XMIdentityR3;

            _vector vCurrentQuat = XMQuaternionRotationMatrix(NormalizedCurrent);
            _vector vPreQuat = XMQuaternionRotationMatrix(NormalizedPrev);
            _vector vLerpedQuat = XMQuaternionSlerp(vPreQuat, vCurrentQuat, fRatio);

            _matrix RotationMatrix = XMMatrixRotationQuaternion(vLerpedQuat);

            // 회전에 스케일 다시 적용
            CurrentRootMatrix.r[0] = XMVectorScale(RotationMatrix.r[0], XMVectorGetX(vCurrentScale));
            CurrentRootMatrix.r[1] = XMVectorScale(RotationMatrix.r[1], XMVectorGetY(vCurrentScale));
            CurrentRootMatrix.r[2] = XMVectorScale(RotationMatrix.r[2], XMVectorGetZ(vCurrentScale));
        }
        else
        {
            // 회전 적용 안 할 경우에도 스케일 유지
            CurrentRootMatrix.r[0] = XMVectorScale(XMVector3Normalize(m_PreRootMatrix.r[0]), XMVectorGetX(vCurrentScale));
            CurrentRootMatrix.r[1] = XMVectorScale(XMVector3Normalize(m_PreRootMatrix.r[1]), XMVectorGetY(vCurrentScale));
            CurrentRootMatrix.r[2] = XMVectorScale(XMVector3Normalize(m_PreRootMatrix.r[2]), XMVectorGetZ(vCurrentScale));
        }

    
        m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(CurrentRootMatrix);
    }
}

void CModel::Check_WaitForComplete()
{
    if (m_AnimationsSetup[m_iCurrentAnimIndex].isWaitForComplete)
        Add_State(WAITFORCOMPLETE);
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
		m_Meshes.emplace_back(pMesh);
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

        m_Materials.emplace_back(pMeshMaterial);
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

        m_Bones.emplace_back(pBone);
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

        m_Animations.emplace_back(pAnimation);
        m_AnimationsSetup.emplace_back(data.vecAnimation[i].animSetup);
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
