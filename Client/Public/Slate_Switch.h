#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CSlate_Switch final : public CPartObject
{
private:
    enum ANIM_STATE
    {
        DIE,            // 눌림
        IDLE,           // 아이들
        SPAWN,          // 올라옴
        END
    };

public:
    enum class ELEVATOR_TYPE { SMALL, LARGE, END };

    typedef struct tagSlateSwitchDesc : public CPartObject::PARTOBJECT_DESC
    {
        LEVEL eLevel{ LEVEL::END };

        ELEVATOR_TYPE eType{ ELEVATOR_TYPE::END };

        _float4x4* pSocketMatrix = { nullptr };

        _bool* pActiveElevator{ nullptr };          // 엘리베이터 가동중인지 아닌지
        _bool* pAvailableSwitch{ nullptr };         // 스위치의 애니메이션이 끝나서 상호 작용 가능해야 할 때 ( 스위치에서 다룰 것 )
        _bool* pSwitchPressed{ nullptr };           // 스위치가 눌렸는지 안눌렸는지

    }SLATE_SWITCH_DESC;

private:
    CSlate_Switch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSlate_Switch(const CSlate_Switch& Prototype);
    virtual ~CSlate_Switch() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    _bool IsDeActiveState() {
        return ANIM_STATE::IDLE == m_eAnimState;
    }

private:
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };

    ANIM_STATE m_eAnimState = { ANIM_STATE::END };

    ELEVATOR_TYPE m_eElevatorType = { ELEVATOR_TYPE::END };

    _float4x4* m_pSocketMatrix = { nullptr };

    _bool* m_pActiveElevator = { nullptr };
    _bool* m_pAvailableSwitch = { nullptr };
    _bool* m_pSwitchPressed = { nullptr };

    _bool m_isSocket = { false };

private:
    HRESULT Ready_Components(void* pArg);

    HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CSlate_Switch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

NS_END