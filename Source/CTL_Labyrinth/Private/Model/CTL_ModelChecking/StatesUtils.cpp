#include "Model/CTL_ModelChecking/StatesUtils.h"

StatesUtils::StatesUtils()
{
}

StatesUtils::~StatesUtils()
{
}

TArray<UStateNode*> StatesUtils::StatesIntersection(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2)
{
    TArray<UStateNode*> result;
    for (UStateNode* node : set1)
    {
        if (set1.Contains(node) && set2.Contains(node))
        {
            result.Add(node);
        }
    }
	return result;
}

TArray<UStateNode*> StatesUtils::StatesUnion(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2)
{
    TSet<UStateNode*> result;
    for (UStateNode* node : set1)
    {
        result.Add(node);
    }

    for (UStateNode* node : set2)
    {
        result.Add(node);
    }
    return result.Array();
}

bool StatesUtils::IsSubSet(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2)
{
    TSet<UStateNode*> set2Set(set2);

    for (UStateNode* Node : set1)
    {
        if (!set2Set.Contains(Node))
        {
            return false;
        }
    }

    return true;
}