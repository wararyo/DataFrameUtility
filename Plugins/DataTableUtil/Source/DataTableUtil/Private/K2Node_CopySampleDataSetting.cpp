#include "K2Node_CopySampleDataSetting.h"
#include "DataTableUtil.h"
#include "GraphEditorSettings.h"
#include "BlueprintGraphDefinitions.h"
#include "BlueprintUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UnrealEd.h"
#include "EditorStyle.h"
#include "KismetCompiler.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "SampleData.h"

#define LOCTEXT_NAMESPACE "SampleData"

struct FK2Node_CopySampleDataSettingHelper
{
	static FString DataTablePinName;
	static FString RowIndexPinName;
};

FString FK2Node_CopySampleDataSettingHelper::DataTablePinName(TEXT("DataTable"));
FString FK2Node_CopySampleDataSettingHelper::RowIndexPinName(TEXT("Index"));

UK2Node_CopySampleDataSetting::UK2Node_CopySampleDataSetting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UK2Node_CopySampleDataSetting::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// 入力クラスによって変更されないピン情報はここに記述

	// 入力実行ピン
	CreatePin(EGPD_Input, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Execute);

	// 出力実行ピン
	CreatePin(EGPD_Output, K2Schema->PC_Exec, TEXT(""), NULL, false, false, K2Schema->PN_Then);

	// Add DataTable pin
	UEdGraphPin* DataTablePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), FK2Node_CopySampleDataSettingHelper::DataTablePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("DataTablePinDescription", "The DataTable you want to retreive a row from"));

	// Row Index pin
	UEdGraphPin* RowNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, FK2Node_CopySampleDataSettingHelper::RowIndexPinName);
	SetPinToolTip(*RowNamePin, LOCTEXT("RowNamePinDescription", "The name of the row to retrieve from the DataTable"));

	// Result pin
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	ResultPin->PinFriendlyName = LOCTEXT("GetDataTableRow Output Row", "Out Row");
	SetPinToolTip(*ResultPin, LOCTEXT("ResultPinDescription", "The returned TableRow, if found"));

	Super::AllocateDefaultPins();
}

FText UK2Node_CopySampleDataSetting::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("GetNodeTitleMenu", "Copy Sample Data Setting");
	}
	else if (auto SampleDataClass = GetSampleDataClass())
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("ClassName"), SampleDataClass->GetDisplayNameText());
			CachedNodeTitle.SetCachedText(FText::Format(LOCTEXT("GetNodeTitleFormat", "{ClassName} 設定をコピーします"), Args), this);
		}
		return CachedNodeTitle;
	}
	return LOCTEXT("GetNodeTitleDefault", "サンプルデータ設定をコピーします");
}

void UK2Node_CopySampleDataSetting::PinDefaultValueChanged(UEdGraphPin* ChangedPin)
{
	if (ChangedPin && (ChangedPin->PinName == FK2Node_CopySampleDataSettingHelper::ClassPinName))
	{
		OnClassPinChanged();
	}
}

FText UK2Node_CopySampleDataSetting::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "サンプルデータ設定をコピーします");
}

bool UK2Node_CopySampleDataSetting::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	return Super::IsCompatibleWithGraph(TargetGraph) && (!Blueprint || FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph);
}

bool UK2Node_CopySampleDataSetting::HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const
{
	UClass* SourceClass = GetSampleDataClass();
	const UBlueprint* SourceBlueprint = GetBlueprint();
	const bool bResult = (SourceClass != NULL) && (SourceClass->ClassGeneratedBy != SourceBlueprint);
	if (bResult && OptionalOutput)
	{
		OptionalOutput->AddUnique(SourceClass);
	}
	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bSuperResult || bResult;
}

void UK2Node_CopySampleDataSetting::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin && (Pin->PinName == FK2Node_CopySampleDataSettingHelper::ClassPinName))
	{
		OnClassPinChanged();
	}
}

void UK2Node_CopySampleDataSetting::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	UClass* SampleDataClass = GetSampleDataClass(&OldPins);
	if (SampleDataClass != NULL)
	{
		CreatePinsForClass(SampleDataClass);
	}
}

void UK2Node_CopySampleDataSetting::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const
{
	UClass* SampleDataClass = GetSampleDataClass();
	const FString SampleDataClassStr = SampleDataClass ? SampleDataClass->GetName() : TEXT("InvalidClass");
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("CopySampleDataSetting")));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
	OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("ObjectClass"), SampleDataClassStr));
}

