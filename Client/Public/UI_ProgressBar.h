#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CUI_ProgressBar abstract : public CUIObject
{
public:
	enum class BAR_DIRECTION { LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP, END };
	enum class BAR_MODE { REDUCE, EXPAND, END };

public:
	typedef struct tagProgressBarDesc : public CUIObject::UIOBJECT_DESC
	{
		_float				fCurrentValue = {};
		_float				fMaxValue = {};
		BAR_DIRECTION		eDirection = {};
		BAR_MODE			eMode = {};

	}PROGRESSBAR_DESC;

protected:
	CUI_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ProgressBar(const CUI_ProgressBar& Prototype);
	virtual ~CUI_ProgressBar() = default;
					
public:
	_float						Get_LerpSpeed() const { return m_fLerpSpeed; }
	_float						Get_CurrentValue() const { return m_fCurrentValue; }
	_float						Get_MaxValue() const { return m_fMaxValue; }

	void						Set_Bar_Direction(BAR_DIRECTION eDirection) { m_eDirection = eDirection; }
	void						Set_Bar_Mode(BAR_MODE eMode) { m_eMode = eMode; }
	void						Set_LerpSpeed(_float fLerpSpeed) { m_fLerpSpeed = fLerpSpeed; }
public:
	void						Set_CurrentValue(_float fCurrentValue) { m_fCurrentValue = fCurrentValue; m_isChange = true; }
	void						Set_MaxValue(_float fMaxValue) { m_fMaxValue = fMaxValue; m_isChange = true; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	_float						Make_Rate(_float fSrc, _float fDst);
	void						Reduce_RightToLeft(_float fRate);
	void						Reduce_LeftToRight(_float fRate);
	void						Expand_RightToLeft(_float fRate);
	void						Expand_LeftToRight(_float fRate);


protected:
	_float3						m_vOriginSize = {};
	_float3						m_vOriginPos = {};

	_float						m_fCurrentValue = {};
	_float						m_fMaxValue = {};
	_float						m_fDisplayRate = {};

	_float						m_fLerpSpeed = {};

	_bool						m_isChange = { false };

	BAR_DIRECTION				m_eDirection = { BAR_DIRECTION::END };
	BAR_MODE					m_eMode = { BAR_MODE::END };

protected:
	class CClientInstance* m_pClientInstance = { nullptr };

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;

};

NS_END