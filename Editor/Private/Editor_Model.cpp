#include "GameInstance.h"
#include "Editor_Model.h"
#include "Editor_Mesh.h"
#include "Editor_MeshMaterial.h"
#include "Editor_Bone.h"
#include "Editor_Animation.h"
#include "GameInstance.h"

#include "Shader.h"

#include <regex>
#include <fstream>
#include <filesystem>
#include "../../Engine/Public/assimp/postprocess.h"


// helper: 쿼터니언 -> 오일러(도 단위)
static void QuatToEulerDegrees(const XMVECTOR& q, float& outPitch, float& outYaw, float& outRoll)
{
    // Assumes quaternion is normalized
    XMFLOAT4 quat;
    XMStoreFloat4(&quat, q);

    // Convert to Euler (pitch=x, yaw=y, roll=z) in radians
    float ysqr = quat.y * quat.y;

    // roll (x-axis rotation)
    float t0 = +2.0f * (quat.w * quat.x + quat.y * quat.z);
    float t1 = +1.0f - 2.0f * (quat.x * quat.x + ysqr);
    float roll = atan2f(t0, t1);

    // pitch (y-axis rotation)
    float t2 = +2.0f * (quat.w * quat.y - quat.z * quat.x);
    t2 = t2 > 1.0f ? 1.0f : (t2 < -1.0f ? -1.0f : t2);
    float pitch = asinf(t2);

    // yaw (z-axis rotation)
    float t3 = +2.0f * (quat.w * quat.z + quat.x * quat.y);
    float t4 = +1.0f - 2.0f * (ysqr + quat.z * quat.z);
    float yaw = atan2f(t3, t4);

    // radians -> degrees
    outPitch = XMConvertToDegrees(pitch);
    outYaw = XMConvertToDegrees(yaw);
    outRoll = XMConvertToDegrees(roll);
}


CEditor_Model::CEditor_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice ,pContext }
{
}


CEditor_Model::CEditor_Model(const CEditor_Model& Prototype)
    : CComponent{ Prototype }
    , m_eModelType{ Prototype.m_eModelType }
    , m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
    , m_pModelFilePath{ Prototype.m_pModelFilePath }
    , m_Model_Data(Prototype.m_Model_Data)

    , m_iNumMeshes{ Prototype.m_iNumMeshes }
    , m_Meshes{ Prototype.m_Meshes }

    , m_iNumMaterials{ Prototype.m_iNumMaterials }
    , m_Materials{ Prototype.m_Materials }

   // , m_Bones{ Prototype.m_Bones }
	, m_iNumAnimations{ Prototype.m_iNumAnimations }
    , m_iRootBoneIndex{Prototype.m_iRootBoneIndex }
    , m_vPreTransformQuat{ Prototype.m_vPreTransformQuat }
    //, m_iRotationBoneIndex{Prototype.m_iRotationBoneIndex }
    //, m_fRootMotionBlendTime{Prototype.m_fRootMotionBlendTime }

{
    for (auto& pPrototypeAnimation : Prototype.m_Animations) {
        CEditor_Animation* pAnimation = pPrototypeAnimation->Clone();
        m_Animations.push_back(pAnimation);
        pAnimation->Set_TrackPositionPtr(&m_fCurrentTrackPosition);
        pAnimation->Set_RootBoneIndex(m_iRootBoneIndex);
    }

    for (auto& pPrototypeBone : Prototype.m_Bones)
        m_Bones.push_back(pPrototypeBone->Clone());

    for (auto& pAinmation : m_Animations)
        Safe_AddRef(pAinmation);

    for (auto& pBone : m_Bones)
        Safe_AddRef(pBone);

    for (auto& pMesh : m_Meshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_Materials)
        Safe_AddRef(pMaterial);


}

HRESULT CEditor_Model::Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    m_eModelType = eModelType;
    m_pModelFilePath = pModelFilePath;
    XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

    //_vector vScale, vRot, vTrans;
    //XMMatrixDecompose(&vScale, &vRot, &vTrans, PreTransformMatrix);
    //m_BaseMatrix = XMMatrixScalingFromVector(vScale); // *XMMatrixRotationQuaternion(vRot);

    // ============ PreTransform 회전 추출 수정 ============
    // 스케일 제거를 위한 정규화된 행렬 생성
    _vector vScale, vRot, vTrans;
    XMMatrixDecompose(&vScale, &vRot, &vTrans, PreTransformMatrix);
    m_vPreTransformQuat = vRot;

    string filePath(pModelFilePath);
    _bool isGLTF = (filePath.substr(filePath.find_last_of(".") + 1) == "gltf");

    _uint iFlag = 0;

    if (isGLTF)
        iFlag = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded;
    else
        iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;


    if (m_eModelType == MODELTYPE::NONANIM)
        iFlag |= aiProcess_PreTransformVertices;

    if (isGLTF)
    {
        // GLTF의 빈 본 제거 방지 (GLTF는 본 구조가 중요함)
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, false);
        // GLTF의 피벗 포인트 보존
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    }

    _char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());
    OutputDebugStringA(("[Model Path] " + filePath + "\n").c_str());

	if (filesystem::exists(filePath))
	{
		string message = "Model file exists: " + filePath + "\n";
		OutputDebugStringA(message.c_str());
	}
	else
	{
		string error = "Model file does not exist: " + filePath + "\n";
		OutputDebugStringA(error.c_str());
		return E_FAIL;
	}

    m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
    if (nullptr == m_pAIScene)
    {
        string error = "Failed to load model: " + string(m_Importer.GetErrorString())+ "\n";
        OutputDebugStringA(error.c_str());
        return E_FAIL;
    }

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    if (FAILED(Ready_Meshes()))
        return E_FAIL;

    if (FAILED(Ready_Materials()))
        return E_FAIL;

    if (FAILED(Ready_Animation()))
        return E_FAIL;


    _char   szName[MAX_PATH];
    _splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);
    m_Model_Data.strModelName = static_cast<string>(szName);
    m_Model_Data.strModelFilePath = pModelFilePath;
    m_Model_Data.iModelType = static_cast<_uint>(eModelType);
    m_Model_Data.iNumMeshes = m_iNumMeshes;
    m_Model_Data.iNumMaterials = m_iNumMaterials;
    m_Model_Data.iNumAnimations = m_iNumAnimations;

    memcpy(&m_Model_Data.vPreTransformMatrix, &m_PreTransformMatrix, sizeof(_float4x4));

    if (m_eModelType == MODELTYPE::ANIM)
    {
        auto it = find_if(m_Bones.begin(), m_Bones.end(),
            [&](CEditor_Bone* pBone) {
                if (pBone->Compare_Name("Root")|| pBone->Compare_Name("Bip001"))
                    return true;
                return false;
            });

        if (it == m_Bones.end())
        {
            OutputDebugStringA(("!!!!!!!!1!!!!!!! 루트본 못 찾음!!!!!!!!!!!!!!!!!!!!!!!!!"));
        }

        m_iRootBoneIndex = static_cast<_uint>(distance(m_Bones.begin(), it));
        OutputDebugStringA(("[Root Boon Index] : " + to_string(m_iRootBoneIndex) + "\n").c_str());

        //m_iRotationBoneIndex = m_iRootBoneIndex + 1;
    }

    return S_OK;
}

HRESULT CEditor_Model::Initialize_Clone(void* pArg)
{

    return S_OK;
}

