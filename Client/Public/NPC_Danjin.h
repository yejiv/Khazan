#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

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

private:
    CBody* m_pStaticCom = { nullptr };
    CBody* m_pTriggerCom = { nullptr };

    class CInteraction_Guide* m_pGuide = { nullptr };

    class CUI_Talk_Dangin* m_pDanginTalkUI = { nullptr };

private:
    EventObject m_Event = {};

    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE };

    class CClientInstance* m_pClientInstance = { nullptr };

private:
    COLLISION_DESC m_TriggerCollisionDesc;

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);
    HRESULT Ready_3D_Talk_UI(void* pArg);
    HRESULT Ready_DefaultSetting(void* pArg);
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

    void Input_Interact_Event(_float fTimeDelta);
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CNPC_Danjin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END