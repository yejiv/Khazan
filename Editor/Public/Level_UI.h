#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "UIObject.h"


NS_BEGIN(Editor)

class CLevel_UI final : public CLevel
{
private:
	CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_UI() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void		Update(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT				Ready_Layer_Camera(const _wstring& strLayerTag);

private:
	void				Show_Hierarchy_Menu(const _char* szDefaultName);
	void				Show_Hierachy(class CUIObject* pRootUIObject);
	void				Add_Child(const _char* szDefaultName);
	void				Remove_Child(const _char* szDefaultName);

	void				Show_Inspector_Menu();

	void				Show_CreateUI_Menu(const _char* szDefaultName);
	
	CUIObject*			Find_UIObject(const _char* szUIObjectName);

private:
	vector<CUIObject*>	m_EditorUIObjects;
	CUIObject*			m_SelectedObject = { nullptr };
	CUIObject*			m_SelectedParrentObject = { nullptr };

private:
	_uint				m_iCurrentCount = { 0 };
	_char				m_szSelectedName[MAX_PATH] = "SelectedName";
	_bool				m_isChildSelected = { false };

public:
	static CLevel_UI*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

NS_END