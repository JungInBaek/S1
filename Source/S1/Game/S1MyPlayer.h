// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/S1Player.h"
#include "S1MyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class S1_API AS1MyPlayer : public AS1Player
{
	GENERATED_BODY()

public:
	AS1MyPlayer();

protected:
	// To add mapping context
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	void Jump(const FInputActionValue& Value);
	void StopJumping(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void LookUp(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void ChangeToGrenadeGun(const FInputActionValue& Value);
	void ChangeToSniperGun(const FInputActionValue& Value);
	void SniperAim(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);

protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* imc_default;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ia_Jump;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ia_Move;

	/** Look Input Action */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Fire;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_GrenadeGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_SniperGun;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Sniper;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;


protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
protected:
	const float MOVE_PACKET_SEND_DELAY = 0.1f;
	float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

	// 걷기 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float walkSpeed = 200.f;

	// 달리기 속도
	UPROPERTY(EditAnywhere, Category = PlayerSetting)
	float runSpeed = 500.f;

	// cache
	FVector2D moveInput;
	FVector direction;
	float yaw;

	// dirty flag
	FVector2D lastInput;
	float lastYaw;

	// state
	Protocol::PlayerState pState;

public:
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	UPROPERTY()
	class UUserWidget* _sniperUI;

	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	UPROPERTY()
	class UUserWidget* _crosshairUI;
};
