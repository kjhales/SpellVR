#include "Magic.h"
#include "MagicCoopCharacter.h"
#include "SpellSystem.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMagicCoopCharacter

AMagicCoopCharacter::AMagicCoopCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	SpellSystemChildComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpellSystemComponent"));
	SpellSystemChildComponent->SetChildActorClass(ASpellSystem::StaticClass());
	SpellSystemChildComponent->AttachParent = RootComponent;
}

//////////////////////////////////////////////////////////////////////////
// Input
void AMagicCoopCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMagicCoopCharacter::OnFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMagicCoopCharacter::OnRelease);

	InputComponent->BindAxis("MoveForward", this, &AMagicCoopCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMagicCoopCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMagicCoopCharacter::TurnAtRate);

	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMagicCoopCharacter::LookUpAtRate);
}

void AMagicCoopCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (SpellSystem == NULL)
	{
		SpellSystemChildComponent->CreateChildActor();
		SpellSystem = (ASpellSystem*) SpellSystemChildComponent->ChildActor;
		//SpellSystem = (ASpellSystem*)GetWorld()->SpawnActor(ASpellSystem::StaticClass());
		SpellSystem->AddSpell(0);
	}
}

void AMagicCoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCoopCharacter::OnRelease()
{
	SpellSystem->ReleaseSpell();
}

void AMagicCoopCharacter::OnFire()
{
	//AActor* Target = GetTarget();

	if (SpellSystem != NULL)
	{
		SpellSystem->Cast(this, Mesh1P, GetActorRotation(), GunOffset, NULL, false);
	}
	
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

/*AActor* AMagicCoopCharacter::GetTarget()
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	FVector Start = FirstPersonCameraComponent->GetComponentTransform().GetLocation();
	FVector End = Start + FirstPersonCameraComponent->GetComponentTransform().GetRotation(
	

	//call GetWorld() from within an actor extending class
	/*GetWorld()->LineTraceSingle(
		RV_Hit,        //result	
		Start,    //start
		End, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
		);

	//if (RV_Hit.bBlockingHit)
	{
		AActor* Target = RV_Hit.GetActor(); //the hit actor if there is one
		return Target;
	}

	return NULL;
	//RV_Hit.ImpactPoint;  //FVector
	//RV_Hit.ImpactNormal;  //FVector
}*/

void AMagicCoopCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMagicCoopCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMagicCoopCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMagicCoopCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
