#include "../Public/Model/CTLModel.h"
#include "Model/CTL_ModelChecking/CTLFormula.h"

UCTLModel::UCTLModel()
    : rootNode(nullptr)
{
}

void UCTLModel::PostInitProperties()
{
    Super::PostInitProperties();

    PredicateManager = NewObject<UPredicateManager>(this);

}

void UCTLModel::AddState(const FState& state)
{
    if (!stateNodes.Contains(state.Id))
    {
        UStateNode* NewNode = NewObject<UStateNode>();
        NewNode->SetStateData(state);

        stateNodes.Add(state.Id, NewNode);

        if (!rootNode)
        {
            rootNode = NewNode;
        }
    }
}

void UCTLModel::AddTransition(FActionsArray actions, UStateNode* FromNode, UStateNode* ToNode)
{
    if (FromNode && ToNode)
    {
        FromNode->AddChild(actions, ToNode);
    }
}


void UCTLModel::AddFormula(int32 FormulaId, UCTLFormula* Formula)
{
    if (Formula && !Formulas.Contains(FormulaId))
    {
        Formulas.Add(FormulaId, Formula);
    }
}

UCTLFormula* UCTLModel::GetFormula(int32 Id) const
{
    const UCTLFormula* const* FoundFormula = Formulas.Find(Id);
    if (FoundFormula)
    {
        return const_cast<UCTLFormula*>(*FoundFormula);
    }
    return nullptr;
}

TArray<UStateNode*> UCTLModel::PreImageExistential(const TArray<UStateNode*>& states, UStateNode* StartNode) const
{
    TArray<UStateNode*> PreImage;

    if (!StartNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid StartNode passed to PreImageExistential"));
        return PreImage;
    }

    TArray<UStateNode*> ReachableStates = GetReachableNodes(StartNode);

    for (UStateNode* StateNode : ReachableStates)
    {

        bool HasSuccessorInQ = false;
        for (UStateNode* Successor : StateNode->GetChildren())
        {
            if (states.Contains(Successor))
            {
                HasSuccessorInQ = true;
                break;
            }
        }

        if (HasSuccessorInQ)
        {
            PreImage.Add(StateNode);
        }
    }

    return PreImage;
}

TArray<UStateNode*> UCTLModel::PreImageUniversal(const TArray<UStateNode*>& states, UStateNode* StartNode) const
{
    TArray<UStateNode*> PreImage;

    if (!StartNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid StartNode passed to PreImageUniversal"));
        return PreImage;
    }

    TArray<UStateNode*> ReachableStates = GetReachableNodes(StartNode);

    for (UStateNode* StateNode : ReachableStates)
    {
        bool AllSuccessorsInQ = true;
        TArray<UStateNode*> Successors = StateNode->GetChildren();

        if (Successors.Num() == 0)
        {
            AllSuccessorsInQ = false;
        }
        else
        {
            for (UStateNode* Successor : Successors)
            {
                if (!states.Contains(Successor))
                {
                    AllSuccessorsInQ = false;
                    break;
                }
            }
        }

        if (AllSuccessorsInQ)
        {
            PreImage.Add(StateNode);
        }
    }

    return PreImage;
}



TArray<UStateNode*> UCTLModel::GetReachableNodes(UStateNode* StartNode) const
{
    TArray<UStateNode*> ReachableNodes;

    if (!StartNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartNode is null."));
        return ReachableNodes;
    }

    TQueue<UStateNode*> Queue;
    Queue.Enqueue(StartNode);

    TSet<UStateNode*> Visited;

    while (!Queue.IsEmpty())
    {
        UStateNode* CurrentNode;
        Queue.Dequeue(CurrentNode);

        if (Visited.Contains(CurrentNode))
        {
            continue;
        }

        Visited.Add(CurrentNode);
        ReachableNodes.Add(CurrentNode);

        for (UStateNode* Successor : CurrentNode->GetChildren())
        {
            if (!Visited.Contains(Successor))
            {
                Queue.Enqueue(Successor);
            }
        }
    }

    return ReachableNodes;
}

