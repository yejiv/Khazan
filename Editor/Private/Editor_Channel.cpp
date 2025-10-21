#include "Editor_Channel.h"
#include "Editor_Bone.h"

CEditor_Channel::CEditor_Channel()
{
}

HRESULT CEditor_Channel::Initialize(const aiNodeAnim* pAIChannel, const vector<CEditor_Bone*>& Bones)
{
    /* 어떤 뼈인지 알아내기 */
    auto iter = find_if(Bones.begin(), Bones.end(), [&](CEditor_Bone* pBone) {
        if (pBone->Compare_Name(pAIChannel->mNodeName.data))
            return true;

        m_iBoneIndex++;

        return false;
        });

    /* 최대 프레임 수 알아내기  */
    m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, max(pAIChannel->mNumRotationKeys, pAIChannel->mNumPositionKeys));

    /* 지역 변수로 뺀 이유 : 프레임이 끝난 녀석은 마지막 value를 유지하기 위해  */
    _float3     vScale{};
    _float4     vRotation{};
    _float3     vTranslation{};

    for (size_t i = 0; i < m_iNumKeyFrames ; i++)
    {
        KEYFRAME KeyFrame{};

        /*  KeyFrame.fTrackPosition은 전부 같겠지만 모든 if문에 해줘야 소실되는 일이 발생하지 않음 */
        if (i < pAIChannel->mNumScalingKeys)
        {
            memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mScalingKeys[i].mTime);
        }

        if (i < pAIChannel->mNumRotationKeys)
        {
            vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

            KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mRotationKeys[i].mTime);
        }

        if (i < pAIChannel->mNumPositionKeys)
        {
            memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mPositionKeys[i].mTime);
        }

        KeyFrame.vScale = vScale;
        KeyFrame.vRotation = vRotation;
        KeyFrame.vTranslation = vTranslation;

        m_KeyFrames.push_back(KeyFrame);

        KEYFRAME_DATA keyFrame_Data;

        memcpy(&keyFrame_Data.scale, &KeyFrame.vScale, sizeof(_float3));
        memcpy(&keyFrame_Data.rotation, &KeyFrame.vRotation, sizeof(_float4));
        memcpy(&keyFrame_Data.translation, &KeyFrame.vTranslation, sizeof(_float3));
        keyFrame_Data.trackPostion = KeyFrame.fTrackPosition;

        m_Channel_Data.vecKeyFrames.push_back(keyFrame_Data);

    }

    m_Channel_Data.strName = string(m_szName);
    m_Channel_Data.iBoneIndex = m_iBoneIndex;
    m_Channel_Data.iNumKeyFrame = m_iNumKeyFrames;

    return S_OK;
}

void CEditor_Channel::Update_TransformationMatrix(const vector<CEditor_Bone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyFrameIndex = 0;

    /* 선택된 애니메이션이 이용하고 있는 이 뼈(Channel)의 현재 재생된 위치(fCurrrentTrackPosition)에 맞는 상태행렬을 만들어 준다. */
    _vector         vScale, vRotation, vTranslation;

    /* 마지막 키프레임상태를 취하낟. */
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&LastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
        m_TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);
    }

    /* 양쪽 키프레임사이에서의 중간상태를 보간하여 만든다. */
    else
    {
        while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
            ++*pCurrentKeyFrameIndex;

        _vector    vSourScale, vDestScale;
        _vector    vSourRotation, vDestRotation;
        _vector    vSourTranslation, vDestTranslation;

        vSourScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
        vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
        vSourTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f);

        vDestScale = XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
        vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
        vDestTranslation = XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f);

        _float      fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);
        vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(vSourTranslation, vDestTranslation, fRatio), 1.f);


    }

    if (m_isBlendPreAnimation)
    {
        // 이전 애니메이션과의 보간 처리 
        vScale = XMVectorLerp(m_vPrevScale, vScale, m_fAnimationRatio);
        vRotation = XMQuaternionSlerp(m_vPrevRotQuat, vRotation, m_fAnimationRatio);
        vTranslation = XMVectorLerp(m_vPrevPositon, vTranslation, m_fAnimationRatio);
    }


    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    m_TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(m_TransformationMatrix);
}

void CEditor_Channel::Set_PrevAnimationBlend(const _float& fAnimationRatio, _matrix& PreAnimationMatrix)
{
    XMMatrixDecompose(&m_vPrevScale, &m_vPrevRotQuat, &m_vPrevPositon, PreAnimationMatrix);


    //_float3 scale;
    //XMStoreFloat3(&scale, m_vPrevScale);
    //char debugMsg[256];
    //sprintf_s(debugMsg, "[Channel %d] PrevScale: (%.4f, %.4f, %.4f), Ratio: %.4f\n",
    //    m_iBoneIndex, scale.x, scale.y, scale.z, fAnimationRatio);
    //OutputDebugStringA(debugMsg);


    m_fAnimationRatio = fAnimationRatio;
    m_isBlendPreAnimation = true;
}

CEditor_Channel* CEditor_Channel::Create(const aiNodeAnim* pAIChannel, const vector<class CEditor_Bone*>& Bones)
{
    CEditor_Channel* pInstance = new CEditor_Channel();

    if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Channel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEditor_Channel::Free()
{
    __super::Free();
}

