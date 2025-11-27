#include "MotionTrail.h"
#include "GameInstance.h"

CMotionTrail::CMotionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CMotionTrail::CMotionTrail(const CMotionTrail& Prototype)
    : CComponent(Prototype)
    , m_pShader { Prototype.m_pShader }
    , m_CachedConfig {Prototype.m_CachedConfig }
{
    Safe_AddRef(m_pShader);
}

HRESULT CMotionTrail::Initialize_Prototype()
{
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_MotionTrail.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    if(FAILED(Ready_CachedConfig()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMotionTrail::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    MOTIONTRAIL_DESC* pDesc = static_cast<MOTIONTRAIL_DESC*>(pArg);

    m_pOwnerMasterModel = pDesc->pOwnerMasterModel;
    Safe_AddRef(m_pOwnerMasterModel);

    m_HasPartModels = pDesc->HasPartModels;

    if (m_HasPartModels)
    {
        m_OwnerPartModels = pDesc->OwnerPartModels;
        for (auto& pModel : m_OwnerPartModels)
            Safe_AddRef(pModel);
    }

    m_Config = pDesc->Config;

    return S_OK;
}

void CMotionTrail::Update(_float fTimeDelta)
{
    // 인터벌 간격으로 프레임 스냅샷 캡처
    // 스냅샷 생명 주기와 프레임 히스토리 관리

    if (true == m_isEnable)
    {
        // 지정한 지속 시간만큼 활성화하는 모드, 아닐 땐 지속적으로 활성화
        if (true == m_isDurationMode)
        {
            m_fDurationTimeAcc += fTimeDelta;

            if (m_fDurationTimeAcc >= m_fDuration)
            {
                m_isEnable = false;
                m_isDurationMode = false;
            }
        }

        m_fCaptureTimeAcc += fTimeDelta;

        // 인터벌 시간 충족, 활성화 상태, 최대 프레임 개수보다 기록된 프레임 수가 작을 때만 스냅샷 캡처
        if (m_fCaptureTimeAcc >= m_Config.fInterval && m_FrameHistory.size() < m_Config.iMaxFrames)
        {
            FRAME_SNAPSHOT Snapshot;
            m_pOwnerMasterModel->Capture_CurrentFrameMatrices(Snapshot.BoneCombinedMatrices, &Snapshot.OwnerWorldMatrix);
            Snapshot.vLifeTime = m_Config.vLifeTime;

            if (false == m_Config.isIndividualColor)
            {
                m_fColorTimeAcc += fTimeDelta * m_Config.fColorUpdateSpeed;

                _float fColorRatio = cosf(XMConvertToRadians(m_fColorTimeAcc)) * 0.5f + 0.5f;

                _vector vStartColor = XMLoadFloat3(&m_Config.vStartColor);
                _vector vTargetColor = XMLoadFloat3(&m_Config.vTargetColor);

                _vector vFinalColor = XMVectorLerp(vStartColor, vTargetColor, fColorRatio);

                _float3 vResultColor{};
                XMStoreFloat3(&vResultColor, vFinalColor);

                Snapshot.vStartColor = vResultColor;
                Snapshot.vTargetColor = vResultColor;
            }
            else
            {
                Snapshot.vStartColor = m_Config.vStartColor;
                Snapshot.vTargetColor = m_Config.vTargetColor;
            }

            m_FrameHistory.push_back(Snapshot);

            m_fCaptureTimeAcc = 0.f;
        }
    }

     for (auto& Snapshot : m_FrameHistory)
        Snapshot.vLifeTime.x += fTimeDelta;

     if (!m_FrameHistory.empty())
     {
         if (m_FrameHistory.front().vLifeTime.x >= m_FrameHistory.front().vLifeTime.y)
            m_FrameHistory.pop_front();

         if (FAILED(m_pGameInstance->Add_RenderComponent(this)))
            return;
     }
}

void CMotionTrail::Start_MotionTrail(_float fDuration)
{
    if (true == m_isDurationMode)
        return;

    m_isEnable = true;
    m_isDurationMode = true;
    m_fDuration = fDuration;
    m_fDurationTimeAcc = 0.f;
}

void CMotionTrail::Update_PartModels(const vector<class CModel*>& NewPartModels)
{
    if (NewPartModels.empty())
        return;

    // 기존 파트 모델들 클리어
    for (auto& pModel : m_OwnerPartModels)
        Safe_Release(pModel);
    m_OwnerPartModels.clear();

    // 데이터 복사
    m_OwnerPartModels = NewPartModels;
    
    // 참조 카운트 증가
    for (auto& pModel : m_OwnerPartModels)
        Safe_AddRef(pModel);
}

void CMotionTrail::Update_MasterModel(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    Safe_Release(m_pOwnerMasterModel);

    m_pOwnerMasterModel = pModel;
    Safe_AddRef(m_pOwnerMasterModel);
}

void CMotionTrail::Set_Config(_wstring strConfig)
{
    auto it = m_CachedConfig.find(strConfig);
    if (it == m_CachedConfig.end()) //못 찾으면 기본으로 세팅
        m_Config = m_CachedConfig[TEXT("MT_Common_WhiteDefault")];
    else
        m_Config = it->second;
}

HRESULT CMotionTrail::Render()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimPower", &m_Config.fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fRimLightIntensity", &m_Config.fRimIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fEmissiveIntensity", &m_Config.fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    // 백업
    m_pOwnerMasterModel->Cache_CurrentBoneMatrices();

    for (size_t i = 0; i < m_FrameHistory.size(); ++i)
    {
        if (m_pOwnerMasterModel->Restore_Frame(m_FrameHistory[m_FrameHistory.size() - 1 - i].BoneCombinedMatrices))
        {
            if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_FrameHistory[m_FrameHistory.size() - 1 - i].OwnerWorldMatrix)))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vLifeTime",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vLifeTime, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vStartColor",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vStartColor, sizeof(_float3))))
                return E_FAIL;

            if (FAILED(m_pShader->Bind_RawValue("g_vTargetColor",
                &m_FrameHistory[m_FrameHistory.size() - 1 - i].vTargetColor, sizeof(_float3))))
                return E_FAIL;

            // 본 행렬 바인딩 (복원된 상태로)
            _uint iNumMeshes = m_pOwnerMasterModel->Get_NumMeshes();

            if (true == m_HasPartModels)
            {
                for (_uint j = 0; j < iNumMeshes; ++j)
                {
                    if (FAILED(m_pOwnerMasterModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", j)))
                        continue;
                }

                for (auto& pModel : m_OwnerPartModels)
                    Render_PartModel(pModel);
            }
            else
            {
                for (_uint j = 0; j < iNumMeshes; ++j)
                {
                    m_pOwnerMasterModel->Bind_Materials(m_pShader, "g_NormalTexture", j, aiTextureType_NORMALS, 0);

                    if (FAILED(m_pOwnerMasterModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", j)))
                        continue;

                    m_pShader->Begin(0);
                    m_pOwnerMasterModel->Render(j);
                }
            }
        }
    }

    // 복원
    m_pOwnerMasterModel->Restore_CurrentBoneMatrices();

    return S_OK;
}

HRESULT CMotionTrail::Render_PartModel(CModel* pModel)
{
    if (false == m_HasPartModels || nullptr == pModel)
        return E_FAIL;

    pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShader, "g_NormalTexture", i, aiTextureType_NORMALS, 0);

        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", i)))
            continue;

        m_pShader->Begin(0);
        pModel->Render(i);
    }

    return S_OK;
}

