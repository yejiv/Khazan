#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Client)

class CLadder final : public CProp_Interactive
{
private:
    enum class LADDER_POINT
    {
        DOWN_POS_CH,
        UP_POS_CH,
        CLIMB_UP,
        CLIMB_DOWN,
        END
    };
    ////////////////////////////////////////////////////////////////////////
    // 
    // TOP
    // IA_BeginLoc_Climb_Down : 플레이어 내려가는 애니메이션 시작 지점
    // ClimbEndLoc_Top : 사다리 위쪽 위치
    // IA_BeginLoc_Kick : 킥은 뭐고
    // 
    ////////////////////////////////////////////////////////////////////////
    // 
    // BOTTOM
    // IA_BeginLoc_Climb_Up : 플레이어 애니메이션 시작 지점
    // ClimbEndLoc_Bottom : 사다리 내려오는 지점
    // 
    ////////////////////////////////////////////////////////////////////////
public:
    typedef struct tagLadderOffset
    {
        _float fOffSetHeight{};
        _int iSegmentCount{};

    }LADDER_OFFSET;

    typedef struct tagLadderDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        LADDER_OFFSET LadderOffSet{};

    }LADDER_DESC;

private:
    CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CLadder(const CLadder& Prototype);
    virtual ~CLadder() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    class CLadder_Bottom* m_pLadderBot = { nullptr };
    class CLadder_Top* m_pLadderTop = { nullptr };

    CBody* m_pTriggerCom[ENUM_CLASS(LADDER_POINT::END)] = { nullptr };

    class CInteraction_Guide* m_pGuide = { nullptr };
    _float4x4 m_matGuide = {};

    _float4 m_vPlayerPosition = {};

    _float4 m_vDownPlayerPos = {};
    _float4 m_vClimbUpPos = {};

    _float4 m_vUpPlayerPos = {};                  // 사다리 위쪽 플레이어 위치 정보
    _float4 m_vClimbDownPos = {};                 // 사다리 액션 중단 액션

    LADDER_POINT m_eLadderPoint = { LADDER_POINT::END };

private:
    void Set_GuideMatrix(_float4 vGuidePos);
    void Set_PlayerPosition();

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);

    void Input_Interact_Event(_float fTimeDelta);

    // TOP
    // IA_BeginLoc_Climb_Down : 플레이어 내려가는 애니메이션 시작 지점
    // ClimbEndLoc_Top : 사다리 위쪽 위치
    // IA_BeginLoc_Kick : 킥은 뭐고
    // 
    ////////////////////////////////////////////////////////////////////////
    // 
    // BOTTOM
    // IA_BeginLoc_Climb_Up : 플레이어 애니메이션 시작 지점
    // ClimbEndLoc_Bottom : 사다리 내려오는 지점

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CLadder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END