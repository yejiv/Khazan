#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CTombStone final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		AFTER_END,
		AFTER_IDLE,
		AFTER_LOOP,
		AFTER_START,
		BEFORE_IDLE,
		BEFORE_START,
		END
	};

public:
	typedef struct tagTombStoneDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{


	}TOMBSTONE_DESC;

private:
	CTombStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTombStone(const CTombStone& Prototype);
	virtual ~CTombStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	ANIM_STATE m_eAnimState = { ANIM_STATE::BEFORE_IDLE };

private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CTombStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END