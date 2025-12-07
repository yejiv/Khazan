#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CWeaponObject abstract :  public CPartObject
{
public:
    typedef struct tagPartObjectDesc
    {
        const _float4x4* pParentMatrix;
    }PARTOBJECT_DESC;

protected:
    CWeaponObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CWeaponObject(const CWeaponObject& Prototype);
    virtual ~CWeaponObject() = default;

public:
    /* Call back */
    void                        On_JustGuardCallback(_bool _is) { if (m_JustGuardCallback) m_JustGuardCallback(_is); }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    /* Call back */
    function<void(_bool)>       m_JustGuardCallback = { nullptr };

protected:
    /* Call back */
    void                        Set_JustGuardCallBack(function<void(_bool)> callback) { m_JustGuardCallback = callback; }



public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

NS_END