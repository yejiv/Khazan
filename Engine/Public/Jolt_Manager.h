#pragma once
#include "Base.h"
#include "Jolt_Layer.h"
#include "Jolt_ContactListener.h"
#include "Jolt_CharacterContactListener.h"
#include "BodyDrawFilter.h"

#ifdef _DEBUG
#include "Jolt_DebugRenderer.h"
#endif

NS_BEGIN(Engine)

class CJolt_Manager final : public CBase
{
public:
    struct RayCastDesc
    {
        _float3 vStart;
        _float3 vEnd;
        ColorArg vColor;
    };
private:
    CJolt_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CJolt_Manager() = default;

public:
    // 초기화/종료/스텝
    HRESULT Initialize(_uint iNumObjectLayer);
    void    Update(_float fDeltaTime);

    Body* CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);
    CharacterVirtual* CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData, BodyInterface** pBodyInterface);

    HRESULT				Set_PhysicsSystem();
    void				Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer) {
        m_pBPLayerIF->SetUp_ObjectToBP(iObjectLayer, iBPLayer);
    };
    void				Set_ObjectFilter(_uint iSrc, _uint iDst) {
        m_pObjectLayerPairFilter->SetUp_ObjectFilter(iSrc, iDst);
    };
    void				Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer) {
        m_pObjectVsBPLayerFilter->SetUp_ObjectVsBPFilter(iObjectLayer, iBPLayer);
    };
    void				Set_ObjectLayerFilter(_uint iObjectLayer, _bool isOn) {
        m_pObjectLayerFilter->SetUpAllow(iObjectLayer, isOn);
    };
#ifdef _DEBUG
    void				Set_DrawFilter(_uint iObjectLayer) {
        m_DrawFilter->SetUp_LayerToFilter(iObjectLayer);
    };
    void				Remove_DrawFilter(_uint iObjectLayer) {
        m_DrawFilter->Remove_LayerToFilter(iObjectLayer);
    };
#endif
    void Set_Gravity(_vector vGravity);
    void Reset_Gravity(); 


public:
    void CharVir_Update(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter);
    void CharVir_ExtendedUpdate(_float fTimeDelta, CharacterVirtual* pCharVir, Vec3 vGravity, _uint iObjectLayer, BodyFilter* pBodyFilter, ShapeFilter* pShapeFilter, CharacterVirtual::ExtendedUpdateSettings tSetting);

public:
    CharacterVirtual* Find_CharacterVirtual(CharacterID id);
	void Remove_CharacterVirtual(CharacterID id);

    void Push_BodyDesc(BodyID id, uint64 pBodyDesc);
    uint64 Find_BodyDesc(BodyID id);
    void Remove_BodyDesc(BodyID id);

    void Destroy_Body(BodyID& id);

public:
    _bool RayCast(_float3 vStart, _float3 vEnd, _float& outFraction, _float4& outPosition, _float3* outNormal = nullptr);

//public:
//    void Clear();
#ifdef _DEBUG

public:
    void Test();
    void Debug_Render();
    void RayCast_Render_Clear();
#endif
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

    // 필수 구성요소
    PhysicsSystem* m_pPhysics = { nullptr };
    TempAllocatorImpl* m_pTempAlloc = { nullptr };
    JobSystemThreadPool* m_pJobSystem = { nullptr };

    PhysicsSettings		m_PhysicsSetting;

    // 레이어/필터/리스너
    CJolt_BPLayerIF*                     m_pBPLayerIF = { nullptr };
    CJolt_ObjectLayerPairFilter*         m_pObjectLayerPairFilter = { nullptr };
    CJolt_ObjectVsBPLayerFilter*         m_pObjectVsBPLayerFilter = { nullptr };
    CJolt_ContactListener*               m_pContactListener = { nullptr };
    CJolt_CharacterContactListener*      m_pCharContactListener = { nullptr };
    CharacterVsCharacterCollisionSimple* m_pCharVsCharCollision = { nullptr };
    CJolt_ObjectLayerFilter*             m_pObjectLayerFilter = { nullptr };
    GroupFilterTable*                    m_pGroupFilterTable = { nullptr };

	map<CharacterID, CharacterVirtual*>  m_CharacterVirtuals;

    map<BodyID, uint64>                 m_BodyDescs;

    vector<RayCastDesc>  m_RayCasts;
private:
    // 생성 파라미터 보관(선택)
    _uint m_iMaxBodies = { 2048 };
    _uint m_iNumBodyMutexes = {};
    _uint m_iMaxBodyPairs = { 2048 };
    _uint m_iMaxContactConstraints = { 2048 };
    _uint m_iJobThreadCount = {};

    _uint m_iNumObjectLayer = {};

#ifdef _DEBUG
private:
    CJolt_DebugRenderer* m_pDebugRenderer = { nullptr };
    BodyManager::DrawSettings m_DrawSetting;
    DrawOnlyLayers* m_DrawFilter;
#endif

public:
    // 엔진 매니저 패턴
    static CJolt_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumObjectLayer);
    virtual void Free() override;
};

NS_END