HRESULT CEditor_Model::Render(_uint iMeshIndex)
{
    if (FAILED(m_Meshes[iMeshIndex]->Bind_Resources()))
        return E_FAIL;

    if (FAILED(m_Meshes[iMeshIndex]->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEditor_Model::Bind_Materials(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iTextureIndex)
{
    if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (m_iNumMaterials <= iMaterialIndex)
		return E_FAIL;
	m_Materials[iMaterialIndex]->Bind_Resources(pShader, pConstantName, iTextureType, iTextureIndex);

	return   S_OK;
}

HRESULT CEditor_Model::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, pConstantName, m_Bones);

	return S_OK;
}

_bool CEditor_Model::Play_Animation(_float fTimeDelta)
{
    if (m_iNumAnimations <= 0)
        return false;

    if (m_iCurrentAnimIndex == 17)
        int a = 100;

    m_isFinished = false;

    // 애니메이션 세트 관련
    if (m_isSetAnimNextPlay)
    {
        _uint iIndex = static_cast<_uint>(m_Model_Data.vecAnimationSets[m_iCurSelectSetAnimIndex].vecAnimIndices[m_iCurSetAnimIndex]);
        Set_Animation(iIndex, m_Model_Data.vecAnimation[m_Model_Data.vecAnimationSets[m_iCurSelectSetAnimIndex].vecAnimIndices[m_iCurSetAnimIndex]].animSetup.isLoop);
        m_isSetAnimNextPlay = false;
    }

    // 애니메이션 변경 시 블랜딩
    if (m_isChangedAnimation)
    {
        m_Animations[m_iCurrentAnimIndex]->OnAnimationBlend(move(m_Animations[m_iPrevAnimIndex]->Get_ChannelMatrices()));
        m_isChangedAnimation = false;
        m_isFirstRootMotionFrame = true; // 새 애니메이션 시작

        char msg[256];
        sprintf_s(msg, "=== ANIM CHANGE: %d -> %d ===\n", m_iPrevAnimIndex, m_iCurrentAnimIndex);
        OutputDebugStringA(msg);
    }

    // 루프 감지를 위한 이전 트랙 포지션 저장
    _float fPrevTrackPosition = m_fCurrentTrackPosition;

    // 애니메이션 업데이트
    m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, &m_isFinished, fTimeDelta);

    // 루프애니메이션의 반복 감지(첫 프레임)
    if ((m_isLoop || m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isLoop) && fPrevTrackPosition > m_fCurrentTrackPosition && m_fCurrentTrackPosition < 0.1f)
    {
        OutputDebugStringA(">>> LOOP DETECTED\n");
        m_isFirstRootMotionFrame = true; // 루프 시작
    }

    if (m_isFirstRootMotionFrame) {
        // 루트 모션 처리
        OnRootMotion();


    }


    _bool isBlending = m_Animations[m_iCurrentAnimIndex]->IsBlending();


    for (_int i = 0; i < (_int)m_Bones.size(); ++i)
    {
        
        if (i == m_iRootBoneIndex )
        {
            /*  애니메이션 재생시 포지션 고정 */
            if (m_isIgnoreRootPos || (m_isFirstRootMotionFrame && m_isIgnoreRootPosFirstFrame)) {
                _matrix rootMat = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
                rootMat.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(rootMat);
            }

            /* 회전 고정 */
            if (m_isIgonreRootRot)
            {
                _matrix rootMat = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
                rootMat.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
                rootMat.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                rootMat.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
                m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(rootMat);

            }

        }

        /*  =========== 뼈들 컴바인드 ===========*/
        m_Bones[i]->Update_CombinedTransformationMatrix(m_PreTransformMatrix, m_Bones);

#pragma region Rotation test

        /* 루트본 차례에서 */
        //if (i == m_iRootBoneIndex)
        //{
        //    /* 애니메이션 첫 번째 프레임. */
        //    if (m_isFirstRootMotionFrame)
        //    {
        //        _matrix currentCombinedRoot = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

        //        if (m_isIgonreRootRot)
        //        {
        //            _matrix currentRotTransform = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();

        //            // ============ Combined에서 PreTransform 제거 ============
        //            _vector s, r, t;
        //            _matrix safeComb = currentCombinedRoot;
        //            safeComb.r[0] = XMVector3Normalize(safeComb.r[0]);
        //            safeComb.r[1] = XMVector3Normalize(safeComb.r[1]);
        //            safeComb.r[2] = XMVector3Normalize(safeComb.r[2]);
        //            XMMatrixDecompose(&s, &r, &t, safeComb);
        //            _vector pureAnimRot = XMQuaternionMultiply(XMQuaternionInverse(m_vPreTransformQuat), r);
        //            _matrix currentRot = XMMatrixAffineTransformation(s, XMVectorZero(), pureAnimRot, t);

        //            // 디버그
        //            _float transYaw = atan2f(2.0f * (XMVectorGetW(pureAnimRot) * XMVectorGetY(pureAnimRot) +
        //                XMVectorGetX(pureAnimRot) * XMVectorGetZ(pureAnimRot)),
        //                1.0f - 2.0f * (XMVectorGetY(pureAnimRot) * XMVectorGetY(pureAnimRot) +
        //                    XMVectorGetZ(pureAnimRot) * XMVectorGetZ(pureAnimRot)));

        //            _float combYaw = atan2f(2.0f * (XMVectorGetW(r) * XMVectorGetY(r) + XMVectorGetX(r) * XMVectorGetZ(r)),
        //                1.0f - 2.0f * (XMVectorGetY(r) * XMVectorGetY(r) + XMVectorGetZ(r) * XMVectorGetZ(r)));

        //            char msg[512];
        //            sprintf_s(msg, "[FIRST_FRAME] PureAnim:%.1f° Comb:%.1f°\n",
        //                XMConvertToDegrees(transYaw),
        //                XMConvertToDegrees(combYaw));
        //            OutputDebugStringA(msg);

        //            m_PreRotRootMatrix = currentCombinedRoot;

        //            // Combined 리셋
        //            _vector scale, rot, pos;
        //            XMMatrixDecompose(&scale, &rot, &pos, currentCombinedRoot);
        //            _matrix resetRotation = XMMatrixAffineTransformation(scale, XMVectorZero(), m_vPreTransformQuat, pos);
        //            m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(resetRotation);
        //        }

        //        ///* ============ 회전용 Combined rootbone 기준으로 초기화 ============ */
        //        //if (m_isIgonreRootRot)
        //        //{
        //        //    _matrix currentRotTransform = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();

        //        //    _vector s, r, t;
        //        //    _matrix safe = currentRotTransform;
        //        //    safe.r[0] = XMVector3Normalize(safe.r[0]);
        //        //    safe.r[1] = XMVector3Normalize(safe.r[1]);
        //        //    safe.r[2] = XMVector3Normalize(safe.r[2]);
        //        //    XMMatrixDecompose(&s, &r, &t, safe);

        //        //    _float transYaw = atan2f(2.0f * (XMVectorGetW(r) * XMVectorGetY(r) + XMVectorGetX(r) * XMVectorGetZ(r)),
        //        //        1.0f - 2.0f * (XMVectorGetY(r) * XMVectorGetY(r) + XMVectorGetZ(r) * XMVectorGetZ(r)));

        //        //    // ============ Combined 리셋 전에 ============
        //        //   // _matrix currentRotCombined = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

        //        //    _matrix safeComb = currentCombinedRoot;
        //        //    safeComb.r[0] = XMVector3Normalize(safeComb.r[0]);
        //        //    safeComb.r[1] = XMVector3Normalize(safeComb.r[1]);
        //        //    safeComb.r[2] = XMVector3Normalize(safeComb.r[2]);
        //        //    XMMatrixDecompose(&s, &r, &t, safeComb);

        //        //    _float combYaw = atan2f(2.0f * (XMVectorGetW(r) * XMVectorGetY(r) + XMVectorGetX(r) * XMVectorGetZ(r)),
        //        //        1.0f - 2.0f * (XMVectorGetY(r) * XMVectorGetY(r) + XMVectorGetZ(r) * XMVectorGetZ(r)));

        //        //    char msg[512];
        //        //    sprintf_s(msg, "[FIRST_FRAME] Trans:%.1f° Comb:%.1f° (Trans × PreTrans(180°) = Comb)\n",
        //        //        XMConvertToDegrees(transYaw),
        //        //        XMConvertToDegrees(combYaw));
        //        //    OutputDebugStringA(msg);

        //        //    // ============ Transformation 저장 ============
        //        //    m_PreRotRootMatrix = currentRotTransform;
        //        //    // m_PreRotRootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

        //        //    // Combined 리셋
        //        //    _vector scale, rot, pos;
        //        //    XMMatrixDecompose(&scale, &rot, &pos, currentCombinedRoot);

        //        //    _matrix resetRotation = XMMatrixAffineTransformation(scale, XMVectorZero(), m_vPreTransformQuat, pos);
        //        //    m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(resetRotation);

        //        //    sprintf_s(msg, "[FIRST_FRAME] After reset - Combined Yaw:180.0° (PreTransform only)\n");
        //        //    OutputDebugStringA(msg);
        //        //}


        //        /* 컴바인드 루트본 위치 zero 초기화 */
        //        m_PreRootMatrix = currentCombinedRoot;
        //        m_PreRootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

        //        /* 애니메이션 절대 좌표 사용 (위치가 프레임 단위로 엄청 크게 변하는 애니메이션일 때 (ex 앞으로 나가는 콤보공격 애니메이션 2번째부터 이용하는 ))*/
        //        if (m_isAbsoluteRootPosition)
        //            m_vFirstFrameRootOffset = currentCombinedRoot.r[3];
        //        else
        //            m_vFirstFrameRootOffset = XMVectorZero();
   

        //        m_vRootMotionDelta = XMMatrixIdentity();
        //        m_vRootDeltaQuat = XMQuaternionIdentity();

        //        _matrix resetPosition = m_PreRootMatrix;
        //        resetPosition.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        //        m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(resetPosition);

        //        m_isFirstRootMotionFrame = false;

        //        OutputDebugStringA("========================================\n\n");
        //    }
        //    else if (m_isRootMotion && !m_isFinished)
        //    {
        //        Update_RootMotion(fTimeDelta, isBlending);
        //    }
        //}
#pragma endregion

        if (i == m_iRootBoneIndex && m_isRootMotion_Pos)
        {
            if (m_isFirstRootMotionFrame)
            {
                _matrix currentCombinedRoot = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

				//// 첫 프레임 - PreRootMatrix : 컴바인드된 루트본으로 갱신
				//// preRoot 회전값 유지, 위치는 0
				m_PreRootMatrix = currentCombinedRoot;
				m_PreRootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

                if (m_isAbsoluteRootPosition)
                {
                    // 애니메이션이 설정한 원본 CombinedTransformationMatrix 위치
                    m_vFirstFrameRootOffset = currentCombinedRoot.r[3];
                }
                else
                {
                    m_vFirstFrameRootOffset = XMVectorZero();
                }

                m_vRootMotionDelta = XMMatrixIdentity();
       
                // 첫 프레임 - 컴바인드된 루트본에서 포지션만 원점으로
                _matrix rootMat = m_PreRootMatrix;
                rootMat.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
                m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(rootMat);

                m_isFirstRootMotionFrame = false;

                OutputDebugStringA("========================================\n\n");
            }
            else if(m_isRootMotion)
            {
                Update_RootMotion(fTimeDelta, isBlending);
            }
        }
    }

    /* Owner에 Transform 적용 */
    if (m_isRootMotion && m_pOwnerTransform)
        Apply_RootMotion_To_Transform();

    // 애니메이션 세트 관련
    if (m_isSetAnimPlaying && m_isFinished)
    {
        ++m_iCurSetAnimIndex;
        m_isSetAnimNextPlay = true;
        if (m_iCurSetAnimIndex == m_iCurSetAnimMaxIndex)
        {
            m_isSetAnimPlaying = false;
            m_isSetAnimFinished = true;
            m_isSetAnimNextPlay = false;
            m_iCurSetAnimIndex = 0;
        }
    }

    return m_isFinished;

}

