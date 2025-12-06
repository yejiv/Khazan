#pragma once

#include "Engine_Defines.h"
#include "Prototype_Manager.h"
#include "ComputeShader_Manager.h"
#include "ThreadPool.h"
#include "Event_Manager.h"


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
	void Update_Engine(TIME_DELTA tTimeDelta);
	HRESULT Clear_Resources(_uint iClearLevelID);
public:
	void Render_Begin(const _float4* pClearColor);
	HRESULT Draw();
	void Render_End(HWND hWnd = 0);
public:
	_float Rand_Normal();
	_float Rand(_float fMin, _float fMax);
    _int Rand(_int iMin, _int iMax);

public:
	_uint Get_StaticLevel();

private:
	void	SetupDebugMessageFilter(ID3D11Device* pDevice);
#pragma endregion

#pragma region Graphic_Device
public:
	void Present_SwapChain(_uint iSyncInterval, _uint iFlag);
	
	bool CreateDeferredContexts(uint32_t count);
	ID3D11DeviceContext* GetDeferredContext(uint32_t idx) const;
	_uint	GetDeferredContext_Count();
	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetImmediate() const;

#pragma endregion

#pragma region LEVEL_MANAGER
public:
	HRESULT Open_Level(_uint iLevelID, class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
	_uint Get_NextLevelID();
	void  Set_NextLevelID(_uint iLevelID);
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
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, TIME_CHANNEL eTimeChannel = TIME_CHANNEL::WORLD, void* pArg = nullptr);
	HRESULT Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject, TIME_CHANNEL eTimeChannel = TIME_CHANNEL::WORLD);

#pragma endregion

#pragma region RENDERER
public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
    HRESULT Add_RenderComponent(class CComponent* pComponent);

#ifdef _DEBUG
public:
	void Set_EnableShadow(_bool isEnable);
	void Set_EnableSSAO(_bool isEnable);
	void Set_EnableFog(_bool isEnable);
	void Set_EnableToonShade(_bool isEnable);
	void Set_EnableOutline(_bool isEnable);
    void Set_EnableRimLight(_bool isEnable);
#endif

public:
	void Set_ToonShadeLevel(_float fLevel);
	OUTLINE_CONFIG Get_OutlineConfig();
	void Set_OutlineConfig(OUTLINE_CONFIG Config);
    void Set_SpecularPower(_float2 vPower);
    RIM_LIGHT_DESC Get_RimLightDesc();
    void Set_RimLightDesc(RIM_LIGHT_DESC Desc);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT	Add_Timer(const _wstring& strTimerTag);
	void	Compute_TimeDelta(const _wstring& strTimerTag);

	_float	Get_ScaledDelta(const _wstring& strTimerTag, TIME_CHANNEL cCH);
	void Update_HitStop(_float fUnScaleTimeDelta);
	void Start_HitStop(TIME_CHANNEL tCH, _float fTargetScale, _float fHold, _float fRecover);
	void Fix_HitStop(TIME_CHANNEL eCH);
	void UnFix_HitStop(TIME_CHANNEL eCH);
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

    _matrix             Get_PrevTransform_Matrix(D3DTS eTransformState) const;
    const _float4x4*    Get_PrevTransform_Float4x4(D3DTS eTransformState) const;
    void                Set_PrevTransform(D3DTS eTransformState, _fmatrix Matrix);
    void                Set_PrevTransform(D3DTS eTransformState, const _float4x4& Matrix);
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable = true);
	void Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc);
	void Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition);
	void Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable);
	_bool Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex);
    const vector<_wstring>& Get_LightTags(_uint iLevelIndex);
    void    Start_LightTransition(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_TRANSITION_DESC& Desc, _bool isRestore = false);
    void    Backup_LightDesc(const _wstring& strLightTag, _uint iLevelIndex);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT	Font_Load(const _wstring& strFontTag, const _char* pFontFilePath, _uint iHeight, _uint iWidth = 0 );
	HRESULT	Draw_Text(const _wstring& strFontTag, const _wstring& strText, _float fX = 0.f, _float fY = 0.f, const _float4& vColor = {1.f, 1.f, 1.f, 1.f}, TEXT_ALIGN eAlign = TEXT_ALIGN::LEFT_TOP);
	HRESULT	Draw_TextBox(const _wstring& strFontTag, const _wstring& strText, _float fX = 0.f, _float fY = 0.f, _float fMaxWidth = 1000.f, _float fOffsetHeight = 0.f, const _float4& vColor = { 1.f, 1.f, 1.f, 1.f }, TEXT_ALIGN eAlign = TEXT_ALIGN::LEFT_TOP);
    HRESULT	DrawTextWorld(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign, _matrix WorldMat);

    HRESULT	Font_Load_Data(const _char* pFontFilePath);
