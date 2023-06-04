// Fill out your copyright notice in the Description page of Project Settings.


#include "RelaxedConeSteppingMapGenerator.h"

// Sets default values
ARelaxedConeSteppingMapGenerator::ARelaxedConeSteppingMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARelaxedConeSteppingMapGenerator::OnConstruction(const FTransform& Transform)
{
	if(!m_generate)
	{
		return;
	}
	m_generate = false;
	m_drawCallIndex = 0;
	m_tempRT = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 256, 256, RTF_RGBA8);
	m_materialInstance = UMaterialInstanceDynamic::Create(m_material, nullptr);
	m_materialInstance->SetVectorParameterValue(FName(TEXT("TexelSize")), FLinearColor(1.0f / m_reliefMap->GetSizeX(), 1.0f / m_reliefMap->GetSizeY(), m_reliefMap->GetSizeX(), 0));
	m_materialInstance->SetTextureParameterValue(FName(TEXT("ReliefMap")), m_reliefMap);

	int texelCountPerRow = m_reliefMap->GetSizeX();
	int rowCount = m_reliefMap->GetSizeY();
	int texelCount = texelCountPerRow * rowCount;
	m_drawCallCount = texelCount / m_texelsToProcessPerDrawCall;
	m_remainingTexelCount = texelCount % m_texelsToProcessPerDrawCall;

	//Make sure the last draw call's result is stored in m_textureRenderTarget 
	if((m_drawCallCount + (m_remainingTexelCount == 0 ? 0 : 1)) % 2 == 0)
	{
		m_rTs[0] = m_textureRenderTarget;
		m_rTs[1] = m_tempRT;
	}
	else
	{
		m_rTs[0] = m_tempRT;
		m_rTs[1] = m_textureRenderTarget;
	}

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_rTs[0], FLinearColor(FColor(255, 255, 255, 255)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_rTs[1], FLinearColor(FColor(255, 255, 255, 255)));
	while(m_drawCallIndex < m_drawCallCount)
	{
		if(m_drawCallIndex < m_drawCallCount)
		{
			bool rTSelector = m_drawCallIndex % 2 == 0;
			m_materialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_rTs[static_cast<int>(rTSelector)]);
			int startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
			m_materialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
			m_materialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_texelsToProcessPerDrawCall);
			UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_rTs[static_cast<int>(!rTSelector)], m_materialInstance);

			m_drawCallIndex++;
			if(m_drawCallIndex == m_drawCallCount)
			{
				if(m_remainingTexelCount != 0)
				{
					UE_LOG(LogTemp, Display, TEXT("Finished generating the cone stepping map with %d draw calls"), m_drawCallIndex + 1);
					m_materialInstance->SetTextureParameterValue(FName(TEXT("ConeSteppingMap")), m_rTs[static_cast<int>(!rTSelector)]);
					startTexelIndex = m_drawCallIndex * m_texelsToProcessPerDrawCall;
					m_materialInstance->SetScalarParameterValue(FName(TEXT("StartTexelIndex")), startTexelIndex);
					m_materialInstance->SetScalarParameterValue(FName(TEXT("EndTexelIndex")), startTexelIndex + m_remainingTexelCount);
					UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_rTs[static_cast<int>(rTSelector)], m_materialInstance);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ARelaxedConeSteppingMapGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARelaxedConeSteppingMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

