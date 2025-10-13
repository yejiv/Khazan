#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "VIBuffer_Point_Instance.h"

NS_BEGIN(Editor)

class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	// Effect Tool Test

	// ===== GUI 관련 변수 =====
	_char m_szSystemName[64] = {};
	_char m_szEmitterName[64] = {};
	vector<class CParticleSystem*> m_ParticleSystems;
	_int m_iSelectedSystem = { -1 };
	_int m_iPrevSelectedSystem = { -1 };
	_int m_iSelectedEmitter = { -1 };
	_int m_iPrevSelectedEmitter = { -1 };
	_wstring m_strEmitterName = {};
	_float4 m_vPaletteColor = { 1.f, 1.f, 1.f, 1.f };

	// ===== 파티클 관련 기본 변수 =====
	CVIBuffer_Point_Instance::POINT_INSTANCE_DESC m_PointInfo = {};
	_bool m_isSpread = {};
	_bool m_isDrop = {};

	// ===== 파티클 관련 확장 변수 =====
	_bool		m_isRandomDir = {};
	_float		m_fAngleMin = {};
	_float		m_fAngleMax = {};
	_bool		m_isGravity = {};
	_float3		m_vGravity = {};

private:
	HRESULT Create_ParticleSystem();
	HRESULT Create_ParticleEmitter();
	HRESULT Delete_ParticleEmitter();

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Camera();

public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END