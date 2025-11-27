#include "Target_Manager.h"

#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
	return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, _bool isClear, ID3D11DepthStencilView* pDSV)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	m_pContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pOriginalDSV);

	_uint		iNumRenderTargets = {};

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr };

	for (auto& pRenderTarget : *pMRTList)
	{
		if (true == isClear)
			pRenderTarget->Clear();

		RenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	}

	if (nullptr != pDSV)
		m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pContext->OMSetRenderTargets(iNumRenderTargets, RenderTargets, nullptr == pDSV ? m_pOriginalDSV : pDSV);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBuffer, m_pOriginalDSV);

	Safe_Release(m_pOriginalDSV);
	Safe_Release(m_pBackBuffer);

	return S_OK;
}

HRESULT CTarget_Manager::Copy_Resource(const _wstring& strTargetTag, ID3D11Texture2D* pSourTexture)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Copy_Resource(pSourTexture);
}

HRESULT CTarget_Manager::Copy_Resource(const _wstring& strDestTargetTag, const _wstring& strSourTargetTag)
{
    CRenderTarget* pSourRenderTarget = Find_RenderTarget(strSourTargetTag);
    if (nullptr == pSourRenderTarget)
        return E_FAIL;

    CRenderTarget* pDestRenderTarget = Find_RenderTarget(strDestTargetTag);
    if (nullptr == pDestRenderTarget)
        return E_FAIL;

    ID3D11Texture2D* pDestTexture = pDestRenderTarget->Get_Texture2D();

    return pSourRenderTarget->Copy_Resource(pDestTexture);
}

void CTarget_Manager::Backup_RT()
{
	m_pContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pOriginalDSV);
}

void CTarget_Manager::Restore_RT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBuffer, m_pOriginalDSV);

	Safe_Release(m_pOriginalDSV);
	Safe_Release(m_pBackBuffer);
}

HRESULT CTarget_Manager::Apply_MRT_OnContext(const wstring& mrtTag, ID3D11DeviceContext* pCtx, ID3D11DepthStencilView* pDSV, bool isClear)
{
	auto* pList = Find_MRT(mrtTag); // 내부에서 list<CRenderTarget*> 반환
	if (!pList) return E_FAIL;

	ID3D11RenderTargetView* rtvs[8] = {};
	UINT cnt = 0;
	for (auto* rt : *pList) {
		if (isClear && rt) rt->Clear(); // 클리어는 보통 메인에서 1회만
		rtvs[cnt++] = rt ? rt->Get_RTV() : nullptr;
	}

	// DSV 선택: 주어진 pDSV가 있으면 그걸, 없으면 “현재 DSV”를 넘겨도 됨
	ID3D11DepthStencilView* dsv = pDSV ? pDSV : m_pOriginalDSV; // 구현에 맞춰 가져오기

	pCtx->OMSetRenderTargets(cnt, rtvs, dsv);
	return S_OK;
}

ID3D11DepthStencilView* CTarget_Manager::Get_CurrentDSV_AddRef()
{
	if (!m_pOriginalDSV) return nullptr;
	m_pOriginalDSV->AddRef();
	return m_pOriginalDSV;
}

#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return  pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CTarget_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    for (auto& Pair : m_RenderTargets)
    {
        if (nullptr != Pair.second)
            Pair.second->Render(pShader, pVIBuffer);
    }

	//  for (auto& Pair : m_MRTs)
	//  {
	//  	for (auto& pRenderTarget : Pair.second)
	//  	{
	//  		if (nullptr != pRenderTarget)
	//  			pRenderTarget->Render(pShader, pVIBuffer);
	//  	}
	//  }

	return S_OK;
}

#endif

CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
	auto	iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
		return nullptr;


	return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto	iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CTarget_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTarget_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);
		Pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
