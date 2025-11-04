#include "Prefab.h"

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

void CPrefab::Add_Effect_Element(CEffect_Element* newElement)
{
    if (newElement)
        m_Children.push_back(newElement);
    else
        MSG_BOX(TEXT("Effect is null!"));
}


void CPrefab::Free()
{
    __super::Free();
}





