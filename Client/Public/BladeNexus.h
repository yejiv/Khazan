#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CBladeNexus final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		AFTER_END,
		AFTER_IDLE,
		AFTER_LOOP,
		AFTER_START,
		BEFORE_END,
		BEFORE_IDLE,
		BEFORE_LOOP,
		BEFORE_START,
		END
	};

public:
	typedef struct tagBladeNexusDesc : CProp_Interactive::PROP_INTERACTIVE_DESC
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

	_uint iiii = {};

private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CBladeNexus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END