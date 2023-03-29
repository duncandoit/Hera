// Copyright Final Fall Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "life_pool_data.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class HERA_API ULifePoolData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Health)
   int Health = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Health)
   int MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Armor)
   int Armor = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Armor)
   int MaxArmor = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shields)
   int Shields = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Shields)
   int MaxShields = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Overhealth)
   int TemporaryArmor = 0;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Overhealth)
   int TemporaryShields = 0;
};
