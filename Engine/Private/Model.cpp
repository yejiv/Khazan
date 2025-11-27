
#include "Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "MeshMaterial.h"
#include "Animation.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice ,pContext }
{
}

CModel::CModel(const CModel& Prototype)
    : CComponent{ Prototype }
    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }
    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }
    , m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_strModelName{ Prototype.m_strModelName }
    , m_strModelFilePath{ Prototype.m_strModelFilePath }
    , m_RootMotionInfo{ Prototype.m_RootMotionInfo }
    , m_AnimationsSetup{ Prototype.m_AnimationsSetup }
    , m_AnimationSets{ Prototype.m_AnimationSets }
    , m_iRootBoneIndex{ Prototype.m_iRootBoneIndex }
    //, m_AnimationSetInfo{ Prototype.m_AnimationSetInfo }
{
    for (auto& pPrototypeAnimation : Prototype.m_Animations) {
        CAnimation* pAnimation = pPrototypeAnimation->Clone();
        m_Animations.push_back(pAnimation);
        pAnimation->Set_TrackPositionPtr(&m_fCurrentTrackPosition);
        pAnimation->Set_RootBoneIndex(m_iRootBoneIndex);

    }
    for (auto& pPrototypeBone : Prototype.m_Bones) {
        m_Bones.push_back(pPrototypeBone->Clone());
    }

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);
}


HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, _bool isSRVCache)
{
    _char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    //OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());

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
    _vector vScale, vRot, vTrans;
    XMMatrixDecompose(&vScale, &vRot, &vTrans, XMLoadFloat4x4(&m_PreTransformMatrix));
    m_RootMotionInfo.vPreTransformQuat = vRot;

    if (FAILED(Ready_Bones(data)))
        return E_FAIL;

    if (FAILED(Ready_Meshes(data)))
        return E_FAIL;

    if (FAILED(Ready_Materials(data, isSRVCache)))
        return E_FAIL;

    if (FAILED(Ready_Animations(data)))
        return E_FAIL;

    _bool isFindRoot = { false };

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
        else {
            m_iRootBoneIndex = static_cast<_uint>(distance(m_Bones.begin(), it));
            isFindRoot = true;
        }
    }


    /* 투사체용 */
    if (!isFindRoot && m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CBone* pBone) {
                if (pBone->Compare_Name("B_Rock"))
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

    return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{
    return S_OK;
}

_float CModel::MakeRatio()
{
    return m_Animations[m_iCurrentAnimIndex]->MakeRatio();
}

void CModel::Set_BlendTime(_float fBlendTime)
{
     m_Animations[m_iCurrentAnimIndex]->Set_AnimBlendTime(fBlendTime); 
}

_uint CModel::Get_BoneIndex(const _char* pBoneName)
{
    auto    iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone) {
        if (true == pBone->Compare_Name(pBoneName))
            return true;
        return false;
        });

    if (iter == m_Bones.end())
        return 0;

    return static_cast<_uint>(distance(m_Bones.begin(), iter));
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

_float4x4* CModel::Get_BoneMatrix(const _int iBoneIndex)
{
    if (m_Bones.size() > iBoneIndex || 0 < iBoneIndex)
        return nullptr;

    return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrixPtr();
}

_float4x4* CModel::Get_LocalBoneMatrix(_int iBoneIndex)
{
    if (m_Bones.size() < iBoneIndex || 0 > iBoneIndex)
        return nullptr;

    return m_Bones[iBoneIndex]->Get_TransformationMatrixPtr();
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

_int CModel::Get_AnimIndexByName(const string& strName)
{
    auto iter = find_if(m_AnimationsSetup.begin(), m_AnimationsSetup.end(),
        [&](ANIMATION_SETUP_DATA& data) {
            return data.strName == strName;
        });

    if (iter != m_AnimationsSetup.end())
        return static_cast<_int>(std::distance(m_AnimationsSetup.begin(), iter));

    return -1; // 못 찾을 경우
}

const vector<_int>& CModel::Get_ChildIndices(_int boneIndex) const
{
    return m_Bones[boneIndex]->Get_ChildBones();
}

void CModel::Set_OwnerTransform(CTransform** pTransform)
{
    m_pOwnerTransform = *pTransform;
    //Safe_AddRef(m_pOwnerTransform);
}

_vector CModel::Get_BoneWorldRotationQuat(_int iBone) const
{
    if (iBone < 0 || iBone >= (_int)m_Bones.size())
        return XMQuaternionIdentity();

    const _matrix W = m_Bones[iBone]->Get_CombinedTransformationMatrix();
    return ExtractQuatFromWorld(W);
}

void CModel::Set_BoneLocalRotation(_int iBone, _vector vLocal)
{
    if (iBone < 0 || iBone >= (_int)m_Bones.size()) return;
    CBone* pBone = m_Bones[iBone];

    _vector S, Q, T;
    XMMatrixDecompose(&S, &Q, &T, pBone->Get_TransformationMatrix());

    _matrix L = XMMatrixAffineTransformation(
        S, XMVectorZero(),
        XMQuaternionNormalize(vLocal),
        T);

    pBone->Set_TransformationMatrix(L);
}

void CModel::Set_BoneWorldRotation(_int iBone, _vector vWorld)
{
    if (iBone < 0 || iBone >= (_int)m_Bones.size()) return;

    const _int pIdx = m_Bones[iBone]->Get_ParentBoneIndex();
    _vector qLocal = XMQuaternionNormalize(vWorld);

    if (pIdx >= 0)
    {
        _vector qParentW = Get_BoneWorldRotationQuat(pIdx);
        // local = inv(parentW) * world
        qLocal = XMQuaternionMultiply(XMQuaternionConjugate(qParentW), vWorld);
    }

    Set_BoneLocalRotation(iBone, qLocal);
}

_float CModel::Get_CurDuration()
{
    return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
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

HRESULT CModel::Bind_Materials(class CDeferredShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iIndex)
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

    // 파츠는 마스터의 본을 사용
    if (m_isSharedSkeleton && m_pMasterSkeleton != nullptr)
    {
        // 마스터 본으로 바인딩 (Mesh 내부에서 본 이름으로 매칭)
        Update_PartLocalBones();
        return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_pMasterSkeleton->m_Bones, &m_PartLocalBoneMatrices);
    }
    return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones, nullptr);
}

