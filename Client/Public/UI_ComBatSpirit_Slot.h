#pragma once
#include "UI_Slot.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CUI_ComBatSpirit_Slot final : public CUI_Slot
{
private:
	CUI_ComBatSpirit_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ComBatSpirit_Slot(const CUI_ComBatSpirit_Slot& Prototype);
	virtual ~CUI_ComBatSpirit_Slot() = default;

public:
	void					Update_Pos(_float2 vPos, _float fOffSetX, _int iMaxIndex);
	void					Update_Gauge(_float fValue);
public:
	virtual HRESULT			Initialize_Prototype(_uint iLevel);
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void			Bubble_EventCall();
	virtual HRESULT			Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;

private:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

	_float					m_fCulGauge = {};
private:
	HRESULT					Ready_Component();

public:
	static CUI_ComBatSpirit_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END