HRESULT CMotionTrail::Ready_CachedConfig()
{
    /* EX.
    KEY : TEXT("MT_[Context]_[Style]_[Option]")
    Context : Common, Spear, GSword, SpearSkill, GSwordSkill,...
    Style : WhiteDefault ,RedStrong ,BlueSoft ,GoldenSlash,...
    Option : 고정값들 변경 시  LT03 → LifeTime 0~0.3 , INT1 → Interval 0.1 ,MF10 → MaxFrame 10
        --------------------------------------------------------------
         보라 계열 (마법 · 잔상 · 이세계)
        --------------------------------------------------------------
          Violet Warp          {0.600f, 0.400f, 1.000f}
          Arcane Purple        {0.490f, 0.322f, 0.871f}
          Mystic Amethyst      {0.671f, 0.486f, 1.000f}
          Dark Void            {0.353f, 0.000f, 0.627f}
          Dimensional Rift     {0.784f, 0.275f, 1.000f}
          Ethereal Glow        {0.706f, 0.549f, 1.000f}
          Phantom Trace        {0.529f, 0.235f, 0.784f}
          Void Smoke           {0.431f, 0.176f, 0.569f}
          Arcane Highlight     {0.824f, 0.627f, 1.000f}
          Night Surge          {0.275f, 0.000f, 0.392f}

        --------------------------------------------------------------
         붉은 계열 (파괴 · 격노 · 폭발)
        --------------------------------------------------------------
          Crimson Slash        {0.871f, 0.157f, 0.176f}
          Blood Burst          {0.686f, 0.000f, 0.020f}
          Rage Ember           {1.000f, 0.247f, 0.118f}
          Flame Surge          {1.000f, 0.369f, 0.145f}
          Inferno Edge         {0.941f, 0.200f, 0.063f}
          Scarlet Drive        {0.784f, 0.039f, 0.102f}
          Lava Pulse           {0.886f, 0.275f, 0.086f}
          Ember Glare          {1.000f, 0.306f, 0.153f}
          Burning Rush         {0.910f, 0.122f, 0.078f}
          Bloodflare           {0.784f, 0.118f, 0.196f}

        --------------------------------------------------------------
         파란 계열 (얼음 · 정신 · 집중)
        --------------------------------------------------------------
          Frost Dash           {0.627f, 0.784f, 1.000f}
          Arctic Whisper       {0.525f, 0.725f, 1.000f}
          Icebound Trace       {0.451f, 0.643f, 0.902f}
          Azure Flow           {0.212f, 0.447f, 0.871f}
          Deep Focus Blue      {0.137f, 0.314f, 0.706f}
          Frozen Mist          {0.725f, 0.843f, 1.000f}
          Mind Surge           {0.275f, 0.549f, 0.933f}
          Spirit Echo          {0.392f, 0.588f, 0.941f}
          Ether Stream         {0.180f, 0.357f, 0.788f}
          Cold Rift            {0.157f, 0.333f, 0.686f}

        --------------------------------------------------------------
         녹색 계열 (독기 · 자연 · 회복)
        --------------------------------------------------------------
          Venom Slash          {0.392f, 0.784f, 0.157f}
          Toxic Drift          {0.314f, 0.627f, 0.098f}
          Nature Pulse         {0.431f, 0.784f, 0.275f}
          Healing Breeze       {0.490f, 0.941f, 0.475f}
          Forest Step          {0.235f, 0.588f, 0.196f}
          Jungle Echo          {0.329f, 0.686f, 0.259f}
          Acid Flash           {0.549f, 0.902f, 0.133f}
          Mold Glow            {0.392f, 0.627f, 0.314f}
          Pure Vitality        {0.553f, 0.882f, 0.467f}
          Toxic Mirage         {0.314f, 0.725f, 0.302f}

        --------------------------------------------------------------
         황금 계열 (신성 · 각성 · 강화)
        --------------------------------------------------------------
          Divine Strike        {1.000f, 0.894f, 0.322f}
          Holy Radiance        {1.000f, 0.953f, 0.667f}
          Golden Impact        {1.000f, 0.804f, 0.196f}
          Sacred Burst         {0.980f, 0.871f, 0.427f}
          Awakening Flash      {1.000f, 0.933f, 0.553f}
          Celestial Shine      {1.000f, 0.980f, 0.686f}
          Solar Gleam          {1.000f, 0.875f, 0.271f}
          Lightforged Edge     {0.980f, 0.910f, 0.455f}
          Ascension Glow       {1.000f, 0.945f, 0.667f}
          Golden Rift          {0.953f, 0.820f, 0.259f}

    */
    MOTIONTRAIL_CONFIG config;

    {
        config.vLifeTime = { 0.f, 0.3f };
        config.vStartColor = { 1.f, 1.f, 1.f };
        config.vTargetColor = { 1.f, 1.f, 1.f };
        config.fRimPower = 2.f;
        config.fRimIntensity = 1.f;
        config.fEmissiveIntensity = 2.f;
        config.isIndividualColor = true;
        config.fColorUpdateSpeed = 1000.f;
        config.fInterval = 0.1f;
        config.iMaxFrames = 10.f;
        m_CachedConfig.emplace( TEXT("MT_Common_WhiteDefault"), config );
    }
    {
        config.vLifeTime = { 0.f, 0.3f };
        config.vStartColor = { 1.f, 1.f, 0.f };
        config.vTargetColor = { 1.f, 1.f, 0.f };
        config.fRimPower = 2.f;
        config.fRimIntensity = 1.f;
        config.fEmissiveIntensity = 2.f;
        config.isIndividualColor = true;
        config.fColorUpdateSpeed = 1000.f;
        config.fInterval = 0.1f;
        config.iMaxFrames = 10.f;
        m_CachedConfig.emplace(TEXT("MT_Common_YellowDefualt"), config);
    }
    {
        config.vLifeTime = { 0.f, 0.3f };
        config.vStartColor = { 1.f, 0.f, 0.f };
        config.vTargetColor = { 1.f, 0.f, 0.f };
        config.fRimPower = 2.f;
        config.fRimIntensity = 1.f;
        config.fEmissiveIntensity = 2.f;
        config.isIndividualColor = true;
        config.fColorUpdateSpeed = 1000.f;
        config.fInterval = 0.1f;
        config.iMaxFrames = 10.f;
        m_CachedConfig.emplace(TEXT("MT_Common_RedDefault"), config);
    }
    {
        config.vLifeTime = { 0.f, 0.3f };
        config.vStartColor = { 0.980f, 0.910f, 0.455f };
        config.vTargetColor = { 1.000f, 0.953f, 0.667f };
        config.fRimPower = 2.f;
        config.fRimIntensity = 1.f;
        config.fEmissiveIntensity = 2.f;
        config.isIndividualColor = true;
        config.fColorUpdateSpeed = 1000.f;
        config.fInterval = 0.1f;
        config.iMaxFrames = 10.f;
        m_CachedConfig.emplace(TEXT("MT_Common_Avoid"), config);
    }
    {
        config.vLifeTime = { 0.f, 0.3f };
        config.vStartColor = { 0.f, 0.f, 1.f };
        config.vTargetColor = { 0.f, 0.f, 1.f };
        config.fRimPower = 2.f;
        config.fRimIntensity = 1.f;
        config.fEmissiveIntensity = 2.f;
        config.isIndividualColor = true;
        config.fColorUpdateSpeed = 1000.f;
        config.fInterval = 0.1f;
        config.iMaxFrames = 10.f;
        m_CachedConfig.emplace(TEXT("MT_Common_BlueDefault"), config);
    }

    return S_OK;
}

CMotionTrail* CMotionTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMotionTrail* pInstance = new CMotionTrail(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMotionTrail::Clone(void* pArg)
{
    CMotionTrail* pInstance = new CMotionTrail(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMotionTrail"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMotionTrail::Free()
{
    __super::Free();

    for (auto& pModel : m_OwnerPartModels)
        Safe_Release(pModel);

    Safe_Release(m_pOwnerMasterModel);

    Safe_Release(m_pShader);
}