void CEditor_Model::Set_Animation(_uint iIndex, _bool isLoop)
{
	if (iIndex >= m_iNumAnimations || m_iNumAnimations <= 0)
		return;

	m_isLoop = isLoop;

	m_iPrevAnimIndex = m_iCurrentAnimIndex;
	m_iCurrentAnimIndex = iIndex;

	m_isChangedAnimation = true;

	m_isIgonreRootRot = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isIgnoreRootRot;
    m_isIgnoreRootPos = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isIgnoreRootPos;
    m_isIgnoreRootPosFirstFrame = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isIgnoreRootPosFirstFrame;
    m_isAbsoluteRootPosition = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isAbsoluteRootPosition;
}

void CEditor_Model::Set_SetAnimation(const string& strKey)
{
    m_iCurSetAnimIndex = 0;

    // vecAnimationSets에서 strKey와 일치하는 세트 찾기
    auto iter = find_if(m_Model_Data.vecAnimationSets.begin(),
        m_Model_Data.vecAnimationSets.end(),
        [&strKey](const ANIMATION_SET_DATA& set) {
            return set.strAnimSetName == strKey;
        });

    if (iter != m_Model_Data.vecAnimationSets.end())
    {
        //m_iCurSelectSetAnimCurIndex = 0;
        m_isSetAnimPlaying = true;
        m_isSetAnimNextPlay = true;
        m_isSetAnimFinished = false;
        m_iCurSelectSetAnimIndex = static_cast<_uint>(distance(m_Model_Data.vecAnimationSets.begin(), iter));
        m_iCurSetAnimMaxIndex = static_cast<_uint>(iter->vecAnimIndices.size());
        m_isLoop = m_Model_Data.vecAnimation[m_Model_Data.vecAnimationSets[m_iCurSelectSetAnimIndex].vecAnimIndices[m_iCurSetAnimIndex]].animSetup.isLoop;
        OutputDebugStringA(("[Animation Set Selected] " + strKey +
            "\n - Set Index: " + to_string(m_iCurSelectSetAnimIndex) +
            "\n - Total Animations: " + to_string(m_iCurSetAnimMaxIndex) + "\n").c_str());

    }
    else
    {
        m_isSetAnimPlaying = false;
        m_isSetAnimNextPlay = false;
        m_isSetAnimFinished = false;
        m_iCurSelectSetAnimIndex = 0;
        m_iCurSetAnimMaxIndex = 0;
        OutputDebugStringA(("[Error] Invalid Animation Set Key: " + strKey + "\n").c_str());
    }
}

CEditor_Animation* CEditor_Model::Get_CurAnimtion()
{
    if (m_iPrevAnimIndex > 0) m_Animations[m_iPrevAnimIndex]->EnbleTrackPosition(true);
    m_Animations[m_iCurrentAnimIndex]->EnbleTrackPosition(true);
    return m_Animations[m_iCurrentAnimIndex];
}

