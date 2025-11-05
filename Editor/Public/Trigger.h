#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CTrigger final : public CProp_Interactive
{
public:
	typedef struct tagTriggerDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{
		_tchar szTriggerKey[MAX_PATH]{};

	}TRIGGER_DESC;

private:
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

private:
	_float4 m_vWireColor = { 0.f, 1.f, 0.f, 1.f };

	string m_strTriggerKey = { "MUST FIX" };

private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END