HRESULT CModel::Bind_PrevBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    // 파츠는 마스터의 본을 사용
    if (m_isSharedSkeleton && m_pMasterSkeleton != nullptr)
    {
        // 마스터 본으로 바인딩 (Mesh 내부에서 본 이름으로 매칭)
        //  Update_PartLocalBones();
        return m_Meshes[iMeshIndex]->Bind_PrevBoneMatrices(pShader, pConstantName, m_pMasterSkeleton->m_CachedPrevBoneMatrices, &m_PartLocalBoneMatrices);
    }
    return m_Meshes[iMeshIndex]->Bind_PrevBoneMatrices(pShader, pConstantName, m_CachedPrevBoneMatrices, nullptr);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
    /* 파츠들은 애니메이션 안돌림 */
    //if (m_pMasterSkeleton != nullptr )
    //    return false;
    m_isAnimStart = true;
    m_isFinished = false;

    /* 애니메이션 세트  */
    if (Has_State(ANIMSET_NEXT))
    {
        if (m_AnimationSetInfo.iCurrentIndex >= static_cast<_int>(m_AnimationSets[m_AnimationSetInfo.iSelectedAnimIndex].vecAnimIndices.size()))
            m_AnimationSetInfo.iCurrentIndex = 0;
        Set_Animation(m_AnimationSets[m_AnimationSetInfo.iSelectedAnimIndex].vecAnimIndices[m_AnimationSetInfo.iCurrentIndex]);
        Remove_State(ANIMSET_NEXT);
    }

    /* 애니메이션 변경 */
    if (Has_State(CHANGE_ANIMATION))
    {
        /* 애니메이션 블랜딩할 이전 애니메이션 뼈 넘겨주기 */
        if (true == m_isBlendEnable)
            m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));
        Remove_State(CHANGE_ANIMATION);
        Add_State(FIRST_FRAME_ANIMATION);
    }
    //if (sdfsdfsdfsd) { 
    //    m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));
    //    Add_State(FIRST_FRAME_ANIMATION);
    //}

    m_fPrevTrackPosition = m_fCurrentTrackPosition;

    // 애니메이션 업데이트
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, Has_State(USED_ANIM_LOOP), Has_State(ANIM_LOOP), &m_isFinished, fTimeDelta);

    //루프 애니메이션 반복 감지 시 첫 프레임인지 확인 
    if (m_AnimationsSetup[m_iCurrentAnimIndex].isLoop && m_fPrevTrackPosition > m_fCurrentTrackPosition && m_fCurrentTrackPosition < 0.1f)
        Add_State(FIRST_FRAME_ANIMATION);

    for (_int i = 0; i < static_cast<_int>(m_Bones.size()); ++i)
    {
        if (i == m_iRootBoneIndex)
        {
            /*  포지션 고정 옵션*/
            if (Has_State(IGNORE_ROOT_POS) || Has_AllStates(FIRST_FRAME_ANIMATION | IGNORE_ROOT_POS_FIRSTFRAME))
            {
                _matrix rootMat = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
                rootMat.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(rootMat);
            }

            /* 회전 고정 옵션*/
            if (Has_State(IGNORE_ROOT_ROT))
            {
                _matrix rootMat = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
                rootMat.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
                rootMat.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                rootMat.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
                m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(rootMat);

            }
        }

        /*  =========== 뼈들 컴바인드 =========== */
        m_Bones[i]->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

        /* 루트 모션 처리 */
        if (i == m_iRootBoneIndex && Has_State(ROOTMOTION_POSITION))
        {
            /* 첫 프레임은  정보 저장 */
            if (Has_State(FIRST_FRAME_ANIMATION))
            {
                _matrix currentCombinedRoot = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

                // 애니메이션 절대 좌표 사용시 (위치가 프레임 단위로 엄청 크게 변하는 애니메이션일 때 (ex 앞으로 나가는 콤보공격 애니메이션 2번째부터 이용하는 ))
                if (Has_State(ABSOLUTE_ROOT_POS))
                    m_RootMotionInfo.vFirstFrameOffset = currentCombinedRoot.r[3];
                else
                    m_RootMotionInfo.vFirstFrameOffset = XMVectorZero();

                /* 이전-현재 차이 값 변수 초기화 */
                m_RootMotionInfo.matDeltaRootMotion = XMMatrixIdentity();

                // preRoot 회전값 유지, 위치는 0
                // 첫 프레임 - 컴바인드된 루트본에서 포지션만 원점으로
                m_RootMotionInfo.matPreRootMotion = currentCombinedRoot;
                m_RootMotionInfo.matPreRootMotion.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(m_RootMotionInfo.matPreRootMotion);

                Remove_State(FIRST_FRAME_ANIMATION);
            }
            else if (Has_State(ROOTMOTION))
            {
                /* 루트모션 진행 */
                Update_RootMotion(fTimeDelta);
            }
        }

    }

    /* 이벤트 체크 */
    Check_Event(fTimeDelta);

    // 모든 본의 Combined Marix가 계산 완료된 상태
    if (m_isMaterSkeleton)
        Cache_CurrentBoneMatrices();

    /* Owner에 Transform 적용 */
    if (m_pOwnerTransform && Has_State(ROOTMOTION))
        Apply_RootMotion_To_Transform();


    if (m_isFinished)
    {

        if (Has_State(ANIM_LOOP))
            Reset_EventTrigger();

        /* 대기중인 애니메이션이 있으면 실행 - Set_Animation(예약 인덱스)*/
        if (Has_State(WAITFORCOMPLETE))
        {
            Remove_State(WAITFORCOMPLETE);
            if (m_OnWaitForComplete != nullptr) {
                m_OnWaitForComplete();
                m_OnWaitForComplete = nullptr;
                return m_isFinished;
            }
        }

        /* 애니메이션 세트 다음동작 및 끝났는지 */
        if (Has_State(ANIMSET_PLAYING))
        {
            ++m_AnimationSetInfo.iCurrentIndex;
            Add_State(ANIMSET_NEXT);

            if (m_AnimationSetInfo.iCurrentIndex == m_AnimationSetInfo.iTotalCount)
            {
                Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT);
                return true;
            }


            return false;
        }

    }

    //#ifdef _DEBUG
    //    static int frameCount = 0;
    //    if (++frameCount % 60 == 0) // 60프레임마다
    //    {
    //        OutputDebugStringA("=== Bone Matrix Debug ===\n");
    //
    //        // 루트 본 Combined 행렬 확인
    //        _matrix rootCombined = m_Bones[0]->Get_CombinedTransformationMatrix();
    //        _vector scale, rot, pos;
    //        if (XMMatrixDecompose(&scale, &rot, &pos, rootCombined))
    //        {
    //            _float3 scaleF;
    //            XMStoreFloat3(&scaleF, scale);
    //            char buffer[256];
    //            sprintf_s(buffer, "Root Combined Scale: (%.6f, %.6f, %.6f)\n",
    //                scaleF.x, scaleF.y, scaleF.z);
    //            OutputDebugStringA(buffer);
    //        }
    //    }
    //#endif

    return m_isFinished;
}

