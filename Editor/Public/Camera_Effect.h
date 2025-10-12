#pragma once
#include "Editor_Defines.h"
#include "Camera.h"

NS_BEGIN(Editor)

class CCamera_Effect final : public CCamera
{
public:
	typedef struct tagCameraEffectDesc : public CCamera::CAMERA_DESC
	{
		_float			fMouseSensor;
	}CAMERA_EFFECT_DESC;

private:
	CCamera_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Effect(const CCamera_Effect& Prototype);
	virtual ~CCamera_Effect() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_float					m_fMouseSensor = { };

public:
	static CCamera_Effect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg)override;
	virtual void			Free() override;
};

NS_END