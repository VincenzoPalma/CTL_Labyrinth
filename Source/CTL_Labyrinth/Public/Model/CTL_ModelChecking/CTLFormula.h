#pragma once

#include "CoreMinimal.h"
#include "StatesUtils.h"
#include "../StateTree/StateNode.h"
#include "CTLFormula.generated.h"


UENUM(BlueprintType)
enum class ECTLOperator : uint8 {
    AND,
    OR,
    NOT,
    EX,
    AX,
    EF,
    AF,
    EG,
    AG,
    EU,
    AU
};

class UCTLModel;

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UCTLFormula : public UObject
{
    GENERATED_BODY()

public:
    virtual ~UCTLFormula() = default;

    UFUNCTION(BlueprintCallable, Category = "CTLFormula")
    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const PURE_VIRTUAL(UCTLFormula::Evaluate, return TArray<UStateNode*>(););

    virtual int32 CountSubformulas() const;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UAtomicBoolFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UAtomicBoolFormula();
    virtual bool EvaluatePredicate(UStateNode* node) const;

    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;

    void Initialize(TFunction<bool(const FState&)> InPredicate);

    virtual int32 CountSubformulas() const;
private:
    TFunction<bool(const FState&)> Predicate;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UAtomicIntFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UAtomicIntFormula();
    virtual bool EvaluatePredicate(UStateNode* node) const;

    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;

    void Initialize(TFunction<bool(const FState&)> InPredicate);

    virtual int32 CountSubformulas() const;
private:
    TFunction<bool(const FState&)> Predicate;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UAtomicDoubleFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UAtomicDoubleFormula();
    virtual bool EvaluatePredicate(UStateNode* node) const;

    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;

    void Initialize(TFunction<bool(const FState&)> InPredicate);

    virtual int32 CountSubformulas() const;
private:
    TFunction<bool(const FState&)> Predicate;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UAtomicStringFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UAtomicStringFormula();
    virtual bool EvaluatePredicate(UStateNode* node) const;

    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;

    void Initialize(TFunction<bool(const FState&)> InPredicate);

    virtual int32 CountSubformulas() const;
private:
    TFunction<bool(const FState&)> Predicate;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UUnaryFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UUnaryFormula();
    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;
    void Initialize(ECTLOperator InOp, UCTLFormula* InSubFormula);
    virtual int32 CountSubformulas() const;
private:
    ECTLOperator Op;
    UCTLFormula* SubFormula;
};

UCLASS(Blueprintable)
class CTL_LABYRINTH_API UBinaryFormula : public UCTLFormula
{
    GENERATED_BODY()

public:
    UBinaryFormula();
    virtual TArray<UStateNode*> Evaluate(const UCTLModel* model, UStateNode* node, TMap<int32, int32>& statesScores) const override;
    void Initialize(ECTLOperator InOp, UCTLFormula* InLeft, UCTLFormula* InRight);
    virtual int32 CountSubformulas() const;
private:
    ECTLOperator Op;
    UCTLFormula* Left;
    UCTLFormula* Right;
};
