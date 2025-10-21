#include "Camera_Compre.h"

#include "GameInstance.h"

static inline float Clamp(float v, float lo, float hi) { return max(lo, min(v, hi)); }

static inline float WrapAngle(float a) {
    const float twoPi = XM_PI * 2.0f;
    while (a > XM_PI) a -= twoPi;
    while (a < -XM_PI) a += twoPi;
    return a;
}

CCamera_Compre::CCamera_Compre(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Compre::CCamera_Compre(const CCamera_Compre& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CCamera_Compre::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Compre::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Camera(pArg), E_FAIL);

    m_isActive = false;

    return S_OK;
}

void CCamera_Compre::Priority_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    if (!m_isAnimation)
    {
        if (m_iCameraType == ENUM_CLASS(CAMERATYPE::FREE))
            Update_Free(fTimeDelta);
        else if (m_iCameraType == ENUM_CLASS(CAMERATYPE::SPRING))
            Update_Spring(fTimeDelta);
    }
    __super::Update_PipeLines();
}

void CCamera_Compre::Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    __super::Play_Animation(fTimeDelta);
}

void CCamera_Compre::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;
}

HRESULT CCamera_Compre::Render()
{
    if (!m_isActive)
        return  S_OK;

    return S_OK;
}

void CCamera_Compre::Update_Free(_float fTimeDelta)
{

    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    _int    iMouseMove = {};

    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

}

void CCamera_Compre::Update_Spring(_float fTimeDelta)
{
    if (m_pObjMatrix == nullptr)
        return;

    _int iMouseMoveX = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X);
    _int iMouseMoveY = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y);

    m_fYaw = WrapAngle(m_fYaw - fTimeDelta * iMouseMoveX * m_fMouseSensor);
    m_fPitch = Clamp(m_fPitch - fTimeDelta * iMouseMoveY * m_fMouseSensor, m_fPitchMin, m_fPitchMax);

    _vector vTargetPos = XMVectorSet(m_pObjMatrix->_41, m_pObjMatrix->_42 + 7.f, m_pObjMatrix->_43, 1.f);
    _vector vDir = XMVectorSet(cosf(m_fPitch) * cosf(m_fYaw), sinf(m_fPitch), cosf(m_fPitch) * sinf(m_fYaw), 0.f);
    _vector vCamPosDes = XMVectorMultiplyAdd(XMVectorSet(-m_fRadius, -m_fRadius, -m_fRadius, 0.f), vDir, vTargetPos);

    _vector vWorldUp, vLook, vRight, vUp;

    _float fAlphaTarget = 1.f - expf(-m_fFollowValue * fTimeDelta);
    m_vLerpMove = XMVectorLerp(m_vLerpMove, vTargetPos, fAlphaTarget);

    _vector vCamPosPrev = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vCamPos = vCamPosDes;

    //if (m_CCDesc.isValid)
    //{
    //    // push-out: x += n * max(0, (n·c + (r+skin) - n·x))
    //    const _float fInflate = m_fRadius + m_fSkin;
    //    _vector vN = XMVectorSet(m_CCDesc.vNormal.x, m_CCDesc.vNormal.y, m_CCDesc.vNormal.z, 0.f);
    //    _vector vCpt = XMVectorSet(m_CCDesc.vPoint.x, m_CCDesc.vPoint.y, m_CCDesc.vPoint.z, 0.f);

    //    _float fNx = XMVectorGetX(XMVector3Dot(vN, vCamPos));
    //    _float fNc = XMVectorGetX(XMVector3Dot(vN, vCpt));
    //    _float fS = (fNc + fInflate) - fNx;

    //    if (fS > 0.f)
    //        vCamPos = XMVectorMultiplyAdd(vN, XMVectorReplicate(fS), vCamPos);

    //    // slide: V ← V - n(n·V)  (표면 따라가기)
    //    _vector vMove = XMVectorSubtract(vCamPos, vCamPosPrev);
    //    _float  fVn = XMVectorGetX(XMVector3Dot(vMove, vN));
    //    if (fVn < 0.f)
    //    {
    //        vMove = XMVectorSubtract(vMove, XMVectorScale(vN, fVn));
    //        vCamPos = XMVectorAdd(vCamPosPrev, vMove);
    //    }

    //    // (선택) 한 프레임만 쓰고 끝내고 싶으면 주석 해제
    //     //m_CCDesc.isValid = false;
    //}
    //else
    //{
    //    vCamPos = vCamPosDes;
    //}

    vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    vLook = XMVector3Normalize(XMVectorSubtract(m_vLerpMove, vCamPos));
    vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLook));
    vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    m_pTransformCom->Set_State(STATE::RIGHT, vRight);
    m_pTransformCom->Set_State(STATE::UP, vUp);
    m_pTransformCom->Set_State(STATE::LOOK, vLook);
    m_pTransformCom->Set_State(STATE::POSITION, vCamPos);

    //m_pCharVirCom->Set_PosRot(vCamPos, m_pTransformCom->Get_Rotation_Quat());

    //m_pGameInstance->Set_Gravity(XMVectorSet(0.f, 0.f, 0.f, 0.f));
    m_pCharVirCom->Sync_Update(m_pTransformCom);
    m_pCharVirCom->Update(fTimeDelta, m_pTransformCom, XMVectorSet(0.f, 0.f, 0.f, 0.f));
    //m_pGameInstance->Reset_Gravity();
}

