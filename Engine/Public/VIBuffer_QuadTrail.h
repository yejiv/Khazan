#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_QuadTrail final : public CVIBuffer
{
public :
	typedef struct tagQuadTrailPointDesc
	{
		XMFLOAT4 vTop;
		XMFLOAT4 vBottom;
		float	fLifeTime;
	}QUAD_TRAIL_POINT;

private:
	CVIBuffer_QuadTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_QuadTrail(const CVIBuffer_QuadTrail& Prototype);
	virtual ~CVIBuffer_QuadTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	void			Update(deque<QUAD_TRAIL_POINT>& vertices);
	virtual HRESULT Render() override;

private :
	_uint			m_iNumDrawIndices = {};

public:
	static CVIBuffer_QuadTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END