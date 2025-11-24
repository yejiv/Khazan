#pragma once
#include "Composite_Node.h"
#include "UtilityAction_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUtilitySelector_Node final : public CComposite_Node
{
private:
    CUtilitySelector_Node();
    virtual ~CUtilitySelector_Node() = default;

public:
    virtual BTNODESTATE Tick(class CBlackBoard* BB) override;
    virtual void        Terminate(BTNODESTATE eState, class CBlackBoard* BB = nullptr) override;
    virtual void        Abort(class CBlackBoard* BB) override;
    virtual void        Add_Child(CBTNode* pNode) override;

private:
    vector<CUtilityAction_Node*>        m_UtilityChildren;
    CUtilityAction_Node*                m_pRunningAction = { nullptr };

public:
    static	CUtilitySelector_Node*  Create();
    virtual void			        Free() override;
};

NS_END
