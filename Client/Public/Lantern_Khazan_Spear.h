#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;

NS_END

NS_BEGIN(Client)

class CLantern_Khazan_Spear final : public CPartObject
{
public:
    typedef struct tagLanternKhazanSpearDesc : public CPartObject::PARTOBJECT_DESC
    {
        class CTransform* pParentTransform = { nullptr };
        _float4x4*        pLHandSocket_Matrix = { nullptr };
        _float4x4*        pLanternSocket_Matrix = { nullptr };

    }LANTERN_KHAZAN_SPEAR_DESC;
private:
    CLantern_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLantern_Khazan_Spear(const CLantern_Khazan_Spear& Prototype);
    virtual ~CLantern_Khazan_Spear() = default;

public:
    virtual HRESULT     Initialize_Prototype();
    virtual HRESULT     Initialize_Clone(void* pArg);
    virtual void        Priority_Update(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Late_Update(_float fTimeDelta);
    virtual HRESULT     Render();

public:
   // void                Set_Enble(_bool isEnble) { m_isEnble = isEnble; }
    void                Set_Equipped(bool equip);
    _bool                Get_Equipped() { return m_isEquip; }

    
private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    class CTransform*       m_pParentTransform = { nullptr };

    _float4x4*              m_pLHandSocket_Matrix = { nullptr };
    _float4x4*              m_pLanternSocket_Matrix = { nullptr };
    _matrix                 m_matOffset; 
    _bool                   m_isEnble = { false };
    _bool                   m_isEquip = { false };
    _bool                   m_isEquiping = { false };

    _float2                 m_fEquipTime2 = { 0.f ,1.1f };
    _float2                 m_fEquipTime1 = { 0.f ,1.f };
    LEVEL                   m_eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

private:
    HRESULT                 Ready_Components();
    HRESULT                 Bind_ShaderResources();

public:
    static CLantern_Khazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END