void UCTLModel::DebugPrintModel() const
{
    if (GEngine)
    {
        // Print root node
        if (rootNode)
        {
            FString RootNodeMessage = FString::Printf(TEXT("Root Node ID: %d"), rootNode->GetState().Id);
            UE_LOG(LogTemp, Log, TEXT("%s"), *RootNodeMessage);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Root Node is not set."));
        }

        // Print states and their data
        for (const auto& StatePair : stateNodes)
        {
            const UStateNode* StateNode = StatePair.Value;
            FString StateMessage = FString::Printf(TEXT("State ID: %d"), StateNode->GetState().Id);
            UE_LOG(LogTemp, Log, TEXT("%s"), *StateMessage);

            for (const TPair<FString, FVariantValue>& Property : StateNode->GetState().Properties)
            {
                FString PropertyMessage;

                if (Property.Value.Type.Equals("bool"))
                {
                    bool BoolValue = Property.Value.BoolValue;
                    PropertyMessage = FString::Printf(TEXT("Bool Property: %s = %s"), *Property.Key, BoolValue ? TEXT("true") : TEXT("false"));
                }
                else if (Property.Value.Type.Equals("int"))
                {
                    int32 IntValue = Property.Value.IntValue;
                    PropertyMessage = FString::Printf(TEXT("Integer Property: %s = %d"), *Property.Key, IntValue);
                }
                else if (Property.Value.Type.Equals("double"))
                {
                    double DoubleValue = Property.Value.DoubleValue;
                    PropertyMessage = FString::Printf(TEXT("Double Property: %s = %f"), *Property.Key, DoubleValue);
                }
                else if (Property.Value.Type.Equals("string"))
                {
                    FString StringValue = Property.Value.StringValue;
                    PropertyMessage = FString::Printf(TEXT("String Property: %s = %s"), *Property.Key, *StringValue);
                }
                else
                {
                    PropertyMessage = FString::Printf(TEXT("  Property: %s has an unsupported type"), *Property.Key);
                }

                UE_LOG(LogTemp, Log, TEXT("%s"), *PropertyMessage);
            }

            // Print child nodes
            for (const auto& Pair : StateNode->GetChildrenMap())
            {
                const FActionsArray& Action = Pair.Key;               
                const UStateNode* ChildNode = Pair.Value;      
                FString ActionString = FString::Join(Action.Keys, TEXT(", "));
                FString ChildMessage = FString::Printf(TEXT("  Child State ID: %d, Actions: [%s]"), ChildNode->GetState().Id, *ActionString);
                UE_LOG(LogTemp, Log, TEXT("%s"), *ChildMessage);
            }
        }
        // Print formulas
        for (const auto& FormulaPair : GetFormulas())
        {

            const UCTLFormula* Formula = FormulaPair.Value;
            if (Formula)
            {
                FString FormulaMessage = FString::Printf(TEXT("Formula ID: %d, Type: %s"), FormulaPair.Key, *Formula->GetClass()->GetName());
                UE_LOG(LogTemp, Log, TEXT("%s"), *FormulaMessage);
            }
        }

        // Print predicates
        const UPredicateManager* PredManager = GetPredicateManager();
        if (PredManager)
        {
            for (const auto& PredicatePair : PredManager->GetPredicates())
            {

                FString PredicateName = PredicatePair.Key;
                FString PredicateMessage = FString::Printf(TEXT("Predicate: %s"), *PredicateName);
                UE_LOG(LogTemp, Log, TEXT("%s"), *PredicateMessage);
            }
        }
    }
}

TArray<UStateNode*> UCTLModel::EvaluateFormula(UStateNode* node, UCTLFormula* formula)
{
    TArray<UStateNode*> result = AStar::ExecuteAStar(this, node, formula);
    return result;
}

void UCTLModel::UpdateModel(UStateNode* node, UCTLFormula* formula, TMap<int32, int32>& statesScores)
{
    UModelParser::UpdateModelFromNode("C:\\Users\\vince\\Documents\\Unreal Projects\\CTL_Labyrinth\\Source\\CTL_Labyrinth\\ModelFiles\\CTLLabyrinthModelNew.json", this, node);
    int subFormulasNum = formula->CountSubformulas();
    for (const UStateNode* currNode : GetReachableNodes(node))
    {
        statesScores.Add(currNode->GetState().Id, subFormulasNum);
    }
    TArray<UStateNode*> result = formula->Evaluate(this, node, statesScores);
}