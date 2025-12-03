#pragma once
#include "UIParent.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CUI_Slot abstract : public CUIParent
{
public:
	enum class UISTATE { DISABLE, ENABLE, END };

	typedef struct UI_Slot_Tag : public CUIObject::tagUIObjectDesc
	{
		_int iIndex;
	}UISLOTDESC;
protected:
	CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Slot(const CUI_Slot& Prototype);
	virtual ~CUI_Slot() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize_Clone(void* pArg) override;
	virtual void					Priority_Update(_float fTimeDelta) override;
	virtual void					Update(_float fTimeDelta) override;
	virtual void					Late_Update(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

	virtual HRESULT					Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	
protected:
	vector<std::function<void()>>	m_Events;
	_int							m_iIndex = { -1 };

protected:
	virtual _bool					ButtonOver(HWND hWnd);
	virtual _bool					ButtonClick(HWND hWnd, _bool IsRight, _bool IsDonw, INPUT_TYPE eType = INPUT_TYPE::UI);

public:
	virtual CGameObject*			Clone(void* pArg) = 0;
	virtual void					Free() override;
};

NS_END