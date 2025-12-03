#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CDecal_Manager final : public CBase
{
private:
	CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDecal_Manager() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);
	HRESULT						Render();
	HRESULT						Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const DECAL_DESC& Desc);

#pragma region 맵 에디터 + 클라이언트에서 사용할 함수 ( 감사합니다 )
public:
    // 맵 에디터에서 이미지 볼때 사용
    class CTexture*             Get_DecalTexture(DECALTYPE eType) { return m_pTexture[ENUM_CLASS(eType)]; }
    // 클론 후 Decal에 푸시만
    void                        Batch_Decal(class CDecal* pDecal) { m_Decals.push_back(pDecal); }
#pragma endregion
    // 셰이더 툴
    ID3D11ShaderResourceView*   Get_DecalTexture(DECALTYPE eType, _uint iIndex);
    _uint                       Get_NumDecalTextures(DECALTYPE eType);

public:
    void Decal_Clear();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	list<class CDecal*>			m_Decals;

	class CShader*				m_pShader = {};
	class CVIBuffer_Cube*		m_pVIBuffer = {};
	class CTexture*				m_pTexture[ENUM_CLASS(DECALTYPE::END)] = { nullptr };

private:
	HRESULT						Ready_Components();

public:
	static CDecal_Manager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END