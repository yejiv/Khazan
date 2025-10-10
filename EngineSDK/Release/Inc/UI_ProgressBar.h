#pragma once
#include "UIObject.h"

class CUI_ProgressBar abstract : public CUIObject
{
protected:
	enum class BAR_TYPE { FRONT, BACK, OUTLINE };

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

protected:
	_float						Make_Rate(_float fSrc, _float fDst);

protected:
	_float3						m_vOriginSize = {};
	_float3						m_vOriginPos = {};


public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;

};

