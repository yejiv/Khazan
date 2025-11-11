#pragma once
#include "Client_Defines.h"
#include "UIObject.h"
#include "VIBuffer_LineTrail.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_LineTrail;
class CShader;
NS_END

NS_BEGIN(Client)

class CScreenTrail : public CUIObject
{
public:
	typedef struct tagLineTrailDesc : public CVIBuffer_LineTrail::LINE_BUFFER_DESC
	{
		_float	fLifeTime;
		_uint	iTextureIdx;
		_uint	iDivisionCount;
	}LINE_TRAIL_DESC;


private:
	CScreenTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CScreenTrail(const CScreenTrail& Prototype);
	virtual ~CScreenTrail() = default;

public:
    void                    Set_TexIndex(_int iTexIndex) { m_iTextureIdx = iTexIndex; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	void					Add_ControlPoint(POINT pos);

private:
	virtual HRESULT			Ready_Component(void* pArg);
	HRESULT					Bind_ShaderResources();

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_LineTrail*	m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private:
	deque<CVIBuffer_LineTrail::LINE_TRAIL_POINT>	m_ControlPoints;
	deque<_float4>	        m_TrailPoints;

private :
	_float2					m_fViewportSize;
	_float					m_ZValue{ 1.f };

	_float					m_fLifeTime;

	_uint					m_iTextureIdx;
	_int					m_iDivisionCount;

public:
	static CScreenTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




