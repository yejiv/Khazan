#pragma once

#include "Base.h"

/* 시간에 따른 뼈의 상태행렬들을 보관한다. */

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& Prototype);
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CHANNEL_DATA& data);

public:
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex);

	/* Animation Blend */
	void Set_PrevAnimationBlend(const _float& fAnimationRatio, _matrix& PreAnimationMatrix);
	void Reset_AnimationBlend();

	/*  Info  */
public:
	_matrix Get_TransformationMatrix() const {return m_TransformationMatrix;}	/* 현재 뼈의 상태 행렬을 리턴한다. */
	_uint Get_BoneIndex() const {return m_iBoneIndex;}	/* 이 Channel에 해당하는 뼈 인덱스를 리턴한다. */
	void    Set_IsRootBone(_bool isRoot) { m_isRootBone = isRoot; }

private:
	_uint							m_iBoneIndex = {};    /* 이 Channel에 해당하는 뼈 인덱스 저장  */
	_uint							m_iNumKeyFrames = {};    /* 최대 프레임 수 저장 */
	vector<KEYFRAME>				m_KeyFrames;    /* 시간에 따른 상태 뼈의 상태들을 보관 */

	_matrix							m_TransformationMatrix = {};	/* 현재 뼈 행렬*/

	/*  Animation Blend, 이전 애니메이션 상태 */
	_bool							m_isBlendPreAnimation = {}; // 이전 애니메이션과의 보간 여부    
	_float							m_fAnimationRatio = {}; // 애니메이션 이전의 상태 행렬과 현재 애니메이션 상태 행렬 사이의 비율   
	_vector							m_vPrevScale{}, m_vPrevRotQuat{}, m_vPrevPositon{}; // 애니메이션 이전의 상태 행렬   

	/* 루트모션 + 애니메이션 블랜딩 */
	_bool							m_isRootBone = false; // 이 채널이 루트본인지 여부

public:
	static CChannel* Create(CHANNEL_DATA& data);
	CChannel* Clone();
	virtual void Free() override;
};

NS_END

