#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIParent abstract : public CUIObject
{
protected:
	CUIParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIParent(const CUIParent& Prototype);
	virtual ~CUIParent() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	virtual void				Add_Child(CUIObject* pChild);
	virtual void				Remove_Child(CUIObject* pChild);
	virtual void				Update_Transform(CUIObject* pParent, _float2 vPos) override;
	virtual void				Update_Scaling(_float fSize) override;
	virtual void				Update_Rotation(_float fAngle) override;

	virtual HRESULT				Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

protected:
	vector<CUIObject*>			m_Children = {};

public:
	virtual CGameObject*		Clone(void* pArg) = 0;
	virtual void				Free() override;


};

NS_END