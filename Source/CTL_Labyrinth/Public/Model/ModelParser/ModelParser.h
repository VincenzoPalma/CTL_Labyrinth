#pragma once

#include "CoreMinimal.h"
#include "../ModelStructures.h"
#include "../CTL_ModelChecking/CTLFormula.h"
#include "Model/ModelParser/JsonDataManager.h"
#include "ModelParser.generated.h"

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UModelParser : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static UCTLModel* LoadModelFromJson(const FString& FilePath);
    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static UCTLModel* ParseStartingState(const FString& FilePath, int StartingStateId);
    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static UCTLModel* LoadPartialModelFromJson(const FString& FilePath, int StartingStateId);
    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static void UpdateModelFromNode(const FString& FilePath, UCTLModel* model, UStateNode* node);

    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static int FindStateWithProperties(const FString& FilePath, const TMap<FString, FVariantValue>& Properties);

    UFUNCTION(BlueprintCallable, Category = "Model Parsing")
    static UCTLModel* FindAndParseState(const FString& FilePath, const TMap<FString, FVariantValue>& Properties);


private:
    static void ParseStateById(const TArray<TSharedPtr<FJsonValue>>& StatesArray, int32 TargetId, UCTLModel* Model);
    static void ParseStates(const TArray<TSharedPtr<FJsonValue>>& StatesArray, UCTLModel* Model);
    static void ParseTransitions(const TArray<TSharedPtr<FJsonValue>>& TransitionsArray, UCTLModel* Model);
    static void ParseFormulas(const TArray<TSharedPtr<FJsonValue>>& FormulasArray, UCTLModel* Model);
    static ECTLOperator StringToOperator(const FString& OperatorString);
};