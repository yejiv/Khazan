#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Client)

class CSpear_Khazan_Spear final : public CPartObject
{
public:
	typedef struct tagSpearKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint* pState = { nullptr };
		class CTransform* pParentTransform = { nullptr };

	}SPEAR_KHAZAN_SPEAR_DESC;

private:
	CSpear_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpear_Khazan_Spear(const CSpear_Khazan_Spear& Prototype);
	virtual ~CSpear_Khazan_Spear() = default;

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

private:
	CShader*					m_pShaderCom = { nullptr };
	CModel*						m_pModelCom = { nullptr };
	class CTransform*			m_pParentTransform = { nullptr };

	_uint*						m_pParentState = { nullptr };
	_float4x4*					m_pWeaponR_Matrix = { nullptr };
    _float4x4*                  m_pBackPack_Matrix = { nullptr };

	_matrix						m_matOffset;
    _bool                       m_isEnble = { true };
    _bool                       m_isEquip = { false };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSpear_Khazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
