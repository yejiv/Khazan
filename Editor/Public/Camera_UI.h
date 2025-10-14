#pragma once
#include"Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Editor)

class CCamera_UI : public CCamera
{
public:
	typedef struct tagCameraFreeDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	}CAMERA_FREE_DESC;

private:
	CCamera_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_UI(const CCamera_UI& Prototype);
	virtual ~CCamera_UI() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;


private:
	_float						m_fMouseSensor = {};

public:
	static CCamera_UI*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

NS_END