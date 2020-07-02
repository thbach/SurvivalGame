// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"


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

void UInteractionComponent::BeginFocus(ASurvivalCharacter* Character)
{
    if (!IsActive() || !GetOwner() || !Character) return;

    OnBeginFocus.Broadcast(Character);

    SetHiddenInGame(false);

    if (!GetOwner()->HasAuthority())
    {
        if (auto* Component = Cast<UPrimitiveComponent>(GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass())) )
        {
            Component->SetRenderCustomDepth(true);
        }
        // for (auto& VisualComp : GetOwner()->GetComponentByClass(UPrimitiveComponent::StaticClass()))
        // {
        //     if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
        //     {
        //         Prim->SetRenderCustomDepth(true);
        //     }
        // }
    }
}

void UInteractionComponent::EndFocus(ASurvivalCharacter* Character)
{

}

void UInteractionComponent::BeginInteract(ASurvivalCharacter* Character)
{

}

void UInteractionComponent::EndInteract(ASurvivalCharacter* Character)
{

}

void UInteractionComponent::Interact(ASurvivalCharacter* Character)
{
    OnInteract.Broadcast(Character);
}
