// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "ChatGPTComponent.generated.h"


// Define an enumeration for the allowed roles
enum class EChatGPTRole
{
	system,
	user,
	assistant
};

UENUM()
enum class EChatGPTModel
{
	gpt35 UMETA(DisplayName = "GPT-3.5"),
	gpt4 UMETA(DisplayName = "GPT-4"),
	gpt4turbo UMETA(DisplayName = "GPT-4 Turbo")
};

struct FChatGPTMessage
{
	FString Role;
	FString Content;

	FChatGPTMessage(EChatGPTRole role, const FString& content)
	{
		Content = content;
		switch (role)
		{
		case EChatGPTRole::system:
			Role = TEXT("system");
			break;
		case EChatGPTRole::assistant:
			Role = TEXT("assistant");
			break;
		case EChatGPTRole::user:
		default:
			Role = TEXT("user");
			break;
		}
	}

	// Convert the message to a JSON object
	TSharedPtr<FJsonObject> ToJsonObject() const
	{
		TSharedPtr<FJsonObject> MessageObj = MakeShareable(new FJsonObject());
		MessageObj->SetStringField(TEXT("role"), Role);
		MessageObj->SetStringField(TEXT("content"), Content);
		return MessageObj;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OPENAISPHINX_API UChatGPTComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UChatGPTComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "AI")
	void Chat(const FString& prompt, TEnumAsByte<EChatGPTModel> model);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Callback function for when the HTTP request completes
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void AddMessage(EChatGPTRole role, const FString& content);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TEnumAsByte<EChatGPTModel> m_model = EChatGPTModel::gpt4turbo;
	FString ModelToString(EChatGPTModel model)
	{
		switch (model)
		{
		case EChatGPTModel::gpt35:
			return TEXT("gpt-3.5");
		case EChatGPTModel::gpt4:
			return TEXT("gpt-4");
		case EChatGPTModel::gpt4turbo:
		default:
			return TEXT("gpt-4-1106-preview");
		}
	}

protected:
	FString m_apiKey;
	FHttpModule* m_httpModule;
	TArray<FChatGPTMessage> m_messages;

};