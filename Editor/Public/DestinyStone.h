#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CDestinyStone final : public CProp_Interactive
{
public:
    typedef struct tagDestinyStoneDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {

    }DESTINYSTONE_DESC;

private:
    CDestinyStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDestinyStone(const CDestinyStone& Prototype);
    virtual ~CDestinyStone() = default;

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
    static CDestinyStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END