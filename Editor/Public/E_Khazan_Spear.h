#pragma once
#include "ContainerObject.h"

NS_BEGIN(Editor)

using DIR = DIRECTION_INFO;

class CE_Khazan_Spear final:  public CContainerObject
{

private:
	CE_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CE_Khazan_Spear(const CE_Khazan_Spear& Prototype);
	virtual ~CE_Khazan_Spear() = default;

public:
	virtual HRESULT                 Initialize_Prototype();
	virtual HRESULT                 Initialize_Clone(void* pArg);
	virtual void                    Priority_Update(_float fTimeDelta);
	virtual void                    Update(_float fTimeDelta);
	virtual void                    Late_Update(_float fTimeDelta);
	virtual HRESULT                 Render();

private:
	class CE_Body_Khazan_Spear*		m_pBody = { nullptr };

private:
	HRESULT			                Ready_Components();
	HRESULT			                Ready_PartObjects();

public:
	static CE_Khazan_Spear*         Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*            Clone(void* pArg) override;
	virtual void                    Free() override;
};

NS_END
