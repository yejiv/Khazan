#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CBigChest final : public CProp_Interactive
{
private:
	enum ANIM_STATE
	{
		CLOSE,
		CLOSING,
		OPEN,
		OPENING,
		END
	};

public:
	typedef struct tagBigChestDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
	{


	}BIGCHEST_DESC;

private:
	CBigChest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBigChest(const CBigChest& Prototype);
	virtual ~CBigChest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	ANIM_STATE m_eAnimState = { ANIM_STATE::CLOSE };

private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CBigChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END