#pragma endregion


#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, _bool isClear = true, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(const _wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture);
    HRESULT Copy_RT_Resource(const _wstring& strDestTargetTag, const _wstring& strSourTargetTag);
	void	Backup_RT();
	void	Restore_RT();

	HRESULT Apply_MRT_OnContext(const wstring& mrtTag,
		ID3D11DeviceContext* pCtx,
		ID3D11DepthStencilView* pDSV,
		bool isClear);

	ID3D11DepthStencilView* Get_CurrentDSV_AddRef();

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
	_bool isPicked(_float3* pOutPosition, _float3* pOutNormal);
#pragma endregion

#pragma region FRUSTUM
	void Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix);
	_bool isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	ContainmentType isIn_Frustum_WorldSpace(const BoundingBox& BoundingBox);
	_bool isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	const _float4* Get_Frustum_Point() const;
    void Get_Frustum_WorldPoints(_float4* pOut);
	const _float4* Get_Frustum_WorldPoints() const;
	const _float4* Get_Frustum_WorldPlanes() const;
	const _float4* Get_Frustum_LocalPlanes() const;
#pragma endregion

#pragma region IMGUI_MANGER
#ifdef _DEBUG
	void    AddWidget(const _wstring Menu, const function<void()>& widget);
	HRESULT CleanMenu(_wstring strMenu);

	_bool HandleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void Set_GizmoObject(class CGameObject* pGameObject);
	void Clear_GizmoObject();
    void Imgui_All_Clean();
#endif
#pragma endregion

#pragma region JOLT_MANAGER
	HRESULT Initialize_Jolt(_uint iNumObjectLayer);
	void Destroy_Jolt();
	void Set_PhysicsSystem();
	void Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer);
	void Set_ObjectFilter(_uint iSrc, _uint iDst);
	void Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer);
	void Set_ObjectLayerFilter(_uint iObjectLayer, _bool isOn = true);
	Body* CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);
    Body* CreateAndAdd_SoftBody(const SoftBodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);
	CharacterVirtual* CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData, BodyInterface** pBodyInterface);	

    void Add_Constraint(Constraint* pConstraint);
    void Remove_Constraint(Constraint* pConstraint);

	void CharVir_Update(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter);
	void CharVir_ExtendedUpdate(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter, CharacterVirtual::ExtendedUpdateSettings tSetting);
    void CharVir_RefreshContact(CharacterVirtual* pCharVir, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter);

	CharacterVirtual* Find_CharacterVirtual(CharacterID id);
	void Remove_CharacterVirtual(CharacterID id);

	void Push_BodyDesc(BodyID id, uint64 BodyDesc);
	uint64 Find_BodyDesc(BodyID id);
	void Remove_BodyDesc(BodyID id);

    void Destroy_Body(BodyID& id);

	void Set_Gravity(_vector vGravity);
	void Reset_Gravity();

	_bool RayCast(_float3 vStart, _float3 vEnd, _float& outFraction, _float4& outPosition, _float3* outNormal = nullptr);

    PhysicsSystem* Get_Jolt();
    BodyInterface* Get_BodyInterface();
    const BodyLockInterfaceLocking* Get_BodyLockInterface();
#ifdef _DEBUG
	void Jolt_Test();

	void				Set_DrawFilter(_uint iObjectLayer);
	void				Remove_DrawFilter(_uint iObjectLayer);
#endif
#pragma endregion

#pragma region THREADPOOL
	future<HRESULT> Add_Task(std::function<HRESULT()> task);
	void Add_FireTask(std::function<HRESULT()> task);
	_uint Get_ThreadCount();
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
	INPUT_TYPE* Get_InputTypePtr();
	INPUT_TYPE  Get_InputType();
#pragma endregion

