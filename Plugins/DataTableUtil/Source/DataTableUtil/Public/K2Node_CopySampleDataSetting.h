#pragma once

#include "K2Node.h"
#include "EdGraph/EdGraphNodeUtils.h" // for FNodeTextCache
#include "K2Node_CopySampleDataSetting.generated.h"

UCLASS()
class DATATABLEUTIL_API UK2Node_CopySampleDataSetting : public UK2Node
{
	GENERATED_UCLASS_BODY()

		// Begin UEdGraphNode interface.
		virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual bool HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin);
	// End UEdGraphNode interface.

	// Begin UK2Node interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	// End UK2Node interface

private:
	void CreatePinsForClass(UClass* InClass, TArray<UEdGraphPin*>* OutClassPins = nullptr);

	UEdGraphPin* GetClassPin(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetSettingPin() const;
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetResultPin() const;

	UClass* GetSampleDataClass(const TArray<UEdGraphPin*>* InPinsToSearch = NULL) const;
	UClass* GetClassPinBaseClass() const;

	bool IsSpawnVarPin(UEdGraphPin* Pin);
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;
	void OnClassPinChanged();

private:
	FNodeTextCache CachedNodeTitle;
};