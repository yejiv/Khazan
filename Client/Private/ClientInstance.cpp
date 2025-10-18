#include "ClientInstance.h"

IMPLEMENT_SINGLETON(CClientInstance)

CClientInstance::CClientInstance()
{

}

#pragma region Client

HRESULT CClientInstance::Initialize(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pDevice = *ppDevice;
	m_pContext = *ppContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);


	return S_OK;
}

void CClientInstance::Update(_float fTimeDelta)
{


}

#pragma endregion

void CClientInstance::Free()
{
	__super::Free();
	
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
