#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CDistortion final : public CBase
{
private:
	CDistortion();
	virtual ~CDistortion() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);

public:
	HRESULT						Bind_Distortion_ShaderResources();
	void						Start_Distortion(const DISTORTION_DESC& Desc);

private:
	DISTORTION_DESC				m_Desc = {};

	_float						m_fTimeAcc = {};
	_bool						m_isEnable = {};

	// Noise Textures

public:
	static CDistortion*			Create();
	virtual void				Free() override;
};

NS_END