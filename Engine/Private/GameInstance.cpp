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
#include "CSM.h"
#include "Shadow.h"
#include "Frustum.h"
#include "Imgui_Manager.h"
#include "Jolt_Manager.h"
#include "Input_Manager.h"
#include "Pool_Manager.h"
#include "Resource_Manager.h"
#include "ComputeShader_Manager.h"
#include "BlackBoard.h"
#include "SSAO.h"
#include "Octree.h"
#include "GaussianBlur.h"
#include "Fog.h"
#include "Vignette.h"
#include "Sequence_Manager.h"
#include "Sequence_Interface.h"
#include "Decal_Manager.h"
#include "Effect_Manager.h"
#include "Distortion.h"
#include "LUT.h"
#include "RadialBlur.h"
#include "MotionBlur.h"
#include "Sound_Manager.h"


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

	m_pThreadPool = CThreadPool::Create();
	if (nullptr == m_pThreadPool)
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

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPool_Manager = CPool_Manager::Create(EngineDesc.iNumLevels);
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

	m_pCSM = CCSM::Create(*ppDevice, *ppContext);
	if (nullptr == m_pCSM)
		return E_FAIL;

    m_pShadow = CShadow::Create(*ppDevice, *ppContext);
    if (nullptr == m_pShadow)
        return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pBlackBoard = CBlackBoard::Create();
	if (nullptr == m_pBlackBoard)
		return E_FAIL;

#pragma region RENDERING_RESOURCES
	m_pSSAO = CSSAO::Create(*ppDevice, *ppContext);
	if (nullptr == m_pSSAO)
		return E_FAIL;

	m_pGaussianBlur = CGaussianBlur::Create(*ppDevice, *ppContext);
	if (nullptr == m_pGaussianBlur)
		return E_FAIL;

	m_pFog = CFog::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFog)
		return E_FAIL;

	m_pDistortion = CDistortion::Create(*ppDevice, *ppContext);
	if (nullptr == m_pDistortion)
		return E_FAIL;

	m_pVignette = CVignette::Create();
	if (nullptr == m_pVignette)
		return E_FAIL;

    m_pLUT = CLUT::Create(*ppDevice, *ppContext);
    if (nullptr == m_pLUT)
        return E_FAIL;

    m_pRadialBlur = CRadialBlur::Create();
    if (nullptr == m_pRadialBlur)
        return E_FAIL;

    m_pMotionBlur = CMotionBlur::Create();
    if (nullptr == m_pMotionBlur)
        return E_FAIL;
#pragma endregion

	m_pSequence_Manager = CSequence_Manager::Create();
	if (nullptr == m_pSequence_Manager)
		return E_FAIL;

	m_pDecal_Manager = CDecal_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pDecal_Manager)
		return E_FAIL;

	m_pEffect_Manager = CEffect_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

    m_pSound_Manager = CSound_Manager::Create();
    if (nullptr == m_pSound_Manager)
        return E_FAIL;

	m_iStaticLevel = EngineDesc.iStaticLevel;

#ifdef _DEBUG
	m_pImgui_Manager = CImgui_Manager::Create(*ppDevice, *ppContext, EngineDesc.Menu_Imgui, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pImgui_Manager)
		return E_FAIL;

	SetupDebugMessageFilter(*ppDevice);
#endif
	return S_OK;
}

void CGameInstance::Update_Engine(TIME_DELTA tTimeDelta)
{
	//m_pPicking->Update();
    if (tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)] > 1.f)
        return; 

	m_pTimer_Manager->Update_HitStop(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pInput_Manager->Update();

	/* 내 게임내에서 반복적인 갱신이 필요한 객체들이 있다라면 갱신을 여기에서 모아서 수행하낟. */
	m_pObject_Manager->Priority_Update(tTimeDelta);
	m_pEffect_Manager->Priority_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::EFFECT)]);

	m_pPipeLine->Update();
	m_pFrustum->Update();

	if (m_pOctree)
		m_pOctree->Culling(m_pFrustum);

	if (m_pOctree)
		m_pOctree->Priority_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pObject_Manager->Update(tTimeDelta);
	m_pEffect_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::EFFECT)]);

	m_pSequence_Manager->ProcessRequests();
	m_pSequence_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	if (m_pOctree)
		m_pOctree->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pDecal_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

    if (m_pJolt_Manager)
        m_pJolt_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pObject_Manager->Late_Update(tTimeDelta);
	m_pEffect_Manager->Late_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::EFFECT)]);

	if (m_pOctree)
		m_pOctree->Late_Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pDecal_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	// Renderer Resources
	m_pCSM->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
    m_pShadow->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
	m_pFog->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
	m_pVignette->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
	m_pDistortion->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);
    m_pRadialBlur->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

    m_pLight_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pLevel_Manager->Update(tTimeDelta.TimeDeltas[ENUM_CLASS(TIME_CHANNEL::WORLD)]);

	m_pComputeShader_Manager->Execute_Job(COMPUTEJOB::UPDATE);