#pragma region POOL_MANAGER
	HRESULT Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strPoolTag, void* pArg, _uint iCount = 1);
	class CGameObject* Pop_PoolObject(_uint iLayerLevelIndex, const _wstring& strPoolTag);
	HRESULT Reset_PoolObject(class CGameObject* pGameObject);
	void Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
#pragma endregion

#pragma region EVENT_MANAGER
	template<typename T>
	_uint Subscribe_Event(_uint iEventType, std::function<void(const T&)> fn) {
		return m_pEvent_Manager ? m_pEvent_Manager->Subscribe<T>(iEventType, std::move(fn)) : 0;
	}

	template<typename T>
	HRESULT Emit_Event(_uint iEventType, const T& payload) {
		return m_pEvent_Manager ? m_pEvent_Manager->Emit<T>(iEventType, payload) : E_FAIL;
	}

	_bool Unsubscribe_Event(_uint iEventType, _uint iListenerId);
	void UnsubscribeAll_Event(_uint iEventType);
	void Clear_AllEvents();

#pragma endregion

#pragma region RESOURCE_MANAGER
	HRESULT Add_Texture(_wstring strTextureTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, _tchar* pTextureFilePath, _uint iNumTexture = 1, void* pArg = nullptr);
	HRESULT Add_Model(_wstring strModelTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, MODELTYPE eModelType, _char* pModelFilePath, _matrix PreTransformMatrix, void* pArg = nullptr);
	class CTexture* Clone_Texture(_wstring strTextureTag);
	class CModel* Clone_Model(_wstring strModelTag);
	class CTexture* Get_Texture(_wstring strTextureTag);
	class CModel* Get_Model(_wstring strModelTag);


    _bool Push_MeshMetrial_SRV(string strFileName, ID3D11ShaderResourceView* pResource);
    _bool Exist_MeshMetrial_SRV_InCache(string strFileName);
    string Convert_FullPath(string strFullPath);
    ID3D11ShaderResourceView* Get_MeshMetrial_SRVFromCache(string strFileName);
#pragma endregion

#pragma region COMPUTESHADER_MANAGER
	void		Add_Job(COMPUTEJOB eJobTag, const CComputeShader_Manager::COMPUTE_JOB_DESC& Desc, _bool isExecuteNow = false);
	void		Execute_Job(COMPUTEJOB eJobTag);
#pragma endregion

#pragma region CAMERA_MANAGER
	//class CBlackBoard* Get_BlackBoard() { return m_pBlackBoard; }
#pragma endregion

#pragma region Octree
	void DeleteOctree();
	HRESULT CreateOctree(const _float3& _vCenter, const _float& fHalfWidth = 256.0f, const _int& _iDepthLimit = 4);
	bool AddStaticObject(class CGameObject* pGameObject, const _float3& vPoint, const _float& fRadius = 0.0f);
#pragma endregion

#pragma region CSM
    _uint				Get_NumCascades();
    void				Set_CurrentCascade(_uint iIndex);
    const _float4x4*    Get_CurrentCascadeLightViewMatrix() const;
    const _float4x4*    Get_CurrentCascadeLightProjMatrix() const;
    HRESULT				Bind_CascadeDSV(_uint iIndex);
    HRESULT				Bind_Cascade_ShaderResources(class CShader* pShader);
    void				Clear_CascadeDSVs();
    void				Start_CascadeShadowTransition(_float fDuration, _float fTargetIntensity);
    CASCADE_CONFIG		Get_CascadeConfig();
    void				Set_CascadeConfig(CASCADE_CONFIG Config);

#ifdef _DEBUG
    HRESULT				Ready_CSM_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT				Render_CSM_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region Shadow
    const _float4x4*    Get_ShadowLightMatrix(D3DTS eTransformState) const;
    HRESULT             Bind_Shadow_ShaderResources(class CShader* pShader);
    void                Bind_ShadowDSV();
    void                Start_ShadowTransition(_float fDuration, _float fTargetIntensity);
    void                Clear_ShadowDSV();

    const SHADOW_DESC&  Get_ShadowDesc() const;
    void                Set_ShadowDesc(const SHADOW_DESC& Desc);
