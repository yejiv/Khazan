#include "Effect_Prefab.h"
#include "Effect_Point_Instance.h"
#include "Effect_Sprite.h"
#include "Effect_Mesh_Instance.h"
#include "GameInstance.h"

CEffect_Prefab::CEffect_Prefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CPrefab{pDevice, pDeviceContext}
{
}

CEffect_Prefab::CEffect_Prefab(const CEffect_Prefab& Prototype)
    : CPrefab(Prototype)
    , m_bEventTriggered(Prototype.m_bEventTriggered)
    , m_eEventTracks(Prototype.m_eEventTracks)
{
    for (auto& element : Prototype.m_Children)
        m_Children.push_back(element->Clone());
}

HRESULT CEffect_Prefab::Initialize_Prototype(const char* filename)
{
    __super::Initialize_Prototype();
    Load(filename);

    return S_OK;
}

HRESULT CEffect_Prefab::Initialize_Clone(void* pArg)
{
    GAMEOBJECT_DESC         Desc{};
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(360.0f);

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    m_bPlaying = true;

    for(auto& element : m_Children)
        element->SetParentsMatrix(m_pTransformCom->Get_WorldMatrixPtr());

    //m_test = *static_cast<_float3*>(pArg);
    //m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_test.x, m_test.y, m_test.z, 1.f));

    return S_OK;
}

void CEffect_Prefab::Priority_Update(_float fTimeDelta)
{
    ///*test 지울 거*/
    //if (m_pGameInstance->Key_Down(DIK_L) && m_test.x == 1.f)    //000
    //    ResetChildren();
    //else if (m_pGameInstance->Key_Down(DIK_O) && m_test.y == 1.f) //100
    //    ResetChildren();
    //
    //const _float4* cam;
    //if (m_pGameInstance->Key_Down(DIK_D))    //000
    //    cam = m_pGameInstance->Get_CamPosition();
    ////test end
    

    
    if (!m_bPlaying)
        return ;    //reset && pool로 돌아가기!

    for (size_t i = 0; i < m_eEventTracks.size(); ++i)
    {
        EFFECT_EVENT track = m_eEventTracks[i];
        if (!m_bEventTriggered[i] && m_fCurTime >= track.fStartTime)
        {
            switch (track.eEventType)
            {
            case EffectEventType::ACTIVATE:
                m_Children[track.iElementIdx]->Active();
                break;
            case EffectEventType::ANIMATE_SPREAD:
                m_Children[track.iElementIdx]->SetSpreadData(&track);
                break;  
            case EffectEventType::ANIMATE_ROTATE:
                m_Children[track.iElementIdx]->SetRotateData(&track);
                break;
            case EffectEventType::ANIMATE_TWINLKE:
                m_Children[track.iElementIdx]->SetTwinkleData(&track);
                break; 
            case EffectEventType::ANIMATE_LINEAR_MOVE:
                m_Children[track.iElementIdx]->SetUpwardData(&track);
                break;
            case EffectEventType::SCROLL_SPEAD:
                m_Children[track.iElementIdx]->SetUpwardData(&track.fScrollSpeed);
                break;
            }
            m_bEventTriggered[i] = true;
        }
    }
}

void CEffect_Prefab::Update(_float fTimeDelta)
{
    if (!m_bPlaying) 
        return; 

    _bool   isFin{true};

    for (auto& child : m_Children)
    {
        if(child->IsActive())
        {
            child->Update(fTimeDelta);
            isFin = false;
        }
    }

    if(isFin == true)
    {
        for (bool isTriggered : m_bEventTriggered)
            if (isTriggered == true)
                break;
        m_bPlaying = false;
    }
}

void CEffect_Prefab::Late_Update(_float fTimeDelta)
{
    for (auto& child : m_Children)
    {
        if (child->IsActive())
            child->Late_Update(fTimeDelta);
    }
}

HRESULT CEffect_Prefab::Render()
{
    return S_OK;
}

void CEffect_Prefab::ResetChildren()
{
    m_bPlaying = true;
    m_fCurTime = 0.f;
    for (_uint i = 0; i < m_Children.size(); ++i)
        m_Children[i]->Reset();
    for (_uint i = 0; i < m_bEventTriggered.size(); ++i)
        m_bEventTriggered[i] = false;
}

void CEffect_Prefab::Load(const char* filename)
{ 
    std::ifstream is{ filename, std::ios::binary };
    if (!is)
    {
        MSG_BOX(TEXT("Failed to open file"));
        return;
    }

    size_t iNumChildren, iNumTracks;
    _uint EffectType {};

    CEffect_Point_Instance::PARTICLE_DESC PointDsc;
    CEffect_Mesh_Instance::PARTICLE_DESC MeshDsc;
    CEffect_Sprite::SPRITE_DESC SpriteDsc; 

    class Engine::CEffect_Element* newEffect{ nullptr };

    is.read(reinterpret_cast<char*>(&iNumChildren), sizeof(size_t));
    is.read(reinterpret_cast<char*>(&iNumTracks), sizeof(size_t));

    for (size_t i = 0; i < iNumChildren; ++i)
    {
        is.read(reinterpret_cast<char*>(&EffectType), sizeof(_uint));

        switch (EffectType)
        {
        case ENUM_CLASS(EffectType::POINT_INSTANCE) :
            is.read(reinterpret_cast<char*>(&PointDsc), sizeof(CEffect_Point_Instance::PARTICLE_DESC));
            newEffect = CEffect_Point_Instance::Create(m_pDevice, m_pContext, &PointDsc);
            break;

        case ENUM_CLASS(EffectType::MESH_INSTANCE):
            is.read(reinterpret_cast<char*>(&MeshDsc), sizeof(CEffect_Mesh_Instance::PARTICLE_DESC));
            newEffect = CEffect_Mesh_Instance::Create(m_pDevice, m_pContext, &MeshDsc);
            break;

        case ENUM_CLASS(EffectType::SPRITE):
            is.read(reinterpret_cast<char*>(&SpriteDsc), sizeof(CEffect_Sprite::SPRITE_DESC));
            newEffect = CEffect_Sprite::Create(m_pDevice, m_pContext, &SpriteDsc);
            break;

        default : 
            MSG_BOX(TEXT("Effect Type Error"));
            return;
        }
        m_Children.push_back(newEffect);
    }

    EFFECT_EVENT EventDsc;
    for (size_t i = 0; i < iNumTracks; ++i)
    {
        is.read(reinterpret_cast<char*>(&EventDsc), sizeof(EFFECT_EVENT));
        m_eEventTracks.push_back(EventDsc);
        m_bEventTriggered.push_back(false);
    }

    is.close();
}

CEffect_Prefab* CEffect_Prefab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* filename)
{
    CEffect_Prefab* pInstance = new CEffect_Prefab(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype(filename)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Prefab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Prefab::Clone(void* pArg)
{
    CEffect_Prefab* pInstance = new CEffect_Prefab(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Prefab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Prefab::Free()
{
    __super::Free();
}

