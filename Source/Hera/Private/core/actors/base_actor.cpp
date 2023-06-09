// Copyright Final Fall Games. All Rights Reserved.


#include "core/actors/base_actor.h"
#include "core/gas/base_asc.h"

AActorBase::AActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// GameplayEffects are not replicated. GameplayTags, Attributes are replicated to clients.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* AActorBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
