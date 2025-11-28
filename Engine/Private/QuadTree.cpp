#include "QuadTree.h"
#include "GameInstance.h"


CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT])
		return S_OK;

	m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1;

	_uint			iLC, iTC, iRC, iBC;

	iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
	iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
	iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
	iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

	m_pChildren[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChildren[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
	m_pChildren[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
	m_pChildren[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
	if (nullptr == m_pChildren[0] ||
		true == isDraw(pGameInstance, pVertexPositions))
	{
		_uint		iIndices[] = {
			m_iCorners[0],
			m_iCorners[1],
			m_iCorners[2],
			m_iCorners[3],
		};

		_bool		isIn[4] = {
			pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVertexPositions[iIndices[0]]), 1.f)),
			pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVertexPositions[iIndices[1]]), 1.f)),
			pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVertexPositions[iIndices[2]]), 1.f)),
			pGameInstance->isIn_Frustum_LocalSpace(XMVectorSetW(XMLoadFloat3(&pVertexPositions[iIndices[3]]), 1.f)),
		};


		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbor[i])
				isDraw[i] = m_pNeighbor[i]->isDraw(pGameInstance, pVertexPositions);
		}

		/* 네 방향 이웃이 모두 나와 같은 사이즈로 그려질 때 */
		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[1];
				pIndices[(*pNumIndices)++] = iIndices[2];
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[2];
				pIndices[(*pNumIndices)++] = iIndices[3];
			}

			return;
		}

		_uint			iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

		/* 네 방향 이웃이 모두 나와 같은 사이즈로 그려지는 것이 아닐때 */
		/* 네 방향 중 어느쪽이 나보다 더 분열되어 그려지는지 색출.  */


		/* 왼쪽 하단 삼각형이 그려질 수 있는 조건이라면. */
		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			/* 왼쪽이 범인인지? */
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			/* 아래쪽이 범인인지? */
			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}

	/* 내 쿼드트리랑 절두체가 겹쳤니? \*/
	_float		fRange = XMVector3Length(XMLoadFloat3(&pVertexPositions[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVertexPositions[m_iCenter])).m128_f32[0];

	if (true == pGameInstance->isIn_Frustum_LocalSpace(XMLoadFloat3(&pVertexPositions[m_iCenter]), fRange))
	{
		for (size_t i = 0; i < CORNER_END; i++)
		{
			if (nullptr != m_pChildren[i])
				m_pChildren[i]->Culling(pGameInstance, pVertexPositions, pIndices, pNumIndices);
		}
	}
}

_bool CQuadTree::isDraw(CGameInstance* pGameInstance, const _float3* pVertexPositions)
{
	_vector	vCamPosition = XMLoadFloat4(pGameInstance->Get_CamPosition());
	_vector vCenterPos = XMLoadFloat3(&pVertexPositions[m_iCenter]);

	if (XMVector3Length(vCamPosition - vCenterPos).m128_f32[0] * 0.2f > (m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT]))
		return true;

	return false;
}

void CQuadTree::SetUp_Neighbors()
{
	if (nullptr == m_pChildren[CORNER_LT]->m_pChildren[CORNER_LT])
		return;

	m_pChildren[CORNER_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RT];
	m_pChildren[CORNER_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_LB];

	m_pChildren[CORNER_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChildren[CORNER_LT];
	m_pChildren[CORNER_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_RB];

	m_pChildren[CORNER_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChildren[CORNER_RT];
	m_pChildren[CORNER_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChildren[CORNER_LB];

	m_pChildren[CORNER_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChildren[CORNER_LT];
	m_pChildren[CORNER_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RB];

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChildren[CORNER_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChildren[CORNER_RT];
		m_pChildren[CORNER_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChildren[CORNER_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChildren[CORNER_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChildren[CORNER_LB];
		m_pChildren[CORNER_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChildren[CORNER_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChildren[CORNER_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChildren[CORNER_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_RT];
	}

	for (size_t i = 0; i < CORNER_END; i++)
	{
		m_pChildren[i]->SetUp_Neighbors();
	}
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX(TEXT("Failed to Created : CQuadTree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();

	for (auto& pChild : m_pChildren)
		Safe_Release(pChild);



}
