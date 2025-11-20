#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"
#include "Amount.h"

NS_BEGIN(Client)
class CAmount_Panel final : public CUI_Panel
{
private:
	CAmount_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmount_Panel(const CAmount_Panel& Prototype);
	virtual ~CAmount_Panel() = default;

public:
	void								Add_Value(_int IValue);
	void								Setting_Index(CAmount::AMOUNT_TYPE eType, _uint* iCulValue);
public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	CAmount::AMOUNT_TYPE				m_eType = { CAmount::AMOUNT_TYPE::END};
	class CUI_TextBox*					m_pCulValueText = { nullptr };
	class CUI_TextBox*					m_pAddValueText = { nullptr };

	_uint*								m_iCulValue = {};
	_int								m_iAddValue = {};

	_int								m_iAccValue = {};
	_bool								m_isAddValue = {};
	_float								m_fAccTime = {};

    _bool                               m_isAdd = {};
private:
	HRESULT								Ready_Component();
	_wstring							IntToWstring(_int iValue);

	void								AddValue_Check(_float fTimeDelta);
public:
	static CAmount_Panel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END