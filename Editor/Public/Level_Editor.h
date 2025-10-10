#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Editor final : public CLevel
{
private:
	CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// Effect Tool Test

	// ===== GUI 관련 변수 =====
	_char m_szSystemName[64] = {};
	vector<class CParticleSystem*> m_ParticleSystems;
	_int m_iSelectedSystem = { -1 };
	_int m_iSelectedEmitter = { -1 };

	// ===== 파티클 관련 기본 변수 =====
	_float3		m_vCenter = {};
	_float3		m_vDir = { 0.f, 1.f, 0.f };
	_float		m_fSpeed = {};
	_float		m_fScale = { 1.f };
	_float		m_fLifeTime = {};
	_bool		m_bLoop = {};
	_int		m_iNumInstance = {};

	// ===== 파티클 관련 확장 변수 =====
	_bool		m_bUseRandomDir = {};
	_float		m_fAngleMin = {};
	_float		m_fAngleMax = {};
	_bool		m_bUseGravity = {};
	_float3		m_vGravity = {};

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);

public:
	static CLevel_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END