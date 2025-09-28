#pragma once

#include "CoreMinimal.h"
#include "Model/StateTree/StateNode.h"
#include "Model/CTLModel.h"
#include "Model/ModelParser/ModelParser.h"
#include "Containers/Queue.h"
#include "Containers/Set.h"

class CTL_LABYRINTH_API AStar
{
public:
	AStar();
	~AStar();

	static TArray<UStateNode*> ExecuteAStar(UCTLModel* model, UStateNode* startingNode, UCTLFormula* formula);
	static void AddToOpenSet(TArray <UStateNode*>& openSet, UStateNode* node, TMap<int32, int32> fScores);
	static TArray<UStateNode*> ReconstructPath(UStateNode* currentNode, const TMap<int32, UStateNode*>& cameFrom);
	static void InitializeScores(const TMap<int32, int32>& statesScores, TMap<int32, int32>& gScore, TMap<int32, int32>& fScore);
	static void UpdateGScores(TMap<int32, int32>& gScores, int nodeId, int newValue);
	static void UpdateFScores(TMap<int32, int32>& fScores, int nodeId, TMap<int32, int32>& gScores, TMap<int32, int32>& statesScores);
};
