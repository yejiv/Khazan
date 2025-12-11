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
	template <typename T>
	static std::vector<std::vector<T>> SplitEvenly(std::vector<T>& items, uint32_t N)
	{
		std::vector<std::vector<T>> chunks;
		chunks.resize(N);

		const size_t total = items.size();
		if (N == 0 || total == 0) return chunks;

		// 몫/나머지로 분할
		const size_t q = total / N;
		size_t r = total % N;

		size_t start = 0;
		for (uint32_t i = 0; i < N; ++i)
		{
			size_t count = q + (r > 0 ? 1 : 0);
			if (r > 0) --r;

			if (count > 0)
			{
				chunks[i].reserve(count);
				for (size_t k = 0; k < count; ++k)
					chunks[i].push_back(items[start + k]);
				start += count;
			}
		}
		return chunks;
	}

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
    HRESULT Add_RenderComponent(class CComponent* pComponent);
	HRESULT Draw();

#ifdef _DEBUG
public:
	void Set_EnableShadow(_bool isEnable) { m_isEnableShadow = isEnable; }
	void Set_EnableSSAO(_bool isEnable) { m_isEnableSSAO = isEnable; }
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
    void Set_SpecularPower(_float2 vPower) { m_vSpecularPower = vPower; }
    void Set_SpecularAttenuation(_float fAttenuation) { m_fSpecularAttenuation = fAttenuation; }
    RIM_LIGHT_DESC Get_RimLightDesc() { return m_RimLightDesc; }
    void Set_RimLightDesc(RIM_LIGHT_DESC Desc) { m_RimLightDesc = Desc; }
    void Set_EnableRimLight(_bool isEnable) { m_isEnableRimLight = isEnable; }

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	list<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];
    list<class CComponent*>		m_RenderComponents;
	vector<class CShader*>		m_pThreadEffect = { nullptr };

private:
	class CShader*				m_pShader = { nullptr };
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };

	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_float						m_fViewportWidth{}, m_fViewportHeight{};
    _float						m_fQuartViewportWidth{}, m_fQuartViewportHeight{};

	// Toon Shade
	_float						m_fToonShadeLevel = { 3.f };

	// Outline
	OUTLINE_CONFIG				m_OutlineConfig = { _float3(0.f, 0.f, 0.f), 0.f, 1.f, 0.01f };

	vector<ID3D11CommandList*>	m_threadCLs;
	mutex						m_Mutex;

    // Specular
    _float2                     m_vSpecularPower = { 32.f, 64.f };
    _float                      m_fSpecularAttenuation = { 1.f };

    // Rim Light
    _bool                       m_isEnableRimLight = { true };
    RIM_LIGHT_DESC              m_RimLightDesc = { 5.f, false, 1.f, 0.3f };

#ifdef _DEBUG
private:
	_bool						m_isEnableDebugRender = {};
	_bool						m_isEnableShadow = { true };
	_bool						m_isEnableSSAO = { true };
	_bool						m_isEnableToonShade = { true };
	_bool						m_isEnableOutline = {};
	_bool						m_isEnableVignette = {};
	_bool						m_isEnableDistortion = {};
#endif

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_Static();
    HRESULT Render_StaticVelocity();
	HRESULT Render_Decal();
	HRESULT Render_Dynamic();
    HRESULT Render_DynamicVelocity();
	HRESULT Render_Outline();
	HRESULT Render_SSAO();
	HRESULT Render_Lights();
	HRESULT Render_PostScene();
	HRESULT Render_NonLight(); 
    HRESULT Render_MotionTrail();
	HRESULT Render_WeightBlend();
	HRESULT Render_Blend();
	HRESULT Render_Fog();
    HRESULT Render_Brightness();
	HRESULT Render_Bloom();
	HRESULT Render_Combined();
    HRESULT Render_RadialBlur();
    HRESULT Render_MotionBlur();
    HRESULT Render_LUT();
	HRESULT Render_Distortion();
	HRESULT Render_UI();

private:
	HRESULT Ready_RenderTargets();
	HRESULT Ready_MRTs();
	HRESULT Ready_Components();
    HRESULT Ready_Matrices();
	HRESULT SetUp_Viewport(_float fWidth, _float fHeight);
    HRESULT Bind_Pipeline_ShaderResources();

	void InitCLSlots(uint32_t N);
	void StoreRecordedCL(uint32_t idx, ID3D11CommandList* pCL);
	ID3D11CommandList* ConsumeRecordedCL(uint32_t idx);

	void Deferred_JobAndImmediate(vector<class CGameObject*>& Deferred, vector<class CGameObject*>& Immediate);

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

private:
	_bool isEnableShadow();
	_bool isEnableSSAO();
	_bool isEnableToonShade();
    _bool isEnableRimLight();

#ifdef _DEBUG
private:
	_bool isEnableDebugRender();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END