#include "GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "Renderer.h"
#include "Timer_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "Picking.h"
#include "Shadow.h"
#include "Frustum.h"
#include "Imgui_Manager.h"
#include "Jolt_Manager.h"
#include "ThreadPool.h"
#include "Input_Manager.h"
#include "Pool_Manager.h"
#include "Event_Manager.h"
#include "Resource_Manager.h"
#include "ComputeShader_Manager.h"
#include "Camera_Manager.h"
#include "BlackBoard.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

#pragma region ENGINE

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Manager = CInput_Manager::Create(EngineDesc.hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pShadow = CShadow::Create(EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pShadow)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pJolt_Manager = CJolt_Manager::Create(*ppDevice, *ppContext, EngineDesc.iNumJoltObjectLayer);
	if (nullptr == m_pJolt_Manager)
		return E_FAIL;

	m_pThreadPool = CThreadPool::Create();
	if (nullptr == m_pThreadPool)
		return E_FAIL;

	m_pPool_Manager = CPool_Manager::Create();
	if (nullptr == m_pPool_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEvent_Manager::Create();
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	m_pResource_Manager = CResource_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pResource_Manager)
		return E_FAIL;

	m_pComputeShader_Manager = CComputeShader_Manager::Create();
	if (nullptr == m_pComputeShader_Manager)
		return E_FAIL;

	m_pCamera_Manager = CCamera_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pCamera_Manager)
		return E_FAIL;

	m_pBlackBoard = CBlackBoard::Create();
	if (nullptr == m_pBlackBoard)
		return E_FAIL;

#ifdef _DEBUG
	m_pImgui_Manager = CImgui_Manager::Create(EngineDesc.iWinSizeX_Imgui, EngineDesc.iWinSizeY_Imgui, EngineDesc.Menu_Imgui);
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;

	SetupDebugMessageFilter(*ppDevice);
#endif
	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	//m_pPicking->Update();

	/* 내 게임내에서 반복적인 갱신이 필요한 객체들이 있다라면 갱신을 여기에서 모아서 수행하낟. */
	m_pObject_Manager->Priority_Update(fTimeDelta);

	//m_pPicking->Update();
	m_pPipeLine->Update();
	m_pFrustum->Update();

	m_pObject_Manager->Update(fTimeDelta);
	m_pObject_Manager->Late_Update(fTimeDelta);

	// Cascade Test
	m_pShadow->Update();

	m_pLevel_Manager->Update(fTimeDelta);

	m_pJolt_Manager->Update(fTimeDelta);

	m_pComputeShader_Manager->Execute_Job(COMPUTEJOB::UPDATE);

	m_pInput_Manager->Update();

#ifdef _DEBUG

#endif
}

HRESULT CGameInstance::Clear_Resources(_uint iClearLevelID)
{
	m_pPool_Manager->Clear();

	/* 기존레벨용 자원들을 날린다. */
	m_pPrototype_Manager->Clear(iClearLevelID);

	m_pObject_Manager->Clear(iClearLevelID);

	m_pLight_Manager->Clear(iClearLevelID);

	m_pCamera_Manager->Clear(iClearLevelID);

	return S_OK;
}

void CGameInstance::Render_Begin(const _float4* pClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Clear_BackBuffer_View(pClearColor);

	m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pRenderer)
		return E_FAIL;

	/* 백버퍼에 그릴것들을 그린다. */
	m_pRenderer->Draw();

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pImgui_Manager->BeginFrame();
	m_pImgui_Manager->Render();
	m_pJolt_Manager->Debug_Render();
#endif

	return S_OK;
}

void CGameInstance::Render_End(HWND hWnd)
{
	if (nullptr == m_pGraphic_Device)
		return;

	m_pGraphic_Device->Present();
}

_float CGameInstance::Rand_Normal()
{
	return static_cast<_float>(rand()) / RAND_MAX;
}

_float CGameInstance::Rand(_float fMin, _float fMax)
{
	return fMin + Rand_Normal() * (fMax - fMin);
}

