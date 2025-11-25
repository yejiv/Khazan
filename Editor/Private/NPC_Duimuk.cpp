#include "NPC_Duimuk.h"

#include "GameInstance.h"

#include "Duimuk_Part.h"

CNPC_Duimuk::CNPC_Duimuk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive{ pDevice, pContext }
{
}

CNPC_Duimuk::CNPC_Duimuk(const CNPC_Duimuk& Prototype)
    : CProp_Interactive{ Prototype }
{
}

HRESULT CNPC_Duimuk::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CNPC_Duimuk::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    CHECK_FAILED(Ready_PartObjects(pArg), E_FAIL);

    m_eAnimState = ANIM_STATE::IDLE1;
    m_pModelCom->Set_Animation(m_eAnimState);
    m_pModelCom->Set_AnimationLoop(true);
    m_pModelCom->Set_AnimationBlend(true);

    return S_OK;
}

void CNPC_Duimuk::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CNPC_Duimuk::Update(_float fTimeDelta)
{
    // BEFORE_IDLE > _START > _LOOP > _END >>                       // 첫 터치떄
    // >> AFTER_IDLE > _START > _LOOP > _END > AFTER_IDLE >>        // 반복

    // 7 > 9 > 8 > 9 > 8 > 9 >>> ...

    _bool isisisis = { false };

    this;

    if (m_pGameInstance->Key_Down(DIK_7))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::TALK_START;
    }
    if (m_pGameInstance->Key_Down(DIK_8))
    {
        isisisis = true;

        m_eAnimState = ANIM_STATE::IDLE1;
    }

    if (isisisis == true)
    {
        m_pModelCom->Set_Animation(m_eAnimState);
    }

    if (true == m_pModelCom->Play_Animation(fTimeDelta))
    {
    }

    __super::Update(fTimeDelta);
}

void CNPC_Duimuk::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this), );

    __super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Duimuk::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(9), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CNPC_Duimuk::Ready_Components(void* pArg)
{
    DUIMUK_DESC* pDesc = static_cast<DUIMUK_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LAYER 함수에서 LEVEL 미입력"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CNPC_Duimuk::Ready_PartObjects(void* pArg)
{
    DUIMUK_DESC* pDesc = static_cast<DUIMUK_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    CDuimuk_Part::DUIMUK_PART_DESC PartDesc = {};

    PartDesc.eLevel = eLevel;
    PartDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    PartDesc.pSocketMatrix = m_pModelCom->Get_BoneMatrix("Weapon_L");

    CHECK_FAILED(__super::Add_PartObject(TEXT("Part_Instrument"), ENUM_CLASS(LEVEL::MAP),
        TEXT("Prototype_GameObject_Prop_NPC_Duimuk_Part"), &PartDesc), E_FAIL);

    return S_OK;
}

HRESULT CNPC_Duimuk::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CNPC_Duimuk* CNPC_Duimuk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CNPC_Duimuk* pInstance = new CNPC_Duimuk(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNPC_Duimuk::Clone(void* pArg)
{
    CNPC_Duimuk* pInstance = new CNPC_Duimuk(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNPC_Duimuk::Free()
{
    __super::Free();


}
