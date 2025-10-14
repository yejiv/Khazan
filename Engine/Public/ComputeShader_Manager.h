#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CComputeShader_Manager final : public CBase
{
public:
	typedef struct tagComputeJobDesc
	{
		class CComputeShader* pShader = { nullptr };
		COMPUTE_PASS_DESC PassDesc;
	}COMPUTE_JOB_DESC;

private:
	CComputeShader_Manager();
	virtual ~CComputeShader_Manager() = default;

public:
	HRESULT		Initialize();

public:
	void		Add_Job(COMPUTEJOB eJobTag, const COMPUTE_JOB_DESC& Desc, _bool isExecuteNow);
	void		Execute_Job(COMPUTEJOB eJobTag);

private:
	vector<vector<COMPUTE_JOB_DESC>>	m_Jobs;

public:
	static CComputeShader_Manager*		Create();
	virtual void						Free() override;
};

NS_END