void CModel::Set_Animation(_uint iIndex)
{
    //if (m_fCurrentTrackPosition < 4.f) 
    //{
    //    /* 같은 애니메이션이 들어오면 pass*/
    //    if (m_iCurrentAnimIndex == iIndex)
    //        return;

    //    // 블렌딩 없이 바로 전환
    //    m_iCurrentAnimIndex = iIndex;
    //    m_fCurrentTrackPosition = 0.f;
    //    return;
    //}

    if (m_iCurrentAnimIndex == iIndex && m_AnimationsSetup[m_iCurrentAnimIndex].isLoop)
        return;

    if (iIndex >= m_iNumAnimations || m_iNumAnimations <= 0)
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

    if (!Has_State(ANIMSET_PLAYING))
        Clear_State();

    Add_State(CHANGE_ANIMATION);

    /*  루트 모션 옵션 설정 */
    Check_RootMotion();

    /* 완료 대기 여부 체크*/    
    Check_WaitForComplete();

    /* 이벤트 설정 */
    Setup_Events();

    m_isAnimStart = false; 

    if (m_strModelName == L"Khazan_Spear" || m_strModelName == L"Khazan_GSword") {
        cout << m_iCurrentAnimIndex << " : " << Get_CurAnimName() <<  endl;

    }

}

void CModel::Set_AnimationSet(const string& strKey)
{
    m_AnimationSetInfo.iCurrentIndex = 0;

    vector<ANIMATION_SET_DATA>::iterator iter = find_if(m_AnimationSets.begin(), m_AnimationSets.end(), [&strKey](const ANIMATION_SET_DATA& set) {
        return set.strAnimSetName == strKey;
        });

    if (iter != m_AnimationSets.end())
    {
        Clear_State();
        Add_State(ANIMSET_PLAYING | ANIMSET_NEXT);

        m_AnimationSetInfo.iSelectedAnimIndex = static_cast<_uint>(distance(m_AnimationSets.begin(), iter));
        m_AnimationSetInfo.iTotalCount = static_cast<_uint>(iter->vecAnimIndices.size());
#ifdef _DEBUG
        OutputDebugStringA(("[CModel::Set_AnimationSet() Success] Animation Set Key: " + strKey + "\n").c_str());
#endif // _DEBUG

    }
    else
    {
        Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT);
#ifdef _DEBUG
        OutputDebugStringA(("[CModel::Set_AnimationSet() Error] Invalid Animation Set Key: " + strKey + "\n").c_str());
#endif // _DEBUG

    }
}


void CModel::Set_AnimationLoop(_bool isLoop)
{
    Add_State(USED_ANIM_LOOP);

    if (isLoop)
        Add_State(ANIM_LOOP);
}

_bool CModel::Check_MinAnimationTime()
{
 
    if (m_AnimationsSetup[m_iCurrentAnimIndex].fBlendOutTime < 1.f )
        return true;
    //if (m_fCurrentTrackPosition >= m_Animations[m_iCurrentAnimIndex]->Get_Duration())
    //    return false;
    return m_AnimationsSetup[m_iCurrentAnimIndex].fBlendOutTime <= m_fCurrentTrackPosition;
}

void CModel::AnimationSetIndexIncrease()
{
    ++m_AnimationSetInfo.iCurrentIndex;
    Add_State(ANIMSET_NEXT);

    if (m_AnimationSetInfo.iCurrentIndex == m_AnimationSetInfo.iTotalCount)
    {
        Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT);
    }
}

