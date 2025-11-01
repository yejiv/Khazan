
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
    , m_iNumMeshes { Prototype.m_iNumMeshes }
    , m_Meshes { Prototype.m_Meshes }

    , m_iNumMaterials { Prototype.m_iNumMaterials }
    , m_Materials { Prototype.m_Materials }

    , m_iNumAnimations{ Prototype.m_iNumAnimations }

    , m_eModelType { Prototype.m_eModelType} 
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_strModelName{ Prototype.m_strModelName }
    , m_strModelFilePath{ Prototype.m_strModelFilePath }
    , m_RootMotionInfo{ Prototype.m_RootMotionInfo }
    , m_AnimationsSetup{Prototype.m_AnimationsSetup }
    , m_AnimationSets {Prototype.m_AnimationSets }
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


HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath)
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

    if (FAILED(Ready_Materials(data)))
        return E_FAIL;

    if (FAILED(Ready_Animations(data)))
        return E_FAIL;

#ifdef _DEBUG
    _bool isFindRoot = { false };
#endif // _DEBUG


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


    /* 피오나용 */
#ifdef _DEBUG
    if (!isFindRoot && m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CBone* pBone) {
                if (pBone->Compare_Name("Bip001"))
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
#endif // _DEBUG

	return S_OK;
}

HRESULT CModel::Initialize_Clone(void* pArg)
{ 
	return S_OK;
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
        Set_Animation(m_AnimationSets[m_AnimationSetInfo.iSelectedAnimIndex].vecAnimIndices[m_AnimationSetInfo.iCurrentIndex]);
        Remove_State(ANIMSET_NEXT);
    }

    /* 애니메이션 변경 */
    if (Has_State(CHANGE_ANIMATION))
	{
        /* 애니메이션 블랜딩할 이전 애니메이션 뼈 넘겨주기 */
		m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));
		Remove_State(CHANGE_ANIMATION);
        Add_State(FIRST_FRAME_ANIMATION);
	}

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
#ifdef _DEBUG
                OutputDebugStringA(("[Set_AnimationSet End !! \n]"));
#endif // _DEBUG

                Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT);
                return true;
            }
#ifdef _DEBUG
            OutputDebugStringA(("[Set_AnimationSet N E X T \n]"));
#endif // _DEBUG

            return false;
        }

    }

    return m_isFinished;
}

void CModel::Set_Animation(_uint iIndex)
{
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
        Remove_State(ANIMSET_PLAYING | ANIMSET_NEXT );
#ifdef _DEBUG
        OutputDebugStringA(("[CModel::Set_AnimationSet() Error] Invalid Animation Set Key: " + strKey + "\n").c_str());
#endif // _DEBUG

    }
}


void CModel::Set_AnimationLoop(_bool isLoop)
{
    Add_State(USED_ANIM_LOOP);

    if(isLoop)
        Add_State(ANIM_LOOP);
}

void CModel::Update_BoneCombinedMatrices()
{
	for (auto bone : m_Bones)
	{
		bone->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);
	}
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

