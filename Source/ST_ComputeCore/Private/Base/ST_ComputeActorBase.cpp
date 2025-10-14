// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Base/ST_ComputeActorBase.h"
#include "Utils/ST_ComputeFunctionLibrary.h"

// Sets default values
AST_ComputeActorBase::AST_ComputeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

#if WITH_EDITOR
	// Billboard
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetHiddenInGame(false, true);
	Billboard->SetupAttachment(RootComponent);
#endif // WITH_EDITOR
}

// Called when the game starts or when spawned
void AST_ComputeActorBase::BeginPlay()
{
	Super::BeginPlay();

	cachedRPCSManager = UST_RPCSManager::Get(GetWorld());

	if (IsValid(cachedRPCSManager))
	{
		cachedRPCSManager->Register(this);
	}
}

// Called when the game ends or when destroyed
void AST_ComputeActorBase::BeginDestroy()
{
	Super::BeginDestroy();
	if (IsValid(cachedRPCSManager))
	{
		cachedRPCSManager->Deregister(this);
	}

}

bool AST_ComputeActorBase::IsPlaying(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	UWorld* World = WorldContextObject->GetWorld();
	if (IsValid(World) && !World->IsGameWorld())
	{
		return false;
	}
#endif

	return true;
}

// Called every frame
void AST_ComputeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	const bool bIsPlaying = AST_ComputeActorBase::IsPlaying(this);

	const bool bCanDebug = !bIsPlaying || (bIsPlaying && bDebugDisplayInRuntime);

	if (bDebugSprite && bCanDebug)
	{
		Billboard->SetHiddenInGame(false, true);
		Billboard->SetVisibility(true);
	}
	else {
		Billboard->SetHiddenInGame(true, true);
		Billboard->SetVisibility(false);
	}

	if (bPreviouslySelected && !IsSelected())
	{
		OnUnselected();
	}
	bPreviouslySelected = false;

	if ((bDebugBounds && bCanDebug) || IsSelected())
	{
		CurrentBoundsColor = BoundsColor;

		SelectedTick();

		if (BoundsConstantParameters.bSphere)
		{
			constexpr int32 Segments = 12;
			DrawDebugSphere(GetWorld(), GetActorLocation(), BoundsConstantParameters.Radius, Segments, CurrentBoundsColor);
		}
		else
		{
			DrawDebugBox(GetWorld(), GetActorLocation(), BoundsConstantParameters.BoundsExtent, CurrentBoundsColor);
		}
	}

#endif // WITH_EDITOR
}

#if WITH_EDITOR
bool AST_ComputeActorBase::ShouldTickIfViewportsOnly() const
{
	// Tick in Editor
	return true;
}

void AST_ComputeActorBase::SelectedTick()
{
	if (!IsSelected())
	{
		CurrentBoundsColor.R = FMath::RoundToInt(CurrentBoundsColor.R * 0.5f);
		CurrentBoundsColor.G = FMath::RoundToInt(CurrentBoundsColor.G * 0.5f);
		CurrentBoundsColor.B = FMath::RoundToInt(CurrentBoundsColor.B * 0.5f);
		return;
	}

	if (!bPreviouslySelected)
	{
		OnSelected();
	}
	bPreviouslySelected = true;
}

void AST_ComputeActorBase::OnSelected() {}
void AST_ComputeActorBase::OnUnselected() {}
#endif

// Start Managed Compute Shader Interface 
void AST_ComputeActorBase::InitComputeShader_GameThread() {}
void AST_ComputeActorBase::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) {}

void AST_ComputeActorBase::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	LastDeltaTime = DeltaTime;
}
void AST_ComputeActorBase::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) {}

void AST_ComputeActorBase::DisposeComputeShader_GameThread() {}
void AST_ComputeActorBase::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) {}
// End Managed Compute Shader Interface 