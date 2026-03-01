// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "Framework/VbImpactConfig.h"

void UVbImpactConfig::ResolveImpact(const FGameplayTag& EventTag, const FGameplayTagContainer& ContextTags, const EPhysicalSurface& Surface,
    FVbImpact& OutImpact) const
{
    OutImpact = FVbImpact{};

    if (!RulesTable || !EventTag.IsValid()) return;

    TArray<FVbImpactRule*> Rows;
    RulesTable->GetAllRows(TEXT(""), Rows);

    const FVbImpactRule* BestExact = nullptr;
    const FVbImpactRule* BestDefault = nullptr;

    for (const FVbImpactRule* Rule : Rows)
    {
        if (!Rule || !Rule->EventTag.IsValid()) continue;

        if (!EventTag.MatchesTag(Rule->EventTag)) continue;

        if (!ContextTags.HasAll(Rule->RequiredTags)) continue;
        if (ContextTags.HasAny(Rule->BlockedTags)) continue;

        if (Rule->Surface == Surface)
        {
            BestExact = Rule;
            break;
        }

        if (Rule->Surface == SurfaceType_Default && !BestDefault)
        {
            BestDefault = Rule;
        }
    }

    if (BestExact)
    {
        OutImpact = BestExact->Effects;
        return;
    }

    if (BestDefault)
    {
        OutImpact = BestDefault->Effects;
    }
}