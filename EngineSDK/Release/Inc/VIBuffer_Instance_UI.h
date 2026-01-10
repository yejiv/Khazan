#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance_UI final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Instance_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Instance_UI(const CVIBuffer_Instance_UI& Prototype);
	virtual ~CVIBuffer_Instance_UI() = default;

public:
	virtual HRESULT						Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual HRESULT						Bind_Resources() override;
	virtual HRESULT						Render() override;

public:
	HRESULT								Add_UIInstance(const VTXINSTANCE_UI* pUIData);
	HRESULT								Update_Buffer();
    HRESULT								Reset();
private:
	VTXINSTANCE_UI*						m_pInstanceVertices = { nullptr };
	_uint								m_iRenderCount = {};

public:
	static CVIBuffer_Instance_UI*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END




