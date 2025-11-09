#pragma once

#include "Client_Defines.h"
#include "Trigger.h"

NS_BEGIN(Engine)
class CBody;
class CTransform;
NS_END

NS_BEGIN(Client)

class CHeinMach_Trigger final : public CTrigger
{
public:
	typedef struct tagHeinMachDesc : public CTrigger::TRIGGER_DESC
	{
		
	}TRIGGER_HEINMACH_DESC;

private:
	enum class DAY_CIRCLE { DAWN, DAY, NONE };

private:
	CHeinMach_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeinMach_Trigger(const CHeinMach_Trigger& Prototype);
	virtual ~CHeinMach_Trigger() = default;

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
	virtual HRESULT Ready_Components(void* pArg) override;
	HRESULT Ready_Collision(void* pArg);
	HRESULT Ready_TriggerType(void* pArg);      // 트리거 타입 세팅
    HRESULT Ready_TriggerSetting(void* pArg);

private:
	virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

private:
	class CSequence_HeinMach_Field* m_pHeinMach_Field = { nullptr };
	class CSequence_HeinMach_Yetuga* m_pHeinMach_Yetuga = { nullptr };
	class CClientInstance* m_pClientInstance = { nullptr };

private:
	GUIDE_TYPE m_eGuideType = { GUIDE_TYPE::END };

	DAY_CIRCLE m_eDayCircle = { DAY_CIRCLE::NONE };

private:
    SKY_DESC m_Sky_Dawn = {};
    SKY_DESC m_Sky_Day = {};

public:
	static CHeinMach_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END