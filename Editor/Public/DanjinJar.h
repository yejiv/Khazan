#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CDanjinJar final : public CProp_Interactive
{
public:
    enum class DANJINJAR_TYPE { A, B, C, D, E, F, G, H, END };

    typedef struct tagDanjinJarMoveStep {
        _float4 vStep1{};
        _float4 vStep2{};
        _float4 vStep3{};
        _float4 vStep4{};
        _float4 vStep5{};
        _float4 vStep6{};
        _float4 vStep7{};
        _float4 vStep8{};
        _float4 vStep9{};
        _float4 vStep10{};
        _float4 vStep11{};
        _float4 vStep12{};
        _float4 vStep13{};
        _float4 vStep14{};
        _float4 vStep15{};
        _float4 vStep16{};

    }DANJINJAR_STEP;

private:
    enum class ANIM_STATE
    {
        DANCE1_ACTIVE,                  // 춤추는 1
        DANCE1_LOOP,                    // 춤추는 1
        FLIPPING_ACTIVE,                // 발리송 트릭
        FLIPPING_LOOP,                  // 발리송 트릭
        LIE_ACTIVE,                     // 눕는 액션
        LIE_LOOP,                       // 눕는 액션
        WALK_ACTIVE,                    // 걷는 액션
        WALK_LOOP,                      // 걷는 액션
        LEAN_ACTIVE,                    // 기대는 액션
        LEAN_LOOP,                      // 기대는 액션
        SHADOWBOXING_ACTIVE,            // 쉐복
        SHADOWBOXING_LOOP,              // 쉐복
        DANCE2_ACTIVE,                  // 춤추는 2
        DANCE2_LOOP,                    // 춤추는 2
        DANCE3_ACTIVE,                  // 춤추는 3
        DANCE3_LOOP,                    // 춤추는 3
        DRSTRANGE_ACTIVE,               // 닥터스트레인지
        DRSTRANGE_LOOP,                 // 닥터스트레인지
        DEACTIVE,                       // 뚜따
        DEACTIVE_IDLE,                  // 뚜따
        END
    };

    enum SKIP_MESH_TYPE
    {
        SMT_HEAD,
        SMT_BODY,
        SMT_LEFT,
        SMT_RIGHT,
        SMT_CENTER
    };

public:
    typedef struct tagDanjinJarDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        DANJINJAR_STEP StepPosition{};

        DANJINJAR_TYPE eJarType{ DANJINJAR_TYPE::A };

    }DANJINJAR_DESC;

private:
    CDanjinJar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDanjinJar(const CDanjinJar& Prototype);
    virtual ~CDanjinJar() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    DANJINJAR_TYPE Get_DanjinJar_ModelType() { return m_eJarType; }
    void Set_DanjinJar_ModelType(DANJINJAR_TYPE eModelType);

    string Get_DanjinJar_ModelType_ByString();

    DANJINJAR_STEP Get_StepPosition() { return m_DanjinJarStep; }
    void StepPositionSetting(_uint iStep);
    void MoveStepPosition(_uint iStep);

private:
    CModel* m_pModelType[ENUM_CLASS(DANJINJAR_TYPE::END)] = { nullptr };

    ANIM_STATE m_eAnimState = { ANIM_STATE::DANCE1_ACTIVE };

    DANJINJAR_TYPE m_eJarType = { DANJINJAR_TYPE::A };

    DANJINJAR_STEP m_DanjinJarStep = {};

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

private:
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);
    void AnimChange(ANIM_STATE eAnimState, _bool isLoop = false);
    bool Skip_Mesh(_uint iMeshIndex);

    void Find_Target();

    void Chase_Target(_float fTimeDelta);

public:
    static CDanjinJar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END