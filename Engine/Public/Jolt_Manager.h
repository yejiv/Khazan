#pragma once
#include "Base.h"
#include "Jolt_Layer.h"
#include "Jolt_ContactListener.h"

#ifdef new
#pragma push_macro("new")
#undef new
#endif

#include <Core/Factory.h>

#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Character//CharacterVirtual.h>
#ifdef new
#pragma pop_macro("new") // DBG_NEW 복원
#endif

#ifdef _DEBUG
#include "Jolt_DebugRenderer.h"
#endif

NS_BEGIN(Engine)

class CJolt_Manager final : public CBase
{
private:
    CJolt_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CJolt_Manager() = default;

public:
    // 초기화/종료/스텝
    HRESULT Initialize(_uint iNumObjectLayer);
    void    Update(_float fDeltaTime);

    Body* CreateAndAdd_Body(const BodyCreationSettings& BodySetting, BodyInterface** pBodyInterface);
    CharacterVirtual* CreateCharacterVirtual(const CharacterVirtualSettings* inSettings, RVec3Arg inPosition, QuatArg inRotation, uint64 inUserData);


    void				Set_PhysicsSystem();
    void				Set_ObjectToBP(_uint iObjectLayer, _uint iBPLayer) {
        m_pBPLayer->SetUp_ObjectToBP(iObjectLayer, iBPLayer);
    };
    void				Set_ObjectFilter(_uint iSrc, _uint iDst) {
        m_pObjectLayerPairFilter->SetUp_ObjectFilter(iSrc, iDst);
    };
    void				Set_ObjectVsBPFilter(_uint iObjectLayer, _uint iBPLayer) {
        m_pObjectVsBPLayerFilter->SetUp_ObjectVsBPFilter(iObjectLayer, iBPLayer);
    };



#ifdef _DEBUG

public:
    void Test();
    void Debug_Render();
#endif
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    // 필수 구성요소
    PhysicsSystem* m_pPhysics = { nullptr };
    TempAllocatorImpl* m_pTempAlloc = { nullptr };
    JobSystemThreadPool* m_pJobSystem = { nullptr };

    PhysicsSettings		m_PhysicsSetting;

    // 레이어/필터/리스너
    CJolt_BPLayer*                       m_pBPLayer = { nullptr };
    CJolt_ObjectLayerPairFilter*         m_pObjectLayerPairFilter = { nullptr };
    CJolt_ObjectVsBPLayerFilter*         m_pObjectVsBPLayerFilter = { nullptr };
    CJolt_ContactListener*               m_pContactListener = { nullptr };

private:
    // 생성 파라미터 보관(선택)
    _uint m_iMaxBodies = { 10240 };
    _uint m_iNumBodyMutexes = {};
    _uint m_iMaxBodyPairs = { 65536 };
    _uint m_iMaxContactConstraints = { 65536 };
    _uint m_iJobThreadCount = {};

#ifdef _DEBUG
private:
    CJolt_DebugRenderer* m_pDebugRenderer = { nullptr };
    BodyManager::DrawSettings m_DrawSetting;
#endif

public:
    // 엔진 매니저 패턴
    static CJolt_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumObjectLayer);
    virtual void Free() override;
};

NS_END
