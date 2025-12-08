#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const { return &m_LightDesc; }
	void Set_LightDesc(const LIGHT_DESC& LightDesc) { m_LightDesc = LightDesc; }
	void Set_Position(const _float4& vPosition) { m_LightDesc.vPosition = vPosition; }
	void Set_Enable(_bool isEnable) { m_isEnable = isEnable; }
	_bool isEnable() { return m_isEnable; }

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
    void Update(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
    void Start_LightTransition(const LIGHT_TRANSITION_DESC& Desc, _bool isRestore);
    void Backup_LightDesc() { m_OriginalLightDesc = m_LightDesc; }

private:
	LIGHT_DESC				m_LightDesc = {};
	_bool					m_isEnable = {};

    // Transition
    LIGHT_TRANSITION_DESC                   m_TargetLightDesc = {};
    _float                                  m_fTransTimeAcc = {};
    _bool                                   m_isTransition = {};
    LIGHT_DESC                              m_StartLightDesc = {};
    _float                                  m_fBlinkPeriod = {};

    // Backup
    LIGHT_DESC                              m_OriginalLightDesc = {};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

NS_END