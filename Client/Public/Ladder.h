#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Client)

class CLadder final : public CProp_Interactive
{
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
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CLadder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END