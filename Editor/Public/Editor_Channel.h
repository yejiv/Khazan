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
    void Set_PrevAnimationBlend(const _float& fAnimationRatio, _matrix& PreAnimationMatrix);

public:
    /* 현재 뼈의 상태 행렬을 리턴한다. */
    _matrix Get_TransformationMatrix() const {  return m_TransformationMatrix; }
    /* 이 Channel에 해당하는 뼈 인덱스를 리턴한다. */
    _uint Get_BoneIndex() const { return m_iBoneIndex; }

public:
    void    Get_Data(CHANNEL_DATA& data) { data = m_Channel_Data; }
    void    Set_IsRootBone(_bool isRoot) { m_isRootBone = isRoot; }

private:
    _char                   m_szName[MAX_PATH] = {};
    _uint                   m_iBoneIndex = {};
    _uint                   m_iNumKeyFrames = {};
    vector<KEYFRAME>        m_KeyFrames;

    CHANNEL_DATA            m_Channel_Data{};

    /* 현재 뼈 행렬*/
    _matrix         m_TransformationMatrix = {};

    /* 이전 애니메이션 상태 */
    _bool           m_isBlendPreAnimation = {}; // 이전 애니메이션과의 보간 여부    
    _float          m_fAnimationRatio = {}; // 애니메이션 이전의 상태 행렬과 현재 애니메이션 상태 행렬 사이의 비율   
    _vector         m_vPrevScale{}, m_vPrevRotQuat{}, m_vPrevPositon{}; // 애니메이션 이전의 상태 행렬   

    /* 루트모션 + 애니메이션 블랜딩 */
    _bool           m_isRootBone = false; // 이 채널이 루트본인지 여부
public:
    static CEditor_Channel* Create(const aiNodeAnim* pAIChannel, const vector<class CEditor_Bone*>& Bones);
    virtual void Free() override;
};

NS_END