void CEditor_Model::ExportModel(string& strPath)
{
    /* 현재 실행파일 저장 */
    _char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    /* 실행파일 위치 Client/default 로 고정  */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
   // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());

    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string();
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 이름으로 폴더 경로 생성
    string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 폴더가 없으면 생성
    if (!filesystem::exists(strModelFolder))
    {
        try
        {
            filesystem::create_directories(strModelFolder);
        }
        catch (const filesystem::filesystem_error& e)
        {
            string errorMsg = "폴더 생성 실패: " + string(e.what());
            MessageBoxA(nullptr, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
            return;
        }
    }

    // 파일 경로 생성 (strFileName 사용)
    string strDatPath = strModelFolder + strFileName + ".dat";
    string strAnimJsonPath = strModelFolder + strFileName + "_Anim.json";
    string strSummayAnimJsonPath = strModelFolder + strFileName + "_Summary_Anim.json";
    string strMaterialJsonPath = strModelFolder + strFileName + "_Material.json";

    // 덮어쓰기 확인
    _bool bDatExists = filesystem::exists(strDatPath);
    _bool bAnimExists = filesystem::exists(strAnimJsonPath);
    _bool bMaterialExists = filesystem::exists(strMaterialJsonPath);

    if (bDatExists || bAnimExists || bMaterialExists)
    {
        wstring msg = TEXT("다음 파일이 이미 존재합니다:\n\n");
        if (bDatExists)
            msg += AnsiToWString(strFileName) + TEXT(".dat\n");
        if (bAnimExists)
            msg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
        if (bMaterialExists)
            msg += AnsiToWString(strFileName) + TEXT("_Material.json\n");

        msg += L"\n덮어쓰시겠습니까?";

        _int result = MessageBox(
            nullptr,
            msg.c_str(),
            TEXT("파일 덮어쓰기 확인"),
            MB_YESNO | MB_ICONQUESTION
        );

        if (result == IDNO)
        {
            MSG_BOX(TEXT("저장을 취소했습니다."));
            return;
        }
    }

    // 1. Binary 저장 (.dat) - 전체 데이터
    Export_Binary(strDatPath);

    // 2. Animation JSON 저장
    if (m_eModelType == MODELTYPE::ANIM)
    {
        if (!Export_AnimationJson(strAnimJsonPath, strSummayAnimJsonPath))
        {
            MSG_BOX(TEXT("Animation JSON 저장 실패"));
            return;
        }
    }

    // 3. Material JSON 저장
    if (!Export_MaterialJson(strMaterialJsonPath))
    {
        MSG_BOX(TEXT("Material JSON 저장 실패"));
        return;
    }

    // 성공 메시지
    _wstring successMsg = TEXT("Export 완료!\n\n");
    successMsg += TEXT("폴더: ") + AnsiToWString(strModelFolder) + TEXT("\n");
    successMsg += AnsiToWString(strFileName) + TEXT(".dat (전체)\n");
    if (m_eModelType == MODELTYPE::ANIM)
        successMsg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
    successMsg += AnsiToWString(strFileName) + TEXT("_Material.json");

    MessageBox(nullptr, successMsg.c_str(), TEXT("Success"), MB_OK | MB_ICONINFORMATION);

    //이전 실행파일로 복귀
    SetCurrentDirectoryA(savedDir);
}

void CEditor_Model::ExportModel_NoMsg(string& strPath)
{
    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string();
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 이름으로 폴더 경로 생성
    string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 폴더가 없으면 생성
    if (!filesystem::exists(strModelFolder))
    {
        try
        {
            filesystem::create_directories(strModelFolder);
        }
        catch (const filesystem::filesystem_error& e)
        {
            string errorMsg = "폴더 생성 실패: " + string(e.what());
            OutputDebugStringA(errorMsg.c_str());
            return;
        }
    }

    // 파일 경로 생성 (strFileName 사용)
    string strDatPath = strModelFolder + strFileName + ".dat";
    string strAnimJsonPath = strModelFolder + strFileName + "_Anim.json";
    string strSummayAnimJsonPath = strModelFolder + strFileName + "_Summary_Anim.json";
    string strMaterialJsonPath = strModelFolder + strFileName + "_Material.json";

    // 덮어쓰기 확인
    _bool bDatExists = filesystem::exists(strDatPath);
    _bool bAnimExists = filesystem::exists(strAnimJsonPath);
    _bool bMaterialExists = filesystem::exists(strMaterialJsonPath);

    // 1. Material JSON 저장 ( .dds로 무조건 확정 )
    if (!Export_MaterialJson_ForDDS(strMaterialJsonPath))
    {
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        OutputDebugStringA("Material JSON 저장 실패");
        return;
    }

    // 2. Binary 저장 (.dat) - 전체 데이터
    Export_Binary_NoMsg(strDatPath);

    // 3. Animation JSON 저장
    if (m_eModelType == MODELTYPE::ANIM)
    {
        if (!Export_AnimationJson(strAnimJsonPath, strSummayAnimJsonPath))
        {
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            OutputDebugStringA("Animation JSON 저장 실패");
            return;
        }
    }

    // 성공 메시지
    _wstring successMsg = TEXT("Export 완료!\n\n");
    successMsg += TEXT("폴더: ") + AnsiToWString(strModelFolder) + TEXT("\n");
    successMsg += AnsiToWString(strFileName) + TEXT(".dat (전체)\n");
    if (m_eModelType == MODELTYPE::ANIM)
        successMsg += AnsiToWString(strFileName) + TEXT("_Anim.json\n");
    successMsg += AnsiToWString(strFileName) + TEXT("_Material.json");

    successMsg += TEXT("\n!!! Success !!!");

    OutputDebugStringA(WStringToAnsi(successMsg).c_str());
}

void CEditor_Model::LoadModel(string& strPath)
{
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
    // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());


    //_char currentDir[MAX_PATH];
    //GetCurrentDirectoryA(MAX_PATH, currentDir);
    OutputDebugStringA(("[Current Working Directory] " + clientDefaultDir.string() + "\n").c_str());

    if (!filesystem::exists(strPath))
    {
        MSG_BOX(TEXT(".dat 파일이 존재하지 않습니다."));
        return;
    }

    std::ifstream ifs(strPath, std::ios::binary);
    if (!ifs.is_open())
    {
        MSG_BOX(TEXT("binary 파일 열기 실패"));
        return;
    }

    m_Model_Data.LoadBinary(ifs);
    ifs.close();
}

void CEditor_Model::LoadNewModel(string& strPath)
{
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
    SetCurrentDirectoryA(clientDefaultDir.string().c_str());
    OutputDebugStringA(("[Current Working Directory] " + clientDefaultDir.string() + "\n").c_str());

    if (!filesystem::exists(strPath))
    {
        MSG_BOX(TEXT(".dat 파일이 존재하지 않습니다."));
        return;
    }

    std::ifstream ifs(strPath, std::ios::binary);
    if (!ifs.is_open())
    {
        MSG_BOX(TEXT("binary 파일 열기 실패"));
        return;
    }

    // 기존 애니메이션 데이터 백업
    std::vector<Engine::ANIMATION_DATA> backupAnimations = m_Model_Data.vecAnimation;
    std::vector<Engine::ANIMATION_SET_DATA> backupAnimSets = m_Model_Data.vecAnimationSets;

    // .dat 파일 로드
    MODEL_DATA datFileData = {};
    datFileData.LoadBinary(ifs);
    ifs.close();

    // 새 모델의 본 데이터는 그대로 유지 (애니메이션 추가된 모델 기준)
    std::vector<Engine::BONE_DATA> newBones = m_Model_Data.vecBones;

    // .dat 파일의 데이터를 현재 모델에 병합
    MergeModelData(datFileData, backupAnimations, backupAnimSets, newBones);
}

void CEditor_Model::Update_DAT_From_JSON(string& strPath)
{
    ///* 파일시스템에서 실행파일 위치를 .exe로 고정 */
    //_char exePath[MAX_PATH];
    //GetModuleFileNameA(NULL, exePath, MAX_PATH);
    //string exeDir = exePath;
    //size_t lastSlash = exeDir.find_last_of("\\/");
    //if (lastSlash != string::npos) exeDir = exeDir.substr(0, lastSlash);
    //SetCurrentDirectoryA(exeDir.c_str());

       /* 현재 실행파일 저장 */
    _char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    /* 실행파일 위치 Client/default 로 고정  */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
    // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());

    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string()+ "/";
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 폴더 경로
    //string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 파일 경로 생성
    string strDatPath = strDirectory + strFileName + ".dat";
    string strAnimJsonPath = strDirectory + strFileName + "_Anim.json";
    string strMaterialJsonPath = strDirectory + strFileName + "_Material.json";

    // .dat 파일 존재 확인
    if (!filesystem::exists(strDirectory))
    {
        _tchar szMessage[MAX_PATH] = {};
        swprintf_s(szMessage, TEXT(".dat 파일이 존재하지 않습니다!\n경로: %S"),
            strDatPath.c_str());
        MessageBox(nullptr, szMessage, TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // 1. 기존 .dat 파일 로드
    {
        ifstream ifs(strDatPath, ios::binary);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 열기 실패"));
            return;
        }
        m_Model_Data.LoadBinary(ifs);
        ifs.close();
    }

    // 2. Animation JSON 로드 (파일이 있으면)
    if (filesystem::exists(strAnimJsonPath))
    {
        ifstream ifs(strAnimJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Animation JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 애니메이션 교체
        m_Model_Data.vecAnimation = j.get<vector<ANIMATION_DATA>>();
        m_Model_Data.iNumAnimations = static_cast<_uint>(m_Model_Data.vecAnimation.size());
    }

    // 3. Material JSON 로드 (파일이 있으면)
    if (filesystem::exists(strMaterialJsonPath))
    {
        ifstream ifs(strMaterialJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Material JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 머티리얼 교체
        m_Model_Data.vecMaterials = j.get<vector<MATERIAL_DATA>>();
        m_Model_Data.iNumMaterials = static_cast<_uint>(m_Model_Data.vecMaterials.size());
    }

    // 4. 업데이트된 데이터를 .dat에 다시 저장
    {
        ofstream ofs(strDatPath, ios::binary);
        if (!ofs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 쓰기 실패"));
            return;
        }
        m_Model_Data.SaveBinary(ofs);
        ofs.close();
    }

    // 성공 메시지
    _tchar szMessage[MAX_PATH] = {};
    swprintf_s(szMessage, TEXT(".dat 파일 업데이트 완료!\n\n폴더: %S\n파일: %S.dat"),
        strDirectory.c_str(), strFileName.c_str());
    MessageBox(nullptr, szMessage, TEXT("Success"), MB_OK | MB_ICONINFORMATION);

    //실행파일 위치 복귀
    SetCurrentDirectoryA(savedDir);

}

HRESULT CEditor_Model::Ready_Meshes()
{
    /* 매쉬 몇 개인지 저장하기 */
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        CEditor_Mesh* pMesh = CEditor_Mesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
        if (pMesh == nullptr)
        {
            MSG_BOX(TEXT("진짜 비상 CEditor_Mesh::Create() 실패!!!!!!"));
            return E_FAIL;
        }
        m_Meshes.push_back(pMesh);

        MESH_DATA meshData;
        pMesh->Get_Data(meshData);
        m_Model_Data.vecMeshes.push_back(meshData);
    }

    return S_OK;
}

HRESULT CEditor_Model::Ready_Materials()
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        CEditor_MeshMaterial* pMeshMaterial = CEditor_MeshMaterial::Create(m_pDevice, m_pContext, m_pModelFilePath, m_pAIScene->mMaterials[i]);
        if (nullptr == pMeshMaterial)
            return E_FAIL;

        m_Materials.push_back(pMeshMaterial);

        MATERIAL_DATA materialData;
        pMeshMaterial->Get_Data(materialData);
        m_Model_Data.vecMaterials.push_back(materialData);
    }

    return S_OK;
}

HRESULT CEditor_Model::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
    CEditor_Bone* pBone = CEditor_Bone::Create(pAINode, iParentIndex);
    if (pBone == nullptr)
        return E_FAIL;

    m_Bones.push_back(pBone);

    BONE_DATA boneData;
    pBone->Get_Data(boneData);
    m_Model_Data.vecBones.push_back(boneData);

    _uint  iIndex = static_cast<_uint>(m_Bones.size() - 1);

    for (size_t i = 0; i < pAINode->mNumChildren; i++)
        Ready_Bones(pAINode->mChildren[i], iIndex);

    return S_OK;
}