void CGameInstance::SetupDebugMessageFilter(ID3D11Device* pDevice)
{
	ID3D11InfoQueue* pInfoQueue = nullptr;
	if (SUCCEEDED(pDevice->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&pInfoQueue)))
	{
		// 무시할 메시지 ID 목록
		D3D11_MESSAGE_ID hideMessages[] =
		{
			D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD,
			D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
			D3D11_MESSAGE_ID_OMSETRENDERTARGETS_INVALIDVIEW,
			D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
		};

		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hideMessages);
		filter.DenyList.pIDList = hideMessages;

		pInfoQueue->AddStorageFilterEntries(&filter);
		pInfoQueue->Release();
	}
}

#pragma endregion

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Open_Level(_uint iLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelID, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevelID()
{
	return m_pLevel_Manager->Get_CurrentLevelID();
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Clone_Prototype(ePrototype, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

_bool CGameInstance::Already_Registered_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag)
{
	if (nullptr == m_pPrototype_Manager)
		return false;

	return m_pPrototype_Manager->Already_Registered_Prototype(iPrototypeLevelIndex, strPrototypeTag);
}

#pragma endregion

#pragma region OBJECT_MANAGER

CComponent* CGameInstance::Find_Component(_uint iLayerLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLayerLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Find_GameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	return m_pObject_Manager->Get_GameObject(iLayerLevelIndex, strLayerTag, iIndex);
}

CGameObject* CGameInstance::Get_BackGameObject(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_BackGameObject(iLayerLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return m_pObject_Manager->Push_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, pGameObject);
}

#pragma endregion

#pragma region RENDERER


HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#ifdef _DEBUG

HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}

#endif


#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion

#pragma region TRANSFORM_MANAGER

_matrix CGameInstance::Get_Transform_Matrix(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eTransformState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eTransformState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eTransformState);
}

const _float4x4* CGameInstance::Get_Transform_Float4x4_Inverse(D3DTS eTransformState) const
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eTransformState);
}

const _float4* CGameInstance::Get_CamPosition() const
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(D3DTS eTransformState, _fmatrix Matrix)
{
	m_pPipeLine->Set_Transform(eTransformState, Matrix);
}

void CGameInstance::Set_Transform(D3DTS eTransformState, const _float4x4& Matrix)
{
	m_pPipeLine->Set_Transform(eTransformState, Matrix);
}

#pragma endregion

#pragma region LIGHT_MANAGER

const LIGHT_DESC* CGameInstance::Get_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
	return m_pLight_Manager->Get_LightDesc(strLightTag, iLevelIndex);
}

HRESULT CGameInstance::Add_Light(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc, _bool isEnable)
{
	return m_pLight_Manager->Add_Light(strLightTag, iLevelIndex, LightDesc, isEnable);
}

void CGameInstance::Set_LightDesc(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	m_pLight_Manager->Set_LightDesc(strLightTag, iLevelIndex, LightDesc);
}

void CGameInstance::Set_LightPosition(const _wstring& strLightTag, _uint iLevelIndex, const _float4& vPosition)
{
	m_pLight_Manager->Set_LightPosition(strLightTag, iLevelIndex, vPosition);
}

void CGameInstance::Set_LightEnable(const _wstring& strLightTag, _uint iLevelIndex, _bool isEnable)
{
	m_pLight_Manager->Set_LightEnable(strLightTag, iLevelIndex, isEnable);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer, iLevelIndex);
}

#pragma endregion

#pragma region FONT_MANAGER

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}

void CGameInstance::DrawText(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRadian, const _float2& vOrigin, const _float2& vScale)
{
	m_pFont_Manager->DrawText(strFontTag, pText, vPosition, vColor, fRadian, vOrigin, vScale);
}

_float2 CGameInstance::Compute_TextSize(const _wstring& strFontTag, const _wstring& strText, _float2 vTextSize)
{
	return m_pFont_Manager->Compute_TextSize(strFontTag, strText, vTextSize);
}

#pragma endregion

