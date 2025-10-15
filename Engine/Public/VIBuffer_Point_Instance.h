#pragma once

#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
private:
	enum CS_PASS { MOVE, END };

public:
	typedef struct tagPointInstanceDesc : public CVIBuffer_Instance::INSTANCE_DESC
	{
		_float3			vDirection = { 0.f, 1.f, 0.f };
		_float3			vVectorScale = { 1.f, 1.f, 1.f };
		_float2			vSpeed = { 1.f, 1.f };
		_float2			vLifeTime = { 1.f, 1.f };
		_bool			isLoop;
		_bool			isRandomVector;
		_float3			vMinAngle;
		_float3			vMaxAngle = { 360.f, 360.f, 360.f };

		// 필요 시 사용(동적인 Pivot을 따라가는 Particle)
		// Pivot 쓸 거면 플래그 같이 CB에 넘겨줘서 현재 위치에서부터 Pivot 빼서 Direction 대신하기
		_bool			isUsePivot;
		_float3			vPivot;
	}POINT_INSTANCE_DESC;

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
	void Move(_float fTimeDelta);

private:
	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};
	_bool					m_isLoop = {};

	// Compute Shader
	class CComputeShader*		m_ComputeShaders[ENUM_CLASS(CS_PASS::END)] = {};

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