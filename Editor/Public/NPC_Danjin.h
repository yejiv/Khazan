#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CNPC_Danjin final : public CProp_Interactive
{
private:
    enum ANIM_STATE
    {
        IDLE,
        TALK_END,
        TALK_IDLE,
        TALK_START,
        END
    };

public:
    typedef struct tagDanjinDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }DANJIN_DESC;

private:
    CNPC_Danjin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CNPC_Danjin(const CNPC_Danjin& Prototype);
    virtual ~CNPC_Danjin() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

public:
    static CNPC_Danjin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END