#pragma once
#include "UI_Tap.h"
#include "Client_Defines.h"

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
	HRESULT								Setting_Panel(_int iTapIndex);
    void								Set_Selete(_bool isSelete);
public:
	virtual HRESULT						Initialize_Prototype(_int iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual HRESULT						Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg) override;
	virtual void						Update_Alpha(_float fAlpha) override;
private:
	_bool								m_bIsSelete = { false };
	_float								m_fAccTime = {1.f};

	_int								m_iTapIndex = { -1 };
	vector<class CSkill_Slot_Panel*>	m_pPanel;

    _bool                               m_isOver = { false };
private:
	HRESULT								Ready_Children();
public:
	static CSkill_Tap*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END