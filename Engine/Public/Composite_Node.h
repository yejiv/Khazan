#pragma once
#include "BTNode.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComposite_Node abstract : public CBTNode
{
protected:
    CComposite_Node();
    virtual ~CComposite_Node() = default;

public:
    virtual BTNODESTATE         Tick(class CBlackBoard* BB) = 0;
    virtual void                Terminate(BTNODESTATE eState) {};
    virtual void                Abort() { Terminate(BTNODESTATE::FAILURE); };

    void                        Add_Child(CBTNode* pChild);

protected:
    vector<CBTNode*>            m_Children;
    _uint                       m_iCurrentIndex = {};

public:
    virtual void Free() override;
};

NS_END