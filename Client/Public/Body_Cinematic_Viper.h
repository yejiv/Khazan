#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CClothBody;
class CBody;
NS_END


NS_BEGIN(Client)

class CBody_Cinematic_Viper final : public CPartObject
{
public:
    typedef struct tagBodyDesc : public PARTOBJECT_DESC
    {
        CTransform* pOwnerTransform = { nullptr };
        class CViper* pOwner = { nullptr };

    }BODY_DESC;

public:
    _float3					Get_BonePoint(const _char* BoneName);
    _float4* Get_BonePointEX(const _char* BoneName);
    _matrix					Get_BoneMatrix(const _char* pBoneName);
    _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName);
    void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }

private:
    CBody_Cinematic_Viper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBody_Cinematic_Viper(const CBody_Cinematic_Viper& Prototype);
    virtual ~CBody_Cinematic_Viper() = default;

public:
    virtual HRESULT			Initialize_Prototype() override;
    virtual HRESULT			Initialize_Clone(void* pArg) override;
    virtual void			Priority_Update(_float fTimeDelta) override;
    virtual void			Update(_float fTimeDelta) override;
    virtual void			Late_Update(_float fTimeDelta) override;
    virtual HRESULT			Render() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


private:
    HRESULT					Ready_Components();
    HRESULT					Bind_ShaderResources();
    void					Carculate_Matrix(_float fTimeDelta);
    void                    Carculate_BakckMatrix(_float fTimeDelta);

private:
    HRESULT					Ready_Colliders();

private:
    class CViper*           m_pOwner = { nullptr };

private:
    CShader*                m_pShaderCom = { nullptr };
    CModel*                 m_pModelCom = { nullptr };
    CTransform*             m_pOwnerTransform = { nullptr };

private:
    _bool					m_isOnAttackCollision = { false };

private:
    class CClothBody* m_pFeelerBody = { nullptr };
    COLLISION_DESC m_tFeelerCollDesc = {};
    class CBody* m_pClothBody = { nullptr };
    COLLISION_DESC m_tClothBodyCollDesc = {};
    _float4x4* m_pClothBodyMatrix = { nullptr };
    _float4x4 m_pClothCombinedMatrix;

public:
    static CBody_Cinematic_Viper*   Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void			        Free() override;
};

NS_END