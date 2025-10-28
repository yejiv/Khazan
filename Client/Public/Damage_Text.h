#pragma once
#include "UI_Text.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Client)
class CDamage_Text final : public CUI_Text
{
public:
	enum class DAMAGE_TYPE { DEFAULT, BACK, SPECIAL, PLAYER, END};
private:
	CDamage_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamage_Text(const CDamage_Text& Prototype);
	virtual ~CDamage_Text() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	_bool						Render_Damage(DAMAGE_TYPE eDamageType, _vector vPos, _uint iDamage);
	virtual void				Reset() override;

private:
	_uint						m_iDamage = {};
		
private:
	void						Update_WolrdPos(_vector vPos);

public:
	static CDamage_Text*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};
NS_END
