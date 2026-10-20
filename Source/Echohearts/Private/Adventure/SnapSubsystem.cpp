// Copyright 2026 Into Deep Studios and Donta L. Owens. All rights reserved.
#include "Adventure/SnapSubsystem.h"

void USnapSubsystem::CapturePhoto(EPhotoSubject Subject, FName SubjectId)
{
    if (SubjectId.IsNone())
    {
        return;
    }
    const bool bFirst = !CatalogedSubjects.Contains(SubjectId);
    CatalogedSubjects.Add(SubjectId);
    OnPhotoCaptured.Broadcast(Subject, bFirst);
}

int32 USnapSubsystem::GetCatalogCount() const
{
    return CatalogedSubjects.Num();
}