HRESULT CEditor_Model::Ready_Animation()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

	_uint iSkipCount = 0;

    for (_uint i = 0; i < m_iNumAnimations; i++)
    {
        CEditor_Animation* pAnimation = CEditor_Animation::Create(m_pAIScene->mAnimations[i], m_Bones);
        if (nullptr == pAnimation)
            return E_FAIL;

		if (pAnimation->Get_Skip())
		{
			Safe_Release(pAnimation);
            iSkipCount++;
			continue;
		}

        m_Animations.push_back(pAnimation);

        ANIMATION_DATA animationData;
        pAnimation->Get_Data(animationData);
        m_Model_Data.vecAnimation.push_back(animationData);
        OutputDebugStringA((pAnimation->Get_Name()+"\n").c_str());
    }

	m_iNumAnimations -= iSkipCount;

    return S_OK;
}

void CEditor_Model::OnRootMotion()
{
    m_isRootMotion = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isRootMotion;

    if (m_isRootMotion)
    {
        m_isRootMotion_Pos = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isApplyRootPosition;
        m_isIgonreRootRot = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.isIgnoreRootRot;

        FLOAT3_DATA scale = m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup.RootMitionScale;
        m_vRootMotionScale = XMVectorSet(scale.x, scale.y, scale.z, 1.f);
    }
}

void CEditor_Model::Update_RootMotion(_float fTimeDelta, _bool isBlending)
{
    //////////////////////////////////////////
    static _float s_totalRotation = 0.0f;
    static _int s_lastAnimIndex = -1;

    if (s_lastAnimIndex != m_iCurrentAnimIndex)
    {
        if (s_lastAnimIndex != -1)
        {
            char msg[128];
            sprintf_s(msg, "[RESET] Anim changed. TotalRot was %.1f°\n",
                XMConvertToDegrees(s_totalRotation));
            OutputDebugStringA(msg);
        }
        s_totalRotation = 0.0f;
        s_lastAnimIndex = m_iCurrentAnimIndex;
    }
////////////////////////////////////////////



    _matrix CurrentCombinedRootMatrix = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    if (m_isRootMotion_Pos)
    {
        // 현재 루트본에서 첫 프레임 오프셋 빼기
        _vector vCurPos = XMVectorSubtract(CurrentCombinedRootMatrix.r[3], m_vFirstFrameRootOffset);

        //_vector vCurPos = CurrentCombinedRootMatrix.r[3];
        _vector vDelta = XMVectorSubtract(vCurPos, m_PreRootMatrix.r[3]);
        vDelta = XMVectorMultiply(vDelta, m_vRootMotionScale);


        m_vRootMotionDelta.r[3] = XMVectorSetW(vDelta, 0.f);
        m_vRootMotionDelta.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f);
        m_vRootMotionDelta.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        m_vRootMotionDelta.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);

        m_PreRootMatrix.r[3] = vCurPos;
    }

