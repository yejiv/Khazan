#include "Editor_Animation.h"
#include "Editor_Channel.h"
#include "GameInstance.h"

CEditor_Animation::CEditor_Animation()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    m_Channels.clear();
    m_CurrentKeyFrameIndices.clear();
}

CEditor_Animation::CEditor_Animation(const CEditor_Animation& Prototype)
    : m_pGameInstance{ CGameInstance::GetInstance() }
   , m_strName                       {Prototype.m_strName                   }
   , m_fDuration                     {Prototype.m_fDuration                 }
   , m_fTickPerSecond                {Prototype.m_fTickPerSecond            }
   , m_fCurrentTrackPosition         {Prototype.m_fCurrentTrackPosition     }
   , m_iNumChannels                  {Prototype.m_iNumChannels              }
   , m_Channels                      {Prototype.m_Channels            }
   , m_CurrentKeyFrameIndices        {Prototype. m_CurrentKeyFrameIndices   }
   , m_Animation_Data                {Prototype.m_Animation_Data            }
{
    Safe_AddRef(m_pGameInstance);
    for (auto channel : m_Channels)
        Safe_AddRef(channel);

}

HRESULT CEditor_Animation::Initialize(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones)
{
    m_strName = string(pAIAnimation->mName.C_Str());
    if (m_strName.find("|Action") != string::npos)
    {
        m_isSkip = true;
        return S_OK;
    }

    size_t pos = m_strName.rfind('|');
    if (pos != std::string::npos)
        m_strName.erase(0, pos + 1);
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

    if (m_isEnbleTrackPosition)
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

void CEditor_Animation::Set_RootBoneIndex(_uint iRootBoneIndex)
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
    for (_uint i = 0; i < m_iNumChannels; ++i) {
        _matrix a = m_Channels[i]->Get_TransformationMatrix();
        m_PreAnimationChannelMatrices.emplace(m_Channels[i]->Get_BoneIndex(), m_Channels[i]->Get_TransformationMatrix());
    }
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

    if (m_fCurBlendTime > m_Animation_Data.fAnimationBlendTime)
        m_isAnimationBlend = false;

    _float fRatio = m_fCurBlendTime / m_Animation_Data.fAnimationBlendTime;

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
    //string name = animName;
    //transform(name.begin(), name.end(), name.begin(), ::toupper); 

    if (animName.find("_F_LF") != string::npos)
        return ENUM_CLASS(DIRECTION::F) + ENUM_CLASS(DIRECTION::L);

    if (animName.find("_F_RF") != string::npos)
        return ENUM_CLASS(DIRECTION::F) + ENUM_CLASS(DIRECTION::R);

    if (animName.find("_ALL") != string::npos)
        return ENUM_CLASS(DIRECTION::ALL);

    if (animName.find("_BLL") != string::npos)
        return ENUM_CLASS(DIRECTION::BLL);

    if (animName.find("_BBL") != string::npos)
        return ENUM_CLASS(DIRECTION::BBL);

    if (animName.find("_CC") != string::npos)
        return ENUM_CLASS(DIRECTION::CC);

    struct DirectionMatch {
        const char* key1;
        const char* key2;
        DIRECTION dir1;
        DIRECTION dir2;
    };

    static const DirectionMatch table[] = {
        {"_F",  nullptr, DIRECTION::F, DIRECTION::NONE },
        {"_B",  nullptr, DIRECTION::B, DIRECTION::NONE },
        {"_L",  nullptr, DIRECTION::L, DIRECTION::NONE },
        {"_R",  nullptr, DIRECTION::R, DIRECTION::NONE },
        {"_U",  nullptr, DIRECTION::U, DIRECTION::NONE },
        {"_D",  nullptr, DIRECTION::D, DIRECTION::NONE },

        {"_LF", "_FL", DIRECTION::L, DIRECTION::F},
        {"_LB", "_BL", DIRECTION::L, DIRECTION::B},
        {"_RF", "_FR", DIRECTION::R, DIRECTION::F},
        {"_RB", "_BR", DIRECTION::R, DIRECTION::B},

        {"_UL", "_LU", DIRECTION::U, DIRECTION::L},
        {"_UR", "_RU", DIRECTION::U, DIRECTION::R},
        {"_DL", "_LD", DIRECTION::D, DIRECTION::L},
        {"_DR", "_RD", DIRECTION::D, DIRECTION::R},

        {"_U_F", nullptr, DIRECTION::U, DIRECTION::F},
        {"_U_B", nullptr, DIRECTION::U, DIRECTION::B},
        {"_U_L", nullptr, DIRECTION::U, DIRECTION::L},
        {"_U_R", nullptr, DIRECTION::U, DIRECTION::R},

        {"_D_F", nullptr, DIRECTION::D, DIRECTION::F},
        {"_D_B", nullptr, DIRECTION::D, DIRECTION::B},
        {"_D_L", nullptr, DIRECTION::D, DIRECTION::L},
        {"_D_R", nullptr, DIRECTION::D, DIRECTION::R},

        {"_CD", nullptr, DIRECTION::C, DIRECTION::D},
        {"_CU", nullptr, DIRECTION::C, DIRECTION::U},
        {"_LC", nullptr, DIRECTION::L, DIRECTION::C},
        {"_LD", nullptr, DIRECTION::L, DIRECTION::D},
        {"_LU", nullptr, DIRECTION::L, DIRECTION::U},
        {"_RC", nullptr, DIRECTION::R, DIRECTION::C},
        {"_RD", nullptr, DIRECTION::R, DIRECTION::D},
        {"_RU", nullptr, DIRECTION::R, DIRECTION::U},

    };

    auto IsValidDirectionToken = [&](const string& key) -> bool
        {
            size_t pos = animName.find(key);
            while (pos != string::npos)
            {
                size_t endPos = pos + key.length();

                if (endPos >= animName.size() || !isalpha(animName[endPos]))
                    return true;

                pos = animName.find(key, pos + 1);
            }

            return false;
        };

    for (size_t i = 0; i < size(table); ++i)
    {
        if (IsValidDirectionToken(table[i].key1))
            return ENUM_CLASS(table[i].dir1) + ENUM_CLASS(table[i].dir2);
        if (table[i].key2 != nullptr && IsValidDirectionToken(table[i].key2))
            return ENUM_CLASS(table[i].dir1) + ENUM_CLASS(table[i].dir2);
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
