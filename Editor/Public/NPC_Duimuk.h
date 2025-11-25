#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CNPC_Duimuk final : public CProp_Interactive
{
private:
    enum ANIM_STATE
    {
        IDLE1,
        TALK_END,
        TALK_IDLE,
        TALK_START,
        IDLE2,
        IDLE3,
        END
    };

public:
    typedef struct tagDuimukDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }DUIMUK_DESC;

private:
    CNPC_Duimuk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNPC_Duimuk(const CNPC_Duimuk& Prototype);
    virtual ~CNPC_Duimuk() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

public:
    static CNPC_Duimuk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END