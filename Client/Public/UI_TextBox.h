#pragma once
#include "UI_Text.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)
class CUI_TextBox final : public CUI_Text
{
private:
	CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_TextBox(const CUI_TextBox& Prototype);
	virtual ~CUI_TextBox() = default;

public:
	void						Update_PosY(_int iIndex, _float2 vPos, _float fOffSetY);

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
private:
	CShader*					m_pShaderCom = { nullptr };
private:
	HRESULT						Ready_Component();

public:
	static CUI_TextBox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END
