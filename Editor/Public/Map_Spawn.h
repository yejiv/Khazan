#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CMap_Spawn final : public CProp_Interactive
{
public:
    typedef struct tagSpawnDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        string strMonsterKey{};

        _int iSubLevel = {};

    }SPAWN_DESC;

private:
    CMap_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMap_Spawn(const CMap_Spawn& Prototype);
    virtual ~CMap_Spawn() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    string Get_MonsterKey() { return m_strMonsterKey; }
    void Set_MonsterKey(const string strMonsterKey) { m_strMonsterKey = strMonsterKey; }

private:
    _float4 m_vWireColor = { 1.f, 0.f, 0.f, 1.f };

    string m_strMonsterKey = {};

private:
    virtual HRESULT Ready_Components(void* pArg) override;

public:
    static CMap_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END