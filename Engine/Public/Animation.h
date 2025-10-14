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
	void Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool isLoop, _bool* pFinished, _float fTimeDelta);

	/*  Info  */
public:
	void		Set_TrackPositionPtr(_float* pTrackPosition);


private:
	class CGameInstance*		m_pGameInstance = { nullptr };

	_float						m_fDuration = {};				/* 애니메이션의 전체 재생 길이 */
	_float						m_fTickPerSecond = {};			/* 초당 이동해야할 거리 : 재생속도 */
	_float*						m_fCurrentTrackPosition = {};   /* 애니메이션 재생중에서의 현재 위치  */


	/* CChannel == 뼈(시간에 따른 뼈의 상태행렬) */
	_uint						m_iNumChannels = {};
	vector<class CChannel*>		m_Channels;	
	vector<_uint>				m_CurrentKeyFrameIndices;


public:
	static	CAnimation* Create(const vector<class CBone*>& Bones, ANIMATION_DATA& data, _uint iCurAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END
