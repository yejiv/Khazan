#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CSkill_Info_Tex final : public CUI_Texture
{
private:
    enum MOVIE_TYPE {
        Assault, FullMoon, MoonLight, MoonlightStance, QuickSlash,
        ShadowSlash, ShadowStrike, SpiralThrust, VitalStrike, Thrust_Vortex,
        Breakthrough, BreathTaking, Bloodshed, Embryonic, BridleOfBattle, GiantHunt,
        InnerFury, LimitBreak, ManifestStrength, Momentum, Phantom, WarCry,
        END };

private:
	CSkill_Info_Tex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Info_Tex(const CSkill_Info_Tex& Prototype);
	virtual ~CSkill_Info_Tex() = default;

public:
    void								Setting_Tex(_int iSkillIndex);

public:
	virtual HRESULT						Initialize_Prototype();
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

    virtual void				        Update_Transform(CUIObject* pParent, _float2 vPos);
private:
	vector<CTexture*>					m_pMovieCom = { nullptr };

	CShader*							m_pShaderCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
    class CUI_Atlas_Icon*               m_pIcon = { nullptr };

	_bool								m_isMovie = { false };
    MOVIE_TYPE                          m_eMovieType = {};
    _float                              m_fDeltaTime = {};
private:
	HRESULT								Ready_Component();
	
public:
	static CSkill_Info_Tex*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END