#ifdef _DEBUG
    HRESULT				Ready_Shadow_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
    HRESULT				Render_Shadow_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region SSAO
	SSAO_CONFIG	Get_SSAOConfig();
	void		Set_SSAOConfig(SSAO_CONFIG Config);
	HRESULT		Bind_SSAO_ShaderResources(class CShader* pShader);
#pragma endregion

#pragma region GAUSSIAN_BLUR
	HRESULT						Bind_GaussianBlur_ShaderResources(class CShader* pShader);
	GAUSSIAN_BLUR_CONFIG		Get_GaussianBlurConfig();
	void						Set_GaussianBlurConfig(GAUSSIAN_BLUR_CONFIG Config);
#pragma endregion

#pragma region FOG
	HRESULT						Bind_Fog_ShaderResources(class CShader* pShader);
	FOG_CONFIG					Get_FogConfig();
	void						Set_FogConfig(FOG_CONFIG Config);
	_uint						Get_NumFogNoiseTextures();
	ID3D11ShaderResourceView*	Get_FogNoiseTexture(_uint iTextureIndex);
	void						Set_FogNoiseTextureIndex(_uint iTextureIndex);
	void						Start_FogTransition(_float fDuration, const FOG_TRANSITION_DESC& Desc);
#pragma endregion

#pragma region VIGNETTE
	HRESULT						Bind_Vignette_ShaderResources(class CShader* pShader);
	void						Set_EnableVignette(_bool isEnable);
	VIGNETTE_CONFIG				Get_VignetteConfig();
	void						Set_VignetteConfig(VIGNETTE_CONFIG Config);
	void						Start_VignetteAnimation(_float fDuration, VIGNETTE_CONFIG Config);
#pragma endregion

#pragma region SEQUENCE_MANAGER
	HRESULT SEQ_AdoptAndPlay(class ISeqInstance* pSeq, SEQ_REQ_PLAY_DESC tDesc, _bool isInit = false);
	void    SEQ_EnqueueAdopt(class ISeqInstance* pSeq, const SEQ_REQ_PLAY_DESC& tDesc);

	HRESULT SEQ_Play(const SEQ_REQ_PLAY_DESC& tDecs);
	HRESULT SEQ_Stop(const SEQ_ID& tId, _bool isImmediate);
	HRESULT SEQ_Pause(const SEQ_ID& tId);
	HRESULT SEQ_Resume(const SEQ_ID& tId);
	HRESULT SEQ_Jump(const SEQ_REQ_JUMP_DESC& tDesc);
#pragma endregion

#pragma region DECAL_MANAGER
	HRESULT                     Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const DECAL_DESC& Desc);
	HRESULT                     Render_Decals();
    CTexture*                   Get_DecalTexture(DECALTYPE eType);
    ID3D11ShaderResourceView*   Get_DecalTexture(DECALTYPE eType, _uint iIndex);
    _uint                       Get_NumDecalTextures(DECALTYPE eType);
    void                        Batch_Decal(class CDecal_Static* pDecal);
    void                        Decal_Clear();
    void                        MapDecal_Clear();
    void                        MapDecal_CleanUp();
#pragma endregion

#pragma region EFFECT_MANAGER
	void		Add_Effect_ToPool(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint iPoolSize);
	_uint		Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector SpawnPos);
	_uint		Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector Quaternion, _gvector Position);
	void		Update_Effect_Position(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector SpawnPos);
	void		Update_Effect_World(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector Quaternion, _gvector Position);
	void		Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID);
	void		Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag);
#pragma endregion

#pragma region DISTORTION
	HRESULT						Bind_Distortion_ShaderResources(class CShader* pShader);
	void						Start_Distortion(const DISTORTION_DESC& Desc);
	DISTORTION_DESC				Get_DistortionDesc();
	_uint						Get_NumDistortionNoiseTextures();
	ID3D11ShaderResourceView*	Get_DistortionNoiseTexture(_uint iTextureIndex);
#pragma endregion

#pragma region LUT
    HRESULT                     Bind_LUT_ShaderResources(class CShader* pShader);
    void                        Set_EnableLUT(_bool isEnable);
    void                        Set_LUTIntensity(_float fIntensity);
#pragma endregion