void CModel::AnimationLoop(_bool isLoop)
{
    Add_State(USED_ANIM_LOOP);
    
    isLoop ? Add_State(ANIM_LOOP) : Remove_State(ANIM_LOOP);
}

void CModel::Set_AnimBlendTime(_uint iAnimIndex, _float fBlendTime)
{
    m_Animations[iAnimIndex]->Set_AnimBlendTime(fBlendTime);
}


void CModel::Update_BoneCombinedMatrices()
{
    //if (m_isSharedSkeleton)
    //	return;

    for (auto bone : m_Bones)
        bone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

}

void CModel::Register_Event(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType, function<void()> OnEvent)
{
    m_EventCallbacks[MakeCallbackKey(strEventKey, eTriggerType)] = OnEvent;
}

void CModel::UnRegister_Event(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType)
{
    auto it = m_EventCallbacks.find(MakeCallbackKey(strEventKey, eTriggerType));
    if (it != m_EventCallbacks.end())
        m_EventCallbacks.erase(it);
}

void CModel::Clear_AllEvent()
{
    m_EventCallbacks.clear();
}


void CModel::Set_MasterSkeleton(CModel* pMaster)
{
    if (nullptr == pMaster)
        return;

    if (m_pMasterSkeleton != nullptr)       //마스터를 가지고 있었다면 해제하고
        Safe_Release(m_pMasterSkeleton);

    m_pMasterSkeleton = pMaster;
    Safe_AddRef(m_pMasterSkeleton);         //마스터를 add 

    m_isSharedSkeleton = true;
    m_isMaterSkeleton = false;

    // 메시 본 매핑
    for (auto& pMesh : m_Meshes)
    {
        pMesh->Build_BoneNameList(m_Bones);
        pMesh->Build_MasterBoneCache(m_pMasterSkeleton->m_Bones);
        pMesh->Build_FallbackBoneCache(m_Bones, m_pMasterSkeleton->m_Bones);
    }
}

void CModel::Attach_Part(CModel* pPart)
{
    if (nullptr == pPart)
        return;

    /* 이미 부착되어 있느지 */
    auto iter = find(m_AttachedParts.begin(), m_AttachedParts.end(), pPart);
    if (iter != m_AttachedParts.end())
        return;

    pPart->Set_MasterSkeleton(this);        //여기서 파츠가 마스터를 add (+마스터)
    pPart->Build_PartToMasterMap();

    m_AttachedParts.emplace_back(pPart);
    Safe_AddRef(pPart);                 //마스터가 파츠 add (+파츠)

    m_isMaterSkeleton = true;

}

void CModel::Detach_Part(CModel* pPart)
{
    auto iter = find(m_AttachedParts.begin(), m_AttachedParts.end(), pPart);
    if (iter == m_AttachedParts.end())
        return;

    if (pPart->m_pMasterSkeleton == this)
    {
        Safe_Release(pPart->m_pMasterSkeleton); //마스터에서 파츠가 들고있는 마스터를 release (-마스터)
        pPart->m_pMasterSkeleton = nullptr;
        pPart->m_isSharedSkeleton = false;
    }

    Safe_Release(*iter);            //마스터가 파츠를 release (-파츠)
    m_AttachedParts.erase(iter);
}


void CModel::Render_AllAttachedParts()
{
    for (auto& pPart : m_AttachedParts)
    {
        for (_uint i = 0; i < pPart->Get_NumMeshes(); ++i)
        {
            pPart->Render(i);
        }
    }
}


void CModel::Build_PartToMasterMap()
{
    if (!m_isSharedSkeleton || m_pMasterSkeleton == nullptr)
        return;

    const auto& masterBones = m_pMasterSkeleton->m_Bones;
    const _uint iMasterCount = masterBones.size();
    const _uint iPartCount = m_Bones.size();

    m_PartToMasterIndex.assign(iPartCount, -1);

    // master bone name -> index 매핑
    unordered_map<_wstring, _int> masterMap;
    masterMap.reserve(iMasterCount * 2);

    for (_int i = 0; i < (_int)iMasterCount; ++i)
    {
        masterMap.emplace(masterBones[i]->Get_Name(), i);
    }

    // 파트 본 이름으로 마스터 인덱스 찾기
    for (_int i = 0; i < (_int)iPartCount; ++i)
    {
        const _wstring& name = m_Bones[i]->Get_Name();
        auto it = masterMap.find(name);
        if (it != masterMap.end())
            m_PartToMasterIndex[i] = it->second;
    }

    // 파트 본 행렬 버퍼도 사이즈 맞춰 준비
    m_PartLocalBoneMatrices.resize(iPartCount);
}

