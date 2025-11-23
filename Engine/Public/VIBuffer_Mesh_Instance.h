#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instance
{
public:
	enum CS_PASS { MOVE, GRAVITY, UPDATE_SPEED, RESET, RESET_SPEED, TURBULENCE, RESET_DEAD_FLAG, END };
	enum class SPEED_VALUE { SPREAD_SPEED, ROTATION_SPEED, UPWARD_SPEED, SCALE_SPEED, SPEED_END };

	typedef struct tagPointInstanceDesc : public INSTANCE_DESC
	{
		_float2 vSpeed{ 0.f, 0.f };
		_float2 vLifeTime;
		_float	fOffset;
		_uint   IsCircle;
		_float	fSizeRatio;
		_float	fTurbulenceSpeed;
		_float	fTurbulenceSampleSize;
		_char	pFilePath[MAX_PATH];
		_char	pNoiseFilePath[MAX_PATH];
		_float3	fRotation{ 0.f, 0.f, 0.f };
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
	void						UpdateTurbulence(_float fTimeDelta, _float fAccTime);
	void						Setting_Speed(SPEED_VALUE type, _float2 range);
	void						Remove_Speed(SPEED_VALUE type);
	void						Remove_Speed();
	void						Setting_Pivot(_float3 pivot);
	void						Setting_Loop(_bool isLoop) { m_bLoop = isLoop;  };
    bool                        isLoop() { return m_bLoop; }

	/*Debug*/
	virtual HRESULT				Bind_Resources() override;


private:
	HRESULT						Ready_SRV(void* pSysmem);
	HRESULT						Ready_UAV();
	HRESULT						Ready_CB();
	HRESULT						Ready_ComputeShader();

    HRESULT                     Start_ComputeShader(CS_PASS pass);

private:
	_bool						IsFinish();

private:
	class CComputeShader*		m_ComputeShaders[ENUM_CLASS(CS_PASS::END)] = {};
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRVNoise = { nullptr };	//공유 가능
	ID3D11UnorderedAccessView*	m_pUAV = { nullptr };
	ID3D11UnorderedAccessView*	m_pUAVSpeed = { nullptr };
	ID3D11Buffer*				m_pCB = { nullptr };
	ID3D11Buffer*				m_pStructuredBuffer = { nullptr };
	ID3D11Buffer*				m_pSpeedBuffer = { nullptr };
	ID3D11Buffer*				m_pStagingBuffer = { nullptr };
	//ID3D11Buffer*               m_pDebugInstanceBuffer = { nullptr };
	//Debug
	//ID3D11Buffer*				m_pDebugInstanceBuffer = { nullptr };
	MESH_INSTANCE_PARAMS*		m_pParticleParams;
    ID3D11SamplerState*         m_pLinearWrapSampler;

private:
	//_float3*					m_pVertexPositions = { nullptr };
	POINT_MESH_DESC				m_sData;
    _bool                       m_bLoop;

private :
	_float3						m_vPivot = {};

public:
	static CVIBuffer_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, INSTANCE_DESC* pArg);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END



