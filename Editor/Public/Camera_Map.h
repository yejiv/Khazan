#pragma once

#include "Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Editor)

class CCamera_Map final : public CCamera
{
public:
	typedef struct tagCameraMapDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;

	}CAMERA_MAP_DESC;

private:
	CCamera_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Map(const CCamera_Map& Prototype);
	virtual ~CCamera_Map() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_float					m_fMouseSensor = { };

private:
	_bool m_isPreviewPos = { false };
	_bool m_isHwakDae = { false };

	_float3 m_vPrevPos = {};

private:
	void Input(_float fTimeDelta);

public:
	static CCamera_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END