void UK2Node_CopySampleDataSetting::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_CopySampleDataSetting::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Sample");
}

void UK2Node_CopySampleDataSetting::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* TargetGraph)
{
	Super::ExpandNode(CompilerContext, TargetGraph);

	static FName CopySettingA_FunctionName = GET_FUNCTION_NAME_CHECKED(USampleDataA, CopySettingA_Static);
	static FName CopySettingB_FunctionName = GET_FUNCTION_NAME_CHECKED(USampleDataB, CopySettingB_Static);
	static FString Class_ParamName = FString(TEXT("Class"));
	static FString Target_ParamName = FString(TEXT("Target"));
	static FString Setting_ParamName = FString(TEXT("Setting"));

	UK2Node_CopySampleDataSetting* CopySampleDataSettingNode = this;
	UEdGraphPin* NodeExec = CopySampleDataSettingNode->GetExecPin();
	UEdGraphPin* ClassPin = CopySampleDataSettingNode->GetClassPin();
	UEdGraphPin* TargetPin = CopySampleDataSettingNode->GetTargetPin();
	UEdGraphPin* SettingPin = CopySampleDataSettingNode->GetSettingPin();
	UEdGraphPin* NodeThen = CopySampleDataSettingNode->GetThenPin();
	UEdGraphPin* NodeResult = CopySampleDataSettingNode->GetResultPin();

	UClass* SampleDataClass = (ClassPin != NULL) ? Cast<UClass>(ClassPin->DefaultObject) : NULL;
	if ((0 == ClassPin->LinkedTo.Num()) && (NULL == SampleDataClass))
	{
//		CompilerContext.MessageLog.Error(*LOCTEXT("CopySampleDataSettingNodeMissingClass_Error", "クラスが設定されていません").ToString(), CopySampleDataSettingNode);
		CopySampleDataSettingNode->BreakAllNodeLinks();
		return;
	}
	if (SampleDataClass != USampleDataA::StaticClass() && SampleDataClass != USampleDataB::StaticClass())
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("CopySampleDataSettingNodeMissingClass_Error", "無効なクラスです").ToString(), CopySampleDataSettingNode);
		CopySampleDataSettingNode->BreakAllNodeLinks();
		return;
	}


	//////////////////////////////////////////////////////////////////////////
	// 'USampleData::CopySettingStatic' を呼び出すノードを作成

	UK2Node_CallFunction* CallCopyNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(CopySampleDataSettingNode, TargetGraph);

	if (SampleDataClass == USampleDataA::StaticClass())
	{
		CallCopyNode->FunctionReference.SetExternalMember(CopySettingA_FunctionName, SampleDataClass);
	}
	else if (SampleDataClass == USampleDataB::StaticClass())
	{
		CallCopyNode->FunctionReference.SetExternalMember(CopySettingB_FunctionName, SampleDataClass);
	}

	CallCopyNode->AllocateDefaultPins();

	UEdGraphPin* CallCopyExec = CallCopyNode->GetExecPin();
	UEdGraphPin* CallCopyTargetPin = CallCopyNode->FindPinChecked(Target_ParamName);
	UEdGraphPin* CallCopySettingPin = CallCopyNode->FindPinChecked(Setting_ParamName);
	UEdGraphPin* CallCopyResult = CallCopyNode->GetReturnValuePin();

	CompilerContext.MovePinLinksToIntermediate(*NodeExec, *CallCopyExec);

	if (TargetPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*TargetPin, *CallCopyTargetPin);
	}

	if (SettingPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SettingPin, *CallCopySettingPin);
	}

	if (NodeResult)
	{
		CallCopyResult->PinType = NodeResult->PinType;
		CompilerContext.MovePinLinksToIntermediate(*NodeResult, *CallCopyResult);
	}

	//////////////////////////////////////////////////////////////////////////
	// 出力実行ピンに、'USampleData::CopySettingStatic' 実行結果ピンを紐付ける

	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, TargetGraph, CallCopyNode, CopySampleDataSettingNode, CallCopyResult, GetSampleDataClass());

	CompilerContext.MovePinLinksToIntermediate(*NodeThen, *LastThen);

	// 処理が終了したのでリンクを外す
	CopySampleDataSettingNode->BreakAllNodeLinks();
}