#pragma region TARGET_MANAGER

HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool isClear)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV, isClear);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(strTargetTag, pShader, pConstantName);
}

HRESULT CGameInstance::Copy_RT_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture)
{
	return m_pTarget_Manager->Copy_Resource(strTargetTag, pSourTexture);
}

#ifdef _DEBUG

HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RT_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render(pShader, pVIBuffer);
}
#endif 

#pragma endregion

#pragma region PICKING

_bool CGameInstance::isPicked(_float3* pOut)
{
	return m_pPicking->isPicked(pOut);
}

_bool CGameInstance::isPicked(_float3* pOut, _uint* iObjectID)
{
	return m_pPicking->isPicked(pOut, iObjectID);
}

_float4 CGameInstance::isPickRenderTargetPixel(_wstring strRenderTargetTag)
{
	return m_pPicking->isPickRenderTargetPixel(strRenderTargetTag);
}

#pragma endregion

#pragma region SHADOW


const _float4x4* CGameInstance::Get_ShadowLight_Transform_Float4x4(D3DTS eTransformState) const
{
	return m_pShadow->Get_Transform_Float4x4(eTransformState);
}

HRESULT CGameInstance::Ready_ShadowLight(SHADOW_LIGHT_DESC LightDesc)
{
	return m_pShadow->Ready_ShadowLight(LightDesc);
}

SHADOW_LIGHT_DESC CGameInstance::Get_ShadowLight()
{
	return m_pShadow->Get_ShadowLight();
}

void CGameInstance::Set_ShadowLight(SHADOW_LIGHT_DESC LightDesc)
{
	m_pShadow->Set_ShadowLight(LightDesc);
}

_uint CGameInstance::Get_NumCascades()
{
	return m_pShadow->Get_NumCascades();
}

void CGameInstance::Set_CurrentCascade(_uint iIndex)
{
	m_pShadow->Set_CurrentCascade(iIndex);
}

const _float4x4* CGameInstance::Get_CurrentLightViewMatrix() const
{
	return m_pShadow->Get_CurrentLightViewMatrix();
}

const _float4x4* CGameInstance::Get_CurrentLightProjMatrix() const
{
	return m_pShadow->Get_CurrentLightProjMatrix();
}

HRESULT CGameInstance::Ready_Cascade()
{
	return m_pShadow->Ready_Cascade();
}

#pragma endregion

#pragma region FRUSTUM
void CGameInstance::Transform_Frustum_ToLocalSpace(_fmatrix WorldMatrix)
{
	m_pFrustum->Transform_ToLocalSpace(WorldMatrix);
}
_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	return m_pFrustum->isIn_LocalSpace(vLocalPos, fRange);
}

const _float4* CGameInstance::Get_WorldPoints() const
{
	return m_pFrustum->Get_WorldPoints();
}

#pragma endregion

#pragma region IMGUI_MANAGER
#ifdef _DEBUG
void CGameInstance::AddWidget(const _wstring Menu, const function<void()>& widget)
{
	m_pImgui_Manager->AddWidget(Menu, widget);
}
HRESULT CGameInstance::CleanMenu(_wstring strMenu)
{
	return m_pImgui_Manager->CleanMenu(strMenu);
}
#endif
#pragma endregion

#pragma region JOLT_MANAGER
void CGameInstance::Set_PhysicsSystem()
{
	m_pJolt_Manager->Set_PhysicsSystem();
}
void CGameInstance::Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer)
{
	m_pJolt_Manager->Set_ObjectToBP(iObjectLayer, iBPLayer);
}
void CGameInstance::Set_ObjectFilter(_uint iSrc, _uint iDst)
{
	m_pJolt_Manager->Set_ObjectFilter(iSrc, iDst);
}
void CGameInstance::Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer)
{
	m_pJolt_Manager->Set_ObjectVsBPFilter(iObjectLayer, iBPLayer);
}
Body* CGameInstance::CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface)
{
	return m_pJolt_Manager->CreateAndAdd_Body(BodySetting, pBodyInterface);
}
CharacterVirtual* CGameInstance::CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData, BodyInterface** pBodyInterface)
{
	return m_pJolt_Manager->CreateCharacterVirtual(inSettings, inPosition, inRotation, inUserData, pBodyInterface);
}

