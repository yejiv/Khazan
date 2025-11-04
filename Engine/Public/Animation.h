#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{

private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT	 Initialize(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation);
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isUsedLoop, _bool isLoop, _bool* pFinished, _float fTimeDelta);
	/* todo : 준영이 CModel_Instance에서 사용하는데 한번 이야기해봐야할 듯 */
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta);


public:
	// 오현형 저 테스트좀 해볼께요..
	_float		MakeRatio();

	/*  Info  */
public:
	void		Set_RootBoneIndex(_uint iRootBoneIndex);
	void		Set_TrackPositionPtr(_float* pTrackPosition);
	void		Set_AnimBlendTime(_float fBlendTime) { m_fBlendTime = fBlendTime; } //클라에서 바꾸고 싶을 때
	_float		Get_TickPerSecond() { return m_fTickPerSecond; }
	_float		Get_Duration() { return m_fDuration; }

	/* Animation Blend*/
public:
	void		OnAnimationBlend(map<_uint, _matrix>&& outChannelMatrices);
	map<_uint, _matrix>& Get_ChannelMatrices();

private:
	class CGameInstance*		m_pGameInstance = { nullptr };

	//ANIMATION_SETUP_DATA		m_SetupData;
	/* Info */
	_float						m_fDuration = {};				/* 애니메이션의 전체 재생 길이 */
	_float						m_fTickPerSecond = {};			/* 초당 이동해야할 거리 : 재생속도 */
	_float*						m_fCurrentTrackPosition = { nullptr };   /* 애니메이션 재생중에서의 현재 위치  */
	_bool						m_isLoop = {};

	/* CChannel == 뼈(시간에 따른 뼈의 상태행렬) */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;	
	vector<_uint>				m_CurrentKeyFrameIndices;

	/* Animation Blend */
	_bool						m_isAnimationBlend = { false }; // 이전 애니메이션과의 보간 여부
	_float						m_fCurBlendTime = {};			//진행 중인 보간 현재 시간
	_float						m_fBlendTime = { 0.25f };		//애니메이션 사이 보간에 사용할 시간
	map<_uint, _matrix>			m_PreAnimationChannelMatrices;	//<뼈 번호, 뼈 행렬>	 // 애니메이션 전환용 마지막 프레임 뼈 행렬 모음. (다시 사용하면 안됨.  move로 넘겨줌)

	_bool						m_isClone = { true };

private:
	/*Animation Blend */
	void		Update_AnimationBlend(const vector<class CBone*>& Bones, _float fTimeDelta);

public:
	static	CAnimation* Create(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END
