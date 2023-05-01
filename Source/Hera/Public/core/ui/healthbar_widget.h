// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "healthbar_widget.generated.h"

class ACharacterBase;

/// TODO: Zach - 4/24/23
// - Figure out why the OwningCharacter is null when referenced by the UMG graph

UCLASS()
class HERA_API UHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	ACharacterBase* OwningCharacter;
	
public: 
   UFUNCTION(BlueprintPure, Category="Hera")
   ACharacterBase* GetOwningCharacter() const { return OwningCharacter; }

   UFUNCTION(BlueprintCallable, Category="Hera")
   void SetOwningCharacter(ACharacterBase* NewOwningCharacter);

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetCurrentHealth(float CurrentHealth);

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetMaxShields(float MaxShields);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetCurrentShields(float CurrentShields);

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetMaxArmor(float MaxArmor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetCurrentArmor(float CurrentArmor);

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetOverHealth(float OverHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Hera")
	void SetOverArmor(float OverArmor);
};
