#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"
#include "UI_MainMenu.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CSkill_Tap final : public CUI_Tap
{
private:
	CSkill_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Tap(const CSkill_Tap& Prototype);
	virtual ~CSkill_Tap() = default;

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
private:

	_bool								m_bIsSelete = { false };
	_float								m_fAccTime = {1.f};

public:
	static CSkill_Tap*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END