void CModel::Update_PartLocalBones()
{
    //if (!m_isSharedSkeleton || nullptr == m_pMasterSkeleton)
    //    return;

    //m_PartLocalBoneMatrices.clear();
    //m_PartLocalBoneMatrices.resize(m_Bones.size());

    //// 중요: 순차적으로 처리 (부모 -> 자식 순서)
    //for (size_t i = 0; i < m_Bones.size(); ++i)
    //{
    //    CBone* pBone = m_Bones[i];
    //    _wstring boneName = pBone->Get_Name();

    //    // 마스터에 있는 본인지 확인
    //    _bool foundInMaster = false;
    //    for (size_t j = 0; j < m_pMasterSkeleton->m_Bones.size(); ++j)
    //    {
    //        if (m_pMasterSkeleton->m_Bones[j]->Compare_Name(boneName))
    //        {
    //            // 마스터 본: 마스터의 Combined Matrix 사용
    //            m_PartLocalBoneMatrices[i] =
    //                *m_pMasterSkeleton->m_Bones[j]->Get_CombinedTransformationMatrixPtr();
    //            foundInMaster = true;
    //            break;
    //        }
    //    }

    //    // 파츠 전용 본 처리
    //    if (!foundInMaster)
    //    {
    //        _int iParentIndex = pBone->Get_ParentBoneIndex();

    //        if (iParentIndex >= 0 && iParentIndex < m_PartLocalBoneMatrices.size())
    //        {
    //            // 핵심: 부모 Combined * 자신의 Local
    //            _matrix matParent = XMLoadFloat4x4(&m_PartLocalBoneMatrices[iParentIndex]);
    //            _matrix matLocal = pBone->Get_TransformationMatrix();  // 초기 로컬 변환

    //            XMStoreFloat4x4(&m_PartLocalBoneMatrices[i], matLocal * matParent);
    //        }
    //        else
    //        {
    //            XMStoreFloat4x4(&m_PartLocalBoneMatrices[i], XMMatrixIdentity());
    //        }
    //    }
    //}

    if (!m_isSharedSkeleton || m_pMasterSkeleton == nullptr)
        return;

    const _uint iPartCount = m_Bones.size();
    if (iPartCount == 0)
        return;

    const auto& masterBones = m_pMasterSkeleton->m_Bones;

    // 방어적: 매핑/버퍼 사이즈 맞추기
    if (m_PartToMasterIndex.size() != iPartCount)
        Build_PartToMasterMap();
    if (m_PartLocalBoneMatrices.size() != iPartCount)
        m_PartLocalBoneMatrices.resize(iPartCount);

    // 부모 -> 자식 순서라고 가정(m_Bones 생성 시 그렇게 되어 있음)
    for (size_t i = 0; i < iPartCount; ++i)
    {
        CBone* pBone = m_Bones[i];
        const _int iMasterIdx = (i < m_PartToMasterIndex.size()) ? m_PartToMasterIndex[i] : -1;

        if (iMasterIdx >= 0)
        {
            // 마스터와 공유되는 본: 마스터 Combined 그대로 사용
            m_PartLocalBoneMatrices[i] =
                *masterBones[iMasterIdx]->Get_CombinedTransformationMatrixPtr();
        }
        else
        {
            // 파츠 전용 본: 부모 기준으로 조합
            const _int iParentIndex = pBone->Get_ParentBoneIndex();

            if (iParentIndex >= 0 && iParentIndex < (_int)iPartCount)
            {
                const _matrix ParentCombinedMatrix =
                    XMLoadFloat4x4(&m_PartLocalBoneMatrices[iParentIndex]);
                const _matrix localMatrix =
                    pBone->Get_TransformationMatrix(); // 파츠 로컬

                // DirectX 스켈레톤 컨벤션: Combined = Local * ParentCombined
                XMStoreFloat4x4(&m_PartLocalBoneMatrices[i], localMatrix * ParentCombinedMatrix);
            }
            else
            {
                // 부모가 마스터에 붙어 있을 가능성도 있지만,
                // 여기선 파츠 루트면 자기 로컬(또는 identity)로 처리
                const _matrix localMatrix = pBone->Get_TransformationMatrix();
                XMStoreFloat4x4(&m_PartLocalBoneMatrices[i], localMatrix);
                // 필요하면 XMMatrixIdentity()로 바꿔도 됨 (디자인 선택)
            }
        }
    }

}

void CModel::Capture_CurrentFrameMatrices(vector<_float4x4>& OutBoneMatrices, _float4x4* pOutWorldMatrix)
{
    if (m_isMaterSkeleton || m_pTransformMatrix)  // 마스터만 캡처
    {
        OutBoneMatrices.clear();
        OutBoneMatrices.reserve(m_Bones.size());

        for (auto pBone : m_Bones)
        {
            OutBoneMatrices.push_back(*pBone->Get_CombinedTransformationMatrixPtr());
        }

        if (nullptr != pOutWorldMatrix)
        {
            if (nullptr != m_pOwnerTransform)
                *pOutWorldMatrix = *m_pOwnerTransform->Get_WorldMatrixPtr();
            else
                *pOutWorldMatrix = *m_pTransformMatrix;
        }
    }
}

_bool CModel::Restore_Frame(const vector<_float4x4>& SnapshotBoneMatrices)
{
    if (SnapshotBoneMatrices.empty())
        return false;

    // Combined 행렬 복원
    for (size_t i = 0; i < m_Bones.size() && i < SnapshotBoneMatrices.size(); ++i)
    {
        m_Bones[i]->Set_CombinedTransformationMatrix(XMLoadFloat4x4(&SnapshotBoneMatrices[i]));
    }

    return true;
}

void CModel::WarmupAnimations()
{
    if (m_eModelType != MODELTYPE::ANIM)
        return;

#ifdef _DEBUG
    OutputDebugStringA("[Model] Starting animation warmup...\n");
#endif

    _int originalAnimIndex = m_iCurrentAnimIndex;
    _float originalTrackPos = m_fCurrentTrackPosition;

    // 모든 애니메이션을 0.1초씩 재생
    for (_uint i = 0; i < m_iNumAnimations; ++i)
    {
        if (m_iCurrentAnimIndex == i && m_AnimationsSetup[m_iCurrentAnimIndex].isLoop)
            return;

        if (i >= m_iNumAnimations || m_iNumAnimations <= 0)
            return;

        if (Has_State(WAITFORCOMPLETE))
        {
            m_iReserveAnimIndex = i;

            OnWaitForComplete([this]() {
                Set_Animation(m_iReserveAnimIndex);
                });

            return;
        }

        m_iPrevAnimIndex = m_iCurrentAnimIndex;
        m_iCurrentAnimIndex = i;

        if (!Has_State(ANIMSET_PLAYING))
            Clear_State();

        Add_State(CHANGE_ANIMATION);

        /*  루트 모션 옵션 설정 */
        Check_RootMotion();

        //Set_Animation(i);


        for (int frame = 0; frame < 3; ++frame)
        {
            Play_Animation(0.066f); // 60fps 기준
        }

#ifdef _DEBUG
        if (i % 10 == 0)
        {
            char buffer[64];
            sprintf_s(buffer, "  Warmed up %d/%d animations\n", i + 1, m_iNumAnimations);
            OutputDebugStringA(buffer);
        }
#endif
    }

    // 원래 상태로 복원
    Set_Animation(originalAnimIndex);
    m_fCurrentTrackPosition = originalTrackPos;

#ifdef _DEBUG
    OutputDebugStringA("[Model] Animation warmup complete!\n");
#endif

}