#pragma region Rotation test


    //if (m_isRootMotion_Rot)
    //{
    //    _matrix CurrentRotCombined = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    //    _vector dummy;
    //    _vector curQuat, preQuat;

    //    _matrix safeCur = CurrentRotCombined;
    //    safeCur.r[0] = XMVector3Normalize(safeCur.r[0]);
    //    safeCur.r[1] = XMVector3Normalize(safeCur.r[1]);
    //    safeCur.r[2] = XMVector3Normalize(safeCur.r[2]);
    //    XMMatrixDecompose(&dummy, &curQuat, &dummy, safeCur);

    //    _matrix safePre = m_PreRotRootMatrix;
    //    safePre.r[0] = XMVector3Normalize(safePre.r[0]);
    //    safePre.r[1] = XMVector3Normalize(safePre.r[1]);
    //    safePre.r[2] = XMVector3Normalize(safePre.r[2]);
    //    XMMatrixDecompose(&dummy, &preQuat, &dummy, safePre);

    //    // Hemisphere fix (쿼터니언 부호 일치)
    //    if (XMVectorGetX(XMQuaternionDot(curQuat, preQuat)) < 0.f)
    //        curQuat = XMVectorNegate(curQuat);

    //    //델타 쿼터니언 계산
    //    _vector localDelta = XMQuaternionMultiply(XMQuaternionInverse(preQuat), curQuat);
    //    localDelta = XMQuaternionNormalize(localDelta);

    //    // Yaw 계산 (쿼터니언 → Yaw)
    //    auto GetYawDeg = [](_vector q) -> float {
    //        float yaw = atan2f(
    //            2.0f * (XMVectorGetW(q) * XMVectorGetY(q) + XMVectorGetX(q) * XMVectorGetZ(q)),
    //            1.0f - 2.0f * (XMVectorGetY(q) * XMVectorGetY(q) + XMVectorGetZ(q) * XMVectorGetZ(q))
    //        );
    //        return XMConvertToDegrees(yaw);
    //        };

    //    _float curYawDeg = GetYawDeg(curQuat);
    //    _float preYawDeg = GetYawDeg(preQuat);
    //    _float deltaYawDeg = GetYawDeg(localDelta);
    //    //각도 정규화 함수 (항상 -180~180 유지)
    //    auto NormalizeAngle = [](float deg) -> float {
    //        while (deg > 180.f) deg -= 360.f;
    //        while (deg < -180.f) deg += 360.f;
    //        return deg;
    //        };

    //    curYawDeg = NormalizeAngle(curYawDeg);
    //    preYawDeg = NormalizeAngle(preYawDeg);
    //    deltaYawDeg = NormalizeAngle(curYawDeg - preYawDeg);

    //    // 누적 회전
    //    s_totalRotation += XMConvertToRadians(deltaYawDeg);

    //    // 쿼터니언 누적 반영
    //    m_vRootDeltaQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(deltaYawDeg));

    //    // 다음 프레임 대비
    //    m_PreRotRootMatrix = CurrentRotCombined;

    //    _vector scale, rot, pos;
    //    XMMatrixDecompose(&scale, &rot, &pos, CurrentRotCombined);

    //    _matrix resetRotation = XMMatrixAffineTransformation(scale, XMVectorZero(), m_vPreTransformQuat, pos);
    //    m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(resetRotation);

    //    // 디버그 로그
    //    static int frameCount = 0;
    //    if (m_fCurrentTrackPosition < 3.0f || fabs(deltaYawDeg) > 0.01f)
    //    {
    //        char msg[512];
    //        sprintf_s(msg, "[RM_ROT #%d] T%.2f | Cur:%.1f° Pre:%.1f° → Δ:%.2f° Total:%.1f°\n",
    //            frameCount++,
    //            m_fCurrentTrackPosition,
    //            curYawDeg, preYawDeg, deltaYawDeg,
    //            XMConvertToDegrees(s_totalRotation));
    //        OutputDebugStringA(msg);
    //    }

        //////////////////////////////////////////////////////////////////////////////////////////////////
        //_float curYaw = atan2f(2.0f * (XMVectorGetW(curQuat) * XMVectorGetY(curQuat) +
        //    XMVectorGetX(curQuat) * XMVectorGetZ(curQuat)),
        //    1.0f - 2.0f * (XMVectorGetY(curQuat) * XMVectorGetY(curQuat) +
        //        XMVectorGetZ(curQuat) * XMVectorGetZ(curQuat)));

        //_float preYaw = atan2f(2.0f * (XMVectorGetW(preQuat) * XMVectorGetY(preQuat) +
        //    XMVectorGetX(preQuat) * XMVectorGetZ(preQuat)),
        //    1.0f - 2.0f * (XMVectorGetY(preQuat) * XMVectorGetY(preQuat) +
        //        XMVectorGetZ(preQuat) * XMVectorGetZ(preQuat)));

        //_vector localDelta = XMQuaternionMultiply(XMQuaternionInverse(preQuat), curQuat);
        //localDelta = XMQuaternionNormalize(localDelta);

        //_float yaw = atan2f(2.0f * (XMVectorGetW(localDelta) * XMVectorGetY(localDelta) +
        //    XMVectorGetX(localDelta) * XMVectorGetZ(localDelta)),
        //    1.0f - 2.0f * (XMVectorGetY(localDelta) * XMVectorGetY(localDelta) +
        //        XMVectorGetZ(localDelta) * XMVectorGetZ(localDelta)));

        //m_vRootDeltaQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);

        //s_totalRotation += yaw;

        //m_PreRotRootMatrix = CurrentRotCombined;

        //_vector scale, rot, pos;
        //XMMatrixDecompose(&scale, &rot, &pos, CurrentRotCombined);

        //_matrix resetRotation = XMMatrixAffineTransformation(scale, XMVectorZero(), m_vPreTransformQuat, pos);
        //m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(resetRotation);

        //// ============ 모든 프레임 로그 (처음 5개만) ============
        //static int frameCount = 0;
        //if (m_fCurrentTrackPosition < 3.0f || abs(yaw) > 0.017f)
        //{
        //    char msg[512];
        //    sprintf_s(msg, "[RM_ROT #%d] T%.2f | Cur:%.1f° Pre:%.1f° → Δ:%.2f° Total:%.1f°\n",
        //        frameCount++,
        //        m_fCurrentTrackPosition,
        //        XMConvertToDegrees(curYaw),
        //        XMConvertToDegrees(preYaw),
        //        XMConvertToDegrees(yaw),
        //        XMConvertToDegrees(s_totalRotation));
        //    OutputDebugStringA(msg);
        //}
    //}

#pragma endregion


    //블랜딩 중에는 원점으로 리셋하지 못해서 블랜딩 끝나고 맨 처음.. 루트본의 위치만큼 앞으로 튀어나가는 현상 발생.
    //블랜딩 여부에 상관없이 리셋하자
    if (m_isRootMotion_Pos || m_isIgnoreRootPos)
    {
        CurrentCombinedRootMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
        m_Bones[m_iRootBoneIndex]->Set_CombinedTransformationMatrix(CurrentCombinedRootMatrix);
    }

   // m_vRootDeltaQuat = XMQuaternionIdentity();

}

void CEditor_Model::Apply_RootMotion_To_Transform()
{
    if (!m_pOwnerTransform)
        return;

    _matrix worldMatrix = m_pOwnerTransform->Get_WorldMatrix();

    //델타 분해 
    _vector deltaScale, deltaRot, deltaPos;
    XMMatrixDecompose(&deltaScale, &deltaRot, &deltaPos, m_vRootMotionDelta);

    // 월드 분해
    _vector worldScale, worldRot, worldPos;
    XMMatrixDecompose(&worldScale, &worldRot, &worldPos, worldMatrix);


    /////////////////////////////////////////////////////////
       // 적용 전 Owner 상태
    _float beforeYaw = atan2f(2.0f * (XMVectorGetW(worldRot) * XMVectorGetY(worldRot) +
        XMVectorGetX(worldRot) * XMVectorGetZ(worldRot)),
        1.0f - 2.0f * (XMVectorGetY(worldRot) * XMVectorGetY(worldRot) +
            XMVectorGetZ(worldRot) * XMVectorGetZ(worldRot)));
    /////////////////////////////////////////////////////////


    // 새로운 회전 = 기존 회전 * 델타 회전 (쿼터니언 곱셈)
    _vector newRot = XMQuaternionMultiply(m_vRootDeltaQuat, worldRot);
    newRot = XMQuaternionNormalize(newRot);
    //_vector newRot = m_vRootDeltaQuat;

    // 새로운 위치 = 기존 위치 + (델타 위치를 월드 회전으로 변환)
    _vector rotatedDeltaPos = XMVector3Rotate(deltaPos, worldRot);
    _vector newPos = worldPos + rotatedDeltaPos;

    // 새 월드 행렬 구성 (스케일 유지 + 새 회전 + 새 위치)
    _matrix newWorld = XMMatrixAffineTransformation(worldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), newRot, newPos);
        //    _matrix newWorld = XMMatrixScalingFromVector(worldScale)
        //* XMMatrixRotationQuaternion(newRot)
        //* XMMatrixTranslationFromVector(newPos);

    // Transform에 적용
    m_pOwnerTransform->Set_WorldMatrix(newWorld);


    //////////////////////////////////////////
