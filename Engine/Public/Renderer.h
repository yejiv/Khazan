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

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
	void Set_EnableShadow(_bool isEnable) { m_isEnableShadow = isEnable; }
	void Set_EnableSSAO(_bool isEnable) { m_isEnableSSAO = isEnable; }
	void Set_EnableFog(_bool isEnable) { m_isEnableFog = isEnable; }
	void Set_EnableToonShade(_bool isEnable) { m_isEnableToonShade = isEnable; }
	void Set_EnableOutline(_bool isEnable) { m_isEnableOutline = isEnable; }
#endif

public:
	void Set_ToonShadeLevel(_float fLevel) { m_fToonShadeLevel = fLevel; }
	OUTLINE_CONFIG Get_OutlineConfig() { return m_OutlineConfig; }
	void Set_OutlineConfig(OUTLINE_CONFIG Config) 
	{ 
		m_OutlineConfig.fAlpha = Config.fAlpha;
		m_OutlineConfig.fBias = Config.fBias;
	}


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

	// Toon Shade
	_float						m_fToonShadeLevel = { 3.f };

	// Outline
	_float						m_fOutlineAlpha = { 1.f };
	_float						m_fOutlineBias = { 0.01f };

	OUTLINE_CONFIG				m_OutlineConfig = { _float3(0.f, 0.f, 0.f), 0.f, 1.f, 0.01f };

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugComponent;
	_bool						m_isEnableDebugRender = {};
	_bool						m_isEnableShadow = { true };
	_bool						m_isEnableSSAO = { true };
	_bool						m_isEnableFog = {};
	_bool						m_isEnableToonShade = {};
	_bool						m_isEnableOutline = {};
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Outline();
	HRESULT Render_SSAO();
	HRESULT Render_Lights();
	HRESULT Render_PostScene();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_Fog();
	HRESULT Render_Blur();
	HRESULT Render_Combined();
	HRESULT Render_UI();

private:
	HRESULT Ready_RenderTargets();
	HRESULT Ready_MRTs();
	HRESULT Ready_Components();
	HRESULT SetUp_Viewport(_float fWidth, _float fHeight);

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

private:
	_bool isEnableShadow();
	_bool isEnableSSAO();
	_bool isEnableFog();

#ifdef _DEBUG
private:
	_bool isEnableDebugRender();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END