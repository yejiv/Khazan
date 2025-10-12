#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Editor)

class CEditor_Channel final : public CBase
{
private:
    CEditor_Channel();
    virtual ~CEditor_Channel() = default;

public:
    HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<class CEditor_Bone*>& Bones);
    void Update_TransformationMatrix(const vector<class CEditor_Bone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

public:
    void    Get_Data(CHANNEL_DATA& data) { data = m_Channel_Data; }
private:
    _char                   m_szName[MAX_PATH] = {};
    _uint                   m_iBoneIndex = {};
    _uint                   m_iNumKeyFrames = {};
    vector<KEYFRAME>        m_KeyFrames;

    CHANNEL_DATA            m_Channel_Data{};
public:
    static CEditor_Channel* Create(const aiNodeAnim* pAIChannel, const vector<class CEditor_Bone*>& Bones);
    virtual void Free() override;
};

NS_END