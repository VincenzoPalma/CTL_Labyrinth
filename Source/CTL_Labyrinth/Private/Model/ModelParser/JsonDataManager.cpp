#include "Model/ModelParser/JsonDataManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UJsonDataManager* UJsonDataManager::Instance = nullptr;

UJsonDataManager* UJsonDataManager::GetInstance()
{
    if (!Instance)
    {
        Instance = NewObject<UJsonDataManager>();
    }
    return Instance;
}

bool UJsonDataManager::LoadJson(const FString& FilePath)
{
    if (JsonData.IsValid())
    {
        return true;
    }

    FString JsonString;

    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file from path: %s"), *FilePath);
        return false;
    }

    TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

    // Deserialize into JsonData
    if (FJsonSerializer::Deserialize(JsonReader, JsonData) && JsonData.IsValid())
    {
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON file from path: %s"), *FilePath);
        return false;
    }
}

TSharedPtr<FJsonObject> UJsonDataManager::GetJsonData() const
{
    return JsonData;
}

void UJsonDataManager::Shutdown()
{
    if (Instance)
    {
        Instance->RemoveFromRoot();
        Instance = nullptr;
    }
}
