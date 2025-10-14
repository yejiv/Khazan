#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagPointInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vPivot;
		_float2			vSpeed;		
		_float2			vLifeTime;
		_bool			isLoop;
	}POINT_INSTANCE_DESC;

	typedef struct tagPointInstanceConstantBuffer
	{
		_float		fTimeDelta;
		_float3		vPivot;
		_uint		iNumInstances;
		_float3		vPadding;
	}POINT_INSTANCE_CB;

	typedef struct tagParticleParams
	{
		_float fSpeed;
		_float3 vPadding;
		_float4 vInitTranslation;
	}PARTICLE_PARAMS;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);

private:
	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};
	_bool					m_isLoop = {};

	// Compute Shader Test
	class CComputeShader*		m_pComputeShader = { nullptr };

	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAV = { nullptr };
	ID3D11Buffer*				m_pCB = { nullptr };
	ID3D11Buffer*				m_pStructuredBuffer = { nullptr };

private:
	HRESULT Ready_ShaderResourceView(void* pSysmem);
	HRESULT Ready_UnorderedAccessView();
	HRESULT Ready_ConstantBuffer();
	HRESULT Ready_ComputeShader();

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END