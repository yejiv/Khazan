#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDecal final : public CGameObject
{
private:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& Prototype);
	virtual ~CDecal() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize_Clone(void* pArg);
	virtual void			Priority_Update(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			Late_Update(_float fTimeDelta);
	virtual HRESULT			Render();
	virtual void			Reset() override;

public:
	HRESULT					Bind_ShaderResources(class CShader* pShader, class CTexture** pTexture, class CVIBuffer_Cube* pVIBuffer);

    // Test
    _bool                   isCameraInDecalBox();

public:
	_float					Get_Opacity() { return m_fOpacity; }
	
	DECAL_DESC				Get_Desc() { return m_Desc; }
	void					Set_Desc(DECAL_DESC Desc);
	
	_uint					Get_TextureIndex() { return m_iTextureIndex; }
	void					Set_TextureIndex(_uint iIndex) { m_iTextureIndex = iIndex; }

	_uint					Get_RandomSeed() { return m_iRandSeed; }
	void					Set_RandomSeed(_uint iSeed) { m_iRandSeed = iSeed; }

    _float                  Get_Threshold() { return m_fThreshold; }
    void                    Set_Threshold(_float fThreshold) { m_fThreshold = fThreshold; }

    // 데코레이션용 데칼
    void                    Set_EnableDecoration(_bool isEnable) { m_isDecoration = isEnable; }

    // 구조체 중 컬러만 변경하는 함수
    void                    Set_DecalColor(_float3 vColor) { m_Desc.vColor = vColor; }
    // 맵 에디터에서 배치 후 월드 행렬로 클라이언트 맵에 세팅
    void                    Set_WorldMatrix(_float4x4 WorldMatrix) { m_pTransformCom->Set_WorldMatrix_4x4(WorldMatrix); }
    // 맵 에디터에서 데칼 큐브 와이어프레임 위한 함수
    void                    Set_WireFrame(_bool isWireFrame) { m_isWireFrame = isWireFrame; }

private:
	_float					m_fTimeAcc = {};
	_float					m_fOpacity = {};
	_uint					m_iTextureIndex = {};
	_uint					m_iRandSeed = {};

    // 패스 1번의 고정 R, G, B 마스크 값
    _float                  m_fThreshold = {};

    // 맵 에디터에서 데칼 큐브 와이어프레임 위한 변수
    _bool                   m_isWireFrame = { false };

    _bool                   m_isDecoration = {};

	DECAL_DESC				m_Desc = {};

public:
	static CDecal*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END