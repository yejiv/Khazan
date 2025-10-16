#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance final : public CVIBuffer_Instance
{
public:
	enum class SPEED_VALUE { SPREAD_SPEED, ROTATION_SPEED, UPWARD_SPEED, SCALE_SPEED, SPEED_END };

	typedef struct tagPointInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed{ 0.f, 0.f };
		_float2 vLifeTime;
		_float	fOffset;
		const _char* pFilePath;
	}POINT_MESH_DESC;

private:
	CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	void					Reset(); 

public:
	virtual HRESULT			Initialize_Prototype(INSTANCE_DESC* pArg);
	virtual HRESULT			Initialize(void* pArg); 

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
	_float3*				m_pVertexPositions = { nullptr };

public:
	static CVIBuffer_Mesh_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pDesc);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END



