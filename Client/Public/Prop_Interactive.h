#pragma once

#include "Client_Defines.h"
#include "Prop.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CProp_Interactive abstract : public CProp
{
public:
	typedef struct tagPropInteractiveDesc : public CProp::PROP_DESC
	{
		_tchar szInteractiveTag[MAX_PATH] = {};

        _int iEventID{ 0 };

		void* pOtherDesc{};

	}PROP_INTERACTIVE_DESC;

protected:
	CProp_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Interactive(const CProp_Interactive& Prototype);
	virtual ~CProp_Interactive() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CModel* m_pModelCom = { nullptr };

	_tchar m_szInteractiveTag[MAX_PATH] = {};

    _int m_iEventID = {};

	_bool m_isCollision = { false };

protected:
	virtual HRESULT Ready_Components(void* pArg) { return S_OK; }
	virtual HRESULT Bind_ShaderResources();

	virtual HRESULT Bind_Materials(_uint iMeshIndex);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

NS_END