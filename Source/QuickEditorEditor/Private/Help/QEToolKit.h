#pragma once
#include "CoreMinimal.h"

class FQEToolkit
    : public FAssetEditorToolkit
    , public FEditorUndoClient
    , public FGCObject
{
public:
	FQEToolkit();
	virtual ~FQEToolkit();
	
	void Initialize(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost);
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
};