// 의미있는 변화만 출력
    _float deltaYaw = atan2f(2.0f * (XMVectorGetW(m_vRootDeltaQuat) * XMVectorGetY(m_vRootDeltaQuat) +
        XMVectorGetX(m_vRootDeltaQuat) * XMVectorGetZ(m_vRootDeltaQuat)),
        1.0f - 2.0f * (XMVectorGetY(m_vRootDeltaQuat) * XMVectorGetY(m_vRootDeltaQuat) +
            XMVectorGetZ(m_vRootDeltaQuat) * XMVectorGetZ(m_vRootDeltaQuat)));

    _float deltaLen = XMVectorGetX(XMVector3Length(rotatedDeltaPos));

    if (abs(deltaYaw) > 0.017f || deltaLen > 0.01f)  // 1도 이상 또는 1cm 이상
    {
        _float beforeYaw = atan2f(2.0f * (XMVectorGetW(worldRot) * XMVectorGetY(worldRot) +
            XMVectorGetX(worldRot) * XMVectorGetZ(worldRot)),
            1.0f - 2.0f * (XMVectorGetY(worldRot) * XMVectorGetY(worldRot) +
                XMVectorGetZ(worldRot) * XMVectorGetZ(worldRot)));

        _float afterYaw = atan2f(2.0f * (XMVectorGetW(newRot) * XMVectorGetY(newRot) +
            XMVectorGetX(newRot) * XMVectorGetZ(newRot)),
            1.0f - 2.0f * (XMVectorGetY(newRot) * XMVectorGetY(newRot) +
                XMVectorGetZ(newRot) * XMVectorGetZ(newRot)));

        char msg[256];
        sprintf_s(msg, "[APPLY] Before:%.1f° + d:%.2f° = After:%.1f°\n",
            XMConvertToDegrees(beforeYaw),
            XMConvertToDegrees(deltaYaw),
            XMConvertToDegrees(afterYaw));
        //OutputDebugStringA(msg);
    }
    /////////////////////////////////////////////////////////



    m_vRootDeltaQuat = XMQuaternionIdentity();
    m_vRootMotionDelta = XMMatrixIdentity();
}

_bool CEditor_Model::Export_AnimationJson(const string& strFilePath, const string& strFilePath2)
{
	/* 전체 애니메이션 JSON 저장*/

	JSON j = m_Model_Data.vecAnimation;
	ofstream ofs(strFilePath);
	if (!ofs.is_open())
		return false;

	ofs << j.dump(4);
	ofs.close();

	if (!ofs.good())
		return false;


	/* 요약본 저장 */

	ANIMATION_SUMMARIES_DATA AnimSummaries;

	for (const auto& anim : m_Model_Data.vecAnimation)
	{
		ANIMATION_SUMMARY_DATA summary;
		summary.strName = anim.strName;
		summary.fDuration = anim.fDuration;
		summary.fTickPerSecond = anim.fTickPerSecond;
		summary.iNumChannels = anim.iNumChannels;
        summary.animSetup = anim.animSetup;

		AnimSummaries.vecSummaries.push_back(summary);
	}
    AnimSummaries.vecAnimationSets = m_Model_Data.vecAnimationSets;

	JSON j2 = AnimSummaries;
	ofstream ofs2(strFilePath2);
	if (!ofs2.is_open())
		return false;

	ofs2 << j2.dump(4);
	ofs2.close();

	if (!ofs2.good())
		return false;


    return true;
}

_bool CEditor_Model::Export_MaterialJson(const string& strFilePath)
{
    _bool    isSuccess = true;
    JSON j = m_Model_Data.vecMaterials;

    ofstream ofs(strFilePath);
    if (ofs.is_open())
    {
        //string jsonStr = j.dump(4);
        //jsonStr = PostProcessJSON(jsonStr);
        //ofs << jsonStr;
		ofs << j.dump(4);
        if (!ofs.good())
            isSuccess = false;

        ofs.close();
    }
    else
    {
        isSuccess = false;
    }

    return isSuccess;
}

_bool CEditor_Model::Export_MaterialJson_ForDDS(const string& strFilePath)
{
    _bool    isSuccess = true;

    for (auto& pMaterial : m_Model_Data.vecMaterials)
    {
        for (auto& Exts : pMaterial.vecExts)
        {
            for (auto& pExt : Exts)
                pExt = ".dds";
        }
        for (auto& FullPaths : pMaterial.vecFullPaths)
        {
            for (auto& pFullPath : FullPaths)
            {
                string strFixFullPath = {};

                _char szDrive[MAX_PATH] = {};
                _char szDirectory[MAX_PATH] = {};
                _char szFileName[MAX_PATH] = {};
                _char szExtern[MAX_PATH] = {};

                _splitpath_s(pFullPath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExtern, MAX_PATH);

                pFullPath = szDrive;
                pFullPath += szDirectory;
                pFullPath += szFileName;
                pFullPath += ".dds";
            }
        }
    }

    JSON j = m_Model_Data.vecMaterials;

    ofstream ofs(strFilePath);
    if (ofs.is_open())
    {
        //string jsonStr = j.dump(4);
        //jsonStr = PostProcessJSON(jsonStr);
        //ofs << jsonStr;
        ofs << j.dump(4);
        if (!ofs.good())
            isSuccess = false;

        ofs.close();
    }
    else
    {
        isSuccess = false;
    }

    return isSuccess;
}

void CEditor_Model::Export_Binary(const string& strFilePath)
{
    // 현재 작업 디렉터리 확인
    filesystem::path currentDir = filesystem::current_path();
    OutputDebugStringA(("[Current Directory] " + currentDir.string() + "\n").c_str());

    // 실제로 열리는 절대 경로 계산
    filesystem::path absolutePath = filesystem::absolute(strFilePath);
    OutputDebugStringA(("[Full Binary Path] " + absolutePath.string() + "\n").c_str());

    ofstream ofs(strFilePath, ios::binary);
    if (!ofs.is_open())
    {
        MSG_BOX(TEXT("binary 파일 열기 실패"));
        return;
    }

    m_Model_Data.SaveBinary(ofs);

    ofs.close();

    MSG_BOX(TEXT("Binary 파일 저장 성공"));
}

void CEditor_Model::Export_Binary_NoMsg(const string& strFilePath)
{
    ofstream ofs(strFilePath, ios::binary);
    if (!ofs.is_open())
    {
        OutputDebugStringA("Binary 파일 열기 실패");
        return;
    }

    m_Model_Data.SaveBinary(ofs);

    ofs.close();

    OutputDebugStringA("Binary 파일 저장 성공");
}


