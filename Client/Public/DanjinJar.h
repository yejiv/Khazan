#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CDanjinJar abstract : public CProp_Interactive
{
public:
    enum class DANJINJAR_TYPE { A, B, C, END };

    typedef struct tagDanjinJarMoveStep {
        _float4 vStep1{};       // 시작 지점
        _float4 vStep2{};       // 2번째 지점
        _float4 vStep3{};       // 3번째 지점
        _float4 vStep4{};       // 4번째 지점
        _float4 vStep5{};       // 5번째 지점
        _float4 vStep6{};       // 6번째 지점
        _float4 vStep7{};       // 7번째 지점
        _float4 vStep8{};       // 8번째 지점
        _float4 vStep9{};       // 9번째 지점
        _float4 vStep10{};       // 10번째 지점
        _float4 vStep11{};       // 11번째 지점
        _float4 vStep12{};       // 12번째 지점
        _float4 vStep13{};       // 13번째 지점
        _float4 vStep14{};       // 14번째 지점
        _float4 vStep15{};       // 15번째 지점
        _float4 vStep16{};       // 16번째 지점

    }DANJINJAR_STEP;

protected:
    enum class MOVE_STATE {
        IDLE,
        MOVE
    };

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
        MESH_BODY,              // 몸통
        MESH_HEAD,              // 뚝배기
        MESH_LEFT,              // 왼쪽 풀때기
        MESH_RIGHT,             // 오른쪽 풀때기
        MESH_CENTER             // 중앙 풀때기

    };

public:
    typedef struct tagDanjinJarInfo
    {
        DANJINJAR_STEP StepPosition{};

        DANJINJAR_TYPE eJarType{ DANJINJAR_TYPE::A };

    }DANJINJAR_INFO;

    typedef struct tagDanjinJarDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        DANJINJAR_INFO DanjinJar_Info{};

    }DANJINJAR_DESC;

protected:
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

protected:
    CBody* m_pTriggerCom = { nullptr };

    //UI테스트
    class CUI_Talk_Danjinjar* m_pTalk = { nullptr };

    ANIM_STATE m_eAnimState = { ANIM_STATE::DANCE1_ACTIVE };
    MOVE_STATE m_eMoveState = { MOVE_STATE::IDLE };

    DANJINJAR_TYPE m_eJarType = { DANJINJAR_TYPE::A };

    DANJINJAR_STEP m_DanjinJarStep = {};

    _float m_fDefaultLength = {};           // Step1 이랑 Step2 의 거리를 기본값으로 두고 나머지 거리의 속도 제어

    _bool m_isMoveFlag = { false };

    _float4 m_vStartPos = {};
    _float4 m_vEndPos = {};

    _float m_fLerpTime = { 0.f };           // fTimeDelta 누적 시간 ( 이동 )
    _float m_fDuration = { 0.f };           // 목적지까지 이동 시간

    _float m_fMoveSpeed = { 1.f };          // 이동 속도

    _bool m_isFindTarget = { false };
    CTransform* m_pTargetCom = { nullptr };

protected:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

protected:
    void AnimChange(ANIM_STATE eAnimState, _bool isLoop = false, _bool isCheck = false);
    bool Skip_Mesh(_uint iMeshIndex);

    void Find_Target();

    void Look_Target(_float fTimeDelta);

    void MoveToNextStep(_float4 vTargetPos, _float4 vStartPos, _float fTimeDelta, _uint& iStep);

    void Set_Duration();
    _float Calculate_StepDistance(_float4 vPosition1, _float4 vPosition2);

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free() override;
};

NS_END