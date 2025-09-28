#include "PredicateManager.h"

void UPredicateManager::RegisterPredicate(const FString& PredicateName, PredicateFunction InPredicate)
{
    Predicates.Add(PredicateName, InPredicate);
}

PredicateFunction UPredicateManager::GetPredicate(const FString& Name) const
{
    const PredicateFunction* Predicate = Predicates.Find(Name);
    if (Predicate)
    {
        return *Predicate;
    }
    return nullptr;
}