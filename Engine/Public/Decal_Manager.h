#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CDecal_Manager final : public CBase
{
private:
	CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDecal_Manager() = default;

public:
	HRESULT						Initialize(_uint iNumDecals);
	void						Update(_float fTimeDelta);
	HRESULT						Render();
	// 풀 태그, 레벨 인덱스, 레이어 태그, 포지션, 노말, 스케일
	HRESULT						Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, 
									const _float3& vPosition, const _float3& vScale);

	// 에디터용
	_float3						Get_DecalColor() { return m_vDecalColor; }
	void						Set_DecalColor(_float3 vColor) { m_vDecalColor = vColor; }
	_uint						Get_NumDecalTextures();
	ID3D11ShaderResourceView*	Get_DecalTexture(_uint iTextureIndex);
	void						Set_DecalTextureIndex(_uint iTextureIndex) { m_iTextureIndex = iTextureIndex; }

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	list<class CDecal*>			m_Decals;
	_uint						m_iMaxDecals = {};
	_uint						m_iNumActiveDecals = {};

	class CShader*				m_pShader = {};
	class CVIBuffer_Cube*		m_pVIBuffer = {};
	class CTexture*				m_pTexture = {};

	_float3						m_vDecalColor = {};
	_uint						m_iTextureIndex = {};

private:
	ID3D11Buffer*				m_pStructuredBuffer = { nullptr };
	ID3D11ShaderResourceView*	m_pDecalSRV = { nullptr };

private:
	HRESULT						Ready_Components();
	HRESULT						Ready_DecalSRV();

public:
	static CDecal_Manager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumDecals);
	virtual void				Free() override;
};

NS_END