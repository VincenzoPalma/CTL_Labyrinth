#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ModelStructures.generated.h"

USTRUCT(BlueprintType)
struct FVariantValue
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Variant")
    FString Type;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Variant")
    FString StringValue;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Variant")
    int32 IntValue;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Variant")
    double DoubleValue;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Variant")
    bool BoolValue;

    FVariantValue() : Type(TEXT("None")), StringValue(TEXT("")), IntValue(0), DoubleValue(0.0f), BoolValue(false) {}

    void SetBool(bool InValue)
    {
        Type = TEXT("bool");
        BoolValue = InValue;
    }

    void SetInt(int32 InValue)
    {
        Type = TEXT("int");
        IntValue = InValue;
    }

    void SetDouble(float InValue)
    {
        Type = TEXT("double");
        DoubleValue = InValue;
    }

    void SetString(const FString& InValue)
    {
        Type = TEXT("string");
        StringValue = InValue;
    }
};

USTRUCT(BlueprintType)
struct FState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
    int32 Id;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
    TMap<FString, FVariantValue> Properties;

};

USTRUCT(BlueprintType)
struct FTransition
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transition")
    FState From;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transition")
    FState To;
};

USTRUCT(BlueprintType)
struct FActionsArray
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FString> Keys;

    bool operator==(const FActionsArray& Other) const
    {
        return Keys == Other.Keys;
    }
};

FORCEINLINE uint32 GetTypeHash(const FActionsArray& Key)
{
    uint32 Hash = 0;
    for (const FString& Str : Key.Keys)
    {
        Hash = HashCombine(Hash, GetTypeHash(Str));
    }
    return Hash;
}