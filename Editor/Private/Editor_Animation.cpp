#include "Editor_Animation.h"
#include "Editor_Channel.h"

CEditor_Animation::CEditor_Animation()
{
}

HRESULT CEditor_Animation::Initialize(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones)
{
    m_strName = string(pAIAnimation->mName.C_Str());
    m_fDuration = static_cast<_float>(pAIAnimation->mDuration);
    m_fTickPerSecond = static_cast<_float>(pAIAnimation->mTicksPerSecond);
    m_iNumChannels = pAIAnimation->mNumChannels;
    m_CurrentKeyFrameIndices.resize(m_iNumChannels);

    //string message = "Loaded animation: " + string(pAIAnimation->mName.C_Str()) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "mTicksPerSecond: " + to_string(m_fTickPerSecond) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "Duration: " + to_string(m_fDuration) + "\n";
    //OutputDebugStringA(message.c_str());

    //message = "\n";
    //OutputDebugStringA(message.c_str());


    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CEditor_Channel* pChannel = CEditor_Channel::Create(pAIAnimation->mChannels[i], Bones);
        if (nullptr == pChannel)
            return E_FAIL;

        m_Channels.push_back(pChannel);

        CHANNEL_DATA data;
        pChannel->Get_Data(data);
        m_Animation_Data.vecChannels.push_back(data);
    }
    m_Animation_Data.strName = m_strName;;
    m_Animation_Data.fDuration = m_fDuration;
    m_Animation_Data.fTickPerSecond = m_fTickPerSecond;
    m_Animation_Data.iNumChannels = m_iNumChannels;
    return S_OK;
}

void CEditor_Animation::Update_TransformationMatrices(const vector<class CEditor_Bone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{
    m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

    if (m_fCurrentTrackPosition >= m_fDuration)
    {
        if (false == isLoop)
        {
            *pFinished = true;
            m_fCurrentTrackPosition = m_fDuration;
            return;
        }
        else
            m_fCurrentTrackPosition = 0.f;

    }

    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);
    
}

CEditor_Animation* CEditor_Animation::Create(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones)
{
    CEditor_Animation* pInstance = new CEditor_Animation();

    if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_Animation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CEditor_Animation* CEditor_Animation::Clone()
{
    return new CEditor_Animation(*this);
}

void CEditor_Animation::Free()
{
    __super::Free();

    for (auto& pChannel : m_Channels)
        Safe_Release(pChannel);

    m_Channels.clear();
}