#ifdef _DEBUG

#endif
}

HRESULT CGameInstance::Clear_Resources(_uint iClearLevelID)
{
#ifdef _DEBUG
	//m_pImgui_Manager->ClearGizmoObject();
#endif
	/* 기존레벨용 자원들을 날린다. */
	m_pPrototype_Manager->Clear(iClearLevelID);

	m_pObject_Manager->Clear(iClearLevelID);

	m_pObject_Manager->Static_Clear();

    m_pPool_Manager->Clear(iClearLevelID);

	m_pLight_Manager->Clear(iClearLevelID);

	m_pEffect_Manager->Clear(iClearLevelID);

    m_pSequence_Manager->Clear();

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
	m_pJolt_Manager->RayCast_Render_Clear();
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

_int CGameInstance::Rand(_int iMin, _int iMax)
{
    static std::mt19937 engine{ std::random_device{}() };
    std::uniform_int_distribution<_int> dist(iMin, iMax);
    return dist(engine);
}

_uint CGameInstance::Get_StaticLevel()
{
	return m_iStaticLevel;
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

void CGameInstance::Present_SwapChain(_uint iSyncInterval, _uint iFlag)
{
	m_pGraphic_Device->Present_SwapChain(iSyncInterval, iFlag);
}

bool CGameInstance::CreateDeferredContexts(uint32_t count)
{
	return m_pGraphic_Device->CreateDeferredContexts(count);
}

ID3D11DeviceContext* CGameInstance::GetDeferredContext(uint32_t idx) const
{
	return m_pGraphic_Device->GetDeferredContext(idx);
}

_uint CGameInstance::GetDeferredContext_Count()
{
	return m_pGraphic_Device->GetDeferredContext_Count();
}

ID3D11Device* CGameInstance::GetDevice() const
{
	return m_pGraphic_Device->GetDevice();
}

ID3D11DeviceContext* CGameInstance::GetImmediate() const
{
	return m_pGraphic_Device->GetImmediate();
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

_uint CGameInstance::Get_NextLevelID()
{
    return m_pLevel_Manager->Get_NextLevelID();
}

void CGameInstance::Set_NextLevelID(_uint iLevelID)
{
    m_pLevel_Manager->Set_NextLevelID(iLevelID);
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

CBase* CGameInstance::Find_Prototype_ForPreview(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag)
{
	if (nullptr == m_pPrototype_Manager)
		return nullptr;

	return m_pPrototype_Manager->Find_Prototype_ForPreview(iPrototypeLevelIndex, strPrototypeTag);
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

HRESULT CGameInstance::Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, TIME_CHANNEL eTimeChannel, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, eTimeChannel, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Push_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject, TIME_CHANNEL eTimeChannel)
{
	return m_pObject_Manager->Push_GameObject_ToLayer(iLayerLevelIndex, strLayerTag, eTimeChannel, pGameObject);
}



#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Add_RenderComponent(CComponent* pComponent)
{
    return m_pRenderer->Add_RenderComponent(pComponent);
}

#ifdef _DEBUG

void CGameInstance::Set_EnableShadow(_bool isEnable)
{
	m_pRenderer->Set_EnableShadow(isEnable);
}

void CGameInstance::Set_EnableSSAO(_bool isEnable)
{
	m_pRenderer->Set_EnableSSAO(isEnable);
}

void CGameInstance::Set_EnableToonShade(_bool isEnable)
{
	m_pRenderer->Set_EnableToonShade(isEnable);
}

void CGameInstance::Set_EnableOutline(_bool isEnable)
{
	m_pRenderer->Set_EnableOutline(isEnable);
}

void CGameInstance::Set_EnableRimLight(_bool isEnable)
{
    m_pRenderer->Set_EnableRimLight(isEnable);
}

void CGameInstance::Set_EnableFog(_bool isEnable)
{
	m_pRenderer->Set_EnableFog(isEnable);
}
#endif

void CGameInstance::Set_ToonShadeLevel(_float fLevel)
{
	m_pRenderer->Set_ToonShadeLevel(fLevel);
}

OUTLINE_CONFIG CGameInstance::Get_OutlineConfig()
{
	return m_pRenderer->Get_OutlineConfig();
}

void CGameInstance::Set_OutlineConfig(OUTLINE_CONFIG Config)
{
	m_pRenderer->Set_OutlineConfig(Config);
}

void CGameInstance::Set_SpecularPower(_float2 vPower)
{
    m_pRenderer->Set_SpecularPower(vPower);
}

RIM_LIGHT_DESC CGameInstance::Get_RimLightDesc()
{
    return m_pRenderer->Get_RimLightDesc();
}

void CGameInstance::Set_RimLightDesc(RIM_LIGHT_DESC Desc)
{
    m_pRenderer->Set_RimLightDesc(Desc);
}

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

_float CGameInstance::Get_ScaledDelta(const _wstring& strTimerTag, TIME_CHANNEL eCH)
{
	return m_pTimer_Manager->Get_ScaledDelta(strTimerTag, eCH);
}

void CGameInstance::Update_HitStop(_float fUnScaleTimeDelta)
{
	m_pTimer_Manager->Update_HitStop(fUnScaleTimeDelta);
}

void CGameInstance::Start_HitStop(TIME_CHANNEL tCH, _float fTargetScale, _float fHold, _float fRecover)
{
	m_pTimer_Manager->Start_HitStop(tCH, fTargetScale, fHold, fRecover);
}

void CGameInstance::Fix_HitStop(TIME_CHANNEL eCH)
{
	m_pTimer_Manager->Fix_HitStop(eCH);
}

void CGameInstance::UnFix_HitStop(TIME_CHANNEL eCH)
{
	m_pTimer_Manager->UnFix_HitStop(eCH);
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

_matrix CGameInstance::Get_PrevTransform_Matrix(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_PrevTransform_Matrix(eTransformState);
}

const _float4x4* CGameInstance::Get_PrevTransform_Float4x4(D3DTS eTransformState) const
{
    return m_pPipeLine->Get_PrevTransform_Float4x4(eTransformState);
}

void CGameInstance::Set_PrevTransform(D3DTS eTransformState, _fmatrix Matrix)
{
    m_pPipeLine->Set_PrevTransform(eTransformState, Matrix);
}

void CGameInstance::Set_PrevTransform(D3DTS eTransformState, const _float4x4& Matrix)
{
    m_pPipeLine->Set_PrevTransform(eTransformState, Matrix);
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

_bool CGameInstance::Is_LightEnable(const _wstring& strLightTag, _uint iLevelIndex)
{
	return m_pLight_Manager->Is_LightEnable(strLightTag, iLevelIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iLevelIndex)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer, iLevelIndex);
}

const vector<_wstring>& CGameInstance::Get_LightTags(_uint iLevelIndex)
{
    return m_pLight_Manager->Get_LightTags(iLevelIndex);
}

void CGameInstance::Start_LightTransition(const _wstring& strLightTag, _uint iLevelIndex, const LIGHT_TRANSITION_DESC& Desc, _bool isRestore)
{
    m_pLight_Manager->Start_LightTransition(strLightTag, iLevelIndex, Desc, isRestore);
}

void CGameInstance::Backup_LightDesc(const _wstring& strLightTag, _uint iLevelIndex)
{
    m_pLight_Manager->Backup_LightDesc(strLightTag, iLevelIndex);
}

#pragma endregion

#pragma region FONT_MANAGER

HRESULT CGameInstance::Font_Load(const _wstring& strFontTag, const _char* pFontFilePath, _uint iHeight, _uint iWidth)
{
	return m_pFont_Manager->Font_Load(strFontTag, pFontFilePath, iHeight, iWidth);
}

HRESULT CGameInstance::Draw_Text(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign)
{
	return m_pFont_Manager->Draw_Text(strFontTag, strText, fX, fY, vColor, eAlign);
}

HRESULT CGameInstance::Draw_TextBox(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, _float fMaxWidth, _float fOffsetHeight, const _float4& vColor, TEXT_ALIGN eAlign)
{
	return m_pFont_Manager->Draw_TextBox(strFontTag, strText, fX, fY, fMaxWidth, fOffsetHeight, vColor, eAlign);
}

HRESULT CGameInstance::DrawTextWorld(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign, _matrix WorldMat)
{
    return m_pFont_Manager->DrawTextWorld(strFontTag, strText, fX, fY, vColor, eAlign, WorldMat);
}

HRESULT CGameInstance::Font_Load_Data(const _char* pFontFilePath)
{
	return m_pFont_Manager->Font_Load_Data(pFontFilePath);
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

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, isClear, pDSV);
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

HRESULT CGameInstance::Copy_RT_Resource(const _wstring& strDestTargetTag, const _wstring& strSourTargetTag)
{
	return m_pTarget_Manager->Copy_Resource(strDestTargetTag, strSourTargetTag);
}

void CGameInstance::Backup_RT()
{
	m_pTarget_Manager->Backup_RT();
}

void CGameInstance::Restore_RT()
{
	m_pTarget_Manager->Restore_RT();
}

HRESULT CGameInstance::Apply_MRT_OnContext(const wstring& mrtTag, ID3D11DeviceContext* pCtx, ID3D11DepthStencilView* pDSV, bool isClear)
{
	return m_pTarget_Manager->Apply_MRT_OnContext(mrtTag, pCtx, pDSV, isClear);
}

ID3D11DepthStencilView* CGameInstance::Get_CurrentDSV_AddRef()
{
	return m_pTarget_Manager->Get_CurrentDSV_AddRef();
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

_bool CGameInstance::isPicked(_float3* pOutPosition, _float3* pOutNormal)
{
	return m_pPicking->isPicked(pOutPosition, pOutNormal);
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

ContainmentType CGameInstance::isIn_Frustum_WorldSpace(const BoundingBox& BoundingBox)
{
	if (!m_pFrustum)
		return CONTAINS;

	return m_pFrustum->isIn_WorldSpace(BoundingBox);
}

_bool CGameInstance::isIn_Frustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	return m_pFrustum->isIn_LocalSpace(vLocalPos, fRange);
}

const _float4* CGameInstance::Get_Frustum_Point() const
{
	return m_pFrustum->Get_Point();
}

void CGameInstance::Get_Frustum_WorldPoints(_float4* pOut)
{
    m_pFrustum->Get_WorldPoints(pOut);
}

const _float4* CGameInstance::Get_Frustum_WorldPoints() const
{
	return m_pFrustum->Get_WorldPoints();
}

const _float4* CGameInstance::Get_Frustum_WorldPlanes() const
{
	return m_pFrustum->Get_WorldPlanes();
}

const _float4* CGameInstance::Get_Frustum_LocalPlanes() const
{
	return m_pFrustum->Get_LocalPlanes();
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
_bool CGameInstance::HandleWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return m_pImgui_Manager->HandleWndProc(hWnd, msg, wParam, lParam);
}
void CGameInstance::Set_GizmoObject(CGameObject* pGameObject)
{
	m_pImgui_Manager->Set_GizmoObject(pGameObject);
}
void CGameInstance::Clear_GizmoObject()
{
	m_pImgui_Manager->Clear_GizmoObject();
}
#endif
#pragma endregion

#pragma region JOLT_MANAGER
HRESULT CGameInstance::Initialize_Jolt(_uint iNumObjectLayer)
{
	m_pJolt_Manager = CJolt_Manager::Create(GetDevice(), GetImmediate(), iNumObjectLayer);
	if (nullptr == m_pJolt_Manager)
		return E_FAIL;

	return S_OK;
}
void CGameInstance::Destroy_Jolt()
{
	Safe_Release(m_pJolt_Manager);
}
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
void CGameInstance::Set_ObjectLayerFilter(_uint iObjectLayer, _bool isOn)
{
	m_pJolt_Manager->Set_ObjectLayerFilter(iObjectLayer, isOn);
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

CharacterVirtual* CGameInstance::Find_CharacterVirtual(CharacterID id)
{
	return m_pJolt_Manager->Find_CharacterVirtual(id);
}

void CGameInstance::Remove_CharacterVirtual(CharacterID id)
{
	m_pJolt_Manager->Remove_CharacterVirtual(id);
}

void CGameInstance::Push_BodyDesc(BodyID id, uint64 pBodyDesc)
{
	return m_pJolt_Manager->Push_BodyDesc(id, pBodyDesc);
}

uint64 CGameInstance::Find_BodyDesc(BodyID id)
{
	return m_pJolt_Manager->Find_BodyDesc(id);
}

void CGameInstance::Remove_BodyDesc(BodyID id)
{
	m_pJolt_Manager->Remove_BodyDesc(id);
}

void CGameInstance::Destroy_Body(BodyID& id)
{
    m_pJolt_Manager->Destroy_Body(id);
}

void CGameInstance::Set_Gravity(_vector vGravity)
{
	m_pJolt_Manager->Set_Gravity(vGravity);
}

void CGameInstance::Reset_Gravity()
{
	m_pJolt_Manager->Reset_Gravity();
}

_bool CGameInstance::RayCast(_float3 vStart, _float3 vEnd, _float& outFraction, _float4& outPosition, _float3* outNormal)
{
	return m_pJolt_Manager->RayCast(vStart, vEnd, outFraction, outPosition, outNormal);
}

#ifdef _DEBUG
void CGameInstance::Jolt_Test()
{
	m_pJolt_Manager->Test();
}
void CGameInstance::Set_DrawFilter(_uint iObjectLayer)
{
	m_pJolt_Manager->Set_DrawFilter(iObjectLayer);
}
void CGameInstance::Remove_DrawFilter(_uint iObjectLayer)
{
	m_pJolt_Manager->Remove_DrawFilter(iObjectLayer);
}
#endif
#pragma endregion

#pragma region THREADPOOL
future<HRESULT> CGameInstance::Add_Task(std::function<HRESULT()> task)
{
	return m_pThreadPool->Add_Task(task);
}
void CGameInstance::Add_FireTask(std::function<HRESULT()> task)
{
	m_pThreadPool->Add_FireTask(task);
}
_uint CGameInstance::Get_ThreadCount()
{
	return m_pThreadPool->Get_ThreadCount();
}
#pragma endregion

#pragma region INPUT_MANAGER

_bool CGameInstance::Key_Pressing(_ubyte byKeyID, _float fTimeDelta, INPUT_TYPE eType, _float* pPressingTime)
{
	return m_pInput_Manager->Key_Pressing(byKeyID, fTimeDelta, eType, pPressingTime);
}
_bool CGameInstance::Key_Down(_ubyte byKeyID, INPUT_TYPE eType)
{
	return m_pInput_Manager->Key_Down(byKeyID, eType);
}
_bool CGameInstance::Key_Up(_ubyte byKeyID, INPUT_TYPE eType)
{
	return m_pInput_Manager->Key_Up(byKeyID, eType);
}
_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
	return m_pInput_Manager->Mouse_Pressing(eMouse, eType);
}
_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
	return m_pInput_Manager->Mouse_Down(eMouse, eType);
}
_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE eMouse, INPUT_TYPE eType)
{
	return m_pInput_Manager->Mouse_Up(eMouse, eType);
}
_long CGameInstance::Mouse_Move(MOUSEMOVESTATE eMouseState, INPUT_TYPE eType)
{
	return m_pInput_Manager->Mouse_Move(eMouseState, eType);
}
void CGameInstance::Change_InputType(INPUT_TYPE eType)
{
	m_pInput_Manager->Change_InputType(eType);
}
INPUT_TYPE* CGameInstance::Get_InputTypePtr()
{
	return m_pInput_Manager->Get_InputTypePtr();
}
INPUT_TYPE CGameInstance::Get_InputType()
{
	return m_pInput_Manager->Get_InputType();
}
#pragma endregion

#pragma region POOL_MANAGER
HRESULT CGameInstance::Add_PoolObject(_uint iPrototypeLevelIndex, const _wstring strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strPoolTag, void* pArg, _uint iCount)
{
	return m_pPool_Manager->Add_PoolObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strPoolTag, pArg, iCount);
}
CGameObject* CGameInstance::Pop_PoolObject(_uint iLayerLevelIndex, const _wstring& strPoolTag)
{
	return m_pPool_Manager->Pop_PoolObject(iLayerLevelIndex, strPoolTag);
}
HRESULT CGameInstance::Reset_PoolObject(CGameObject* pGameObject)
{
	return m_pPool_Manager->Reset_PoolObject(pGameObject);
}
void CGameInstance::Push_PoolObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	m_pPool_Manager->Push_PoolObject_ToLayer(iLayerLevelIndex, strLayerTag, pGameObject);
}
#pragma endregion

#pragma region EVENT_MANAGER
_bool CGameInstance::Unsubscribe_Event(_uint iEventType, _uint iListenerId)
{
	return m_pEvent_Manager->Unsubscribe(iEventType, iListenerId);
}
void CGameInstance::UnsubscribeAll_Event(_uint iEventType)
{
	m_pEvent_Manager->UnsubscribeAll(iEventType);
}
void CGameInstance::Clear_AllEvents()
{
	m_pEvent_Manager->ClearAll();
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

_bool CGameInstance::Push_MeshMetrial_SRV(string strFileName, ID3D11ShaderResourceView* pResource)
{
    return m_pResource_Manager->Push_MeshMetrial_SRV(strFileName, pResource);
}

_bool CGameInstance::Exist_MeshMetrial_SRV_InCache(string strFileName)
{
    return m_pResource_Manager->Exist_MeshMetrial_SRV_InCache(strFileName);
}

string CGameInstance::Convert_FullPath(string strFullPath)
{
    return m_pResource_Manager->Convert_FullPath(strFullPath);
}

ID3D11ShaderResourceView* CGameInstance::Get_MeshMetrial_SRVFromCache(string strFileName)
{
    return m_pResource_Manager->Get_MeshMetrial_SRVFromCache(strFileName);
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

#pragma region CSM

_uint CGameInstance::Get_NumCascades()
{
    return m_pCSM->Get_NumCascades();
}

void CGameInstance::Set_CurrentCascade(_uint iIndex)
{
    m_pCSM->Set_CurrentCascade(iIndex);
}

const _float4x4* CGameInstance::Get_CurrentCascadeLightViewMatrix() const
{
    return m_pCSM->Get_CurrentLightViewMatrix();
}

const _float4x4* CGameInstance::Get_CurrentCascadeLightProjMatrix() const
{
    return m_pCSM->Get_CurrentLightProjMatrix();
}

HRESULT CGameInstance::Bind_CascadeDSV(_uint iIndex)
{
    return m_pCSM->Bind_CascadeDSV(iIndex);
}

HRESULT CGameInstance::Bind_Cascade_ShaderResources(CShader* pShader)
{
    return m_pCSM->Bind_Cascade_ShaderResources(pShader);
}

void CGameInstance::Clear_CascadeDSVs()
{
    m_pCSM->Clear_DSVs();
}

void CGameInstance::Start_CascadeShadowTransition(_float fDuration, _float fTargetIntensity)
{
    m_pCSM->Start_CascadeShadowTransition(fDuration, fTargetIntensity);
}

CASCADE_CONFIG CGameInstance::Get_CascadeConfig()
{
    return m_pCSM->Get_CascadeConfig();
}

void CGameInstance::Set_CascadeConfig(CASCADE_CONFIG Config)
{
    m_pCSM->Set_CascadeConfig(Config);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_CSM_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return m_pCSM->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_CSM_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    return m_pCSM->Render(pShader, pVIBuffer);
}
#endif

#pragma endregion

#pragma region SHADOW

const _float4x4* CGameInstance::Get_ShadowLightMatrix(D3DTS eTransformState) const
{
    return m_pShadow->Get_ShadowLightMatrix(eTransformState);
}

HRESULT CGameInstance::Bind_Shadow_ShaderResources(CShader* pShader)
{
    return m_pShadow->Bind_Shadow_ShaderResources(pShader);
}

void CGameInstance::Bind_ShadowDSV()
{
    m_pShadow->Bind_ShadowDSV();
}

void CGameInstance::Start_ShadowTransition(_float fDuration, _float fTargetIntensity)
{
    m_pShadow->Start_ShadowTransition(fDuration, fTargetIntensity);
}

void CGameInstance::Clear_ShadowDSV()
{
    m_pShadow->Clear_DSV();
}

const SHADOW_DESC& CGameInstance::Get_ShadowDesc() const
{
    return m_pShadow->Get_ShadowDesc();
}

void CGameInstance::Set_ShadowDesc(const SHADOW_DESC& Desc)
{
    m_pShadow->Set_ShadowDesc(Desc);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_Shadow_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    return m_pShadow->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_Shadow_Debug(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    return m_pShadow->Render(pShader, pVIBuffer);
}
#endif

#pragma endregion

#pragma region SSAO

SSAO_CONFIG CGameInstance::Get_SSAOConfig()
{
	return m_pSSAO->Get_SSAOConfig();
}

void CGameInstance::Set_SSAOConfig(SSAO_CONFIG Config)
{
	m_pSSAO->Set_SSAOConfig(Config);
}

HRESULT CGameInstance::Bind_SSAO_ShaderResources(CShader* pShader)
{
	return m_pSSAO->Bind_SSAO_ShaderResources(pShader);
}
#pragma endregion

#pragma region GAUSSIANBLUR
HRESULT CGameInstance::Bind_GaussianBlur_ShaderResources(CShader* pShader)
{
	return m_pGaussianBlur->Bind_GaussianBlur_ShaderResources(pShader);
}
GAUSSIAN_BLUR_CONFIG CGameInstance::Get_GaussianBlurConfig()
{
	return m_pGaussianBlur->Get_GaussianBlurConfig();
}
void CGameInstance::Set_GaussianBlurConfig(GAUSSIAN_BLUR_CONFIG Config)
{
	m_pGaussianBlur->Set_GaussianBlurConfig(Config);
}
#pragma endregion

#pragma region FOG
HRESULT CGameInstance::Bind_Fog_ShaderResources(CShader* pShader)
{
	return m_pFog->Bind_Fog_ShaderResources(pShader);
}
FOG_CONFIG CGameInstance::Get_FogConfig()
{
	return m_pFog->Get_FogConfig();
}
void CGameInstance::Set_FogConfig(FOG_CONFIG Config)
{
	m_pFog->Set_FogConfig(Config);
}
_uint CGameInstance::Get_NumFogNoiseTextures()
{
	return m_pFog->Get_NumFogNoiseTextures();
}
ID3D11ShaderResourceView* CGameInstance::Get_FogNoiseTexture(_uint iTextureIndex)
{
	return m_pFog->Get_FogNoiseTexture(iTextureIndex);
}
void CGameInstance::Set_FogNoiseTextureIndex(_uint iTextureIndex)
{
	m_pFog->Set_FogNoiseTextureIndex(iTextureIndex);
}
void CGameInstance::Start_FogTransition(_float fDuration, const FOG_TRANSITION_DESC& Desc)
{
	m_pFog->Start_FogTransition(fDuration, Desc);
}
#pragma endregion

#pragma region VIGNETTE
HRESULT CGameInstance::Bind_Vignette_ShaderResources(CShader* pShader)
{
	return m_pVignette->Bind_Vignette_ShaderResources(pShader);
}

void CGameInstance::Set_EnableVignette(_bool isEnable)
{
	m_pVignette->Set_EnableVignette(isEnable);
}

VIGNETTE_CONFIG CGameInstance::Get_VignetteConfig()
{
	return m_pVignette->Get_VignetteConfig();
}

void CGameInstance::Set_VignetteConfig(VIGNETTE_CONFIG Config)
{
	m_pVignette->Set_VignetteConfig(Config);
}
void CGameInstance::Start_VignetteAnimation(_float fDuration, VIGNETTE_CONFIG Config)
{
	m_pVignette->Start_VignetteAnimation(fDuration, Config);
}
#pragma endregion

#pragma region SEQUENCE_MANAGER
HRESULT CGameInstance::SEQ_AdoptAndPlay(ISeqInstance* pSeq, SEQ_REQ_PLAY_DESC tDesc)
{
	return m_pSequence_Manager->AdoptAndPlay(pSeq, tDesc);
}

void CGameInstance::SEQ_EnqueueAdopt(ISeqInstance* pSeq, const SEQ_REQ_PLAY_DESC& tDesc)
{
	m_pSequence_Manager->EnqueueAdopt(pSeq, tDesc);
}

HRESULT CGameInstance::SEQ_Play(const SEQ_REQ_PLAY_DESC& tDecs)
{
	return m_pSequence_Manager->Play(tDecs);
}

HRESULT CGameInstance::SEQ_Stop(const SEQ_ID& tId, _bool isImmediate)
{
	return m_pSequence_Manager->Stop(tId, isImmediate);
}

HRESULT CGameInstance::SEQ_Pause(const SEQ_ID& tId)
{
	return m_pSequence_Manager->Pause(tId);
}

HRESULT CGameInstance::SEQ_Resume(const SEQ_ID& tId)
{
	return m_pSequence_Manager->Resume(tId);
}

HRESULT CGameInstance::SEQ_Jump(const SEQ_REQ_JUMP_DESC& tDesc)
{
	return m_pSequence_Manager->Jump(tDesc);
}

#pragma endregion

#pragma region DECAL_MANAGER

HRESULT CGameInstance::Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const DECAL_DESC& Desc)
{
	return m_pDecal_Manager->Spawn_Decal(strPoolTag, iLayerLevelIndex, strLayerTag, Desc);
}

HRESULT CGameInstance::Render_Decals()
{
	return m_pDecal_Manager->Render();
}

CTexture* CGameInstance::Get_DecalTexture(DECALTYPE eType)
{
    return m_pDecal_Manager->Get_DecalTexture(eType);
}

void CGameInstance::Batch_Decal(CDecal* pDecal)
{
    m_pDecal_Manager->Batch_Decal(pDecal);
}

void CGameInstance::Decal_Clear()
{
    m_pDecal_Manager->Decal_Clear();
}

#pragma endregion

#pragma region EFFECT_MANAGER

void CGameInstance::Add_Effect_ToPool(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint iPoolSize)
{ 
	m_pEffect_Manager->Add_Effect_ToPool(iLayerLevelIndex, strPrototypeTag, iPoolSize);
}

_uint CGameInstance::Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector SpawnPos)
{
 	return m_pEffect_Manager->Spawn_Effect(iLayerLevelIndex, strPrototypeTag, SpawnPos);
}

_uint CGameInstance::Spawn_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _fvector Quaternion, _gvector Position)
{
 	return m_pEffect_Manager->Spawn_Effect(iLayerLevelIndex, strPrototypeTag, Quaternion, Position);
}

void CGameInstance::Update_Effect_Position(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector SpawnPos)
{
	m_pEffect_Manager->Update_Effect_Position(iLayerLevelIndex, strPrototypeTag, ID, SpawnPos);
}

void CGameInstance::Update_Effect_World(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID, _fvector Quaternion, _gvector Position)
{
	m_pEffect_Manager->Update_Effect_World(iLayerLevelIndex, strPrototypeTag, ID, Quaternion, Position);
}

void CGameInstance::Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag, _uint ID)
{
	m_pEffect_Manager->Stop_Effect(iLayerLevelIndex, strPrototypeTag, ID);
}

void CGameInstance::Stop_Effect(_uint iLayerLevelIndex, const _wstring& strPrototypeTag)
{
    m_pEffect_Manager->Stop_Effect(iLayerLevelIndex, strPrototypeTag);
}

#pragma endregion

#pragma region DISTORTION

HRESULT CGameInstance::Bind_Distortion_ShaderResources(CShader* pShader)
{
	return m_pDistortion->Bind_Distortion_ShaderResources(pShader);
}

void CGameInstance::Start_Distortion(const DISTORTION_DESC& Desc)
{
	m_pDistortion->Start_Distortion(Desc);
}

DISTORTION_DESC CGameInstance::Get_DistortionDesc()
{
	return m_pDistortion->Get_DistortionDesc();
}

_uint CGameInstance::Get_NumDistortionNoiseTextures()
{
	return m_pDistortion->Get_NumDistortionNoiseTextures();
}

ID3D11ShaderResourceView* CGameInstance::Get_DistortionNoiseTexture(_uint iTextureIndex)
{
	return m_pDistortion->Get_DistortionNoiseTexture(iTextureIndex);
}

#pragma endregion

#pragma region DISTORTION

HRESULT CGameInstance::Bind_LUT_ShaderResources(CShader* pShader)
{
	return m_pLUT->Bind_LUT_ShaderResources(pShader);
}

void CGameInstance::Set_EnableLUT(_bool isEnable)
{
    m_pLUT->Set_EnableLUT(isEnable);
}

void CGameInstance::Set_LUTIntensity(_float fIntensity)
{
    m_pLUT->Set_LUTIntensity(fIntensity);
}

#pragma endregion

#pragma region RADIAL_BLUR

HRESULT CGameInstance::Bind_RadialBlur_ShaderResources(CShader* pShader)
{
    return m_pRadialBlur->Bind_RadialBlur_ShaderResources(pShader);
}

RADIAL_BLUR_DESC CGameInstance::Get_RadialBlurDesc()
{
    return m_pRadialBlur->Get_RadialBlurDesc();
}

void CGameInstance::Set_RadialBlurDesc(const RADIAL_BLUR_DESC& Desc)
{
    m_pRadialBlur->Set_RadialBlurDesc(Desc);
}

void CGameInstance::Set_RadialBlurCenter(_fvector vCenter, _float fOffset)
{
    m_pRadialBlur->Set_RadialBlurCenter(vCenter, fOffset);
}

void CGameInstance::Start_RadialBlur(const RADIAL_BLUR_DESC& Desc)
{
    m_pRadialBlur->Start_RadialBlur(Desc);
}

void CGameInstance::Set_EnableRadialBlur(_bool isEnable)
{
    m_pRadialBlur->Set_EnableRadialBlur(isEnable);
}

#pragma endregion

#pragma region MOTION_BLUR

HRESULT CGameInstance::Bind_MotionBlur_ShaderResources(CShader* pShader)
{
    return m_pMotionBlur->Bind_MotionBlur_ShaderResources(pShader);
}

MOTION_BLUR_DESC CGameInstance::Get_MotionBlurDesc()
{
    return m_pMotionBlur->Get_MotionBlurDesc();
}

void CGameInstance::Set_MotionBlurDesc(const MOTION_BLUR_DESC& Desc)
{
    m_pMotionBlur->Set_MotionBlurDesc(Desc);
}

void CGameInstance::Set_EnableMotionBlur(_bool isEnable)
{
    m_pMotionBlur->Set_EnableMotionBlur(isEnable);
}

void CGameInstance::Set_Gloval_Volume(_float fVolume)
{
    m_pSound_Manager->Set_Gloval_Volume(fVolume);
}

void CGameInstance::ADD_Gloval_Volume(_float fVolume)
{
    m_pSound_Manager->ADD_Gloval_Volume(fVolume);
}

void CGameInstance::PlaySoundOnce(const TCHAR* pSoundKey, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    m_pSound_Manager->PlaySoundOnce(pSoundKey, fVolume, ppOutChannel);
}

void CGameInstance::PlaySoundLoop(const TCHAR* pSoundKey, float fVolume, FMOD_CHANNEL** ppOutChannel)
{
    m_pSound_Manager->PlaySoundLoop(pSoundKey, fVolume, ppOutChannel);
}

void CGameInstance::StopAll()
{
    m_pSound_Manager->StopAll();
}

void CGameInstance::StopByKey(const TCHAR* pSoundKey)
{
    m_pSound_Manager->StopByKey(pSoundKey);
}

void CGameInstance::StopByChannel(FMOD_CHANNEL** ppOutChannel)
{
    m_pSound_Manager->StopByChannel(ppOutChannel);
}

bool CGameInstance::IsPlayingByKey(const TCHAR* pSoundKey)
{
    return m_pSound_Manager->IsPlayingByKey(pSoundKey);;
}

void CGameInstance::SetVolumeByKey(const TCHAR* pSoundKey, float fVolume)
{
    m_pSound_Manager->SetVolumeByKey(pSoundKey, fVolume);
}

#pragma endregion

#pragma region OCTREE

void CGameInstance::DeleteOctree()
{
	Safe_Release(m_pOctree);
	m_pOctree = nullptr;
}

HRESULT CGameInstance::CreateOctree(const _float3& vCenter, const _float& fHalfWidth, const _int& iDepthLimit)
{
	m_pOctree = COctree::Create(vCenter, fHalfWidth, iDepthLimit);
	if (!m_pOctree)
		return E_FAIL;

	return S_OK;
}

bool CGameInstance::AddStaticObject(CGameObject* pGameObject, const _float3& vPoint, const _float& fRadius)
{
	if (!m_pOctree)
		return false;

	return m_pOctree->AddStaticObject(pGameObject, vPoint, fRadius);
}


#pragma endregion

void CGameInstance::Release_Engine()
{
	Release();

#ifdef _DEBUG
	Safe_Release(m_pImgui_Manager);
#endif
    Safe_Release(m_pSound_Manager);
	Safe_Release(m_pDecal_Manager);
	Safe_Release(m_pSequence_Manager);
	Safe_Release(m_pThreadPool);

	Safe_Release(m_pOctree);

    Safe_Release(m_pMotionBlur);
    Safe_Release(m_pRadialBlur);
    Safe_Release(m_pLUT);
	Safe_Release(m_pDistortion);
	Safe_Release(m_pVignette);
	Safe_Release(m_pFog);
	Safe_Release(m_pGaussianBlur);
    Safe_Release(m_pSSAO);
    Safe_Release(m_pShadow);
	Safe_Release(m_pCSM);

	Safe_Release(m_pComputeShader_Manager);
	
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Manager);
	Safe_Release(m_pResource_Manager);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pEffect_Manager);

	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
    Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pPrototype_Manager);	
	Safe_Release(m_pLevel_Manager);
    Safe_Release(m_pPool_Manager);
    Safe_Release(m_pEvent_Manager);
    Safe_Release(m_pJolt_Manager);
	Safe_Release(m_pGraphic_Device);
    
}

void CGameInstance::Free()
{
	__super::Free();
}