void CGameInstance::CharVir_Update(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter)
{
	m_pJolt_Manager->CharVir_Update(fTimeDelta, pCharVir, vGravity, iObjectLayer, pBodyFilter, pShapeFilter);
}

void CGameInstance::CharVir_ExtendedUpdate(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter, CharacterVirtual::ExtendedUpdateSettings tSetting)
{
	m_pJolt_Manager->CharVir_ExtendedUpdate(fTimeDelta, pCharVir, vGravity, iObjectLayer, pBodyFilter, pShapeFilter, tSetting);
}

void CGameInstance::Set_Gravity(_vector vGravity)
{
	m_pJolt_Manager->Set_Gravity(vGravity);
}

void CGameInstance::Reset_Gravity()
{
	m_pJolt_Manager->Reset_Gravity();
}

#ifdef _DEBUG
void CGameInstance::Jolt_Test()
{
	m_pJolt_Manager->Test();
}
#endif
#pragma endregion

#pragma region THREADPOOL
future<void> CGameInstance::Enqueue(std::function<void()> job)
{
	return m_pThreadPool->Enqueue(job);
}
future<any> CGameInstance::EnqueueAny(std::function<any()> job)
{
	return m_pThreadPool->EnqueueAny(job);
}
void CGameInstance::Submit(std::function<void()> job)
{
	m_pThreadPool->Submit(job);
}
#pragma endregion

#pragma region INPUT_MANAGER
_bool CGameInstance::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, _float* pPressingTime)
{
	return m_pInput_Manager->Key_Pressing(byKeyID, fTimeDelta, pPressingTime);
}
_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	return m_pInput_Manager->Key_Down(byKeyID);
}
_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	return m_pInput_Manager->Key_Up(byKeyID);
}
_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Manager->Mouse_Pressing(eMouse);
}
_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Manager->Mouse_Down(eMouse);
}
_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Manager->Mouse_Up(eMouse);
}
_long CGameInstance::Mouse_Move(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Manager->Mouse_Move(eMouseState);
}
#pragma endregion

#pragma region POOL_MANAGER
HRESULT CGameInstance::Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, const _wstring& strPoolTag, void* pArg, _uint iCount)
{
	return m_pPool_Manager->Add_PoolObject(iPrototypeLevelIndex, strPrototypeTag, strPoolTag, pArg, iCount);
}
CPool* CGameInstance::Pop_PoolObject(const _wstring& strPoolTag)
{
	return m_pPool_Manager->Pop_PoolObject(strPoolTag);
}
HRESULT CGameInstance::Reset_PoolObject(CPool* pPoolObject)
{
	return m_pPool_Manager->Reset_PoolObject(pPoolObject);
}
HRESULT CGameInstance::Reset_PoolObject(CGameObject* pGameObject)
{
	return m_pPool_Manager->Reset_PoolObject(pGameObject);
}
void CGameInstance::Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CPool* pPoolObject)
{
	m_pPool_Manager->Push_PoolObject_ToLayer(iLayerLevelIndex, strLayerTag, pPoolObject);
}
#pragma endregion

#pragma region EVENT_MANAGER
_uint CGameInstance::Subscribe(_uint iEventType, std::function<void()> fEvent)
{
	return m_pEvent_Manager->Subscribe(iEventType, fEvent);
}
void CGameInstance::UnSubscribeAll(_uint iEventType)
{
	m_pEvent_Manager->UnSubscribeAll(iEventType);
}
void CGameInstance::UnSubscribe(_uint iEventType, _uint iID)
{
	m_pEvent_Manager->UnSubscribe(iEventType, iID);
}
HRESULT CGameInstance::Emit(_uint iEventType)
{
	return m_pEvent_Manager->Emit(iEventType);
}
void CGameInstance::Event_Clear()
{
	m_pEvent_Manager->Clear();
}
#pragma endregion

