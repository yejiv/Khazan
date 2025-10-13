#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Editor)

class CEditor_Animation final : public CBase
{
private:
	CEditor_Animation();
	virtual ~CEditor_Animation() = default;

public:
	HRESULT	 Initialize(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones);
	void	Update_TransformationMatrices(const vector<class CEditor_Bone*>& Bones, _bool isLoop, _bool* pFinished,  _float fTimeDelta);

public:
	void	Get_Data(ANIMATION_DATA& data) { data = m_Animation_Data; }
private:
	string								m_strName = {};
	_float								m_fDuration = {};				
	_float								m_fTickPerSecond = {};			
	_float								m_fCurrentTrackPosition = {};	
	_uint								m_iNumChannels = {};			
	vector< class CEditor_Channel* >	m_Channels;
	vector<_uint>						m_CurrentKeyFrameIndices;

	ANIMATION_DATA						m_Animation_Data{};

public:
	static	CEditor_Animation* Create(const aiAnimation* pAIAnimation, const vector<class CEditor_Bone*>& Bones);
	CEditor_Animation* Clone();
	virtual void Free();
};

NS_END