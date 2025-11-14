#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	enum CS_PASS { MOVE, GRAVITY, UPDATE_SPEED, RESET, RESET_SPEED, TURBULENCE, END };
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
		_char	pNoiseFilePath[MAX_PATH];
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	void						Reset();

public:
	virtual HRESULT				Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual HRESULT				Bind_Resources() override;
	virtual HRESULT				Render() override;

public:
	_bool						Update(_float fTimeDelta);
	void						UpdateGravity(_float fTimeDelta);
	void						UpdateTurbulence(_float fTimeDelta, _float fAccTime);
	void						Setting_Speed(SPEED_VALUE type, _float2 range);
	void						Remove_Speed(SPEED_VALUE type);
	void						Remove_Speed();
	void						Setting_Pivot(_float3 pivot);
	void						Setting_Loop(_bool isLoop) { m_bLoop = isLoop; };
    bool                        isLoop() { return m_bLoop; }
     
private:
	HRESULT						Ready_SRV(void* pSysmem);
	HRESULT						Ready_UAV();
	HRESULT						Ready_CB();
	HRESULT						Ready_ComputeShader();

private :
	_bool						IsFinish();

private :
	class CComputeShader*				m_ComputeShaders[ENUM_CLASS(CS_PASS::END)] = {};//공유 가능? -> 확인
	ID3D11ShaderResourceView*			m_pSRV = { nullptr };		//공유 가능? -> 확인
	ID3D11ShaderResourceView*			m_pSRVNoise = { nullptr };	//공유 가능
	ID3D11UnorderedAccessView*			m_pUAV = { nullptr };
	ID3D11UnorderedAccessView*			m_pUAVSpeed = { nullptr };	
	//아래 버퍼들은 많아도 상관 없음!
	ID3D11Buffer*						m_pCB = { nullptr };
	ID3D11Buffer*						m_pStructuredBuffer = { nullptr };	//uav -> copy용
	ID3D11Buffer*						m_pSpeedBuffer = { nullptr };		//uavspeed -> Cpu write용
	ID3D11Buffer*						m_pStagingBuffer = { nullptr };		//CB -> Cpu Read용
	POINT_INSTANCE_PARAMS*				m_pParticleParams;

	ID3D11Buffer*						m_pDebugStagingBuffer = { nullptr }; //debug

private:
	_float3								m_vPivot = {}; 
	POINT_INSTANCE_DESC					m_sData;
    _bool                               m_bLoop;

public:
	static CVIBuffer_Point_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END




