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
	virtual HRESULT Initialize() override;
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT			Ready_Layer_BackGround(const _wstring& strLayerTag);



private:
	void			Show_Hierarchy_Menu();
	void			Show_Inspector_Menu();
	void			Show_CreateUI_Menu(const char* szDefaultFileName);


private:
	_bool			m_isUIObjectMenu = { false };


private:
	vector<CUIObject*>	m_EditorUIObjects;
	CUIObject*			m_SelectedObject = { nullptr };

public:
	static CLevel_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END