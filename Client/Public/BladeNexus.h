#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CBladeNexus final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		AFTER_END, AFTER_IDLE, AFTER_LOOP, AFTER_START,
		BEFORE_END, BEFORE_IDLE, BEFORE_LOOP, BEFORE_START,
		END
	};

public:
	typedef struct tagBladeNexusDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{


	}BLADENEXUS_DESC;

private:
	CBladeNexus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBladeNexus(const CBladeNexus& Prototype);
	virtual ~CBladeNexus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	ANIM_STATE m_eAnimState = { ANIM_STATE::BEFORE_IDLE };

private:
	_float3 m_vPlayerPosition = {};
	_bool m_isBNOn = { false };
	_bool m_isBNOff = { false };

private:
	virtual HRESULT Ready_Components(void* pArg) override;

	void Animation_Update(_float fTimeDelta);
	void Animation_Change(_float fTimeDelta);

public:
	static CBladeNexus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END