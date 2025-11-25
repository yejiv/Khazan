#pragma once

#include "Client_Defines.h"
#include "Trigger.h"

NS_BEGIN(Engine)
class CBody;
class CTransform;
NS_END

NS_BEGIN(Client)

class CViper_Trigger final : public CTrigger
{
public:
	typedef struct tagViperDesc : public CTrigger::TRIGGER_DESC
	{

	}TRIGGER_VIPER_DESC;

private:
	enum class DAY_CIRCLE { DAWN, DAY, NONE };

private:
	CViper_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CViper_Trigger(const CViper_Trigger& Prototype);
	virtual ~CViper_Trigger() = default;

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
	HRESULT Ready_TriggerType(void* pArg);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
	//class CSequence_Viper_Field* m_pViper_Field = { nullptr };
	//class CSequence_Viper_Boss* m_pViper_Boss = { nullptr };
	class CClientInstance* m_pClientInstance = { nullptr };

	DAY_CIRCLE m_eDayCircle = { DAY_CIRCLE::NONE };

public:
	static CViper_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END