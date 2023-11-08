// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatGPTComponent.h"

// Sets default values for this component's properties
UChatGPTComponent::UChatGPTComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UChatGPTComponent::BeginPlay()
{
	Super::BeginPlay();

	// Read API key from file
	FString apiKeyFilePath = FPaths::ProjectConfigDir() / "ApiKeys.txt";
	FString apiKeyContent;
	if (FFileHelper::LoadFileToString(apiKeyContent, *apiKeyFilePath))
	{
		// If your file has the format "ApiKey=YOUR_API_KEY_HERE", use this instead:
		 FFileHelper::LoadFileToString(apiKeyContent, *apiKeyFilePath);
		 apiKeyContent.Split(TEXT("="), nullptr, &m_apiKey);
	}
	m_httpModule = &FHttpModule::Get();

	// ...

}

void UChatGPTComponent::OnResponseReceived(FHttpRequestPtr req, FHttpResponsePtr res, bool bWasSuccessful)
{
	if (bWasSuccessful && res.IsValid())
	{
		TSharedPtr<FJsonObject> resObj;
		TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(res->GetContentAsString());

		if (FJsonSerializer::Deserialize(reader, resObj) && resObj.IsValid())
		{
			FString resContent;
			TArray<TSharedPtr<FJsonValue>> choices = resObj->GetArrayField("choices");
			if (choices.Num() > 0)
			{
				TSharedPtr<FJsonObject> msgObj = choices[0]->AsObject()->GetObjectField("message");
				resContent = msgObj->GetStringField("content");
				AddMessage(EChatGPTRole::assistant, resContent);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, resContent);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, res->GetContentAsString());
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to get message"));
		}
	}
}

void UChatGPTComponent::AddMessage(EChatGPTRole role, const FString& content)
{
	m_messages.Add(FChatGPTMessage(role, content));
}


// Called every frame
void UChatGPTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UChatGPTComponent::Chat(const FString& msg, TEnumAsByte<EChatGPTModel> model)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Starting send %s"), *msg));
	AddMessage(EChatGPTRole::user, msg);
	TSharedPtr<IHttpRequest> req = m_httpModule->CreateRequest();

	req->OnProcessRequestComplete().BindUObject(this, &UChatGPTComponent::OnResponseReceived);
	req->SetURL(TEXT("https://api.openai.com/v1/chat/completions"));
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *m_apiKey));

	TSharedPtr<FJsonObject> reqObj = MakeShareable(new FJsonObject());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, ModelToString(model));
	reqObj->SetStringField(TEXT("model"), ModelToString(model));

	TArray<TSharedPtr<FJsonValue>> jsonMsgs;
	for (const FChatGPTMessage& msg : m_messages)
	{
		jsonMsgs.Add(MakeShareable(new FJsonValueObject(msg.ToJsonObject())));
	}
	reqObj->SetArrayField(TEXT("messages"), jsonMsgs);

	FString reqBody;
	TSharedRef<TJsonWriter<>> writer = TJsonWriterFactory<>::Create(&reqBody);
	FJsonSerializer::Serialize(reqObj.ToSharedRef(), writer);

	req->SetContentAsString(reqBody);
	
	req->ProcessRequest();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SENT"));
}