void CEditor_Model::MergeModelData(
    const MODEL_DATA& datFileData,
    const vector<ANIMATION_DATA>& currentAnimations,
    const vector<ANIMATION_SET_DATA>& currentAnimSets,
    const vector<BONE_DATA>& newBones)
{
    // 1. Mesh, Material은 .dat 파일 것으로 덮어쓰기
    m_Model_Data.vecMeshes = datFileData.vecMeshes;
    m_Model_Data.vecMaterials = datFileData.vecMaterials;
    m_Model_Data.iNumMeshes = datFileData.iNumMeshes;
    m_Model_Data.iNumMaterials = datFileData.iNumMaterials;

    // 2. Bone은 새 모델(애니메이션 추가된 모델) 것 유지
    m_Model_Data.vecBones = newBones;

    // 3. 애니메이션 병합: .dat의 애니메이션 데이터를 이름 기준으로 현재 모델에 적용
    // 현재 모델의 애니메이션 이름 -> 인덱스 맵 생성
    std::unordered_map<std::string, size_t> currentAnimNameToIndex;
    for (size_t i = 0; i < currentAnimations.size(); ++i)
    {
        currentAnimNameToIndex[currentAnimations[i].strName] = i;
    }

    // .dat 파일의 애니메이션 이름 -> setup 데이터 맵 생성
    std::unordered_map<std::string, ANIMATION_SETUP_DATA> datAnimSetupMap;
    for (const auto& datAnim : datFileData.vecAnimation)
    {
        datAnimSetupMap[datAnim.strName] = datAnim.animSetup;
    }

    // 현재 모델의 애니메이션에 .dat의 setup 데이터 덮어쓰기
    m_Model_Data.vecAnimation = currentAnimations; // 현재 애니메이션 구조 유지
    for (auto& anim : m_Model_Data.vecAnimation)
    {
        auto it = datAnimSetupMap.find(anim.strName);
        if (it != datAnimSetupMap.end())
        {
            // 같은 이름의 애니메이션이 .dat에 있으면 setup 데이터만 덮어쓰기
            anim.animSetup = it->second;
        }
        // 없으면 현재 setup 데이터 유지 (새로 추가된 애니메이션)
    }

    // 4. Animation Set 재구성
    // .dat의 Animation Set을 현재 모델의 애니메이션 인덱스에 맞게 변환
    m_Model_Data.vecAnimationSets.clear();

    for (const auto& datAnimSet : datFileData.vecAnimationSets)
    {
        Engine::ANIMATION_SET_DATA newAnimSet;
        newAnimSet.strAnimSetName = datAnimSet.strAnimSetName;

        // .dat의 인덱스를 현재 모델의 인덱스로 변환
        for (int oldIndex : datAnimSet.vecAnimIndices)
        {
            if (oldIndex >= 0 && oldIndex < (int)datFileData.vecAnimation.size())
            {
                const std::string& animName = datFileData.vecAnimation[oldIndex].strName;

                // 현재 모델에서 같은 이름의 애니메이션 찾기
                auto it = currentAnimNameToIndex.find(animName);
                if (it != currentAnimNameToIndex.end())
                {
                    newAnimSet.vecAnimIndices.push_back(static_cast<int>(it->second));
                }
            }
        }

        // 유효한 인덱스가 있는 경우만 추가
        if (!newAnimSet.vecAnimIndices.empty())
        {
            m_Model_Data.vecAnimationSets.push_back(newAnimSet);
        }
    }

    // 5. 애니메이션 카운트 업데이트
    m_Model_Data.iNumAnimations = static_cast<unsigned int>(m_Model_Data.vecAnimation.size());

    // 6. 기타 메타데이터는 .dat 파일 것으로
    m_Model_Data.strModelFilePath = datFileData.strModelFilePath;
    m_Model_Data.strModelName = datFileData.strModelName;
    m_Model_Data.iModelType = datFileData.iModelType;
    m_Model_Data.vPreTransformMatrix = datFileData.vPreTransformMatrix;
}

string CEditor_Model::PostProcessJSON(const string& jsonStr)
{
    istringstream iss(jsonStr);
    ostringstream oss;
    string line;
    string arrayBuffer;
    bool inShortArray = false;

    while (getline(iss, line))
    {
        // 배열 시작 감지
        if (line.find('[') != string::npos &&
            line.find(']') == string::npos)
        {
            // 다음 몇 줄을 확인해서 짧은 배열인지 판단
            arrayBuffer = line;
            inShortArray = true;
            continue;
        }

        if (inShortArray)
        {
            arrayBuffer += line;

            // 배열 끝 감지
            if (line.find(']') != string::npos)
            {
                // 한 줄로 압축
                string compressed = CompressArray(arrayBuffer);
                oss << compressed << "\n";
                inShortArray = false;
                arrayBuffer.clear();
                continue;
            }
        }
        else
        {
            oss << line << "\n";
        }
    }

    return oss.str();
}

string CEditor_Model::CompressArray(const string& arrayStr)
{
    string result = arrayStr;

    // 줄바꿈 제거
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());

    // 연속된 공백을 하나로
    result = regex_replace(result, regex(R"(\s+)"), " ");

    return result;
}


CEditor_Model* CEditor_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
    CEditor_Model* pInstance = new CEditor_Model(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Model"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CEditor_Model::Clone(void* pArg)
{
    CEditor_Model* pInstance = new CEditor_Model(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Model"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEditor_Model::Free()
{
    __super::Free();

    Safe_Release(m_pOwnerTransform);

    for (auto& pAinmation : m_Animations)
        Safe_Release(pAinmation);
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

    m_Importer.FreeScene();

}




void CEditor_Model::DebugDumpRootBonePerFrame()
{
    if (m_iRootBoneIndex >= m_Bones.size()) return;

    // combined matrix for root bone (PreTransform already applied in Update_CombinedTransformationMatrix)
    _matrix rootMat = m_Bones[m_iRootBoneIndex]->Get_CombinedTransformationMatrix();

    // decompose
    _vector vScale, vQuat, vTrans;
    XMMatrixDecompose(&vScale, &vQuat, &vTrans, rootMat);

    XMFLOAT3 scale3, trans3;
    XMFLOAT4 quat4;
    XMStoreFloat3(&scale3, vScale);
    XMStoreFloat3(&trans3, vTrans);
    XMStoreFloat4(&quat4, vQuat);

    float pitchDeg, yawDeg, rollDeg;
    QuatToEulerDegrees(vQuat, pitchDeg, yawDeg, rollDeg);

    float trackPos = m_fCurrentTrackPosition; // 또는 *m_pTrackPosPtr 등 사용

    // matrix elements (row-major view)
    XMFLOAT4X4 mat;
    XMStoreFloat4x4(&mat, rootMat);

    char buf[1024];
    sprintf_s(buf, "[RootBone Debug] Track: %.4f | Pos: (%.6f, %.6f, %.6f) | Scale: (%.6f, %.6f, %.6f) | Quat: (%.6f, %.6f, %.6f, %.6f) | Euler(deg): P(%.2f) Y(%.2f) R(%.2f)\n"
        " Mat rows:\n"
        "  r0: %.6f %.6f %.6f %.6f\n"
        "  r1: %.6f %.6f %.6f %.6f\n"
        "  r2: %.6f %.6f %.6f %.6f\n"
        "  r3: %.6f %.6f %.6f %.6f\n",
        trackPos,
        trans3.x, trans3.y, trans3.z,
        scale3.x, scale3.y, scale3.z,
        quat4.x, quat4.y, quat4.z, quat4.w,
        pitchDeg, yawDeg, rollDeg,
        mat._11, mat._12, mat._13, mat._14,
        mat._21, mat._22, mat._23, mat._24,
        mat._31, mat._32, mat._33, mat._34,
        mat._41, mat._42, mat._43, mat._44);

    OutputDebugStringA(buf);
}
