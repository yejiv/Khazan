#pragma once
#include "Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Editor)

class CEditor_TempCamera final : public CCamera
{
public:
	typedef struct tagCameraDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	}TEMPCAMERA_DESC;

protected:
	CEditor_TempCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor_TempCamera(const CEditor_TempCamera& Prototype);
	virtual ~CEditor_TempCamera() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float			m_fMouseSensor = { };
	_float			m_fCameraSpeed = { 2.f };

public:
	static CEditor_TempCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END