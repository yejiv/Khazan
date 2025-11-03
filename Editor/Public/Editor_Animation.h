#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Editor)

class CEditor_Animation final : public CBase
{
private:
	CEditor_Animation();
	CEditor_Animation(const CEditor_Animation& Prototype);
	virtual ~CEditor_Animation() = default;

public:
	HRESULT	 Initialize(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones);
	void	Update_TransformationMatrices(const vector<class CEditor_Bone*>& Bones, _bool isLoop, _bool* pFinished,  _float fTimeDelta);

public:
	void	Get_Data(ANIMATION_DATA& data) { data = m_Animation_Data; }
	string	Get_Name() const { return m_strName; }

public:
	void					Set_RootBoneIndex(_uint iRootBoneIndex);
	void					OnAnimationBlend(map<_uint, _matrix>&& outChannelMatrices);
	map<_uint, _matrix>&	Get_ChannelMatrices();
	void					Set_TrackPositionPtr(_float* pTrackPosition);
	_bool					IsBlending() const { return m_isAnimationBlend; }

	void					EnbleTrackPosition(_bool isEnble) { m_isEnbleTrackPosition = isEnble; }//tool
	const _float&			Get_Duration() const { return m_fDuration; } //tool

	_bool					Get_Skip() { return m_isSkip; }

private:
	string								m_strName = {};
	_float								m_fDuration = {};				
	_float								m_fTickPerSecond = {};			
	_float*								m_fCurrentTrackPosition = {nullptr};	
	_uint								m_iNumChannels = {};			
	vector< class CEditor_Channel* >	m_Channels;
	vector<_uint>						m_CurrentKeyFrameIndices;

	ANIMATION_DATA						m_Animation_Data{};

	/* 애니메이션 블랜딩 */
	_bool							m_isAnimationBlend = { false }; // 이전 애니메이션과의 보간 여부
	_float							m_fCurBlendTime = {};
	_float							m_fBlendTime = { 0.3f }; //애니메이션 사이 보간에 사용할 시간
	map<_uint, _matrix>				m_PreAnimationChannelMatrices;	//<뼈 번호, 뼈 행렬>	 // 애니메이션 전환용 마지막 프레임 뼈 행렬 모음. (다시 사용하면 안됨.  move로 넘겨줌)

	/* anim tool  */
	_bool							m_isEnbleTrackPosition = { true };

	_bool							m_isSkip = { false };	/* action 빈 깡통 들어오면 스킵  */

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void		Update_AnimationBlend(const vector<class CEditor_Bone*>& Bones, _float fTimeDelta);	/* 애니메이션 블랜딩  */
	_uint		GetAnimDirection(const string& animName);

public:
	static	CEditor_Animation* Create(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones);
	CEditor_Animation* Clone();
	virtual void Free();
};

NS_END
