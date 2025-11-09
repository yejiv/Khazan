#include "Prefab.h"
#include "Effect_Element.h"

CPrefab::CPrefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject{pDevice, pDeviceContext}
{
}

CPrefab::CPrefab(const CPrefab& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CPrefab::Initialize_Clone(void* pArg)
{ 
    if (FAILED(__super::Initialize_Clone(&pArg)))
        return E_FAIL;

    m_fCurTime = 0.f;

    return S_OK;
}

void CPrefab::UpdatePosition(_fvector Pos)
{
    _matrix Parents_Translation = XMMatrixTranslationFromVector(Pos);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * Parents_Translation);
}

void CPrefab::UpdateWorldMatrix(_fvector Quaternion, _gvector Position)
{
    _matrix Paraent_Rotation = XMMatrixRotationQuaternion(Quaternion);
    _matrix Parents_Translation = XMMatrixTranslationFromVector(Position);
    _matrix Parentle = Paraent_Rotation * Parents_Translation;

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * Parentle);
}

void CPrefab::Add_Effect_Element(CEffect_Element* newElement)
{
    if (newElement)
        m_Children.push_back(newElement);
    else
        MSG_BOX(TEXT("Effect is null!"));
}

void CPrefab::SetClose()
{
    for(auto child : m_Children)
        child->SetLoopOff();
}


void CPrefab::Free()
{
    __super::Free();

    for (auto child : m_Children)
        Safe_Release(child);
}





