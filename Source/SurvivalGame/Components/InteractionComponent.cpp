// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "../Player/SurvivalCharacter.h"
#include "../Widgets/InteractionWidget.h"


UInteractionComponent::UInteractionComponent()
{
    SetComponentTickEnabled(false);

    InteractionTime = 0.f;
    InteractionDistance = 200.f;
    InteractableNameText = FText::FromString(TEXT("Interactable Object"));
    InteractableActionText = FText::FromString(TEXT("Interact"));

    Space = EWidgetSpace::Screen;
    DrawSize = FIntPoint(600, 100);
    bDrawAtDesiredSize = true;

    SetActive(true);
    SetHiddenInGame(true);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
    InteractableNameText = NewNameText;
    RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
    InteractableActionText = NewActionText;
    RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
    Super::Deactivate();

    for (int32 i = Interactors.Num() - 1; i >= 0; --i)
    {
        if (auto* Interactor = Cast<ASurvivalCharacter>(Interactors[i]))
        {
            EndFocus(Interactor);
            EndInteract(Interactor);
        }
    }

    Interactors.Empty();
}

bool UInteractionComponent::CanInteract(ASurvivalCharacter* Character) const
{
    const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() > 1;
    return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

void UInteractionComponent::RefreshWidget()
{
    if (!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
    {
        // Make sure the widget is initized and that we are displaying the right values (these may have changed)
        if (auto* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
        {
            InteractionWidget->UpdateInteractionWidget(this);
        }
    }
}

void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
    if (!IsActive() || !GetOwner() || !Character) return;

    OnBeginFocus.Broadcast(Character);

    SetHiddenInGame(false);

    if (!GetOwner()->HasAuthority())
    {
        TArray<UPrimitiveComponent* > OutComponents;
        GetOwner()->GetComponents(OutComponents);
        if (OutComponents.Num() > 0)
        {
            OutComponents[0]->SetRenderCustomDepth(true);
        }
    }

    RefreshWidget();
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{

    OnEndFocus.Broadcast(Character);

    SetHiddenInGame(true);

    if (!GetOwner()->HasAuthority())
    {
        if (auto* Component = Cast<UPrimitiveComponent>(GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass())) )
        {
            Component->SetRenderCustomDepth(false);
        }
    }
}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{
    if (CanInteract(Character))
    {
        Interactors.AddUnique(Character);
        OnBeginInteract.Broadcast(Character);
    }
}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{
    Interactors.RemoveSingle(Character);
    OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
    if (CanInteract(Character))
    {
        OnInteract.Broadcast(Character);
    }
}

float UInteractionComponent::GetInteractPercentage()
{
    if (Interactors.IsValidIndex(0))
    {
        if (auto* Interactor = Cast<ASurvivalCharacter>(Interactors[0]))
        {
            if (Interactor && Interactor->IsInteracting())
            {
                return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
            }

        }

    }

    return 0.0f;
}
