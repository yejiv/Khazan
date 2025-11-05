#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"
#include "UI_Logo.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_Logo_List final : public CUI_Tap
{
private:
	CUI_Logo_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Logo_List(const CUI_Logo_List& Prototype);
	virtual ~CUI_Logo_List() = default;

public:
	void								Update_Pos(CUI_Logo::LISTTYPE eType, _float2 vPos, _float fOffSetY);
	void								Set_Selete(_bool isSelete);
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CTexture*							m_pMaskTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	_bool								m_bIsSelete = { false };
	_float								m_fAccTime = {1.f};
	
	CUI_Logo::LISTTYPE					m_eType = { CUI_Logo::LISTTYPE::END };

	class CUI_TextBox*					m_pText = { nullptr };
	class CUI_Default_Tex*				m_pDecoL = { nullptr };
	class CUI_Default_Tex*				m_pDecoR = { nullptr };
private:
	HRESULT								Ready_Componet();

public:
	static CUI_Logo_List*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END