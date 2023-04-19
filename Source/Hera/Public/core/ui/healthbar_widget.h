// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "healthbar_widget.generated.h"

UCLASS()
class HERA_API UHealthbarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public: 
   UPROPERTY(BlueprintReadOnly)
	class ACharacterBase* OwningCharacter;

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
   void DecreaseHealthPool(
      float Health,
      float Shields,
      float Armor,
      float OverHealth,
      float OverArmor,
		float AmountHealed
   );

   UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
   void IncreaseHealthPool(
      float Health,
      float Shields,
      float Armor,
      float OverHealth,
      float OverArmor,
		float AmountHealed
   );

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPoolPercentage(float HealthPoolPercentage);
};
