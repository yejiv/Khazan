#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CTrigger : public CProp_Interactive
{
public:
	typedef struct tagTriggerDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{
		string strTriggerKey = {};

	}TRIGGER_DESC;

protected:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger(const CTrigger& Prototype);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	string Get_TriggerKey() { return m_strTriggerKey; }
	void Set_TriggerKey(const string strTriggerKey) { m_strTriggerKey = strTriggerKey; }

protected:
	CBody* m_pTriggerCom = { nullptr };

	string m_strTriggerKey = {};

	_bool m_isRender = { false };

protected:
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Ready_Collision(void* pArg);

protected:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) {};
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) {};
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) {};

protected:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END