void CCamera_Compre::Collision_Enter(CGameObject* pObject, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    _vector vNormal = XMVector3Normalize(XMLoadFloat3(&vContactPoint));
    XMStoreFloat3(&m_CCDesc.vNormal, vNormal);
    m_CCDesc.vPoint = vContactPoint;
    m_CCDesc.isValid = true;
    m_CCDesc.fTtl = 0.08f;
}

void CCamera_Compre::Collision_Stay(CGameObject* pObject, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    _vector vNormal = XMVector3Normalize(XMLoadFloat3(&vContactPoint));
    XMStoreFloat3(&m_CCDesc.vNormal, vNormal);
    m_CCDesc.vPoint = vContactPoint;
    m_CCDesc.isValid = true;
    m_CCDesc.fTtl = 0.08f;
}

HRESULT CCamera_Compre::Ready_Camera(void* pArg)
{
    CAMERA_COMPRE_DESC* pDesc = static_cast<CAMERA_COMPRE_DESC*>(pArg);

    if (pDesc->iCameraType == ENUM_CLASS(CAMERATYPE::SPRING))
    {
        CHECK_FAILED(Ready_Body(), E_FAIL);
    }

    return S_OK;
}

HRESULT CCamera_Compre::Ready_Body()
{
    CCharacterVirtual::CV_SPHERESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::SPHERE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 1.f;
    tCharVirDesc.fMaxSlopeAngle = 50.f;
    tCharVirDesc.pGameObject = this;
    tCharVirDesc.fSupportingVolume = Plane(Vec3::sAxisY(), -0.02f);
    tCharVirDesc.fPadding = 0.05f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);

    return S_OK;
}

CCamera_Compre::CAMERA_COMPRE_DESC CCamera_Compre::Get_Desc()
{
    CAMERA_COMPRE_DESC tDesc{};

    tDesc.fFar = m_fFar;
    tDesc.fFovy = m_fFovy;
    tDesc.fMouseSensor = m_fMouseSensor;
    tDesc.fNear = m_fNear;
    tDesc.iCameraType = m_iCameraType;
    tDesc.strCameraTag = m_strCameraTag;
    XMStoreFloat4(&tDesc.vEye, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&tDesc.vAt, m_pTransformCom->Get_State(STATE::LOOK));

    return tDesc;
}

CCamera_Compre* CCamera_Compre::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Compre* pInstance = new CCamera_Compre(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Compre"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Compre::Clone(void* pArg)
{
    CCamera_Compre* pInstance = new CCamera_Compre(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CCamera_Compre"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Compre::Free()
{
    __super::Free();

    Safe_Release(m_pCharVirCom);
}
