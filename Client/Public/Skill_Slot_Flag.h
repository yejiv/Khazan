#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END


NS_BEGIN(Client)
class CSkill_Slot_Flag final : public CUI_Panel
{
private:
    CSkill_Slot_Flag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkill_Slot_Flag(const CSkill_Slot_Flag& Prototype);
    virtual ~CSkill_Slot_Flag() = default;

public:
    virtual HRESULT						Initialize_Prototype(_int iLevel);
    virtual HRESULT						Initialize_Clone(void* pArg) override;
    virtual void						Priority_Update(_float fTimeDelta) override;
    virtual void						Update(_float fTimeDelta) override;
    virtual void						Late_Update(_float fTimeDelta) override;
    virtual HRESULT			            Render() override;

private:
	CShader*				            m_pShaderCom = { nullptr };
	CTexture*				            m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			            m_pVIBufferCom = { nullptr };

private:
	HRESULT					            Ready_Component();

public:
    static CSkill_Slot_Flag*            Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void						Free() override;
};

NS_END