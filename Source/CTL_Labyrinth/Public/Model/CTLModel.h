#pragma once

#include "StateTree/StateNode.h"
#include "PredicateManager.h"
#include "Model/CTL_ModelChecking/CTLFormula.h"
#include "Model/ModelParser/ModelParser.h"
#include "CoreMinimal.h"
#include "ModelStructures.h"
#include "HAL/PlatformTime.h"
#include "AStar.h"
#include "CTLModel.generated.h"

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UCTLModel : public UObject
{
    GENERATED_BODY()

public:
    UCTLModel();

    void AddState(const FState& state);
    void AddTransition(FActionsArray actions, UStateNode* FromNode, UStateNode* ToNode);
    void AddFormula(int32 FormulaId, UCTLFormula* Formula);

    UFUNCTION(BlueprintCallable, Category = "Model")
    UCTLFormula* GetFormula(int32 Id) const;

    UFUNCTION(BlueprintCallable, Category = "Model")
    UStateNode* GetRootNode() const {
        return rootNode;
    }

    UFUNCTION(BlueprintCallable, Category = "Model")
    const TMap<int32, UStateNode*>& GetStateNodes() const { 
        return stateNodes; 
    }

    UFUNCTION(BlueprintCallable, Category = "Model")
    const TMap<int32, UCTLFormula*>& GetFormulas() const {
        return Formulas;
    }

    UFUNCTION(BlueprintCallable, Category = "Model")
    UPredicateManager* GetPredicateManager() const
    {
        return PredicateManager;
    }

    UFUNCTION(BlueprintCallable, Category = "Model")
    TArray<UStateNode*> GetReachableNodes(UStateNode* StartNode) const;

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void DebugPrintModel() const;


    virtual void PostInitProperties() override;

    TArray<UStateNode*> PreImageUniversal(const TArray<UStateNode*>& states, UStateNode* StartNode) const;

    TArray<UStateNode*> PreImageExistential(const TArray<UStateNode*>& states, UStateNode* StartNode) const;

    UFUNCTION(BlueprintCallable, Category = "Model")
    TArray<UStateNode*> EvaluateFormula(UStateNode* node, UCTLFormula* formula);

    void UpdateModel(UStateNode* node, UCTLFormula* formula, TMap<int32, int32>& statesScores);
private:
    UPROPERTY()
    TMap<int32, UStateNode*> stateNodes;

    UPROPERTY()
    UStateNode* rootNode;

    UPROPERTY()
    TMap<int32, UCTLFormula*> Formulas;

    UPROPERTY()
    UPredicateManager* PredicateManager;

};
