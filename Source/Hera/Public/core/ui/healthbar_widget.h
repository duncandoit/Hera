// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "healthbar_widget.generated.h"

class ACharacterBaseValid;
class UAbilitySystemComponentBase;

UCLASS()
class HERA_API UHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: 
   UFUNCTION(BlueprintPure, Category="Hera")
   ACharacterBaseValid* GetOwningCharacter() const { return OwningCharacter; }
   
	UFUNCTION(BlueprintPure, Category="Hera")
   UAbilitySystemComponentBase* GetOwningASC() const { return OwningASC; }

   UFUNCTION(BlueprintCallable, Category="Hera")
   void SetOwningCharacter(ACharacterBaseValid* NewOwningCharacter);

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

private:
	ACharacterBaseValid* OwningCharacter;
	UAbilitySystemComponentBase* OwningASC;
};
