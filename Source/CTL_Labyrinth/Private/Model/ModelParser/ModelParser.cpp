#include "Model/ModelParser/ModelParser.h"
#include "../Public/Model/CTLModel.h"
#include "Serialization/JsonSerializer.h"
#include "Json.h"
#include "JsonUtilities.h"


//Parses the whole model from a JSON file
UCTLModel* UModelParser::LoadModelFromJson(const FString& FilePath)
{
    UCTLModel* Model = NewObject<UCTLModel>();
    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();

    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {
                ParseStates(*StatesArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* TransitionsArray;
            if (JsonData->TryGetArrayField(TEXT("transitions"), TransitionsArray))
            {
                ParseTransitions(*TransitionsArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'transitions' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* FormulasArray;
            if (JsonData->TryGetArrayField(TEXT("formulas"), FormulasArray))
            {
                ParseFormulas(*FormulasArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'formulas' array or it is invalid."));
            }
        }
    }

    return Model;
}

//Parses a partial model, made by the state with the given id and it's adjacent states, from a JSON file
UCTLModel* UModelParser::LoadPartialModelFromJson(const FString& FilePath, int StartingStateId)
{
    UCTLModel* Model = NewObject<UCTLModel>();

    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();

    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {

                const TSharedPtr<FJsonObject> StateObject = (*StatesArray)[StartingStateId - 1]->AsObject();

                if (StateObject->HasField(TEXT("properties")))
                {
                    TSharedPtr<FJsonObject> PropertiesObject = StateObject->GetObjectField(TEXT("properties"));

                    FState State;
                    State.Id = StartingStateId;

                    for (const TPair<FString, TSharedPtr<FJsonValue>>& Property : PropertiesObject->Values)
                    {
                        FVariantValue PropertyValue;

                        if (Property.Value->Type == EJson::Boolean)
                        {
                            PropertyValue.SetBool(Property.Value->AsBool());
                        }
                        else if (Property.Value->Type == EJson::Number)
                        {
                            double NumberValue = Property.Value->AsNumber();
                            if (NumberValue == static_cast<int32>(NumberValue))
                            {
                                PropertyValue.SetInt(static_cast<int32>(NumberValue));
                            }
                            else
                            {
                                PropertyValue.SetDouble(NumberValue);
                            }
                        }
                        else if (Property.Value->Type == EJson::String)
                        {
                            PropertyValue.SetString(Property.Value->AsString());
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Property value for '%s' is invalid."), *Property.Key);
                        }
                        State.Properties.Add(Property.Key, PropertyValue);

                    }

                    Model->AddState(State);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("State with ID %d is missing 'properties' field."), StartingStateId);
                }

            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* TransitionsArray;
            if (JsonData->TryGetArrayField(TEXT("transitions"), TransitionsArray))
            {
                for (const TSharedPtr<FJsonValue>& TransitionValue : *TransitionsArray)
                {
                    const TSharedPtr<FJsonObject> TransitionObject = TransitionValue->AsObject();
                    if (TransitionObject->GetIntegerField(TEXT("from")) == StartingStateId)
                    {
                        int32 TargetId = TransitionObject->GetIntegerField(TEXT("to"));
                        ParseStateById(*StatesArray, TargetId, Model);

                        int32 FromId = TransitionObject->GetNumberField(TEXT("from"));
                        int32 ToId = TransitionObject->GetNumberField(TEXT("to"));
                        const TArray<TSharedPtr<FJsonValue>>* ActionsArray;
                        FActionsArray Actions;

                        if (TransitionObject->TryGetArrayField(TEXT("actions"), ActionsArray)) {
                            for (const TSharedPtr<FJsonValue>& ActionValue : *ActionsArray) {
                                Actions.Keys.Add(ActionValue->AsString());
                            }

                            const UStateNode* const* FromNodePtr = Model->GetStateNodes().Find(FromId);
                            const UStateNode* const* ToNodePtr = Model->GetStateNodes().Find(ToId);

                            if (FromNodePtr && ToNodePtr)
                            {
                                UStateNode* FromNode = const_cast<UStateNode*>(*FromNodePtr);
                                UStateNode* ToNode = const_cast<UStateNode*>(*ToNodePtr);

                                Model->AddTransition(Actions, FromNode, ToNode);
                            }
                            else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Failed to add transition from State ID: %d to State ID: %d. One or both states are not found."), FromId, ToId);
                            }
                        }
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'transitions' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* FormulasArray;
            if (JsonData->TryGetArrayField(TEXT("formulas"), FormulasArray))
            {
                ParseFormulas(*FormulasArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'formulas' array or it is invalid."));
            }
        }
    }
    
    return Model;
}

void UModelParser::UpdateModelFromNode(const FString& FilePath, UCTLModel* model, UStateNode* node)
{
    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();
    int32 StartingStateId = node->GetState().Id;
    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {
                const TArray<TSharedPtr<FJsonValue>>* TransitionsArray;
                if (JsonData->TryGetArrayField(TEXT("transitions"), TransitionsArray))
                {
                    TQueue<int32> OpenSet;
                    TSet<int32> Visited;

                    OpenSet.Enqueue(StartingStateId);
                    Visited.Add(StartingStateId);

                    while (!OpenSet.IsEmpty())
                    {
                        int32 CurrentStateId;
                        OpenSet.Dequeue(CurrentStateId);

                        for (const TSharedPtr<FJsonValue>& TransitionValue : *TransitionsArray)
                        {
                            const TSharedPtr<FJsonObject> TransitionObject = TransitionValue->AsObject();
                            if (TransitionObject->GetIntegerField(TEXT("from")) == CurrentStateId)
                            {
                                int32 TargetId = TransitionObject->GetIntegerField(TEXT("to"));
                                ParseStateById(*StatesArray, TargetId, model);

                                int32 FromId = TransitionObject->GetNumberField(TEXT("from"));
                                int32 ToId = TransitionObject->GetNumberField(TEXT("to"));
                                const TArray<TSharedPtr<FJsonValue>>* ActionsArray;
                                FActionsArray Actions;

                                if (TransitionObject->TryGetArrayField(TEXT("actions"), ActionsArray)) {
                                    for (const TSharedPtr<FJsonValue>& ActionValue : *ActionsArray) {
                                        Actions.Keys.Add(ActionValue->AsString());
                                    }


                                    const UStateNode* const* FromNodePtr = model->GetStateNodes().Find(FromId);
                                    const UStateNode* const* ToNodePtr = model->GetStateNodes().Find(ToId);

                                    if (FromNodePtr && ToNodePtr)
                                    {
                                        UStateNode* FromNode = const_cast<UStateNode*>(*FromNodePtr);
                                        UStateNode* ToNode = const_cast<UStateNode*>(*ToNodePtr);

                                        model->AddTransition(Actions, FromNode, ToNode);

                                        if (!Visited.Contains(ToId))
                                        {
                                            OpenSet.Enqueue(ToId);
                                            Visited.Add(ToId);
                                        }
                                    }
                                    else
                                    {
                                        UE_LOG(LogTemp, Warning, TEXT("Failed to add transition from State ID: %d to State ID: %d. One or both states are not found."), FromId, ToId);
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'transitions' array or it is invalid."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }
        }
    }
}


int UModelParser::FindStateWithProperties(const FString& FilePath, const TMap<FString, FVariantValue>& Properties)
{
    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();

    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {
                for (const TSharedPtr<FJsonValue>& StateValue : *StatesArray)
                {
                    const TSharedPtr<FJsonObject> StateObject = StateValue->AsObject();

                    const TSharedPtr<FJsonObject>* PropertiesObject;
                    if (StateObject->TryGetObjectField(TEXT("properties"), PropertiesObject))
                    {
                        bool bMatch = true;
                        for (const auto& PropertyPair : Properties)
                        {
                            const FString& Key = PropertyPair.Key;
                            const FVariantValue& ExpectedValue = PropertyPair.Value;

                            if (!(*PropertiesObject)->HasField(Key))
                            {
                                bMatch = false;
                                break;
                            }

                            if (ExpectedValue.Type == TEXT("bool"))
                            {
                                if ((*PropertiesObject)->GetBoolField(Key) != ExpectedValue.BoolValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("int"))
                            {
                                if ((*PropertiesObject)->GetIntegerField(Key) != ExpectedValue.IntValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("double"))
                            {
                                if ((*PropertiesObject)->GetNumberField(Key) != ExpectedValue.DoubleValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("string"))
                            {
                                if ((*PropertiesObject)->GetStringField(Key) != ExpectedValue.StringValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Unsupported type for property '%s'."), *Key);
                                bMatch = false;
                                break;
                            }
                        }

                        if (bMatch)
                        {
                            int32 StateId = StateObject->GetIntegerField(TEXT("id"));
                            return StateId;
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("State does not contain 'properties' object."));
                    }
                }

                UE_LOG(LogTemp, Warning, TEXT("No state found with the specified properties."));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }
        }
    }
    return -1;
}


void UModelParser::ParseStateById(const TArray<TSharedPtr<FJsonValue>>& StatesArray, int32 TargetId, UCTLModel* Model)
{
    //checks if the node is already in the model
    if (Model->GetStateNodes().Contains(TargetId))
    {
        return;
    }

    for (const TSharedPtr<FJsonValue>& StateValue : StatesArray)
    {
        const TSharedPtr<FJsonObject> StateObject = StateValue->AsObject();
        if (StateObject->GetIntegerField(TEXT("id")) == TargetId)
        {
            int32 Id = StateObject->GetNumberField(TEXT("id"));

            if (StateObject->HasField(TEXT("properties")))
            {
                TSharedPtr<FJsonObject> PropertiesObject = StateObject->GetObjectField(TEXT("properties"));

                FState State;
                State.Id = Id;

                for (const TPair<FString, TSharedPtr<FJsonValue>>& Property : PropertiesObject->Values)
                {
                    FVariantValue PropertyValue;

                    if (Property.Value->Type == EJson::Boolean)
                    {
                        PropertyValue.SetBool(Property.Value->AsBool());
                    }
                    else if (Property.Value->Type == EJson::Number)
                    {
                        double NumberValue = Property.Value->AsNumber();
                        if (NumberValue == static_cast<int32>(NumberValue))
                        {
                            PropertyValue.SetInt(static_cast<int32>(NumberValue));
                        }
                        else
                        {
                            PropertyValue.SetDouble(NumberValue);
                        }
                    }
                    else if (Property.Value->Type == EJson::String)
                    {
                        PropertyValue.SetString(Property.Value->AsString());
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Property value for '%s' is invalid."), *Property.Key);
                    }
                    State.Properties.Add(Property.Key, PropertyValue);
                }

                Model->AddState(State);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("State with ID %d is missing 'properties' field."), Id);
            }
            break;
        }
    }
}

UCTLModel* UModelParser::ParseStartingState(const FString& FilePath, int StartingStateId)
{
    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();
    UCTLModel* Model = NewObject<UCTLModel>();

    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
   
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {

                const TSharedPtr<FJsonObject> StateObject = (*StatesArray)[StartingStateId - 1]->AsObject();

                if (StateObject->HasField(TEXT("properties")))
                {
                    TSharedPtr<FJsonObject> PropertiesObject = StateObject->GetObjectField(TEXT("properties"));

                    FState State;
                    State.Id = StartingStateId;

                    for (const TPair<FString, TSharedPtr<FJsonValue>>& Property : PropertiesObject->Values)
                    {
                        FVariantValue PropertyValue;

                        if (Property.Value->Type == EJson::Boolean)
                        {
                            PropertyValue.SetBool(Property.Value->AsBool());
                        }
                        else if (Property.Value->Type == EJson::Number)
                        {
                            double NumberValue = Property.Value->AsNumber();
                            if (NumberValue == static_cast<int32>(NumberValue))
                            {
                                PropertyValue.SetInt(static_cast<int32>(NumberValue));
                            }
                            else
                            {
                                PropertyValue.SetDouble(NumberValue);
                            }
                        }
                        else if (Property.Value->Type == EJson::String)
                        {
                            PropertyValue.SetString(Property.Value->AsString());
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Property value for '%s' is invalid."), *Property.Key);
                        }
                        State.Properties.Add(Property.Key, PropertyValue);
                    }

                    Model->AddState(State);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("State with ID %d is missing 'properties' field."), StartingStateId);
                }

            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* FormulasArray;
            if (JsonData->TryGetArrayField(TEXT("formulas"), FormulasArray))
            {
                ParseFormulas(*FormulasArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'formulas' array or it is invalid."));
            }
        }
    }

    return Model;
}




void UModelParser::ParseStates(const TArray<TSharedPtr<FJsonValue>>& StatesArray, UCTLModel* Model)
{
    for (const TSharedPtr<FJsonValue>& StateValue : StatesArray)
    {
        TSharedPtr<FJsonObject> StateObject = StateValue->AsObject();
        if (StateObject.IsValid())
        {

            if (StateObject->HasField(TEXT("id")))
            {
                int32 Id = StateObject->GetNumberField(TEXT("id"));

                if (StateObject->HasField(TEXT("properties")))
                {
                    TSharedPtr<FJsonObject> PropertiesObject = StateObject->GetObjectField(TEXT("properties"));

                    FState State;
                    State.Id = Id;

                    for (const TPair<FString, TSharedPtr<FJsonValue>>& Property : PropertiesObject->Values)
                    {
                        FVariantValue PropertyValue;

                        if (Property.Value->Type == EJson::Boolean)
                        {
                            PropertyValue.SetBool(Property.Value->AsBool());
                        }
                        else if (Property.Value->Type == EJson::Number)
                        {
                            double NumberValue = Property.Value->AsNumber();
                            if (NumberValue == static_cast<int32>(NumberValue))
                            {
                                PropertyValue.SetInt(static_cast<int32>(NumberValue));
                            }
                            else
                            {
                                PropertyValue.SetDouble(NumberValue);
                            }
                        }
                        else if (Property.Value->Type == EJson::String)
                        {
                            PropertyValue.SetString(Property.Value->AsString());
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Property value for '%s' is invalid."), *Property.Key);
                        }
                        State.Properties.Add(Property.Key, PropertyValue);
                    }

                    Model->AddState(State);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("State with ID %d is missing 'properties' field."), Id);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("State object is missing 'id' field"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("State value is not a valid JSON object."));
        }
    }
}


void UModelParser::ParseTransitions(const TArray<TSharedPtr<FJsonValue>>& TransitionsArray, UCTLModel* Model)
{
    for (const TSharedPtr<FJsonValue>& TransitionValue : TransitionsArray)
    {
        TSharedPtr<FJsonObject> TransitionObject = TransitionValue->AsObject();
        if (TransitionObject.IsValid())
        {
            int32 FromId = TransitionObject->GetNumberField(TEXT("from"));
            int32 ToId = TransitionObject->GetNumberField(TEXT("to"));
            const TArray<TSharedPtr<FJsonValue>>* ActionsArray;
            FActionsArray Actions;

            if (TransitionObject->TryGetArrayField(TEXT("actions"), ActionsArray)) {
                for (const TSharedPtr<FJsonValue>& ActionValue : *ActionsArray) {
                    Actions.Keys.Add(ActionValue->AsString());
                }

                const UStateNode* const* FromNodePtr = Model->GetStateNodes().Find(FromId);
                const UStateNode* const* ToNodePtr = Model->GetStateNodes().Find(ToId);

                if (FromNodePtr && ToNodePtr)
                {
                    UStateNode* FromNode = const_cast<UStateNode*>(*FromNodePtr);
                    UStateNode* ToNode = const_cast<UStateNode*>(*ToNodePtr);

                    Model->AddTransition(Actions, FromNode, ToNode);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to add transition from State ID: %d to State ID: %d. One or both states are not found."), FromId, ToId);
                }
            }
        }
    }
}

void UModelParser::ParseFormulas(const TArray<TSharedPtr<FJsonValue>>& FormulasArray, UCTLModel* Model)
{
    auto PredicateManager = Model->GetPredicateManager();

    for (const TSharedPtr<FJsonValue>& FormulaValue : FormulasArray)
    {
        TSharedPtr<FJsonObject> FormulaObject = FormulaValue->AsObject();
        if (FormulaObject.IsValid())
        {
            // Extract the type and ID of the formula
            FString Type = FormulaObject->GetStringField(TEXT("type"));
            int32 Id = FormulaObject->GetNumberField(TEXT("id"));
            FVariantValue ComparisonValue;
            UCTLFormula* NewFormula = nullptr;

            if (Type == TEXT("AtomicBool"))
            {
                FString PredicateName = FormulaObject->GetStringField(TEXT("predicate"));

                if (!PredicateManager->GetPredicate(PredicateName))
                {
                    PredicateManager->RegisterPredicate(PredicateName, [PredicateName](const FState& State) -> bool {
                        if (State.Properties[PredicateName].Type.Equals("bool"))
                        {
                            return State.Properties[PredicateName].BoolValue;
                        }
                        return false;
                        });
                }

                NewFormula = NewObject<UAtomicBoolFormula>();
                auto Predicate = PredicateManager->GetPredicate(PredicateName);
                if (Predicate)
                {
                    UAtomicBoolFormula* AtomicFormula = Cast<UAtomicBoolFormula>(NewFormula);
                    AtomicFormula->Initialize(Predicate);
                    Model->AddFormula(Id, AtomicFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Predicate %s not found"), *PredicateName);
                }
            }
            else if (Type == TEXT("AtomicInt"))
            {
                FString PredicateName = FormulaObject->GetStringField(TEXT("predicate"));
                FString ComparisonType = FormulaObject->GetStringField(TEXT("comparison_type"));
                ComparisonValue.SetInt(FormulaObject->GetNumberField(TEXT("comparison_value")));

                if (!PredicateManager->GetPredicate(PredicateName))
                {
                    PredicateManager->RegisterPredicate(PredicateName, [PredicateName, ComparisonValue, ComparisonType](const FState& State) -> bool {
                        if (State.Properties[PredicateName].Type.Equals("int"))
                        {
                            int32 PropertyIntValue = State.Properties[PredicateName].IntValue;
                            if (ComparisonType == TEXT("="))
                            {
                                return PropertyIntValue == ComparisonValue.IntValue;
                            }
                            else if (ComparisonType == TEXT(">="))
                            {
                                return PropertyIntValue >= ComparisonValue.IntValue;
                            }
                            else if (ComparisonType == TEXT("<="))
                            {
                                return PropertyIntValue <= ComparisonValue.IntValue;
                            }
                        }
                        return false;
                        });
                }

                NewFormula = NewObject<UAtomicIntFormula>();
                auto Predicate = PredicateManager->GetPredicate(PredicateName);
                if (Predicate)
                {
                    UAtomicIntFormula* AtomicFormula = Cast<UAtomicIntFormula>(NewFormula);
                    AtomicFormula->Initialize(Predicate);
                    Model->AddFormula(Id, AtomicFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Predicate %s not found"), *PredicateName);
                }
            }
            else if (Type == TEXT("AtomicDouble"))
            {
                FString PredicateName = FormulaObject->GetStringField(TEXT("predicate"));
                FString ComparisonType = FormulaObject->GetStringField(TEXT("comparison_type"));
                ComparisonValue.SetDouble(FormulaObject->GetNumberField(TEXT("comparison_value")));

                if (!PredicateManager->GetPredicate(PredicateName))
                {
                    PredicateManager->RegisterPredicate(PredicateName, [PredicateName, ComparisonValue, ComparisonType](const FState& State) -> bool{
                        if (State.Properties[PredicateName].Type.Equals("double"))
                        {
                            double PropertyDoubleValue = State.Properties[PredicateName].DoubleValue;
                            if (ComparisonType == TEXT("="))
                            {
                                return PropertyDoubleValue == ComparisonValue.DoubleValue;
                            }
                            else if (ComparisonType == TEXT(">="))
                            {
                                return PropertyDoubleValue >= ComparisonValue.DoubleValue;
                            }
                            else if (ComparisonType == TEXT("<="))
                            {
                                return PropertyDoubleValue <= ComparisonValue.DoubleValue;
                            }
                        }
                        return false;
                        });
                }

                NewFormula = NewObject<UAtomicDoubleFormula>();
                auto Predicate = PredicateManager->GetPredicate(PredicateName);
                if (Predicate)
                {
                    UAtomicDoubleFormula* AtomicFormula = Cast<UAtomicDoubleFormula>(NewFormula);
                    AtomicFormula->Initialize(Predicate);
                    Model->AddFormula(Id, AtomicFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Predicate %s not found"), *PredicateName);
                }
            }
            else if (Type == TEXT("AtomicString"))
            {
                FString PredicateName = FormulaObject->GetStringField(TEXT("predicate"));
                ComparisonValue.SetString(FormulaObject->GetStringField(TEXT("comparison_value")));

                if (!PredicateManager->GetPredicate(PredicateName))
                {
                    PredicateManager->RegisterPredicate(PredicateName, [PredicateName, ComparisonValue](const FState& State) -> bool{
                        return State.Properties[PredicateName].StringValue.Equals(ComparisonValue.StringValue);
                        });
                }

                NewFormula = NewObject<UAtomicStringFormula>();
                auto Predicate = PredicateManager->GetPredicate(PredicateName);
                if (Predicate)
                {
                    UAtomicStringFormula* AtomicFormula = Cast<UAtomicStringFormula>(NewFormula);
                    AtomicFormula->Initialize(Predicate);
                    Model->AddFormula(Id, AtomicFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Predicate %s not found"), *PredicateName);
                }
            }
            else if (Type == TEXT("Unary"))
            {
                FString OperatorString = FormulaObject->GetStringField(TEXT("operator"));
                ECTLOperator Operator = StringToOperator(OperatorString);
                int32 SubFormulaId = FormulaObject->GetNumberField(TEXT("subformula_id"));

                NewFormula = NewObject<UUnaryFormula>();
                UCTLFormula* SubFormula = Model->GetFormula(SubFormulaId);
                if (SubFormula)
                {
                    UUnaryFormula* UnaryFormula = Cast<UUnaryFormula>(NewFormula);
                    UnaryFormula->Initialize(Operator, SubFormula);
                    Model->AddFormula(Id, UnaryFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Subformula with ID %d not found"), SubFormulaId);
                }
            }
            else if (Type == TEXT("Binary"))
            {
                FString OperatorString = FormulaObject->GetStringField(TEXT("operator"));
                ECTLOperator Operator = StringToOperator(OperatorString);
                int32 LeftId = FormulaObject->GetNumberField(TEXT("left_id"));
                int32 RightId = FormulaObject->GetNumberField(TEXT("right_id"));

                NewFormula = NewObject<UBinaryFormula>();
                UCTLFormula* LeftFormula = Model->GetFormula(LeftId);
                UCTLFormula* RightFormula = Model->GetFormula(RightId);
                if (LeftFormula && RightFormula)
                {
                    UBinaryFormula* BinaryFormula = Cast<UBinaryFormula>(NewFormula);
                    BinaryFormula->Initialize(Operator, LeftFormula, RightFormula);
                    Model->AddFormula(Id, BinaryFormula);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Left or right formula with IDs %d or %d not found"), LeftId, RightId);
                }
            }
        }
    }
}

ECTLOperator UModelParser::StringToOperator(const FString& OperatorString)
{
    if (OperatorString == TEXT("AND")) return ECTLOperator::AND;
    if (OperatorString == TEXT("OR")) return ECTLOperator::OR;
    if (OperatorString == TEXT("NOT")) return ECTLOperator::NOT;
    if (OperatorString == TEXT("EX")) return ECTLOperator::EX;
    if (OperatorString == TEXT("AX")) return ECTLOperator::AX;
    if (OperatorString == TEXT("EF")) return ECTLOperator::EF;
    if (OperatorString == TEXT("AF")) return ECTLOperator::AF;
    if (OperatorString == TEXT("EG")) return ECTLOperator::EG;
    if (OperatorString == TEXT("AG")) return ECTLOperator::AG;
    if (OperatorString == TEXT("EU")) return ECTLOperator::EU;
    if (OperatorString == TEXT("AU")) return ECTLOperator::AU;

    UE_LOG(LogTemp, Warning, TEXT("Unknown operator: %s"), *OperatorString);
    return ECTLOperator::AND;
}

UCTLModel* UModelParser::FindAndParseState(const FString& FilePath, const TMap<FString, FVariantValue>& Properties)
{
    UJsonDataManager* JsonManager = UJsonDataManager::GetInstance();
    UCTLModel* Model = NewObject<UCTLModel>();
    if (JsonManager->LoadJson(FilePath))
    {
        TSharedPtr<FJsonObject> JsonData = JsonManager->GetJsonData();

        if (JsonData.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* StatesArray;
            if (JsonData->TryGetArrayField(TEXT("states"), StatesArray))
            {
                for (const TSharedPtr<FJsonValue>& StateValue : *StatesArray)
                {
                    const TSharedPtr<FJsonObject> StateObject = StateValue->AsObject();

                    const TSharedPtr<FJsonObject>* PropertiesObject;
                    if (StateObject->TryGetObjectField(TEXT("properties"), PropertiesObject))
                    {
                        bool bMatch = true;
                        for (const auto& PropertyPair : Properties)
                        {
                            const FString& Key = PropertyPair.Key;
                            const FVariantValue& ExpectedValue = PropertyPair.Value;

                            if (!(*PropertiesObject)->HasField(Key))
                            {
                                bMatch = false;
                                break;
                            }

                            if (ExpectedValue.Type == TEXT("bool"))
                            {
                                if ((*PropertiesObject)->GetBoolField(Key) != ExpectedValue.BoolValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("int"))
                            {
                                if ((*PropertiesObject)->GetIntegerField(Key) != ExpectedValue.IntValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("double"))
                            {
                                if ((*PropertiesObject)->GetNumberField(Key) != ExpectedValue.DoubleValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else if (ExpectedValue.Type == TEXT("string"))
                            {
                                if ((*PropertiesObject)->GetStringField(Key) != ExpectedValue.StringValue)
                                {
                                    bMatch = false;
                                    break;
                                }
                            }
                            else
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Unsupported type for property '%s'."), *Key);
                                bMatch = false;
                                break;
                            }
                        }

                        if (bMatch)
                        {
                            FState State;
                            State.Id = StateObject->GetIntegerField(TEXT("id"));

                            for (const auto& Property : (*PropertiesObject)->Values)
                            {
                                FVariantValue PropertyValue;

                                if (Property.Value->Type == EJson::Boolean)
                                {
                                    PropertyValue.SetBool(Property.Value->AsBool());
                                }
                                else if (Property.Value->Type == EJson::Number)
                                {
                                    double NumberValue = Property.Value->AsNumber();
                                    if (NumberValue == static_cast<int32>(NumberValue))
                                    {
                                        PropertyValue.SetInt(static_cast<int32>(NumberValue));
                                    }
                                    else
                                    {
                                        PropertyValue.SetDouble(NumberValue);
                                    }
                                }
                                else if (Property.Value->Type == EJson::String)
                                {
                                    PropertyValue.SetString(Property.Value->AsString());
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Warning, TEXT("Property value for '%s' is invalid."), *Property.Key);
                                }
                                State.Properties.Add(Property.Key, PropertyValue);
                            }

                            Model->AddState(State);
                        }

                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("State does not contain 'properties' object."));
                    }
                }

            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'states' array or it is invalid."));
            }

            const TArray<TSharedPtr<FJsonValue>>* FormulasArray;
            if (JsonData->TryGetArrayField(TEXT("formulas"), FormulasArray))
            {
                ParseFormulas(*FormulasArray, Model);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("JSON file does not contain 'formulas' array or it is invalid."));
            }
        }
    }
    return Model;
}