#ifdef _DEBUG
void CModel::Debug_RanderState()
{

}
#endif

HRESULT CModel::Render(_uint iMeshIndex)
{
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Deferred_Render(_uint iMeshIndex, ID3D11DeviceContext* pDeferredContext)
{
    if (FAILED(m_Meshes[iMeshIndex]->Deferred_Bind_Resources(pDeferredContext)))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Deferred_Render(pDeferredContext)))
        return E_FAIL;

    return S_OK;
}

void CModel::Check_RootMotion()
{
    if (m_AnimationsSetup[m_iCurrentAnimIndex].isRootMotion) {
        Add_State(ROOTMOTION);
        m_AnimationsSetup[m_iCurrentAnimIndex].isApplyRootPosition ? Add_State(ROOTMOTION_POSITION) : Remove_State(ROOTMOTION_POSITION);
        m_AnimationsSetup[m_iCurrentAnimIndex].isIgnoreRootPos ? Add_State(IGNORE_ROOT_POS) : Remove_State(IGNORE_ROOT_POS);
        m_AnimationsSetup[m_iCurrentAnimIndex].isIgnoreRootRot ? Add_State(IGNORE_ROOT_ROT) : Remove_State(IGNORE_ROOT_ROT);
        m_AnimationsSetup[m_iCurrentAnimIndex].isIgnoreRootPosFirstFrame ? Add_State(IGNORE_ROOT_POS_FIRSTFRAME) : Remove_State(IGNORE_ROOT_POS_FIRSTFRAME);
        m_AnimationsSetup[m_iCurrentAnimIndex].isAbsoluteRootPosition ? Add_State(ABSOLUTE_ROOT_POS) : Remove_State(ABSOLUTE_ROOT_POS);
        m_AnimationsSetup[m_iCurrentAnimIndex].isRootMotion ? Add_State(ROOTMOTION) : Remove_State(ROOTMOTION);

        FLOAT3_DATA scale = m_AnimationsSetup[m_iCurrentAnimIndex].RootMitionScale;
        m_RootMotionInfo.vScale = XMVectorSet(scale.x, scale.y, scale.z, 1.f);
    }
    else {
        // 루트 모션을 사용하지 않는 경우 상태 제거
        Remove_State(ROOTMOTION_ALL);
    }
}

void CModel::Update_RootMotion(_float fTimeDelta)
{
    _matrix CurrentCombinedRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    if (Has_State(ROOTMOTION_POSITION))
    {
        /* 현재 루트본에서 첫 프레임 오프셋 빼기 */
        _vector vCurPos = XMVectorSubtract(CurrentCombinedRootMatrix.r[3], m_RootMotionInfo.vFirstFrameOffset);

        /* 델타 구하기 */
        _vector vDelta = XMVectorSubtract(vCurPos, m_RootMotionInfo.matPreRootMotion.r[3]);
        vDelta = XMVectorMultiply(vDelta, m_RootMotionInfo.vScale);

        /* 델타 행렬 갱신 */
        m_RootMotionInfo.matDeltaRootMotion.r[3] = XMVectorSetW(vDelta, 0.f);
        m_RootMotionInfo.matDeltaRootMotion.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        m_RootMotionInfo.matDeltaRootMotion.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        m_RootMotionInfo.matDeltaRootMotion.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

        /* 이전 포지션 현재 포지션으로 갱신 */
        m_RootMotionInfo.matPreRootMotion.r[3] = vCurPos;

        XMStoreFloat3(&m_vDelta, vDelta);

    }

    /* 리셋 */
    if (Has_State(ROOTMOTION_POSITION | IGNORE_ROOT_POS))
    {
        CurrentCombinedRootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(CurrentCombinedRootMatrix);
    }
}

