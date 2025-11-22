#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CStatue final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { IDLE_0, ROTATE_180, IDLE_180, ROTATE_270, IDLE_270, ROTATE_360, ROTATE_90, IDLE_90 };

public:
    typedef struct tagStatueRotation
    {
        // 0 : 0도 | 1 : 90도 | 2 : 180도 | 3 : 270도
        _int iUnLockRotation = {};

    }STATUE_ROTATION;

    typedef struct tagStatueDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        STATUE_ROTATION StatueRotation{};

    }STATUE_DESC;

private:
    CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CStatue(const CStatue& Prototype);
    virtual ~CStatue() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    _int Get_StatueRotation() { return m_iRotation; }
    _int Get_StatueUnLockRotation() { return m_iUnLockRotation; }
    void Set_StatueUnLockRotation(_int iUnLockRotation) { m_iUnLockRotation = iUnLockRotation; }

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE_0 };
    _bool m_isActive = { false };

    _float m_fTimeAcc = { 0.f };

    _int m_iUnLockRotation = {};
    _int m_iRotation = {};

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END