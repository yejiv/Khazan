#pragma once
#include "Client_Defines.h"
#include "LineTrail.h"
#include "VIBuffer_LineTrail.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_LineTrail;
class CShader;
NS_END

NS_BEGIN(Client)

class CScreenTrail : public CLineTrail
{
private:
	CScreenTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CScreenTrail(const CScreenTrail& Prototype);
	virtual ~CScreenTrail() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	void					Add_ControlPoint(POINT pos);

private:
	virtual HRESULT			Ready_Component(void* pArg) override;
	HRESULT					Bind_ShaderResources();

private :
	_float2					m_fViewportSize;
	_float					m_ZValue { 1.f };

public:
	static CScreenTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




