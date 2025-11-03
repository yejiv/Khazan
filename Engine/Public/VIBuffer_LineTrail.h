#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_LineTrail final : public CVIBuffer
{
public:
	typedef struct tagLineTrailPointDesc
	{
		XMFLOAT4 vPos;
		float	fLifeTime;
	}LINE_TRAIL_POINT;

	typedef struct tagLineBufferDesc
	{
		_float fOffset;
	}LINE_BUFFER_DESC;

private:
	CVIBuffer_LineTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_LineTrail(const CVIBuffer_LineTrail& Prototype);
	virtual ~CVIBuffer_LineTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	void			Update(deque<_float4>& vertices, const _float4* CamPos);
	void			Update(deque<_float4>& vertices);
	virtual HRESULT Render() override;

private :
	_uint			m_iNumDrawIndices = {};

	_float			m_fOffset;

public:
	static CVIBuffer_LineTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END