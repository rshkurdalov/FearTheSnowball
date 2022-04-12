#include "CapturePoint.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

ACapturePoint::ACapturePoint() 
{
    CaptureCollider = CreateDefaultSubobject<USphereComponent>("CaptureCollider");
    CaptureCollider->SetupAttachment(RootComponent);

    Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
    Decal->SetupAttachment(RootComponent);

    UpdateAreaRadius();
}

void ACapturePoint::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);

    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(ACapturePoint, CaptureAreaRadius)) {
        UpdateAreaRadius();
    }
}

void ACapturePoint::UpdateAreaRadius() 
{
    CaptureCollider->SetSphereRadius(CaptureAreaRadius);
    const float decalSide = 2 * CaptureAreaRadius;
    Decal->DecalSize = { decalSide, decalSide , 100.f};
}