#pragma once

#include "Prototype_Manager.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#ifdef new
#pragma pop_macro("new") // DBG_NEW º¹¿ø
#endif


#define IMGUI_DEFINE_MATH_OPERATORS

NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

#pragma region ENGINE
public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Clear_Resources(_uint iClearLevelID);
public:
	void Render_Begin(const _float4* pClearColor);
	HRESULT Draw();
	void Render_End(HWND hWnd = 0);
public:
	_float Rand_Normal();
	_float Rand(_float fMin, _float fMax);

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	class CComponent* Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif
#pragma endregion
//
#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);
#pragma endregion

#pragma region PIPELINE
public:
	_matrix Get_Transform_Matrix(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4(D3DTS eTransformState) const;
	_matrix Get_Transform_Matrix_Inverse(D3DTS eTransformState) const;
	const _float4x4* Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const;
	const _float4* Get_CamPosition() const;
	void Set_Transform(D3DTS eTransformState, _fmatrix Matrix);
	void Set_Transform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion


#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void DrawText(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), const _float2& vScale = _float2(1.f, 1.f));
#pragma endregion


#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture);

#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region PICKING 
	_bool isPicked(_float3* pOut);
#pragma endregion

#pragma region SHADOW
	const _float4x4* Get_ShadowLight_Transform_Float4x4(D3DTS eTransformState) const;
	HRESULT Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc);
#pragma endregion

#pragma region FRUSTUM
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
#pragma endregion

#ifdef _DEBUG
#pragma region IMGUI_MANGER
	void    AddWidget(const _wstring Menu, const function<void()>& widget);
#pragma endregion
#endif

#pragma region JOLT_MANAGER
	void Set_PhysicsSystem();
	void Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer);
	void Set_ObjectFilter(_uint iSrc, _uint iDst);
	void Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer);
	Body* CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);

#ifdef _DEBUG
	void Jolt_Test();
#endif
#pragma endregion

#pragma region THREADPOOL
	future<void> Enqueue(std::function<void()> job);
	future<any> EnqueueAny(std::function<any()> job);
	void Submit(std::function<void()> job);
#pragma

#pragma region INPUT_MANAGER
	_bool		Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);

	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse);
#pragma

#pragma region POOL_MANAGER
	HRESULT Add_PoolObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg, _uint iCount = 1);
	class CPool* Pop_PoolObject(const _wstring& strPoolTag);
	HRESULT Reset_PoolObject(class CPool* pPoolObject);
	HRESULT Reset_PoolObject(class CGameObject* pGameObject);
	void Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CPool* pPoolObject);
#pragma

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CPicking*				m_pPicking = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CTarget_Manager*		m_pTarget_Manager = { nullptr };
	class CShadow*				m_pShadow = { nullptr };
	class CFrustum*				m_pFrustum = { nullptr };
	class CJolt_Manager*		m_pJolt_Manager = { nullptr };
	class CThreadPool*			m_pThreadPool = { nullptr };
	class CInput_Manager*		m_pInput_Manager = { nullptr };
	class CPool_Manager*		m_pPool_Manager = { nullptr };
#ifdef _DEBUG
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
#endif
	

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END