#pragma region RESOURCE_MANAGER
HRESULT CGameInstance::Add_Texture(_wstring strTextureTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, _tchar* pTextureFilePath, _uint iNumTexture, void* pArg)
{
	return m_pResource_Manager->Add_Texture(strTextureTag, iPrototypeLevelIndex, strPrototypeTag, pTextureFilePath, iNumTexture, pArg);
}
HRESULT CGameInstance::Add_Model(_wstring strModelTag, _uint iPrototypeLevelIndex, _wstring strPrototypeTag, MODELTYPE eModelType, _char* pModelFilePath, _matrix PreTransformMatrix, void* pArg)
{
	return m_pResource_Manager->Add_Model(strModelTag, iPrototypeLevelIndex, strPrototypeTag, eModelType, pModelFilePath, PreTransformMatrix, pArg);
}
CTexture* CGameInstance::Clone_Texture(_wstring strTextureTag)
{
	return m_pResource_Manager->Clone_Texture(strTextureTag);
}
CModel* CGameInstance::Clone_Model(_wstring strModelTag)
{
	return m_pResource_Manager->Clone_Model(strModelTag);
}
CTexture* CGameInstance::Get_Texture(_wstring strTextureTag)
{
	return m_pResource_Manager->Get_Texture(strTextureTag);
}
CModel* CGameInstance::Get_Model(_wstring strModelTag)
{
	return m_pResource_Manager->Get_Model(strModelTag);
}
#pragma endregion

#pragma region COMPUTESHADER_MANAGER
void CGameInstance::Add_Job(COMPUTEJOB eJobTag, const CComputeShader_Manager::COMPUTE_JOB_DESC& Desc, _bool isExecuteNow)
{
	m_pComputeShader_Manager->Add_Job(eJobTag, Desc, isExecuteNow);
}
void CGameInstance::Execute_Job(COMPUTEJOB eJobTag)
{
	m_pComputeShader_Manager->Execute_Job(eJobTag);
}
#pragma endregion

#pragma region CAMERA_MANAGER
HRESULT CGameInstance::Add_Camera(_uint iLevelIndex, CCamera* pCamera)
{
	return m_pCamera_Manager->Add_Camera(iLevelIndex, pCamera);
}
void CGameInstance::Change_Camera(_uint iLevelIndex, _uint iCameraType)
{
	m_pCamera_Manager->Change_Camera(iLevelIndex, iCameraType);
}
void CGameInstance::Change_Camera(_uint iLevelIndex, _wstring strCameraTag)
{
	m_pCamera_Manager->Change_Camera(iLevelIndex, strCameraTag);
}
vector<class CCamera*> CGameInstance::Get_pCameras(_uint iNumLevel)
{
	return m_pCamera_Manager->Get_pCameras(iNumLevel);
}
CCamera* CGameInstance::Get_ActiveCamera()
{
	return m_pCamera_Manager->Get_ActiveCamera();
}
#pragma endregion

//
//void CGameInstance::Transform_Picking_ToLocalSpace(CTransform* pTransformCom)
//{
//	m_pPicking->Transform_ToLocalSpace(pTransformCom);
//}
//
//_bool CGameInstance::isPicked_InLocalSpace(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* pOut)
//{
//	return m_pPicking->isPicked_InLocalSpace(vPointA, vPointB, vPointC, pOut);
//}

void CGameInstance::Release_Engine()
{
	Release();

#ifdef _DEBUG
	m_pImgui_Manager->Shutdown();
	Safe_Release(m_pImgui_Manager);
#endif

	Safe_Release(m_pComputeShader_Manager);
	Safe_Release(m_pPool_Manager);
	Safe_Release(m_pThreadPool);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pShadow);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pResource_Manager);
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pBlackBoard);

	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pPrototype_Manager);
	Safe_Release(m_pJolt_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Free()
{
	__super::Free();


}