#ifdef _DEBUG
void CModel::Debug_RanderState()
{
    ImGui::SeparatorText("Model State");

    // 전체 State 값 표시
    ImGui::Text("State Value: 0x%08X", m_iState);
    ImGui::Spacing();

    // 각 상태별 표시
    struct StateInfo {
        MODEL_STATE flag;
        const char* name;
        ImVec4 activeColor;
    };

    StateInfo states[] = {
        {ANIM_LOOP,         "ANIM_LOOP",            ImVec4(0.0f, 1.0f, 0.0f, 1.0f)},
        {USED_ANIM_LOOP,    "USED_ANIM_LOOP",       ImVec4(0.0f, 0.8f, 0.0f, 1.0f)},
        {CHANGE_ANIMATION,  "CHANGE_ANIMATION",     ImVec4(1.0f, 1.0f, 0.0f, 1.0f)},
        {ANIMSET_PLAYING,   "ANIMSET_PLAYING",      ImVec4(0.0f, 1.0f, 1.0f, 1.0f)},
        {ANIMSET_NEXT,      "ANIMSET_NEXT",         ImVec4(0.5f, 0.5f, 1.0f, 1.0f)},
        {ROOTMOTION,        "ROOTMOTION",           ImVec4(1.0f, 0.5f, 0.0f, 1.0f)},
        {ROOTMOTION_POSITION, "ROOTMOTION_POSITION", ImVec4(1.0f, 0.3f, 0.0f, 1.0f)},
        //{ROOTMOTION_ROTATION, "ROOTMOTION_ROTATION", ImVec4(1.0f, 0.3f, 0.3f, 1.0f)},
        {WAITFORCOMPLETE,   "WAITFORCOMPLETE",      ImVec4(1.0f, 0.0f, 1.0f, 1.0f)}
    };

    ImGui::BeginChild("StateFlags", ImVec2(0, 200), true);
    {
        for (const auto& state : states)
        {
            _bool isActive = Has_State(state.flag);

            if (isActive)
                ImGui::TextColored(state.activeColor, "[ON]  %s", state.name);
            else
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[OFF] %s", state.name);
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // 애니메이션 관련 정보
    ImGui::SeparatorText("Animation Info");
    ImGui::Text("Current Anim Index: %d", m_iCurrentAnimIndex);
    ImGui::Text("Prev Anim Index: %d", m_iPrevAnimIndex);
    ImGui::Text("Reserve Anim Index: %d", m_iReserveAnimIndex);
    ImGui::Text("Track Position: %.2f", m_fCurrentTrackPosition);
    ImGui::Text("Is Finished: %s", m_isFinished ? "YES" : "NO");

    ImGui::Spacing();

    // 애니메이션 세트 정보
    if (Has_State(ANIMSET_PLAYING))
    {
        ImGui::SeparatorText("Animation Set Info");
        //ImGui::Text("Current Set Index: %d", m_iCurrentAnimSetsIndex);
        //ImGui::Text("Set Max Index: %d", m_iCurrentAnimSetsMaxIndex);
        //ImGui::Text("Current Anim in Set: %d", m_iCurrentAnimSetIndex);
    }

    ImGui::Spacing();

    // 루트 모션 정보
    //if (Has_State(ROOTMOTION))
    //{
        ImGui::SeparatorText("Root Motion Info");
        ImGui::Text("Root Bone Index: %d", m_iRootBoneIndex);
/*        ImGui::Text("Blend Time: %.2f / %.2f",m_fCurrentRootMotionBlendTime, m_fRootMotionBlendTime);

        _float4 scale;
        XMStoreFloat4(&scale, m_vRootMotionScale);
        ImGui::Text("Scale: (%.1f, %.1f, %.1f)", scale.x, scale.y, scale.z); */
    //}

    ImGui::Spacing();

    // 이벤트 정보
    ImGui::SeparatorText("Event Info");
    ImGui::Text("Registered Events: %d", (_int)m_EventCallbacks.size());
    ImGui::Text("Current Events: %d", (_int)m_CurrentEvents.size());

    if (!m_EventCallbacks.empty())
    {
        if (ImGui::TreeNode("Registered Event Keys"))
        {
            for (const auto& pair : m_EventCallbacks)
            {
                ImGui::BulletText("%s", pair.first.c_str());
            }
            ImGui::TreePop();
        }
    }

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

    if (m_pOwnerTransform)
    {
        // 월드 분해
        _vector worldScale, worldRot, worldPos;
        XMMatrixDecompose(&worldScale, &worldRot, &worldPos, m_pOwnerTransform->Get_WorldMatrix());

        // 새로운 회전 = 기존 회전 * 델타 회전 (쿼터니언 곱셈)
        //_vector newRot = XMQuaternionMultiply(m_vRootDeltaQuat, worldRot);
        //newRot = XMQuaternionNormalize(newRot);

        // 새로운 위치 = 기존 위치 + (델타 위치를 월드 회전으로 변환)
        _vector rotatedDeltaPos = XMVector3Rotate(deltaPos, worldRot);
        _vector newPos = worldPos + rotatedDeltaPos;

        _matrix newWorld = XMMatrixAffineTransformation(worldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), worldRot, newPos);
        m_pOwnerTransform->Set_WorldMatrix(newWorld);
    }
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
                if(!event.isTriggered[ANIM_EVENT::TRIGGER_INDEX::CONTINUE])
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
        if(!event.isTriggerOnce)
            fill(begin(event.isTriggered), end(event.isTriggered), false);

    fill(m_PrevFrameInRange.begin(), m_PrevFrameInRange.end(), false);

}

string CModel::MakeCallbackKey(const string& strEventKey, ANIM_EVENT_TRIGGERTYPE eTriggerType)
{
    return strEventKey + "_" + to_string(ENUM_CLASS(eTriggerType));
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

    if(m_pOwnerTransform)
        Safe_Release(m_pOwnerTransform);


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
