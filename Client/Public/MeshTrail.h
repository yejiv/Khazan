#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_QuadTrail.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_QuadTrail;
class CShader;
NS_END

NS_BEGIN(Client)

class CMeshTrail : public CGameObject
{
public:
	typedef struct tagTrailDesc
	{
		_float	fLifeTime;
		_uint	iTextureIdx;
		_uint	iDivisionCount;
        _float4 vColor;
        _float4 vSubColor;
	}TRAIL_DESC;

private:
	CMeshTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMeshTrail(const CMeshTrail& Prototype);
	virtual ~CMeshTrail() = default;

public:
	virtual HRESULT			    Initialize_Prototype() override;
	virtual HRESULT			    Initialize_Clone(void* pArg) override;
	virtual void			    Priority_Update(_float fTimeDelta) override;
	virtual void			    Update(_float fTimeDelta) override;
	virtual void			    Late_Update(_float fTimeDelta) override;
	virtual HRESULT			    Render() override;

	void					    Add_ControlPoint(_fvector top, _gvector bottom);
    const TRAIL_CONFIG&         Get_TrailConfig() const;
    void                        Set_TrailConfig(const TRAIL_CONFIG& Config);
    
    _uint                       Get_NumTrailTextures();
    ID3D11ShaderResourceView*   Get_TrailTexture(_uint iIndex);

private:
	HRESULT					    Ready_Component();
	HRESULT					    Bind_ShaderResources();

private:
	CTexture*				    m_pTextureCom = { nullptr };
	CVIBuffer_QuadTrail*	    m_pVIBufferCom = { nullptr };
	CShader*				    m_pShaderCom = { nullptr };

private :
    _float					    m_fLifeTime = {};
    _uint					    m_iTextureIdx = {};
    _int					    m_iDivisionCount = {};
    _float4                     m_vColor = {};
    _float4                     m_vSubColor = {};

private :
	deque<CVIBuffer_QuadTrail::QUAD_TRAIL_POINT>		m_ControlPoints;
	deque<CVIBuffer_QuadTrail::QUAD_TRAIL_POINT>		m_TrailPoints;

public:
	static CMeshTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;

};

NS_END