void UK2Node_CopySampleDataSetting::CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins)
{
	check(InClass != NULL);

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	const USampleData* ClassDefaultObject = CastChecked<USampleData>(InClass->GetDefaultObject(false));

	// 入力クラスによって変更するピン情報はここに記述

	// コピー先インスタンス
	UEdGraphPin* TargetPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), InClass, false, false, FK2Node_CopySampleDataSettingHelper::TargetPinName);
	SetPinToolTip(*TargetPin, LOCTEXT("TargetPinDescription", "コピー先インスタンス"));

	// 出力ピン
	UEdGraphPin* ResultPin = CreatePin(EGPD_Output, K2Schema->PC_Object, TEXT(""), InClass, false, false, K2Schema->PN_ReturnValue);
	SetPinToolTip(*ResultPin, LOCTEXT("ResultPinDescription", "コピー後のインスタンス"));

	if (OutClassPins)
	{
		OutClassPins->Add(TargetPin);
		OutClassPins->Add(ResultPin);
	}

	// コピーする設定
	UClass* SettingClass = ClassDefaultObject->GetSettingClass();
	if (SettingClass)
	{
		UEdGraphPin* SettingPin = CreatePin(EGPD_Input, K2Schema->PC_Object, TEXT(""), ClassDefaultObject->GetSettingClass(), false, false, FK2Node_CopySampleDataSettingHelper::SettingPinName);
//		SetPinToolTip(*SettingPin, LOCTEXT("SettingPinDescription", "コピーする設定"));

		if (OutClassPins)
		{
			OutClassPins->Add(SettingPin);
		}
	}
}

UEdGraphPin* UK2Node_CopySampleDataSetting::GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* Pin = NULL;
	for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
	{
		UEdGraphPin* TestPin = *PinIt;
		if (TestPin && TestPin->PinName == FK2Node_CopySampleDataSettingHelper::ClassPinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_CopySampleDataSetting::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_CopySampleDataSettingHelper::TargetPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_CopySampleDataSetting::GetSettingPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_CopySampleDataSettingHelper::SettingPinName);
	check(Pin == NULL || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_CopySampleDataSetting::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(K2Schema->PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_CopySampleDataSetting::GetResultPin() const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPin(K2Schema->PN_ReturnValue);
	check(Pin == NULL || Pin->Direction == EGPD_Output);
	return Pin;
}

UClass* UK2Node_CopySampleDataSetting::GetSampleDataClass(const TArray<UEdGraphPin*>* InPinsToSearch /*=NULL*/) const
{
	UClass* SampleDataClass = NULL;
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

	UEdGraphPin* ClassPin = GetClassPin(PinsToSearch);
	if (ClassPin && ClassPin->DefaultObject != NULL && ClassPin->LinkedTo.Num() == 0)
	{
		SampleDataClass = CastChecked<UClass>(ClassPin->DefaultObject);
	}
	else if (ClassPin && ClassPin->LinkedTo.Num())
	{
		auto ClassTarget = ClassPin->LinkedTo[0];
		SampleDataClass = ClassTarget ? Cast<UClass>(ClassTarget->PinType.PinSubCategoryObject.Get()) : nullptr;
	}

	return SampleDataClass;
}

UClass* UK2Node_CopySampleDataSetting::GetClassPinBaseClass() const
{
	return USampleData::StaticClass();
}

bool UK2Node_CopySampleDataSetting::IsSpawnVarPin(UEdGraphPin* Pin)
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	return(Pin->PinName != K2Schema->PN_Execute &&
		Pin->PinName != K2Schema->PN_Then &&
		Pin->PinName != FK2Node_CopySampleDataSettingHelper::ClassPinName);
}

void UK2Node_CopySampleDataSetting::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

void UK2Node_CopySampleDataSetting::OnClassPinChanged()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	TArray<UEdGraphPin*> OldPins = Pins;
	TArray<UEdGraphPin*> OldClassPins;

	for (int32 i = 0; i < OldPins.Num(); i++)
	{
		UEdGraphPin* OldPin = OldPins[i];
		if (IsSpawnVarPin(OldPin))
		{
			Pins.Remove(OldPin);
			OldClassPins.Add(OldPin);
		}
	}

	CachedNodeTitle.MarkDirty();

	UClass* SampleDataClass = GetSampleDataClass();
	TArray<UEdGraphPin*> NewClassPins;
	if (SampleDataClass != NULL)
	{
		CreatePinsForClass(SampleDataClass, &NewClassPins);
	}

	RewireOldPinsToNewPins(OldClassPins, NewClassPins);

	DestroyPinList(OldClassPins);

	UEdGraph* Graph = GetGraph();
	Graph->NotifyGraphChanged();

	FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
}

#undef LOCTEXT_NAMESPACE