#pragma region RADIAL_BLUR
    HRESULT						Bind_RadialBlur_ShaderResources(class CShader* pShader);
    RADIAL_BLUR_DESC		    Get_RadialBlurDesc();
    void						Set_RadialBlurDesc(const RADIAL_BLUR_DESC& Desc);
    void                        Set_RadialBlurCenter(_fvector vCenter, _float fOffset = 0.f);
    void                        Start_RadialBlur(const RADIAL_BLUR_DESC& Desc);
    void                        Set_EnableRadialBlur(_bool isEnable);
#pragma endregion

#pragma region MOTION_BLUR
    HRESULT						Bind_MotionBlur_ShaderResources(class CShader* pShader);
    MOTION_BLUR_DESC		    Get_MotionBlurDesc();
    void						Set_MotionBlurDesc(const MOTION_BLUR_DESC& Desc);
    void                        Set_EnableMotionBlur(_bool isEnable);
#pragma endregion

#pragma region SOUND_MANAGER
    void                        Set_Gloval_Volume(_float fVolume);
    void                        ADD_Gloval_Volume(_float fVolume);

    void                        PlaySoundOnce(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
    void                        PlaySoundLoop(const TCHAR* pSoundKey, float fVolume = 1.0f, FMOD_CHANNEL** ppOutChannel = nullptr);
    void                        StopAll();

    void                        StopByKey(const TCHAR* pSoundKey);
    void                        StopByChannel(FMOD_CHANNEL** ppOutChannel);
    bool                        IsPlayingByKey(const TCHAR* pSoundKey);
    void                        SetVolumeByKey(const TCHAR* pSoundKey, float fVolume);
#pragma endregion

private:
	class CGraphic_Device*		    m_pGraphic_Device = { nullptr };
	class CLevel_Manager*		    m_pLevel_Manager = { nullptr };
	class CObject_Manager*		    m_pObject_Manager = { nullptr };
	class CPrototype_Manager*	    m_pPrototype_Manager = { nullptr };
	class CRenderer*			    m_pRenderer = { nullptr };
	class CTimer_Manager*		    m_pTimer_Manager = { nullptr };
	class CPicking*				    m_pPicking = { nullptr };
	class CPipeLine*			    m_pPipeLine = { nullptr };
	class CLight_Manager*		    m_pLight_Manager = { nullptr };
	class CFont_Manager*		    m_pFont_Manager = { nullptr };
	class CTarget_Manager*		    m_pTarget_Manager = { nullptr };
	class CFrustum*				    m_pFrustum = { nullptr };
	class CJolt_Manager*		    m_pJolt_Manager = { nullptr };
	CThreadPool*				    m_pThreadPool = { nullptr };
	class CInput_Manager*		    m_pInput_Manager = { nullptr };
	class CPool_Manager*		    m_pPool_Manager = { nullptr };
	class CEvent_Manager*		    m_pEvent_Manager = { nullptr };
	class CResource_Manager*	    m_pResource_Manager = { nullptr };
	class CComputeShader_Manager*   m_pComputeShader_Manager = { nullptr };
	class CBlackBoard*			    m_pBlackBoard = { nullptr };
	class CSequence_Manager*	    m_pSequence_Manager = { nullptr };
	class CDecal_Manager*		    m_pDecal_Manager = { nullptr };
	class CEffect_Manager*		    m_pEffect_Manager = { nullptr };
	class CSound_Manager*           m_pSound_Manager = { nullptr };
	// 임시(이후 렌더링 리소스 클래스 안으로 이전할 예정)
	class CCSM*				        m_pCSM = { nullptr };
    class CShadow*                  m_pShadow = { nullptr };
	class CSSAO*				    m_pSSAO = { nullptr };
	class COctree*				    m_pOctree = { nullptr };
	class CGaussianBlur*		    m_pGaussianBlur = { nullptr };
	class CFog*					    m_pFog = { nullptr };
	class CVignette*			    m_pVignette = { nullptr };
	class CDistortion*			    m_pDistortion = { nullptr };
    class CLUT*                     m_pLUT = { nullptr };
    class CRadialBlur*              m_pRadialBlur = { nullptr };
    class CMotionBlur*              m_pMotionBlur = { nullptr };

#ifdef _DEBUG
	class CImgui_Manager*           m_pImgui_Manager = { nullptr };
#endif


	_uint m_iStaticLevel = {};

public:
	void Release_Engine();
	virtual void Free() override;
};

NS_END
