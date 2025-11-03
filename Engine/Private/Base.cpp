#include "../Public/Base.h"

CBase::CBase()
{

}

unsigned int CBase::AddRef()
{
	lock_guard<mutex> lock(mutex);

	return ++m_iRefCnt;
}

unsigned int CBase::Release()
{
	lock_guard<mutex> lock(mutex);
	if (0 == m_iRefCnt)
	{
		Free();

		/* 삭제한다. */
		delete this;

		return 0;
	}
	else
	{
		return m_iRefCnt--;
	}
}

void CBase::Free()
{
}
