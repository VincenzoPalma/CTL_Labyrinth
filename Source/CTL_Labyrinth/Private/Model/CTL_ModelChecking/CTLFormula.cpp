#include "../Public/Model/CTL_ModelChecking/CTLFormula.h"
#include "Model/CTLModel.h"

UAtomicBoolFormula::UAtomicBoolFormula()
{
}

void UAtomicBoolFormula::Initialize(TFunction<bool(const FState&)> InPredicate)
{
    Predicate = InPredicate;
}

bool UAtomicBoolFormula::EvaluatePredicate(UStateNode* stateNode) const
{
    if (stateNode)
    {
        return Predicate(stateNode->GetState());
    }
    return false;
}

UAtomicIntFormula::UAtomicIntFormula()
{
}

void UAtomicIntFormula::Initialize(TFunction<bool(const FState&)> InPredicate)
{
    Predicate = InPredicate;
}

bool UAtomicIntFormula::EvaluatePredicate(UStateNode* stateNode) const
{
    if (stateNode)
    {
        return Predicate(stateNode->GetState());
    }
    return false;
}

UAtomicDoubleFormula::UAtomicDoubleFormula()
{
}

void UAtomicDoubleFormula::Initialize(TFunction<bool(const FState&)> InPredicate)
{
    Predicate = InPredicate;
}

bool UAtomicDoubleFormula::EvaluatePredicate(UStateNode* stateNode) const
{
    if (stateNode)
    {
        return Predicate(stateNode->GetState());
    }
    return false;
}

UAtomicStringFormula::UAtomicStringFormula()
{
}

void UAtomicStringFormula::Initialize(TFunction<bool(const FState&)> InPredicate)
{
    Predicate = InPredicate;
}

bool UAtomicStringFormula::EvaluatePredicate(UStateNode* stateNode) const
{
    if (stateNode)
    {
        return Predicate(stateNode->GetState());
    }
    return false;
}

TArray<UStateNode*> UAtomicBoolFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{

    TArray<UStateNode*> satisfyingStates;

    if (!model)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid model passed to Evaluate"));
        return satisfyingStates;
    }

    // If stateNode is null, verifies all states in the model
    if (!stateNode)
    {
        const TMap<int32, UStateNode*>& allStateNodes = model->GetStateNodes();
        for (const auto& StateNodeEntry : allStateNodes)
        {

            UStateNode* node = StateNodeEntry.Value;
            if (EvaluatePredicate(node))
            {
                satisfyingStates.Add(node);
            }
        }
    }
    else
    {
        // Verifies the predicate on the passed stateNode and all reachable nodes
        for (UStateNode* currentNode: model->GetReachableNodes(stateNode)){
            if (EvaluatePredicate(currentNode))
            {
                satisfyingStates.Add(currentNode);
            }
        }
    }

    return satisfyingStates;
}

TArray<UStateNode*> UAtomicIntFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{

    TArray<UStateNode*> satisfyingStates;

    if (!model)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid model passed to Evaluate"));
        return satisfyingStates;
    }

    // If stateNode is null, verifies all states in the model
    if (!stateNode)
    {
        const TMap<int32, UStateNode*>& allStateNodes = model->GetStateNodes();
        for (const auto& StateNodeEntry : allStateNodes)
        {

            UStateNode* node = StateNodeEntry.Value;
            if (EvaluatePredicate(node))
            {
                satisfyingStates.Add(node);
            }
        }
    }
    else
    {
        // Verifies the predicate on the passed stateNode and all reachable nodes
        for (UStateNode* currentNode : model->GetReachableNodes(stateNode)) {
            if (EvaluatePredicate(currentNode))
            {
                satisfyingStates.Add(currentNode);
            }
        }
    }

    return satisfyingStates;
}

TArray<UStateNode*> UAtomicDoubleFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{

    TArray<UStateNode*> satisfyingStates;

    if (!model)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid model passed to Evaluate"));
        return satisfyingStates;
    }

    // If stateNode is null, verifies all states in the model
    if (!stateNode)
    {
        const TMap<int32, UStateNode*>& allStateNodes = model->GetStateNodes();
        for (const auto& StateNodeEntry : allStateNodes)
        {

            UStateNode* node = StateNodeEntry.Value;
            if (EvaluatePredicate(node))
            {
                satisfyingStates.Add(node);
            }
        }
    }
    else
    {
        // Verifies the predicate on the passed stateNode and all reachable nodes
        for (UStateNode* currentNode : model->GetReachableNodes(stateNode)) {
            if (EvaluatePredicate(currentNode))
            {
                satisfyingStates.Add(currentNode);
            }
        }
    }

    return satisfyingStates;
}

