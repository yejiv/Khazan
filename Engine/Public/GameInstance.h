#pragma once

#include "Prototype_Manager.h"
#include "ComputeShader_Manager.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
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

private:
	void	SetupDebugMessageFilter(ID3D11Device* pDevice);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
#pragma endregion

#pragma region PROTOTYPE_MANAGER
public:
	HRESULT Add_Prototype(_uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	class CBase* Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototpyeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	// 프로토 타입 등록 여부 검사 함수
	_bool Already_Registered_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);
	CBase* Find_Prototype_ForPreview(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag);

#pragma endregion

#pragma region OBJECT_MANAGER
public:
	class CComponent* Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	class CGameObject* Find_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex = 0);
	class CGameObject* Get_BackGameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject);
#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
	void Set_EnableShadow(_bool isEnable);
#endif
#pragma endregion

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

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable = true);
	void Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc);
	void Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition);
	void Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable);
	_bool Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void DrawText(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, const _float2& vOrigin = _float2(0.f, 0.f), const _float2& vScale = _float2(1.f, 1.f));
	_float2 Compute_TextSize(const _wstring& strFontTag, const _wstring& strText, _float2 vTextSize);
#pragma endregion


#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr, _bool isClear = true);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture);
	void	Backup_RT();
	void	Restore_RT();
#ifdef _DEBUG
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RT_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region PICKING 
	_bool isPicked(_float3* pOut);
	// Target_World의 W도 추가로 값 빼오는 용도 ( 맵 오브젝트 ID용으로 사용 )
	_bool isPicked(_float3* pOut, _uint* iObjectID);
	_float4 isPickRenderTargetPixel(_wstring strRenderTargetTag);
#pragma endregion

#pragma region SHADOW
	_uint Get_NumCascades();
	void Set_CurrentCascade(_uint iIndex);
	const _float4x4* Get_CurrentShadowLightViewMatrix() const;
	const _float4x4* Get_CurrentShadowLightProjMatrix() const;
	const _float* Get_CascadeSplits() const;
	void	Set_CascadeSplits(const _float* pSplits);
	const _float4x4* Get_ShadowLightViewMatrices() const;
	const _float4x4* Get_ShadowLightProjMatrices() const;
	HRESULT Bind_ShadowDSV(_uint iIndex);
	HRESULT	Bind_ShadowSRVArray(class CShader* pShader, const _char* pConstantName);
	_float	Get_ShadowBias();
	void	Set_ShadowBias(_float fBias);
	_float  Get_ShadowLamda();
	void	Set_ShadowLamda(_float fLamda);

	void	Clear_ShadowDSVs();

	_float4 Get_ShadowLightDir();
	void Set_ShadowLightDir(const _float4 vLightDir);
#pragma endregion

#pragma region FRUSTUM
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	const _float4* Get_WorldPoints() const;
#pragma endregion

#pragma region IMGUI_MANGER
#ifdef _DEBUG
	void    AddWidget(const _wstring Menu, const function<void()>& widget);
	HRESULT CleanMenu(_wstring strMenu);
#endif
#pragma endregion

#pragma region JOLT_MANAGER
	void Set_PhysicsSystem();
	void Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer);
	void Set_ObjectFilter(_uint iSrc, _uint iDst);
	void Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer);
	Body* CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);
	CharacterVirtual* CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData, BodyInterface** pBodyInterface);

	void CharVir_Update(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter);
	void CharVir_ExtendedUpdate(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter, CharacterVirtual::ExtendedUpdateSettings tSetting);

	void Set_Gravity(_vector vGravity);
	void Reset_Gravity();
#ifdef _DEBUG
	void Change_DebugRender();
	void Jolt_Test();
#endif
#pragma endregion

#pragma region THREADPOOL
	future<void> Enqueue(std::function<void()> job);
	future<any> EnqueueAny(std::function<any()> job);
	void Submit(std::function<void()> job);
#pragma

#pragma region INPUT_MANAGER
	_bool		Key_Pressing(_ubyte byKeyID, _float fTimeDelta, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY, _float* pPressingTime = nullptr);
	_bool		Key_Down(_ubyte byKeyID, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);
	_bool		Key_Up(_ubyte byKeyID, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);

	_bool		Mouse_Pressing(MOUSEKEYSTATE eMouse, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);
	_bool		Mouse_Down(MOUSEKEYSTATE eMouse, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);
	_bool		Mouse_Up(MOUSEKEYSTATE eMouse, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);

	_long		Mouse_Move(MOUSEMOVESTATE eMouseState, INPUT_TYPE eType = INPUT_TYPE::GAMEPLAY);

	void		Change_InputType(INPUT_TYPE eType);
#pragma endregion

#pragma region POOL_MANAGER
	HRESULT Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg = nullptr, _uint iCount = 1);
	class CGameObject* Pop_PoolObject(const _wstring& strPoolTag);
	HRESULT Reset_PoolObject(class CGameObject* pPoolObject);
	void Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pPoolObject);
#pragma endregion

#pragma region EVENT_MANAGER
	_uint Subscribe(_uint iEventType, std::function<void()> fEvent);
	void UnSubscribeAll(_uint iEventType);
	void UnSubscribe(_uint iEventType, _uint iID);
	HRESULT Emit(_uint iEventType);
	void Event_Clear();
#pragma endregion

#pragma region RESOURCE_MANAGER
	HRESULT Add_Texture(_wstring strTextureTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, _tchar* pTextureFilePath, _uint iNumTexture = 1, void* pArg = nullptr);
	HRESULT Add_Model(_wstring strModelTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, MODELTYPE eModelType, _char* pModelFilePath, _matrix PreTransformMatrix, void* pArg = nullptr);
	class CTexture* Clone_Texture(_wstring strTextureTag);
	class CModel* Clone_Model(_wstring strModelTag);
	class CTexture* Get_Texture(_wstring strTextureTag);
	class CModel* Get_Model(_wstring strModelTag);
#pragma endregion

#pragma region COMPUTESHADER_MANAGER
	void		Add_Job(COMPUTEJOB eJobTag, const CComputeShader_Manager::COMPUTE_JOB_DESC& Desc, _bool isExecuteNow = false);
	void		Execute_Job(COMPUTEJOB eJobTag);
#pragma endregion

#pragma region CAMERA_MANAGER
	HRESULT Add_Camera(_uint iLevelIndex, class CCamera* pCamera);
	void Change_Camera(_uint iLevelIndex, _uint iCameraType);
	void Change_Camera(_uint iLevelIndex, _wstring strCameraTag);
	vector<class CCamera*> Get_pCameras(_uint iNumLevel);
	class CCamera* Get_ActiveCamera();

	void Save_Json_Camera(_uint iLevelIndex, _wstring strCameraTag, nlohmann::ordered_json& pOutData);
#pragma endregion

#pragma region CAMERA_MANAGER
	class CBlackBoard* Get_BlackBoard() { return m_pBlackBoard; }
#pragma endregion


private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
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
	class CEvent_Manager*		m_pEvent_Manager = { nullptr };
	class CResource_Manager*	m_pResource_Manager = { nullptr };
	class CComputeShader_Manager*	m_pComputeShader_Manager = { nullptr };
	class CCamera_Manager*		m_pCamera_Manager = { nullptr };
	class CBlackBoard*			m_pBlackBoard = { nullptr };

#ifdef _DEBUG
	class CImgui_Manager* m_pImgui_Manager = { nullptr };
#endif
	

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
