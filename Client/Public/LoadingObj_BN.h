#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CLoadingObj_BN final : public CProp_Interactive
{
private:
    enum ANIM_STATE
    {
        AFTER_END, AFTER_IDLE, AFTER_LOOP, AFTER_START,
        BEFORE_END, BEFORE_IDLE, BEFORE_LOOP, BEFORE_START,
        END
    };

    enum class BLADENEXUS_ID
    {
        HEINMACH_ENTER = 0, HEINMACH_CAVE, HEINMACH_CLIFF, HEINMACH_YETUGA,
        EMBARS_UNDER, EMBARS_DEEP, EMBARS_CORE,
        ID_END
    };

public:
    typedef struct tagLoadingObjBladeNexusDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }LOADINGOBJ_BLADENEXUS_DESC;

private:
    CLoadingObj_BN(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLoadingObj_BN(const CLoadingObj_BN& Prototype);
    virtual ~CLoadingObj_BN() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::AFTER_IDLE };

    EventObject m_Event = {};

private:
    _float m_fEmissiveIntensity = { 0.f };
    _bool m_isEnableEmissive = { true };
    _bool m_isEnableBloom = { true };

    _uint m_iEventID = { 0 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    static CLoadingObj_BN* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END