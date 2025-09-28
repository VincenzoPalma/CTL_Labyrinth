#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "JsonDataManager.generated.h"

UCLASS()
class CTL_LABYRINTH_API UJsonDataManager : public UObject
{
    GENERATED_BODY()

public:
    static UJsonDataManager* GetInstance();

    bool LoadJson(const FString& FilePath);

    TSharedPtr<FJsonObject> GetJsonData() const;

    UFUNCTION(BlueprintCallable, Category = "JsonData")
    static void Shutdown();

private:
    UJsonDataManager() {}

    static UJsonDataManager* Instance;

    TSharedPtr<FJsonObject> JsonData;

};
