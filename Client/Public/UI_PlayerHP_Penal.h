#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_PlayerHP_Penal final : public CUI_Panel
{
private:
	enum class UIANIMSTATE { ON, OFF, END};

private:
	CUI_PlayerHP_Penal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PlayerHP_Penal(const CUI_PlayerHP_Penal& Prototype);
	virtual ~CUI_PlayerHP_Penal() = default;

public:
	void						On_Penal();
    void                        Off_Penal();
public:
	virtual HRESULT				Initialize_Prototype(_uint iLevel);
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

	virtual void				Bubble_EventCall();
	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	_float						m_fAccTime = {};
	UIANIMSTATE					m_eAnimState = { UIANIMSTATE::END };
private:
	virtual	HRESULT				Ready_Prototype();

public:
	static CUI_PlayerHP_Penal*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END