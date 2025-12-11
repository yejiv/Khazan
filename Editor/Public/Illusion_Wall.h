#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CIllusion_Wall final : public CProp_Interactive
{
public:
    typedef struct tagIllusionWallDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }ILLUSION_WALL_DESC;

private:
    CIllusion_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIllusion_Wall(const CIllusion_Wall& Prototype);
    virtual ~CIllusion_Wall() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

public:
    static CIllusion_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END