void CModel::Apply_RootMotion_To_Transform()
{
    //델타 분해 
    _vector deltaScale, deltaRot, deltaPos;
    XMMatrixDecompose(&deltaScale, &deltaRot, &deltaPos, m_RootMotionInfo.matDeltaRootMotion);
    //_vector pos = m_pOwnerTransform->Get_State(STATE::POSITION);
    //if(/*XMVectorGetX(XMVector3Length(deltaPos)) > 0.1f &&*/ m_iCurrentAnimIndex ==120)
    //if (m_strModelName == L"Khazan_Spear" || m_strModelName == L"Khazan_GSword") {
    //    cout << "Before :" << pos .m128_f32[0]<< " ," << pos.m128_f32[1] << " ," << pos.m128_f32[2] << endl;
    //}
    /* trasnform 컴포넌트를 연결 시*/
    if (m_pOwnerTransform)
    {
        // 월드 분해
            _vector worldScale, worldRot, worldPos;
            XMMatrixDecompose(&worldScale, &worldRot, &worldPos, m_pOwnerTransform->Get_WorldMatrix());

            // 새로운 회전 = 기존 회전 * 델타 회전 (쿼터니언 곱셈)
            //_vector newRot = XMQuaternionMultiply(m_vRootDeltaQuat, worldRot);
            //newRot = XMQuaternionNormalize(newRot);

            //if (m_strModelName == L"Khazan_GSword" && m_iCurrentAnimIndex == 120 && m_fCurrentTrackPosition< 10.f )
            //    deltaPos.m128_f32[2] = m_fCurrentTrackPosition < 10.f ? 0.32f : 0.f; // deltaPos.m128_f32[2] < -0.1f ? deltaPos.m128_f32[2] * -1.f : deltaPos.m128_f32[2];

            // 새로운 위치 = 기존 위치 + (델타 위치를 월드 회전으로 변환)
            _vector rotatedDeltaPos = XMVector3Rotate(deltaPos, worldRot);
            _vector newPos = worldPos + rotatedDeltaPos;

            _matrix newWorld = XMMatrixAffineTransformation(worldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), worldRot, newPos);
            m_pOwnerTransform->Set_WorldMatrix(newWorld);
   }
    /* 월드 행렬을 연결 시 */
    else if (m_pOwnerTransformMatrix)
    {
        _matrix matWorld = XMLoadFloat4x4(m_pOwnerTransformMatrix);

        _vector worldScale, worldRot, worldPos;
        XMMatrixDecompose(&worldScale, &worldRot, &worldPos, matWorld);

        _vector rotatedDeltaPos = XMVector3Rotate(deltaPos, worldRot);
        _vector newPos = worldPos + rotatedDeltaPos;

        _matrix newWorld = XMMatrixAffineTransformation(worldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), worldRot, newPos);

        XMStoreFloat4x4(m_pOwnerTransformMatrix, newWorld);
    }

    //pos = m_pOwnerTransform->Get_State(STATE::POSITION);
    //if (/*XMVectorGetX(XMVector3Length(deltaPos)) > 0.1f &&*/ m_iCurrentAnimIndex == 120)
    //    if (m_strModelName == L"Khazan_Spear" || m_strModelName == L"Khazan_GSword") {
    //        cout << "After :" << pos.m128_f32[0] << " ," << pos.m128_f32[1] << " ," << pos.m128_f32[2] << endl;
    //        cout << "delta :" << deltaPos.m128_f32[0] << " ," << deltaPos.m128_f32[1] << " ," << deltaPos.m128_f32[2] << endl;
    //        cout << "----------------------------------------------------------------------------------------------------------" << endl;
    //    }

    //m_vRootDeltaQuat = XMQuaternionIdentity();
    m_RootMotionInfo.matDeltaRootMotion = XMMatrixIdentity();

}

void CModel::Check_WaitForComplete()
{
    if (m_AnimationsSetup[m_iCurrentAnimIndex].isWaitForComplete)
        Add_State(WAITFORCOMPLETE);
}

void CModel::Setup_Events()
{
    if (!m_AnimationsSetup[m_iCurrentAnimIndex].isEvent) {
        m_CurrentEvents.clear();
        m_PrevFrameInRange.clear();
        Remove_State(EVENT);
        return;
    }

    Add_State(EVENT);

    m_CurrentEvents.clear();
    m_PrevFrameInRange.clear();

    for (size_t i = 0; i < m_AnimationsSetup[m_iCurrentAnimIndex].vecEventFrames.size(); i++)
    {
        ANIM_EVENT event;

        event.strEventKey = m_AnimationsSetup[m_iCurrentAnimIndex].vecEventKeys[i];
        event.vFrameRange = _float2(m_AnimationsSetup[m_iCurrentAnimIndex].vecEventFrames[i].x, m_AnimationsSetup[m_iCurrentAnimIndex].vecEventFrames[i].y);
        for (size_t i = 0; i < 4; i++)
            event.isTriggered[i] = false;



        m_AnimationsSetup[m_iCurrentAnimIndex].vecEventTriggers[i] & 0x1 ? event.isTriggerOnce = true : event.isTriggerOnce = false;
        m_AnimationsSetup[m_iCurrentAnimIndex].vecEventTriggers[i] & 0x2 ? event.isTriggerOnEnter = true : event.isTriggerOnEnter = false;
        m_AnimationsSetup[m_iCurrentAnimIndex].vecEventTriggers[i] & 0x4 ? event.isTriggerOnExit = true : event.isTriggerOnExit = false;
        m_AnimationsSetup[m_iCurrentAnimIndex].vecEventTriggers[i] & 0x8 ? event.isTriggerContinuous = true : event.isTriggerContinuous = false;

        m_CurrentEvents.push_back(event);
        m_PrevFrameInRange.push_back(false);
    }

}

