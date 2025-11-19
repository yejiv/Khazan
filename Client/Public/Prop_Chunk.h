#pragma once
#include "Client_Defines.h"
#include "Prop.h"

NS_BEGIN(Engine)
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CProp_Chunk final : public CProp
{
public:
    typedef struct tagPropChunkDesc : public CProp::PROP_DESC
    {
        _wstring strModelTag;
    }PROP_CHUNK_DESC;
private:
    CProp_Chunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CProp_Chunk(const CProp_Chunk& Prototype);
    virtual ~CProp_Chunk() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual void			Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void			Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

public:
    void Destory(_float3 vVelocity, _float3 vHitPos);

private:
    class CModel* m_pModelCom = { nullptr };
    class CBody* m_pBodyCom = { nullptr };

    _bool m_isDestruct = { false };
    _bool m_isDestroyStart = { false };
    _float m_fLifeTime = {};

    _float3 m_vVelocity = {};
    _float3 m_vHitPos = {};
private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    virtual HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CProp_Chunk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END