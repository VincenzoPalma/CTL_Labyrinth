#include "AStar.h"

AStar::AStar()
{
}

AStar::~AStar()
{
}

TArray<UStateNode*> AStar::ExecuteAStar(UCTLModel* model, UStateNode* startingNode, UCTLFormula* formula)
{
	//Structures for the path, if found
	TArray<UStateNode*> finalPath;
	TMap<int32, UStateNode*> cameFrom;
	//Initialization and evaluation of the score for the starting state
	int subFormulasNum = formula->CountSubformulas();
	TMap<int32, int32> statesScores;
	statesScores.Add(startingNode->GetState().Id, subFormulasNum);
	formula->Evaluate(model, startingNode, statesScores);

	//Initialization of the g and f values for the starting state
	TMap<int32, int32> gScores;
	TMap<int32, int32> fScores;
	int startingNodeId = startingNode->GetState().Id;
	UpdateGScores(gScores, startingNodeId, 0);
	UpdateFScores(fScores, startingNodeId, gScores, statesScores);

	//Initialization of the priority queue
	TArray<UStateNode*> openSet;
	AddToOpenSet(openSet, startingNode, fScores);

	TSet<UStateNode*> closedSet;

	//Main loop
	while (!openSet.IsEmpty()) {
		//The state in position 0 will always be the best choice
		UStateNode* currentNode = openSet[0];
		openSet.RemoveAt(0);

		//Get the adjacent states, updating the model and initializing their scores
		if (currentNode->GetChildren().IsEmpty())
		{
			model->UpdateModel(currentNode, formula, statesScores);
			InitializeScores(statesScores, gScores, fScores);
		}

		//Add state to closed set so that it will not be visited in the future
		closedSet.Add(currentNode);

		//If the current state satifies the entire formula, then it is a target state
		for (UStateNode* node : closedSet) {
			if (*statesScores.Find(node->GetState().Id) == 0) {
				finalPath = ReconstructPath(currentNode, cameFrom);
				return finalPath;
			}
		}

		
		//Loop for each adjacent state of the current state
		for (UStateNode* node : currentNode->GetChildren()) {
			int nodeId = node->GetState().Id;

			//Skip it if it was already visited
			if (closedSet.Contains(node)) {
				continue;
			}

			//Calculate a possible new g score
			int tentative_gScore = *gScores.Find(currentNode->GetState().Id) + 1; //1 in this case. The weight of the edge generally.
			//Update the scores and add the state to the cameFrom map, used to build the resulting path
			if (tentative_gScore < *gScores.Find(nodeId)) {
				UpdateGScores(gScores, nodeId, tentative_gScore);
				UpdateFScores(fScores, nodeId, gScores, statesScores);
				cameFrom.Add(nodeId, currentNode);

				//Add it to the priority queue
				if (!openSet.Contains(node))
				{
					AddToOpenSet(openSet, node, fScores);
				}
			}
		}
	}
	return finalPath; //Empty array if no path was found
}

void AStar::AddToOpenSet(TArray<UStateNode*>& openSet, UStateNode* node, TMap<int32, int32> fScores)
{
	if (openSet.IsEmpty()) {
		openSet.Add(node);
	}
	else {
		int32 newNodeScore = *fScores.Find(node->GetState().Id);

		bool added = false;
		for (int32 i = 0; i < openSet.Num(); ++i) {
			UStateNode* currentNode = openSet[i];
			int32 currentNodeScore = *fScores.Find(currentNode->GetState().Id);

			if (newNodeScore < currentNodeScore) {
				openSet.Insert(node, i);
				added = true;
				break;
			}
		}

		if (!added) {
			openSet.Add(node);
		}
	}
}

void AStar::UpdateGScores(TMap<int32, int32>& gScores, int nodeId, int newValue)
{
	gScores.Remove(nodeId);
	gScores.Add(nodeId, newValue);
}

void AStar::UpdateFScores(TMap<int32, int32>& fScores, int nodeId, TMap<int32, int32>& gScores, TMap<int32, int32>& statesScores)
{
	fScores.Remove(nodeId);
	fScores.Add(nodeId, *gScores.Find(nodeId) + *statesScores.Find(nodeId));
}

TArray<UStateNode*> AStar::ReconstructPath(UStateNode* currentNode, const TMap<int32, UStateNode*>& cameFrom)
{
	TArray<UStateNode*> totalPath;
	totalPath.Add(currentNode);

	while (cameFrom.Contains(currentNode->GetState().Id))
	{
		currentNode = cameFrom[currentNode->GetState().Id];
		totalPath.Add(currentNode);
	}

	Algo::Reverse(totalPath);
	return totalPath;
}

void AStar::InitializeScores(const TMap<int32, int32>& statesScores, TMap<int32, int32>& gScore, TMap<int32, int32>& fScore)
{
	for (const TPair<int32, int32>& entry : statesScores)
	{
		int32 key = entry.Key;

		if (!gScore.Contains(key)) {
			gScore.Add(key, MAX_int32);
		}
		if (!fScore.Contains(key)) {
			fScore.Add(key, MAX_int32);
		}
	}
}
