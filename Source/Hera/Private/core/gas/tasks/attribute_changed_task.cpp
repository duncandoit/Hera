// Copyright Final Fall Games. All Rights Reserved.


#include "core/gas/tasks/attribute_changed_task.h"

UAttributeChangedTask* UAttributeChangedTask::ListenForAttributeChange(
   UAbilitySystemComponent* AbilitySystemComponent, 
   FGameplayAttribute Attribute
)
{
	auto Task = NewObject<UAttributeChangedTask>();
	Task->ASC = AbilitySystemComponent;
	Task->AttributeToListenFor = Attribute;

	if (!IsValid(AbilitySystemComponent) || !Attribute.IsValid())
	{
		Task->RemoveFromRoot();
		return nullptr;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
      Task, 
      &UAttributeChangedTask::OnAttributeChanged
   );

	return Task;
}

UAttributeChangedTask* UAttributeChangedTask::ListenForAttributesChange(
   UAbilitySystemComponent* AbilitySystemComponent, 
   TArray<FGameplayAttribute> Attributes
)
{
	auto Task = NewObject<UAttributeChangedTask>();
	Task->ASC = AbilitySystemComponent;
	Task->AttributesToListenFor = Attributes;

	if (!IsValid(AbilitySystemComponent) || Attributes.Num() < 1)
	{
		Task->RemoveFromRoot();
		return nullptr;
	}

	for (auto Attribute : Attributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(
         Task, 
         &UAttributeChangedTask::OnAttributeChanged
      );
	}

	return Task;
}

void UAttributeChangedTask::EndTask()
{
	if (IsValid(ASC))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);

		for (auto Attribute : AttributesToListenFor)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAttributeChangedTask::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	AttributeChangedDelegate.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}