#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CImp_Sword final : public CPartObject
{
public:
    typedef struct tagWeaponDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        class CImp_Melee* pOwner = { nullptr };
        _float4x4* pSocketMatrix = { nullptr };
        _bool* pDissolve = { nullptr };
        _float* pDecreaseAlpha;
    }WEAPON_DESC;

public:
    _float4*                Get_BonePointEX(const _char* pBoneName);
    _matrix					Get_BoneMatrix(const _char* pBoneName);
    _float4x4               Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }
    void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }

private:
    CImp_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CImp_Sword(const CImp_Sword& Prototype);
    virtual ~CImp_Sword() = default;

public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


private:
    HRESULT					Ready_Components();
    HRESULT                 Ready_Collision();
    HRESULT					Bind_ShaderResources();
    HRESULT                 Bind_Dissolve();

private:
    class CImp_Melee*       m_pOwner = { nullptr };

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    CTransform*             m_pOwnerTransform = { nullptr };
    class CTexture* m_pDissolveCom = { nullptr };

    _bool					m_isOnAttackCollision = { false };
    CBody*                  m_pBodyComp = { nullptr };
    _float4x4*              m_pSocketMatrix = { nullptr };

private:
    _float4					m_vLockOnPoint = {};
    _bool* m_pDissolve = { nullptr };
    _float* m_pDecreaseAlpha = { nullptr };

public:
    static CImp_Sword*      Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END