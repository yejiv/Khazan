#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	//enum class FX_POINT_TYPE { SPREAD, DROP, LOOP, END };
	enum class SPEED_VALUE { SPREAD_SPEED, ROTATION_SPEED, UPWARD_SPEED, SCALE_SPEED, SPEED_END };

	typedef struct tagPointInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed{ 0.f, 0.f };
		_float2 vLifeTime;
		_float	fOffset;
		_bool   IsCircle;
		_float3 vSourceColor = _float3(1.f, 1.f, 1.f);
		_float	fRotationPerSec;
		_float	fSizeRatio;
		//FX_POINT_TYPE eType;
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	void					Reset();

public:
	virtual HRESULT			Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual HRESULT			Bind_Resources() override;
	virtual HRESULT			Render() override;

public:
	void					Update(_float fTimeDelta);
	void					Setting_Speed(SPEED_VALUE type, _float2 range);
	void					Remove_Speed(SPEED_VALUE type);
	void					Remove_Speed();
	void					Setting_Pivot(_float3 pivot);
	void					Setting_Loop(_bool isLoop) { m_IsLoop = isLoop; };

private:
	_float3					m_vPivot = {};
	_float*					m_fSpeed[ENUM_CLASS(SPEED_VALUE::SPEED_END)];
	_bool					m_IsLoop = {};
	_float					m_fRotationPerSec = {};
	_float					m_fOffset = {};
	_float3					m_fRange = {};
	_float2					m_fScale = {};

	_float3					m_vSourceColor = {};
	_bool					m_bIsCircle = {};

public:
	static CVIBuffer_Point_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END




