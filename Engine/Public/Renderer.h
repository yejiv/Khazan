#pragma once

#include "Base.h"

/* 추가하는 기능. */
/* 화면에 그려질 객체들을 그리는 순서대로 분류하여 보관한 클래스 */
/* 보관한 순서대로 객첻르의 렌더콜을 해준다. */

NS_BEGIN(Engine)

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

	// Test
public:
	void Set_ShadowBias(_float fBias) { m_fBias = fBias; }

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);

#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	list<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];

private:
	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

	_float						m_fViewportWidth{}, m_fViewportHeight{};

	// Cascade
	ID3D11DepthStencilView*		m_CascadeShadowDSVs[g_iNumCascades];
	ID3D11ShaderResourceView*	m_pCascadeShadowSRVArray = { nullptr };

	// PCF
	ID3D11SamplerState*			m_pComparisonSampler = { nullptr };
	_float						m_fBias = {};

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugComponent;
	_bool						m_isRenderDebug = {};
	vector<class CGameObject*>	m_CascadeObjects;
#endif
	

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Lights();
	HRESULT Render_Combined();
	HRESULT Render_Blur();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT SetUp_Viewport(_float fWidth, _float fHeight);
	HRESULT Ready_Cascade_Shadow_Resources();
	HRESULT Ready_Comparison_Sampler();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

NS_END