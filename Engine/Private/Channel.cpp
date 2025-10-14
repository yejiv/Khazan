
#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNEL_DATA& data)
{
    // 데이터 유효성 검사 추가
    if (data.vecKeyFrames.empty())
    {
        OutputDebugStringA("[CChannel::Initialize] 키프레임 데이터가 비어있습니다.\n");
        return E_FAIL;
    }

    // 본 인덱스 유효성 검사
    if (data.iBoneIndex == UINT_MAX) 
    {
        OutputDebugStringA("[CChannel::Initialize] 유효하지 않은 본 인덱스입니다.\n");
        return E_FAIL;
    }

    /* 어떤 뼈인지 알아내기 */
    m_iBoneIndex = data.iBoneIndex;

    /* 최대 프레임 수 알아내기  */
    m_iNumKeyFrames = data.iNumKeyFrame;

    // 키프레임 수 검증
    if (m_iNumKeyFrames != data.vecKeyFrames.size())
    {
        OutputDebugStringA("[CChannel::Initialize] 키프레임 수가 일치하지 않습니다.\n");
        m_iNumKeyFrames = static_cast<_uint>(data.vecKeyFrames.size());
    }


    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME KeyFrame{};

        memcpy(&KeyFrame.vScale, &data.vecKeyFrames[i].scale, sizeof(_float3));
        memcpy(&KeyFrame.vTranslation, &data.vecKeyFrames[i].translation, sizeof(_float3));
        memcpy(&KeyFrame.vRotation, &data.vecKeyFrames[i].rotation, sizeof(_float4));
        KeyFrame.fTrackPosition = data.vecKeyFrames[i].trackPostion;

        m_KeyFrames.push_back(KeyFrame);
    }


    return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{   
    /* 트랙포지션이 0으로 되돌아갈 시  키프레임 인덱스도 0으로 되돌리기 위함. */
    if (fCurrentTrackPosition == 0.f)
        *pCurrentKeyFrameIndex  = 0;

    /* 선택된 애니메이션이 이용하고 있는 이 뼈(Channel)의 현재 재생된 위치(fCurrrentTrackPosition)에 맞는 상태행렬을 만들어 준다. */
    _vector         vScale, vRotation, vTranslation;

    /* 마지막 키프레임상태를 취한다. */
    KEYFRAME        LastKeyFrame = m_KeyFrames.back();

    //마지막 프레임이 지나도 유지할 수 있도록
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

        _float fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) / (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);
        vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);
        vTranslation = XMVectorSetW(XMVectorLerp(XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), 1.f), XMVectorSetW(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), 1.f), fRatio), 1.f);
    }

    /*_matrix         TransformationMatrix = XMMatrixScaling() * XMMatrixRotationQuaternion() * XMMatrixTranslation();*/
    _matrix         TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

    Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(CHANNEL_DATA& data)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(data)))
    {
        MSG_BOX(TEXT("Failed to Created : CChannel"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}

