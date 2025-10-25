#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instance
{
public:
	enum CS_PASS { MOVE, GRAVITY, UPDATE_SPEED, RESET, RESET_SPEED, END };
	enum class SPEED_VALUE { SPREAD_SPEED, ROTATION_SPEED, UPWARD_SPEED, SCALE_SPEED, SPEED_END };

	typedef struct tagPointInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed{ 0.f, 0.f };
		_float2 vLifeTime;
		_float	fOffset;
		_uint   IsCircle;
		_float	fSizeRatio;
		_char	pFilePath[MAX_PATH];
	}POINT_MESH_DESC;

private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	void						Reset(); 

public:
	virtual HRESULT				Initialize_Prototype(INSTANCE_DESC* pArg);
	virtual HRESULT				Initialize_Clone(void* pArg); 

public:
	_bool						Update(_float fTimeDelta);
	void						UpdateGravity(_float fTimeDelta);
	void						Setting_Speed(SPEED_VALUE type, _float2 range);
	void						Remove_Speed(SPEED_VALUE type);
	void						Remove_Speed();
	void						Setting_Pivot(_float3 pivot);
	void						Setting_Loop(_bool isLoop) { m_IsLoop = isLoop; };


private:
	HRESULT						Ready_SRV(void* pSysmem);
	HRESULT						Ready_UAV();
	HRESULT						Ready_CB();
	HRESULT						Ready_ComputeShader();

private:
	_bool						IsFinish();

private:
	class CComputeShader*		m_ComputeShaders[ENUM_CLASS(CS_PASS::END)] = {};
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAVSpeed = { nullptr };
	ID3D11Buffer*				m_pCB = { nullptr };
	ID3D11Buffer*				m_pStructuredBuffer = { nullptr };
	ID3D11Buffer*				m_pSpeedBuffer = { nullptr };
	ID3D11Buffer*				m_pStagingBuffer = { nullptr };
	POINT_INSTANCE_PARAMS*		m_pParticleParams; 

private:
	_float3*					m_pVertexPositions = { nullptr };

private :
	_float3						m_vPivot = {};
	_float3						m_vRange = {};
	_bool						m_IsLoop = {}; 

	_float						m_fOffset = {}; 
	_bool						m_bIsCircle = {};

	_bool						m_bIsFollow;

public:
	static CVIBuffer_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, INSTANCE_DESC* pArg);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END



