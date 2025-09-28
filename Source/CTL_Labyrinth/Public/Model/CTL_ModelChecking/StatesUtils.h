#pragma once

#include "CoreMinimal.h"
#include "Model/StateTree/StateNode.h"

class CTL_LABYRINTH_API StatesUtils
{
public:
	StatesUtils();
	~StatesUtils();

	static TArray<UStateNode*> StatesIntersection(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2);
	static TArray<UStateNode*> StatesUnion(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2);
	static bool IsSubSet(const TArray<UStateNode*>& set1, const TArray<UStateNode*>& set2);
};
