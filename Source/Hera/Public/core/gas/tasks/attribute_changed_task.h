// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "attribute_changed_task.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
   FAttributeChangedDelegate, 
   /*param 1*/FGameplayAttribute, Attribute,
   /*param 2*/float, NewValue, 
   /*param 3*/float, OldValue
);

/// Register an async listener for all attribute changes in an AbilitySystemComponent. 
/// Useful for binding to UI elements.
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class HERA_API UAttributeChangedTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FAttributeChangedDelegate AttributeChangedDelegate;
	
	// Listens for an attribute changing.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAttributeChangedTask* ListenForAttributeChange(
      UAbilitySystemComponent* AbilitySystemComponent, 
      FGameplayAttribute Attribute
   );

	// Listens for an attribute changing.
	// Version that takes in an array of Attributes. Check the Attribute output for which Attribute changed.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAttributeChangedTask* ListenForAttributesChange(
      UAbilitySystemComponent* AbilitySystemComponent, 
      TArray<FGameplayAttribute> Attributes
   );

	// You must call this function manually when you want the AsyncTask to end.
	// For UMG Widgets, you would call it in the Widget's Destruct event.
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeToListenFor;
	TArray<FGameplayAttribute> AttributesToListenFor;

	void OnAttributeChanged(const FOnAttributeChangeData& Data);
};
