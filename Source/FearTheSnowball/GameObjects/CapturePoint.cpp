#include "CapturePoint.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "PlayerState/MatchPlayerState.h"
#include "GameMode/MatchGameMode.h"

ACapturePoint::ACapturePoint() 
{
    PrimaryActorTick.bCanEverTick = true;

    CaptureCollider = CreateDefaultSubobject<USphereComponent>("CaptureCollider");
    SetRootComponent(CaptureCollider);
    CaptureCollider->OnComponentBeginOverlap.AddDynamic(this, &ACapturePoint::OnBeginOverlap);
    CaptureCollider->OnComponentEndOverlap.AddDynamic(this, &ACapturePoint::OnEndOverlap);

    Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
    Decal->DecalSize = {0.5f, 1.f, 1.f};
    Decal->SetWorldRotation(FRotator(90.f, 0.f, 0.f));
    Decal->SetupAttachment(RootComponent);
    Decal->SetRelativeLocation(FVector::ZeroVector);

    bReplicates = true;
    bNetLoadOnClient = true;

    UpdateAreaRadius();
}

void ACapturePoint::BeginPlay()
{
    Super::BeginPlay();
    
    CurrentCapturing.progress = CapturingPoints / 3.f;

    if (HasAuthority())
        RecalculateCapturingState(true);
}

void ACapturePoint::Tick(float DeltaSeconds) 
{
    Super::Tick(DeltaSeconds);

    if (HasAuthority())
        TickServer(DeltaSeconds);
    else
        TickClient(DeltaSeconds);
}

void ACapturePoint::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);

    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(ACapturePoint, CaptureAreaRadius))
        UpdateAreaRadius();
}

void ACapturePoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority())
        return;

    auto controller = GetPlayerController(OtherActor);

    if (controller != nullptr)
        InPointPlayers.Add(controller, GetPlayerColor(controller));

    RecalculateCapturingState();
}

void ACapturePoint::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!HasAuthority())
        return;

    auto controller = GetPlayerController(OtherActor);

    if (controller != nullptr)
        InPointPlayers.Remove(controller);

    RecalculateCapturingState();
}

APlayerController* ACapturePoint::GetPlayerController(AActor* OtherActor)
{
    APawn* character = Cast<APawn>(OtherActor);

    if (!character || !character->IsPlayerControlled())
        return nullptr;

    return Cast<APlayerController>(character->GetController());
}

FColor ACapturePoint::GetPlayerColor(APlayerController* controller) {
    if (AMatchPlayerState* matchPlayerState = controller->GetPlayerState<AMatchPlayerState>())
        return matchPlayerState->Color;

    return FColor::Red;
}

void ACapturePoint::UpdateAreaRadius() 
{
    CaptureCollider->SetSphereRadius(CaptureAreaRadius);
    Decal->SetWorldScale3D({ CaptureAreaRadius, CaptureAreaRadius, CaptureAreaRadius });
}

void ACapturePoint::TickServer(float DeltaSeconds)
{
    UpdateProgress(DeltaSeconds);
    HandleProgress();
}

void ACapturePoint::TickClient(float DeltaSeconds)
{
    UpdateProgress(DeltaSeconds);
    const float currentFill = CurrentCapturing.progress / CapturingPoints;
    OnPointRadiusChange((float)growingDirection, currentFill, CurrentCapturing.color);
}

void ACapturePoint::RecalculateCapturingState(bool forceUpdate)
{
    int32 oldGrowingDirection = growingDirection;

    if (InPointPlayers.Contains(CurrentCapturing.player))
    {
        if (InPointPlayers.Num() == 1) 
        {
            growingDirection = 1;
        }
        else
        {
            growingDirection = 0;
        }
    }
    else if (InPointPlayers.Num() != 0)
    {
        growingDirection = -1;
    } 
    else
    {
        growingDirection = 0;
    }

    if (forceUpdate || oldGrowingDirection != growingDirection)
        UpdateClientCapturingState(growingDirection, CurrentCapturing.progress, CurrentCapturing.color);
}

void ACapturePoint::UpdateClientCapturingState_Implementation(int32 state, float progress, FColor color)
{
    if (HasAuthority())
        return;

    growingDirection = state;
    CurrentCapturing.progress = progress;
    CurrentCapturing.color = color;
}

void ACapturePoint::UpdateProgress(float DeltaTime)
{
    if (CurrentCapturing.progress > CapturingPoints)
        return;
        
    CurrentCapturing.progress += growingDirection * CapturingSpeed * DeltaTime;
}

void ACapturePoint::HandleProgress()
{
    if (CurrentCapturing.progress <= 0.0)
    {
        if (InPointPlayers.Num() != 0) {
            CurrentCapturing.progress = 0.001;
            CurrentCapturing.player = InPointPlayers.CreateConstIterator().Key();
            CurrentCapturing.color = InPointPlayers.CreateConstIterator().Value();
            RecalculateCapturingState(true);
        }
    }
    else if (CurrentCapturing.progress >= CapturingPoints)
    {
        if (auto gameMode = GetWorld()->GetAuthGameMode<AMatchGameMode>())
        {
            gameMode->WinGame(CurrentCapturing.player);
        }
    }
}