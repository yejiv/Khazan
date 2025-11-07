#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Get_GameObject(_uint iIndex);
	class CGameObject* Get_BackGameObject() { return m_GameObjects.back(); }
	TIME_CHANNEL		Get_TimeChannel() { return m_eTimeChannel; }

public:
	void				Set_TimeChannel(TIME_CHANNEL eChannel) { m_eTimeChannel = eChannel; }

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject) {
		m_GameObjects.push_back(pGameObject);
		return S_OK;
	}

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);


	void DeadObject_Clear();

private:
	list<class CGameObject*>			m_GameObjects;
	list<class CGameObject*>			m_DeadGameObjects;

	class CGameInstance*				m_pGameInstance = { nullptr };

	TIME_CHANNEL						m_eTimeChannel = { TIME_CHANNEL::WORLD };

public:
	static CLayer* Create();
	virtual void Free() override;
};

NS_END