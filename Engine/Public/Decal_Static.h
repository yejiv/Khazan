#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDecal_Static final : public CGameObject
{
private:
    CDecal_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CDecal_Static(const CDecal_Static& Prototype);
    virtual ~CDecal_Static() = default;

public:
    virtual HRESULT			Initialize_Prototype();
    virtual HRESULT			Initialize_Clone(void* pArg);
    virtual void			Priority_Update(_float fTimeDelta);
    virtual void			Update(_float fTimeDelta);
    virtual void			Late_Update(_float fTimeDelta);
    virtual HRESULT			Render();

public:
    HRESULT					Bind_ShaderResources(class CShader* pShader, class CTexture** pTexture, class CVIBuffer_Cube* pVIBuffer);
    _bool                   isCameraInDecalBox();   // Culling

public:
    // 맵 데칼 구조체 가져오기
    STATIC_DECAL_DESC			Get_Desc() { return m_Desc; }
    // 맵 데칼 구조체 세팅하기
    void					Set_Desc(STATIC_DECAL_DESC MapDesc);

    _uint					Get_TextureIndex() { return m_Desc.iTextureIndex; }
    void					Set_TextureIndex(_uint iIndex) { m_Desc.iTextureIndex = iIndex; }
    _float                  Get_Threshold() { return m_fThreshold; }
    void                    Set_Threshold(_float fThreshold) { m_fThreshold = fThreshold; }

    // 구조체 중 컬러만 변경하는 함수
    void                    Set_DecalColor(_float3 vColor) { m_Desc.vColor = vColor; }
    // 맵 에디터에서 배치 후 월드 행렬로 클라이언트 맵에 세팅
    void                    Set_WorldMatrix(_float4x4 WorldMatrix) { m_pTransformCom->Set_WorldMatrix_4x4(WorldMatrix); }
    // 맵 에디터에서 데칼 큐브 와이어프레임 위한 함수
    void                    Set_WireFrame(_bool isWireFrame) { m_isWireFrame = isWireFrame; }

private:

    // 패스 1번의 고정 R, G, B 마스크 값
    _float                  m_fThreshold = {};

    // 맵 에디터에서 데칼 큐브 와이어프레임 위한 변수
    _bool                   m_isWireFrame = { false };

    // 맵 데칼용 구조체
    STATIC_DECAL_DESC          m_Desc = {};

public:
    static CDecal_Static* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void			Free() override;
};

NS_END