TArray<UStateNode*> UAtomicStringFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{

    TArray<UStateNode*> satisfyingStates;

    if (!model)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid model passed to Evaluate"));
        return satisfyingStates;
    }

    // If stateNode is null, verifies all states in the model
    if (!stateNode)
    {
        const TMap<int32, UStateNode*>& allStateNodes = model->GetStateNodes();
        for (const auto& StateNodeEntry : allStateNodes)
        {

            UStateNode* node = StateNodeEntry.Value;
            if (EvaluatePredicate(node))
            {
                satisfyingStates.Add(node);
            }
        }
    }
    else
    {
        // Verifies the predicate on the passed stateNode and all reachable nodes
        for (UStateNode* currentNode : model->GetReachableNodes(stateNode)) {
            if (EvaluatePredicate(currentNode))
            {
                satisfyingStates.Add(currentNode);
            }
        }
    }

    return satisfyingStates;
}

UUnaryFormula::UUnaryFormula()
{
}

void UUnaryFormula::Initialize(ECTLOperator InOp, UCTLFormula* InSubFormula)
{
    Op = InOp;
    SubFormula = InSubFormula;
}

TArray<UStateNode*> UUnaryFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{
    // Temporary array to collect results
    TArray<UStateNode*> satisfyingStatesArray;

    // Set to ensure uniqueness
    TSet<UStateNode*> satisfyingStatesSet;

    // Check if model, stateNode, and SubFormula are valid
    if (!model || !SubFormula)
    {
        return satisfyingStatesArray;
    }

    // Evaluate the sub-formula
    TArray<UStateNode*> SubResults = SubFormula->Evaluate(model, stateNode, statesScores);

    switch (Op)
    {
    case ECTLOperator::NOT:
    {
        // Find all states not satisfying the sub-formula
        for (const auto& StateNode : model->GetReachableNodes(stateNode))
        {
            if (!SubResults.Contains(StateNode))
            {
                satisfyingStatesSet.Add(StateNode);
            }
        }
        
        satisfyingStatesArray = satisfyingStatesSet.Array();

        return satisfyingStatesArray;

        break;
    }

    case ECTLOperator::EX:
    {
        // Find all states from which there exists a successor satisfying the sub-formula
        TArray<UStateNode*> PreImage = model->PreImageExistential(SubResults, stateNode);
        for (UStateNode* Node : PreImage)
        {
            satisfyingStatesSet.Add(Node);
        }

        satisfyingStatesArray = satisfyingStatesSet.Array();

        break;
    }

    case ECTLOperator::AX:
    {
        // Find all states from which all successors satisfy the sub-formula
        TArray<UStateNode*> PreImage = model->PreImageUniversal(SubResults, stateNode);
        for (UStateNode* Node : PreImage)
        {
            satisfyingStatesSet.Add(Node);
        }

        satisfyingStatesArray = satisfyingStatesSet.Array();

        break;
    }

    case ECTLOperator::EF:
    {
        // Find all states where a path exists that contains at least a state satisfying the sub-formula

        TArray<UStateNode*> CurrentStates;

        do
        {
            CurrentStates = SubResults;
            TArray<UStateNode*> preImage = model->PreImageExistential(CurrentStates, stateNode);
            SubResults = StatesUtils::StatesUnion(SubResults, preImage);
        } while (!StatesUtils::IsSubSet(SubResults, CurrentStates));

        satisfyingStatesArray = SubResults;

        break;
    }

    case ECTLOperator::AF:
    {
        // Find all states where every path contains at least a state satisfying the sub-formula

        TArray<UStateNode*> CurrentStates;

        do
        {
            CurrentStates = SubResults;
            TArray<UStateNode*> preImage = model->PreImageUniversal(CurrentStates, stateNode);
            SubResults = StatesUtils::StatesUnion(SubResults, preImage);
        } while (!StatesUtils::IsSubSet(SubResults, CurrentStates));

        satisfyingStatesArray = SubResults;

        break;
    }

    case ECTLOperator::EG:
    {
        // Find all states where a path exists that is entirely within states satisfying the sub-formula
        TArray<UStateNode*> AllStates = model->GetReachableNodes(stateNode);

        while (!StatesUtils::IsSubSet(AllStates, SubResults))
        {
            AllStates = SubResults;
            SubResults = StatesUtils::StatesIntersection(model->PreImageExistential(AllStates, stateNode), SubResults);
        }

        satisfyingStatesArray = AllStates;

        break;
    }

    case ECTLOperator::AG:
    {
        // Find all states where every path is entirely within states satisfying the sub-formula
        TArray<UStateNode*> AllStates = model->GetReachableNodes(stateNode);

        while (!StatesUtils::IsSubSet(AllStates, SubResults))
        {
            AllStates = SubResults;
            SubResults = StatesUtils::StatesIntersection(model->PreImageUniversal(AllStates, stateNode), SubResults);
        }

        satisfyingStatesArray = AllStates;

        break;
    }

    default:
        break;
    }

    for (UStateNode* Node : satisfyingStatesArray)
    {
        statesScores.Add(Node->GetState().Id) = statesScores.FindOrAdd(Node->GetState().Id) - 1;
    }

    return satisfyingStatesArray;
}



