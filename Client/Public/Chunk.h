#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CBody;
class CShader;
class CTexture;
NS_END

NS_BEGIN(Client)

class CChunk final : public CGameObject
{
public:
    typedef struct tagChunkDesc : public CGameObject::GAMEOBJECT_DESC
    {
        _wstring strModelTag;
        _vector vPos;
        _float3 vScale;        
    }CHUNK_DESC;
private:
    CChunk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CChunk(const CChunk& Prototype);
    virtual ~CChunk() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void    Priority_Update(_float fTimeDelta) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Pos(_vector vPos);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    void Destory(_float3 vVelocity, _float3 vHitPos, _bool isHit = true, _float fLifeTime = 3.f, _float fImpulse = 10.f, _float fTorque = 0.f);
    void Explode(_float3 vExplosionPos, _float fImpulse, _float fTorque, _float fLifeTime = 10.f);

private:
    CShader* m_pShaderCom = { nullptr };
    class CModel* m_pModelCom = { nullptr };
    class CBody* m_pBodyCom = { nullptr };
    CTexture* m_pDissolveTextureCom = { nullptr };

    _bool m_isDestruct = { false };
    _bool m_isDestroyStart = { false };
    _float m_fLifeTime = {};

    _float3 m_vVelocity = {};
    _float3 m_vHitPos = {};
    _float3 m_vCenterPos = {};
    _float m_fImpulse = {};
    _float m_fRadialImpulse = {};
    _float m_fTangentialImpulse = {};
    _float m_fUpImpulse = {};
    _float m_fTorque = {};

    _float3 m_vExplosionPos = {};
    _bool m_isHit = { false };
    _bool m_isExplode = { false };
    _bool m_isVortexExplode = { false };
    _bool m_isVortex = { false };

    _float m_fTimeAcc = { 0.f };
    _float m_fDecreaseAlpha = { 0.f };

    _float4 m_vEdgeColor;

private:
    HRESULT Ready_Components(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    virtual HRESULT Bind_ShaderResources();
    HRESULT Bind_Materials(_uint iMeshIndex);
    HRESULT Bind_DissolveValues();

public:
    static CChunk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void                Free() override;
};
NS_END