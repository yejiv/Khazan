
#include "Animation.h"
#include "Channel.h"
#include "GameInstance.h"

CAnimation::CAnimation()
{
    m_Channels.clear();
    m_CurrentKeyFrameIndices.clear();
}

CAnimation::CAnimation(const CAnimation& Prototype)
    : m_pGameInstance{ CGameInstance::GetInstance() }   
    , m_fDuration ( Prototype.m_fDuration)
    , m_fTickPerSecond(Prototype.m_fTickPerSecond)
    , m_fCurrentTrackPosition(Prototype.m_fCurrentTrackPosition)
    , m_iNumChannels(Prototype.m_iNumChannels)
    , m_Channels(Prototype.m_Channels)
    , m_CurrentKeyFrameIndices(Prototype.m_CurrentKeyFrameIndices)
{
    Safe_AddRef(m_pGameInstance);

    for (auto& pChannel : m_Channels)
        Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation)
{
    m_fDuration = data.fDuration;
    m_iNumChannels = data.iNumChannels;
    m_fTickPerSecond = data.fTickPerSecond;

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

void CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{    
#ifdef _DEBUG
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_SPACE))
    {
        OutputDebugStringA((" [ 트랙 위치 ] : " + to_string(*m_fCurrentTrackPosition) + "\n").c_str());
    }
#endif // _DEBUG

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
    {
        m_Channels[i]->Update_TransformationMatrix(Bones, *m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);
    }
}

void CAnimation::Set_TrackPositionPtr(_float* pTrackPosition)
{
    m_fCurrentTrackPosition = pTrackPosition;
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
