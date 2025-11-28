#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CMotionTrail;
NS_END

NS_BEGIN(Client)

class CGSword_Khazan_GS final : public CPartObject
{
public:
	typedef struct tagGSwordKhazanGSDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint* pState = { nullptr };
		class CTransform* pParentTransform = { nullptr };

	}GSWORD_KHAZAN_GS_DESC;

private:
	CGSword_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGSword_Khazan_GS(const CGSword_Khazan_GS& Prototype);
	virtual ~CGSword_Khazan_GS() = default;

public:
	_float4x4*  Get_BoneMatrix(const _char* pBoneName);
	const _matrix& Get_OffestMatrix() const { return m_matOffset; }
    void		Set_matWeaponR(_float4x4* mat) { m_pWeaponR_Matrix = mat; }
    void		Set_matBackPack(_float4x4* mat) { m_pBackPack_Matrix = mat; }

   // _bool*      Get_isEquip() { return &m_isEquip; }
    void        Set_Equipped(bool equip) { m_isEquip = equip; }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;

public:
	CModel* Get_Model() { return m_pModelCom; }
    void    Set_Enble(_bool isEnble) { m_isEnble = isEnble; }

    /*  Motion Trail */
    const MOTIONTRAIL_CONFIG&   Get_MotionTrailConfig();
    void                        Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config);
    void                        Set_EnableMotionTrail(_bool isEnable);
    _bool                       isEnableMotionTrail();
    void                        Start_MotionTrail(_float fDuration);
    void                        Set_MotionTrailCallBack(function<void(const _wstring&, _bool)> callback) { m_OnMotionTrailCallBack = callback; }
    void                        On_MotionTrail(const _wstring strKey, _bool isActive) {
        m_pMotionTrailCom->Set_Config(strKey);
        m_isActiveMotionTrail = isActive;
    }

private:
	CShader*					m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr }; //현재 모델
    CModel*                     m_pModelCom_Punish_Spear = { nullptr };
    CModel*                     m_pModelCom_Flash_Spear = { nullptr };
    CModel*                     m_pModelCom_Meteor_GSword = { nullptr };
    CModel*                     m_pModelCom_Execution_GSword = { nullptr };
    CMotionTrail*               m_pMotionTrailCom = { nullptr };
    class CClientInstance*      m_pClientInstance = { nullptr };
    class CTransform*			m_pParentTransform = { nullptr };

	_uint*						m_pParentState = { nullptr };
	_float4x4*					m_pWeaponR_Matrix = { nullptr };
    _float4x4*                  m_pBackPack_Matrix = { nullptr };

	_matrix						m_matOffset;
    _bool                       m_isEnble = { true };
    _bool                       m_isEquip = { true };

    /* Motion Trail */
    function<void(const _wstring&, _bool)>  m_OnMotionTrailCallBack;
    _bool                       m_isActiveMotionTrail = { false };

private:
    void        Change_Weapon(EQUIPMENTTYPE type, const _wstring& strPartName);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGSword_Khazan_GS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
