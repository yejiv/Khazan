#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInteraction_Guide final : public CUI_Panel
{
public:
	enum class GUIDE_TYPE { DEFAULT, PROGRESS, END };

public:
	void					Setting_Guide(GUIDE_TYPE eType, const _float4x4* pTagetMat, _float2 vOffset, _wstring strText, _float fDelayTime = 0.f);
	_bool					IsPressing();
private:
	CInteraction_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Guide(const CInteraction_Guide& Prototype);
	virtual ~CInteraction_Guide() = default;

public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual void				Reset() override;

private:
	class CUI_TextBox*			m_pTextBox = { nullptr };
	class CUI_Guide_Gauge*		m_pGauge = { nullptr };
	class CInteraction_Icon*	m_pIcon = {nullptr};
	const _float4x4*			m_pTagetMat = { nullptr };

	GUIDE_TYPE					m_eGuideType = {};
	_bool						m_bPrePressingState = { false };
	_bool						m_isPressing = {false};
	_float						m_fDelayTime = {};
	_float						m_fAccTime = {};
	_bool						m_bIsFiash = {};
private:
	HRESULT						Ready_Prototype();
	HRESULT						Ready_Children();
	void						Update_WorldPos();

public:
	static CInteraction_Guide*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END
