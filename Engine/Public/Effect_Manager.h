#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CEffect_Manager final : public CBase
{
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT		Initialize(_uint iNumLevels);
	void		Priority_Update(_float fEffectTimeDelta);
	void		Update(_float fEffectTimeDelta);
	void		Late_Update(_float fEffectTimeDelta);
	void		Clear(_uint iLevelIndex);

	// 1. Prototype 태그, Level, 만들어줄 개수
	void		Add_Effect_ToPool(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint iPoolSize);

	// 2. GameInstance�� ȣ���� Spwan �Լ�
	_uint		Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector SpwanPos);
	_uint		Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector Quaternion, _gvector Position);

	// 3. Running중인 객체들에 대한 position 업데이트 -> Effect Layers 컨테이너에 접근해서 적용해줄거임
	void		Update_Effect_Position(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector SpwanPos);
	void		Update_Effect_World(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector Quaternion, _gvector Position);

	// 4. Loop인 이펙트를 멈추고(Loop를 끈다?) Pool로 돌아가게한다. 주로 맵 이펙트이 자주 쓰일 것 같다. 마찬가지로 Effect Layers 컨테이너에 ID로 접근
	void		Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID);

private:
	class CGameInstance*								m_pGameInstance = { nullptr };
	_uint												m_iNumLevels = {};
	_uint												m_iCurLevel;
	unordered_map<_wstring, vector<class CPrefab*>>*	m_pEffectLayers = {};	//본체 - 인덱스로 바로 접근해야함
	unordered_map<_wstring, deque<class CPrefab*>>*		m_pEffectPools = {};	//위,끝만 삽입 삭제가 일어나서 deque
	unordered_map<_wstring, list<class CPrefab*>>*		m_pRunningEffects = {};	//중간 삽입도 일어나서 list

private:
	vector<class CPrefab*>* Find_Effect_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	deque<class CPrefab*>* Find_Effect_Pool(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	list<class CPrefab*>* Find_RunningEffect_Layer(const _wstring& strLayerTag);

public:
	static CEffect_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

NS_END