
#include "Animation.h"
#include "Channel.h"
#include "GameInstance.h"

CAnimation::CAnimation()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
    m_Channels.clear();
    m_CurrentKeyFrameIndices.clear();
    Safe_AddRef(m_pGameInstance);
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_pGameInstance{ CGameInstance::GetInstance() }   
    , m_fDuration{ Prototype.m_fDuration }
    , m_fTickPerSecond{ Prototype.m_fTickPerSecond }
    , m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
    , m_iNumChannels{ Prototype.m_iNumChannels }
   // , m_Channels{Prototype.m_Channels}
    , m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
    , m_fBlendTime { Prototype.m_fBlendTime }
    , m_isLoop { Prototype.m_isLoop }
{
    for (auto& pPrototypeChannel : Prototype.m_Channels)
    {
        m_Channels.push_back(pPrototypeChannel->Clone());
    }
    Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimation::Initialize(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation)
{
   // OutputDebugStringA(("[CAnimation::Initialize] 애니 이름 : " + data.strName + "\n").c_str());

    m_fDuration = data.fDuration;
    m_iNumChannels = data.iNumChannels;
    m_fTickPerSecond = data.fTickPerSecond;
    m_fBlendTime = data.fAnimationBlendTime;
    m_isLoop = data.animSetup.isLoop;

    m_CurrentKeyFrameIndices.resize(m_iNumChannels);
    if (data.vecChannels.size() > 10000)
    {
        OutputDebugStringA(("[CAnimation::Initialize] 애니메이션 번호 : " + to_string(iCurAnimation) + "  비정상적인 Channel의 크기!!!!!!!!!!!!!\n").c_str());
        return S_OK;
    }

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(data.vecChannels[i]);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isUsedLoop, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{
#ifdef _DEBUG
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        OutputDebugStringA((" [ 트랙 위치 ] : " + to_string(*m_fCurrentTrackPosition) + "\n").c_str());
    }
#endif // _DEBUG

    * m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (*m_fCurrentTrackPosition >= m_fDuration)
    {

        if (isUsedLoop)
        {
            if (!isLoop)
            {
                *pFinished = true;
                *m_fCurrentTrackPosition = m_fDuration;
                return;
            }
            else
                *m_fCurrentTrackPosition = 0.f;
        }
        else
        {
            if (!m_isLoop)
            {
                *pFinished = true;
                *m_fCurrentTrackPosition = m_fDuration;
                return;
            }
            else
                *m_fCurrentTrackPosition = 0.f;
        }

    }

    /* 애니메이션 블랜딩 */
    if (m_isAnimationBlend)
        Update_AnimationBlend(Bones, fTimeDelta);

    /* 채널(뼈) 갱신 */
    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_Channels[i]->Update_TransformationMatrix(Bones, *m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);

}

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{
    *m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (*m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;
            *m_fCurrentTrackPosition = m_fDuration;
            return;
        }
        else
            *m_fCurrentTrackPosition = 0.f;
    }

    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_Channels[i]->Update_TransformationMatrix(Bones, *m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);
}

void CAnimation::Set_RootBoneIndex(_uint iRootBoneIndex)
{
    for (auto& pChannel : m_Channels)
    {
        if (pChannel->Get_BoneIndex() == iRootBoneIndex)
        {
            pChannel->Set_IsRootBone(true);
            break;
        }
    }
}

void CAnimation::Set_TrackPositionPtr(_float* pTrackPosition)
{
    m_fCurrentTrackPosition = pTrackPosition;
}

void CAnimation::OnAnimationBlend(map<_uint, _matrix>&& outChannelMatrices)
{
    *m_fCurrentTrackPosition = 0.f;
    fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
    m_isAnimationBlend = true;
    m_fCurBlendTime = 0.f;
    m_PreAnimationChannelMatrices = move(outChannelMatrices);
}

map<_uint, _matrix>& CAnimation::Get_ChannelMatrices()
{
    m_PreAnimationChannelMatrices.clear();
    for (_uint i = 0; i < m_iNumChannels; ++i) {
        m_PreAnimationChannelMatrices.emplace(m_Channels[i]->Get_BoneIndex(), m_Channels[i]->Get_TransformationMatrix());
    }

    return m_PreAnimationChannelMatrices;
}


//map<_uint, _matrix> CAnimation::Get_ChannelMatrices()  
//{
//    map<_uint, _matrix> result;  // 로운 map 생성
//
//    for (_uint i = 0; i < m_iNumChannels; ++i)
//    {
//        _matrix channelMatrix = m_Channels[i]->Get_TransformationMatrix();
//
//#ifdef _DEBUG
//        // 유효성 검증
//        _vector scale, rotQuat, position;
//        if (XMMatrixDecompose(&scale, &rotQuat, &position, channelMatrix))
//        {
//            _float3 scaleF3;
//            XMStoreFloat3(&scaleF3, scale);
//
//            if (scaleF3.x < 0.01f || scaleF3.y < 0.01f || scaleF3.z < 0.01f)
//            {
//                char buffer[256];
//                sprintf_s(buffer, "[Get_ChannelMatrices] Channel %d (Bone %d): Scale (%.3f, %.3f, %.3f) - Using Identity\n",
//                    i, m_Channels[i]->Get_BoneIndex(), scaleF3.x, scaleF3.y, scaleF3.z);
//                OutputDebugStringA(buffer);
//
//                channelMatrix = XMMatrixIdentity();
//            }
//        }
//        else
//        {
//            char buffer[256];
//            sprintf_s(buffer, "[Get_ChannelMatrices] Channel %d: Matrix decompose failed - Using Identity\n", i);
//            OutputDebugStringA(buffer);
//
//            channelMatrix = XMMatrixIdentity();
//        }
//#endif
//
//        result.emplace(m_Channels[i]->Get_BoneIndex(), channelMatrix);
//    }
//
//#ifdef _DEBUG
//    OutputDebugStringA(("[Get_ChannelMatrices] Collected " + to_string(result.size()) + " matrices\n").c_str());
//#endif
//
//    return result;  // RVO(Return Value Optimization)로 효율적으로 반환
//}


void CAnimation::Update_AnimationBlend(const vector<class CBone*>& Bones, _float fTimeDelta)
{
    /* ratio 계산 */
    m_fCurBlendTime += fTimeDelta;

    if (m_fCurBlendTime > m_fBlendTime) {
        m_isAnimationBlend = false;

        for (_uint i = 0; i < m_iNumChannels; ++i)
            m_Channels[i]->Reset_AnimationBlend();

        return;
    }

    _float fRatio = m_fCurBlendTime / m_fBlendTime;

    /* 채널에 보간에 사용 할 이전 뼈 행렬 세팅하기 */
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        _uint iBoneIndex = m_Channels[i]->Get_BoneIndex();

        if (m_isAnimationBlend && m_PreAnimationChannelMatrices.find(iBoneIndex) != m_PreAnimationChannelMatrices.end())
        {
            _matrix a = m_PreAnimationChannelMatrices[iBoneIndex];

            m_Channels[i]->Set_PrevAnimationBlend(fRatio, m_PreAnimationChannelMatrices[iBoneIndex]);
        }
    }
}

CAnimation* CAnimation::Create(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Initialize(Bones, data, iCurAnimation)))
    {
        MSG_BOX(TEXT("Failed to Created : CAnimation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CAnimation* CAnimation::Clone()
{
    return new CAnimation(*this);
}

void CAnimation::Free()
{
    __super::Free();


    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();


    Safe_Release(m_pGameInstance);
}