void CModel::Check_Event(_float fTimeDelta)
{
    if (m_CurrentEvents.empty())
        return;

    //_float fPrevTime = m_fCurrentTrackPosition - (m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond() * fTimeDelta);

    for (size_t i = 0; i < m_CurrentEvents.size(); i++)
    {
        ANIM_EVENT& event = m_CurrentEvents[i];

        //if (event.isTriggered && event.isTriggerOnce)
        //    continue;

       // 단일 이벤트
        if (event.vFrameRange.y == 0.f || event.vFrameRange.x == event.vFrameRange.y)
        {
            if (m_fPrevTrackPosition < event.vFrameRange.x && event.vFrameRange.x <= m_fCurrentTrackPosition)
            {
                if (!event.isTriggerOnce || !event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::ENTER])
                {
                    Trigger_Event(event.strEventKey, ANIM_EVENT_TRIGGERTYPE::ENTER);
                    event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::ENTER] = true;
                }
            }
        }
        // 구간 이벤트 
        else
        {
            _bool  isInRange = (event.vFrameRange.x <= m_fCurrentTrackPosition && m_fCurrentTrackPosition <= event.vFrameRange.y);
            _bool  wasInRange = m_PrevFrameInRange[i];

            // 진입
            if (isInRange && !wasInRange && event.isTriggerOnEnter)
            {
                if (!event.isTriggerOnce || !event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::ENTER])
                {
                    Trigger_Event(event.strEventKey, ANIM_EVENT_TRIGGERTYPE::ENTER);
                    event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::ENTER] = true;
                }
            }
            //탈출
            else if (!isInRange && wasInRange && event.isTriggerOnExit)
            {
                if (!event.isTriggerOnce || !event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::EXIT])
                {
                    Trigger_Event(event.strEventKey, ANIM_EVENT_TRIGGERTYPE::EXIT);
                    event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::EXIT] = true;
                }
            }
            //범위 내에 계속 발동
            else if (isInRange && event.isTriggerContinuous)
            {
                if (!event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::CONTINUE])
                    Trigger_Event(event.strEventKey, ANIM_EVENT_TRIGGERTYPE::CONTINUE);
            }
            // 탈출했으면 반복 애니메이션중에서 첫번째 루프에서 범위내 계속 발동 끄기.
            else if (!isInRange && wasInRange && event.isTriggerContinuous && event.isTriggerOnce)
            {
                event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::CONTINUE] = true;
            }

            m_PrevFrameInRange[i] = isInRange;

        }

    }

}

void CModel::Trigger_Event(string strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType)
{
    auto it = m_EventCallbacks.find(MakeCallbackKey(strEventKey, eTriggerType));

    if (it != m_EventCallbacks.end())
        it->second();
    //#ifdef _DEBUG
    //    else
    //    {
    //        OutputDebugStringA(("[CModel::Trigger_Event] No callback registered for: " + strEventKey + "\n").c_str());
    //    }
    //#endif

}

void CModel::Reset_EventTrigger()
{
    for (auto event : m_CurrentEvents)
        if (!event.isTriggerOnce)
            fill(begin(event.isTriggered), end(event.isTriggered), false);

    fill(m_PrevFrameInRange.begin(), m_PrevFrameInRange.end(), false);

}

string CModel::MakeCallbackKey(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType)
{
    return strEventKey + "_" + to_string(ENUM_CLASS(eTriggerType));
}

void CModel::Cache_CurrentBoneMatrices()
{
    if (m_CachedBoneMatrices.size() != m_Bones.size())
        m_CachedBoneMatrices.resize(m_Bones.size());

    if (m_CachedPrevBoneMatrices.size() != m_CachedBoneMatrices.size())
        m_CachedPrevBoneMatrices.reserve(m_CachedBoneMatrices.size());

    m_CachedPrevBoneMatrices = m_CachedBoneMatrices;

    for (size_t i = 0; i < m_Bones.size(); ++i)
    {
        m_CachedBoneMatrices[i] = *m_Bones[i]->Get_CombinedTransformationMatrixPtr();
    }
}

void CModel::Restore_CurrentBoneMatrices()
{
    if (m_CachedBoneMatrices.empty())
        return;

    for (size_t i = 0; i < m_Bones.size(); ++i)
    {
        m_Bones[i]->Set_CombinedTransformationMatrix(XMLoadFloat4x4(&m_CachedBoneMatrices[i]));
    }
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

        /* 파츠가 사용할 본 인덱스 넘겨주기 */
      //  pMesh->Build_BoneNameList(m_Bones);
    }

    return S_OK;
}

HRESULT CModel::Ready_Materials(MODEL_DATA& data, _bool isSRVCache)
{

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CMeshMaterial* pMeshMaterial = CMeshMaterial::Create(m_pDevice, m_pContext, data.vecMaterials[i], isSRVCache);
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

    if (m_eModelType == MODELTYPE::ANIM)
    {
        for (_int i = 0; i < m_Bones.size(); i++)
        {
            if (m_Bones[i]->Get_ParentBoneIndex() == -1)
                continue;

            m_Bones[m_Bones[i]->Get_ParentBoneIndex()]->Push_ChildBone(i);
        }
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


inline void CModel::DFS_BuildChainsFromRoot(_int cur, _int maxDepth, vector<_int>& path, vector<vector<_int>>& outChains, _int minLen)
{
    path.push_back(cur);

    // 종료 조건: 깊이 도달 or 리프 본
    const auto& children = this->Get_ChildIndices(cur);
    const bool atDepthLimit = (int)path.size() >= maxDepth + 1; // 노드 수 기준
    const bool isLeaf = children.empty();

    if (isLeaf || atDepthLimit)
    {
        if ((int)path.size() >= minLen) outChains.push_back(path);
        path.pop_back();
        return;
    }

    // 자식이 여러 개면 각각 “새로운 체인 브랜치”
    for (int child : children)
    {
        DFS_BuildChainsFromRoot(child, maxDepth, path, outChains, minLen);
    }

    path.pop_back();

}

inline vector<vector<_int>> CModel::BuildChainsFromRoot(_int rootBone, _int maxDepth, _int minLen)
{
    std::vector<std::vector<int>> chains;
    std::vector<int> path;
    DFS_BuildChainsFromRoot(rootBone, maxDepth, path, chains, minLen);

    // path는 [root, ..., leaf], CBoneChainPhysic가 요구하는 포맷과 동일
    return chains;

}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _bool isSRVCache)
{
    CModel* pInstance = new CModel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, isSRVCache)))
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

    /*if(m_pOwnerTransform)
        Safe_Release(m_pOwnerTransform);*/


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
