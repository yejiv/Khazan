#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"
#include "VIBuffer_LineTrail.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_LineTrail;
class CShader;
NS_END

NS_BEGIN(Editor)

class CLineTrail : public CGameObject
{
	typedef struct tagLineTrailDesc : public CVIBuffer_LineTrail::LINE_BUFFER_DESC
	{
		_float	fLifeTime;
		_uint	iTextureIdx;
		_uint	iDivisionCount;
	}LINE_TRAIL_DESC;

private:
	CLineTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CLineTrail(const CLineTrail& Prototype);
	virtual ~CLineTrail() = default;

public:
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	void					Add_ControlPoint(_fvector pos);

private:
	HRESULT					Ready_Component(void* pArg);
	HRESULT					Bind_ShaderResources();

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_LineTrail*	m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private :
	_float					m_fLifeTime;

	_uint					m_iTextureIdx;
	_int					m_iDivisionCount;

private :
	deque<CVIBuffer_LineTrail::LINE_TRAIL_POINT>	m_ControlPoints;
	deque<_float4>	m_TrailPoints;

public:
	static CLineTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




