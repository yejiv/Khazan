#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CPool_Manager final : public CBase
{
private:
	CPool_Manager();
	virtual ~CPool_Manager() = default;

public:
	HRESULT Add_PoolObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg, _uint iCount = 1);
	class CPool* Pop_PoolObject(const _wstring& strPoolTag);
	HRESULT Reset_PoolObject(class CPool* pPoolObject);
	HRESULT Reset_PoolObject(class CGameObject* pGameObject);

	void Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CPool* pPoolObject);

	void Clear();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	map<const _wstring, deque<class CPool*>>			m_Pools;

private:
	deque<class CPool*>* Find_Pool(const _wstring& strPoolTag);

public:
	static CPool_Manager* Create();
	virtual void Free() override;
};

NS_END