#pragma once

#include "Base.h"

/* 시간에 따른 뼈의 상태행렬들을 보관한다. */

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CHANNEL_DATA& data);

public:
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);


	/*  Info  */
public:
	/* 현재 뼈의 상태 행렬을 리턴한다. */
	_matrix Get_TransformationMatrix() const {return m_TransformationMatrix;}

	/* 이 Channel에 해당하는 뼈 인덱스를 리턴한다. */
	_uint Get_BoneIndex() const {return m_iBoneIndex;}


private:
	_uint							m_iBoneIndex = {};    /* 이 Channel에 해당하는 뼈 인덱스 저장  */
	_uint							m_iNumKeyFrames = {};    /* 최대 프레임 수 저장 */
	vector<KEYFRAME>				m_KeyFrames;    /* 시간에 따른 상태 뼈의 상태들을 보관 */

	_matrix         m_TransformationMatrix = {};	/* 현재 뼈 행렬*/

public:
	static CChannel* Create(CHANNEL_DATA& data);
	virtual void Free() override;
};

NS_END