UBinaryFormula::UBinaryFormula()
{
}

void UBinaryFormula::Initialize(ECTLOperator InOp, UCTLFormula* InLeft, UCTLFormula* InRight)
{
    Op = InOp;
    Left = InLeft;
    Right = InRight;
}

TArray<UStateNode*> UBinaryFormula::Evaluate(const UCTLModel* model, UStateNode* stateNode, TMap<int32, int32>& statesScores) const
{
    // Temporary array to collect results
    TArray<UStateNode*> satisfyingStatesArray;

    // Set to ensure uniqueness
    TSet<UStateNode*> satisfyingStatesSet;

    // Check if stateNode, Left, or Right are null
    if (!stateNode || !Left || !Right)
    {
        return satisfyingStatesArray;
    }

    // Evaluate the left and right formulas
    TArray<UStateNode*> leftStates = Left->Evaluate(model, stateNode, statesScores);
    TArray<UStateNode*> rightStates = Right->Evaluate(model, stateNode, statesScores);

    switch (Op)
    {
    case ECTLOperator::AND:
    {
        // Find states satisfying both Left and Right formulas
        for (UStateNode* leftNode : leftStates)
        {
            if (rightStates.Contains(leftNode))
            {
                satisfyingStatesSet.Add(leftNode);
            }
        }
        satisfyingStatesArray = satisfyingStatesSet.Array();

        break;
    }

    case ECTLOperator::OR:
    {
        // Find states satisfying either Left or Right formula
        for (UStateNode* node : leftStates)
        {
            satisfyingStatesSet.Add(node);
        }

        for (UStateNode* node : rightStates)
        {
            satisfyingStatesSet.Add(node);
        }
        satisfyingStatesArray = satisfyingStatesSet.Array();

        break;
    }

    case ECTLOperator::EU:
    {
        // Existential Until: States from which we can eventually reach a state satisfying Right,
        // while satisfying Left up to that point
        while (!StatesUtils::IsSubSet(rightStates, satisfyingStatesArray)) {
            satisfyingStatesArray = StatesUtils::StatesUnion(satisfyingStatesArray, rightStates);
            TArray<UStateNode*> preImage = model->PreImageExistential(satisfyingStatesArray, stateNode);
            rightStates = StatesUtils::StatesIntersection(preImage, leftStates);
        }
     
        break;
    }

    case ECTLOperator::AU:
    {
        // Always Until: States from which every path remains in states satisfying Right,
        // and eventually reaches a state satisfying Left
        while (!StatesUtils::IsSubSet(rightStates, satisfyingStatesArray)) {
            satisfyingStatesArray = StatesUtils::StatesUnion(satisfyingStatesArray, rightStates);
            TArray<UStateNode*> preImage = model->PreImageUniversal(satisfyingStatesArray, stateNode);
            rightStates = StatesUtils::StatesIntersection(preImage, leftStates);
        }

        break;
    }

    default:
        break;
    }

    for (UStateNode* Node : satisfyingStatesArray)
    {
        statesScores.Add(Node->GetState().Id) = statesScores.FindOrAdd(Node->GetState().Id) - 1;
    }

    return satisfyingStatesArray;
}

int32 UCTLFormula::CountSubformulas() const
{
    return 0;
}

int32 UAtomicDoubleFormula::CountSubformulas() const
{
    return 0;
}

int32 UAtomicBoolFormula::CountSubformulas() const
{
    return 0;
}

int32 UAtomicIntFormula::CountSubformulas() const
{
    return 0;
}

int32 UAtomicStringFormula::CountSubformulas() const
{
    return 0;
}

int32 UUnaryFormula::CountSubformulas() const
{
    if (Op == ECTLOperator::NOT)
    {
        return (SubFormula ? SubFormula->CountSubformulas() : 0);
    }
    else {
        return 1 + (SubFormula ? SubFormula->CountSubformulas() : 0);
    }
}

int32 UBinaryFormula::CountSubformulas() const
{
    int32 leftCount = Left ? Left->CountSubformulas() : 0;
    int32 rightCount = Right ? Right->CountSubformulas() : 0;
    return 1 + leftCount + rightCount;
}