// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RelaxedConeSteppingMapGenerator.generated.h"

UCLASS()
class MYPROJECT3_API ARelaxedConeSteppingMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* m_textureRenderTarget;
	UPROPERTY(EditAnywhere)
	UTexture2D* m_reliefMap;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* m_material;
	UPROPERTY(EditAnywhere)
	int m_rowsPerDrawCall = 1;

	// Sets default values for this actor's properties
	ARelaxedConeSteppingMapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
