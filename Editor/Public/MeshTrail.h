#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_QuadTrail;
class CShader;
NS_END

NS_BEGIN(Editor)

class CMeshTrail : public CGameObject
{
public:
	typedef struct tagTrailDesc
	{
		_float	fLifeTime;
		_uint	iTextureIdx;
		_uint	iDivisionCount;
	}TRAIL_DESC;

private:
	CMeshTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMeshTrail(const CMeshTrail& Prototype);
	virtual ~CMeshTrail() = default;

public:
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	void					Add_ControlPoint(_float4 top, _float4 bottom);

private:
	HRESULT					Ready_Component();
	HRESULT					Bind_ShaderResources();

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_QuadTrail*	m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

private :
	_float					m_fCurTime;
	_float					m_fLifeTime;

	_uint					m_iTextureIdx;
	_int					m_iDivisionCount;
	_bool					m_bOn {false};

private :
	deque<TRAIL_POINT>		m_ControlPoints;
	deque<TRAIL_POINT>		m_TrailPoints;

public:
	static CMeshTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




