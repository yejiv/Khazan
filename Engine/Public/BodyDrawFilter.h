#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class DrawOnlyLayers final : public JPH::BodyDrawFilter
{
public:
    DrawOnlyLayers()
    {
    }

    void SetTargetPos(const Vec3& vPos)
    {
        m_vTargetPos = vPos;
    }

    bool ShouldDraw(const JPH::Body& b) const override
    {
        if (b.GetObjectLayer() == 0)
        {
            if (mLayers.find(b.GetObjectLayer()) != mLayers.end())
            {
                if ((m_vTargetPos - b.GetPosition()).Length() < 30.f)
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else
        {
            return mLayers.find(b.GetObjectLayer()) != mLayers.end();
        }        
    }

    void SetUp_LayerToFilter(_uint iObjectLayer)
    {
        mLayers.insert((ObjectLayer)iObjectLayer);
    }

    _bool Remove_LayerToFilter(_uint iObjectLayer)
    {
        const auto key = static_cast<ObjectLayer>(iObjectLayer);
        auto it = mLayers.find(key);
        if (it != mLayers.end())
        {
            mLayers.erase(it);        // 또는 mLayers.erase(key); (동일 효과)
            return true;              // 지웠음
        }
        return false;                 // 없었음
    }

private:
    std::unordered_set<JPH::ObjectLayer> mLayers;
    Vec3 m_vTargetPos = Vec3(0.f ,0.f ,0.f);
};

NS_END