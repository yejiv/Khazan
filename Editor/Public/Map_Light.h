#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Editor)

class CMap_Light final : public CGameObject
{
public:
    typedef struct tagMapLightDesc
    {
        const LIGHT_DESC* pLightDesc{ nullptr };

    }MAP_LIGHT_DESC;

private:
    enum class SHADER_PASS { MAIN, WIREFRAME, SOLIDFRAME, SHADOW, MAP, MAP_ICE, SNOWMAP, SNOWMAP_ICE, END };

private:
    CMap_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CMap_Light(const CMap_Light& Prototype);
    virtual ~CMap_Light() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CModel* m_pModelCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };

private:
    const LIGHT_DESC* m_pLightDesc = { nullptr };

private:
    HRESULT Ready_Components(void* pArg);
    virtual HRESULT Bind_ShaderResources();

    HRESULT Bind_Materials(_uint iMeshIndex);

public:
    static CMap_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END