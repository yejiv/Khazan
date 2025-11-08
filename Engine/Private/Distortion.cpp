#include "Distortion.h"

CDistortion::CDistortion()
{
}

HRESULT CDistortion::Initialize()
{
	return S_OK;
}

void CDistortion::Update(_float fTimeDelta)
{
}

HRESULT CDistortion::Bind_Distortion_ShaderResources()
{
	return S_OK;
}

void CDistortion::Start_Distortion(const DISTORTION_DESC& Desc)
{
}

CDistortion* CDistortion::Create()
{
	return nullptr;
}

void CDistortion::Free()
{
	__super::Free();
}
