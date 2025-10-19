#include "Editor_Animation.h"
#include "Editor_Channel.h"
#include "GameInstance.h"

CEditor_Animation::CEditor_Animation()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
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
    m_Animation_Data.strName = m_strName;
    m_Animation_Data.fDuration = m_fDuration;
    m_Animation_Data.fTickPerSecond = m_fTickPerSecond;
    m_Animation_Data.iNumChannels = m_iNumChannels;

    m_Animation_Data.animSetup.strName = m_strName;
    m_Animation_Data.animSetup.iDirection = GetAnimDirection(m_strName);


    return S_OK;
}

void CEditor_Animation::Update_TransformationMatrices(const vector<class CEditor_Bone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta)
{

    if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta) && m_pGameInstance->Key_Down(DIK_LCONTROL))
    {
        OutputDebugStringA(("------[ 트랙 위치  : " + to_string(*m_fCurrentTrackPosition) + "  ]-------\n").c_str());
    }

    if(m_isEnbleTrackPosition)
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

    if (m_isAnimationBlend)
        Update_AnimationBlend(Bones, fTimeDelta);


    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_Channels[i]->Update_TransformationMatrix(Bones, *m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i]);
    
}

void CEditor_Animation::OnAnimationBlend(map<_uint, _matrix>&& outChannelMatrices)
{
    *m_fCurrentTrackPosition = 0.f;
    fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
    m_isAnimationBlend = true;
    m_fCurBlendTime = 0.f;
    m_PreAnimationChannelMatrices = move(outChannelMatrices);
}

map<_uint, _matrix>& CEditor_Animation::Get_ChannelMatrices()
{
    m_PreAnimationChannelMatrices.clear();
    for (_uint i = 0; i < m_iNumChannels; ++i)
        m_PreAnimationChannelMatrices.emplace(m_Channels[i]->Get_BoneIndex(), m_Channels[i]->Get_TransformationMatrix());

    return m_PreAnimationChannelMatrices;
}

void CEditor_Animation::Set_TrackPositionPtr(_float* pTrackPosition)
{
    m_fCurrentTrackPosition = pTrackPosition;
}


void CEditor_Animation::Update_AnimationBlend(const vector<class CEditor_Bone*>& Bones, _float fTimeDelta)
{
    /* ratio 계산 */
    m_fCurBlendTime += fTimeDelta;

    if (m_fCurBlendTime > m_fBlendTime)
        m_isAnimationBlend = false;

    _float fRatio = m_fCurBlendTime / m_fBlendTime;

    /* 채널에 보간에 사용 할 이전 뼈 행렬 세팅하기 */
    for (_uint i = 0; i < m_iNumChannels; ++i)
    {
        _uint iBoneIndex = m_Channels[i]->Get_BoneIndex();

        if (m_isAnimationBlend && m_PreAnimationChannelMatrices.find(iBoneIndex) != m_PreAnimationChannelMatrices.end())
        {
            m_Channels[i]->Set_PrevAnimationBlend(fRatio, m_PreAnimationChannelMatrices[iBoneIndex]);
        }
    }
}

_uint CEditor_Animation::GetAnimDirection(const string& animName)
{
    string name = animName;
    transform(name.begin(), name.end(), name.begin(), ::toupper); //대문자 통일 

    struct DirectionMatch {
        const char* key1;
        const char* key2;
        ANIM_DIRECTION dir;
    };

    static const DirectionMatch table[] = {
        {"_F",  nullptr, ANIM_DIRECTION::F},
        {"_B",  nullptr, ANIM_DIRECTION::B},
        {"_L",  nullptr, ANIM_DIRECTION::L},
        {"_R",  nullptr, ANIM_DIRECTION::R},
        {"_U",  nullptr, ANIM_DIRECTION::U},
        {"_D",  nullptr, ANIM_DIRECTION::D},

        {"_LF", "_FL", ANIM_DIRECTION::LF},
        {"_LB", "_BL", ANIM_DIRECTION::LB},
        {"_RF", "_FR", ANIM_DIRECTION::RF},
        {"_RB", "_BR", ANIM_DIRECTION::RB},

       // {"_F_RF", "_F_FR", ANIM_DIRECTION::F_RF},
       // {"_F_LF", "_F_FL", ANIM_DIRECTION::F_LF},
       // {"_B_RB", "_B_BR", ANIM_DIRECTION::B_RB},
       // {"_B_LB", "_B_BL", ANIM_DIRECTION::B_LB},

        {"_U_F", nullptr, ANIM_DIRECTION::U_F},
        {"_U_B", nullptr, ANIM_DIRECTION::U_B},
        {"_U_L", nullptr, ANIM_DIRECTION::U_L},
        {"_U_R", nullptr, ANIM_DIRECTION::U_R},

        {"_D_F", nullptr, ANIM_DIRECTION::D_F},
        {"_D_B", nullptr, ANIM_DIRECTION::D_B},
        {"_D_L", nullptr, ANIM_DIRECTION::D_L},
        {"_D_R", nullptr, ANIM_DIRECTION::D_R},

        {"_BBL", nullptr, ANIM_DIRECTION::BBL},
        {"_BLL", nullptr, ANIM_DIRECTION::BLL},
    };

    for (size_t i = 0; i < size(table); ++i)
    {
        if (name.find(table[i].key1) != std::string::npos)
            return static_cast<unsigned int>(table[i].dir);
        if (table[i].key2 && name.find(table[i].key2) != std::string::npos)
            return static_cast<unsigned int>(table[i].dir);
    }

    return 0;
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

    Safe_Release(m_pGameInstance);

}
