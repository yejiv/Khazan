#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_ProgressBar abstract : public CUIObject
{
protected:
	CUI_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ProgressBar(const CUI_ProgressBar& Prototype);
	virtual ~CUI_ProgressBar() = default;
					
public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	_float						m_fProgress_Value = {};

	_float						m_fCurrentValue = {};
	_float						m_fMaxValue = {};
	_float						m_fPreValue = {};
	_float						m_fLerpValue = {};

	_float						m_fLerpSpeed = {};

protected:
	void						Progress_Update();
public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;

};

NS_END