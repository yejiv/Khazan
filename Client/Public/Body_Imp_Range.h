#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END

NS_BEGIN(Client)

class CBody_Imp_Range final : public CPartObject
{
public:
    typedef struct tagBodyDesc : public PARTOBJECT_DESC
    {
        CTransform*         pOwnerTransform = { nullptr };
        class CImp_Range*   pOwner = { nullptr };

    }BODY_DESC;

public:
    _float4x4*              Get_BoneMatrix_Ptr(const _char* pBoneName);
    _float4*                Get_BonePointEX(const _char* pBoneName);
    _matrix					Get_BoneMatrix(const _char* pBoneName);

    void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }
    
private:
    CBody_Imp_Range(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Imp_Range(const CBody_Imp_Range& Prototype);
    virtual ~CBody_Imp_Range() = default;

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
    HRESULT					Bind_ShaderResources();

private:
    class CImp_Range*        m_pOwner = { nullptr };

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    CTransform*             m_pOwnerTransform = { nullptr };

    _bool					m_isOnAttackCollision = { false };
    _float4x4*              m_pLockOnBoneMatrix = { nullptr };

private:
    _float4					m_vLockOnPoint = {};



public:
    static CBody_Imp_Range* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*    Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END