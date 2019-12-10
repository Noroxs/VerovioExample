/////////////////////////////////////////////////////////////////////////////
// Name:        harm.cpp
// Author:      Laurent Pugin
// Created:     2016
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "harm.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "doc.h"
#include "editorial.h"
#include "f.h"
#include "fb.h"
#include "functorparams.h"
#include "measure.h"
#include "system.h"
#include "text.h"
#include "verticalaligner.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// Harm
//----------------------------------------------------------------------------

Harm::Harm()
    : ControlElement("harm-")
    , TextListInterface()
    , TextDirInterface()
    , TimeSpanningInterface()
    , AttLang()
    , AttNNumberLike()
{
    RegisterInterface(TextDirInterface::GetAttClasses(), TextDirInterface::IsInterface());
    RegisterInterface(TimeSpanningInterface::GetAttClasses(), TimeSpanningInterface::IsInterface());
    RegisterAttClass(ATT_LANG);
    RegisterAttClass(ATT_NNUMBERLIKE);

    Reset();
}

Harm::~Harm() {}

void Harm::Reset()
{
    ControlElement::Reset();
    TextDirInterface::Reset();
    TimeSpanningInterface::Reset();
    ResetLang();
    ResetNNumberLike();
}

void Harm::AddChild(Object *child)
{
    if (child->Is({ REND, TEXT })) {
        assert(dynamic_cast<TextElement *>(child));
    }
    else if (child->Is(FB)) {
        assert(dynamic_cast<Fb *>(child));
    }
    else if (child->IsEditorialElement()) {
        assert(dynamic_cast<EditorialElement *>(child));
    }
    else {
        LogError("Adding '%s' to a '%s'", child->GetClassName().c_str(), this->GetClassName().c_str());
        assert(false);
    }

    child->SetParent(this);
    m_children.push_back(child);
    Modify();
}

//----------------------------------------------------------------------------
// Harm functor methods
//----------------------------------------------------------------------------

int Harm::PrepareFloatingGrps(FunctorParams *functorParams)
{
    PrepareFloatingGrpsParams *params = dynamic_cast<PrepareFloatingGrpsParams *>(functorParams);
    assert(params);

    std::string n = this->GetN();

    for (auto &kv : params->m_harms) {
        if (kv.first == n) {
            this->SetDrawingGrpId(kv.second->GetDrawingGrpId());
            return FUNCTOR_CONTINUE;
        }
    }

    // first harm@n, create a new group
    this->SetDrawingGrpObject(this);
    params->m_harms.insert(std::make_pair(n, this));

    return FUNCTOR_CONTINUE;
}

int Harm::AdjustHarmGrpsSpacing(FunctorParams *functorParams)
{
    AdjustHarmGrpsSpacingParams *params = dynamic_cast<AdjustHarmGrpsSpacingParams *>(functorParams);
    assert(params);

    // If the harm is empty, do not adjust spacing
    if (!this->HasContentBB()) {
        return FUNCTOR_CONTINUE;
    }

    int currentGrpId = this->GetDrawingGrpId();

    // No group ID, nothing to do - should probably never happen
    if (currentGrpId == 0) {
        return FUNCTOR_SIBLINGS;
    }

    // We are filling the array of grp ids for the system
    if (params->m_currentGrp == 0) {
        // Look if we already have this grpId
        if (std::find(params->m_grpIds.begin(), params->m_grpIds.end(), currentGrpId) == params->m_grpIds.end()) {
            // if not, then just add to the list of grpIds for the system
            params->m_grpIds.push_back(currentGrpId);
        }
        // This is it for this pass
        return FUNCTOR_SIBLINGS;
    }
    // We are processing harm for a grp Id which is not the current one, skip it
    else if (currentGrpId != params->m_currentGrp) {
        return FUNCTOR_SIBLINGS;
    }

    /************** Find the widest positioner **************/

    // Get all the positioners for this object - all of them (all staves) because we can have different staff sizes
    ArrayOfFloatingPositioners positioners;
    params->m_currentSystem->m_systemAligner.FindAllPositionerPointingTo(&positioners, this);

    FloatingPositioner *harmPositioner = NULL;
    // Something is probably not right if nothing found - maybe no @staff
    if (positioners.empty()) {
        LogDebug("Something was wrong when searching positioners for %s '%s'", this->GetClassName().c_str(),
            this->GetUuid().c_str());
        return FUNCTOR_SIBLINGS;
    }

    // Keep the one with the lowest left position (this will also be the widest)
    for (auto const &positoner : positioners) {
        if (!harmPositioner || (harmPositioner->GetContentLeft() > positoner->GetContentLeft())) {
            harmPositioner = positoner;
        }
    }

    /************** Calculate the adjustment **************/

    assert(this->GetStart());
    assert(harmPositioner);

    // Not much to do when we hit the first syllable of the system
    if (params->m_previousHarmPositioner == NULL) {
        params->m_previousHarmStart = this->GetStart();
        params->m_previousHarmPositioner = harmPositioner;
        params->m_previousMeasure = NULL;
        return FUNCTOR_SIBLINGS;
    }

    int xShift = 0;

    // We have a previous harm from the previous measure - we need to add the measure with because the measures are
    // not aligned yet
    if (params->m_previousMeasure) {
        xShift = params->m_previousMeasure->GetWidth();
    }

    int overlap = params->m_previousHarmPositioner->GetContentRight() - (harmPositioner->GetContentLeft() + xShift);
    // Two units as default spacing
    int wordSpace = 2 * params->m_doc->GetDrawingUnit(100);

    // Adjust it proportionally to the lyric size
    wordSpace
        *= params->m_doc->GetOptions()->m_lyricSize.GetValue() / params->m_doc->GetOptions()->m_lyricSize.GetDefault();
    overlap += wordSpace;

    if (overlap > 0) {
        // We are adjusting syl in two different measures - move only the to right barline of the first measure
        if (params->m_previousMeasure) {
            params->m_overlapingHarm.push_back(std::make_tuple(params->m_previousHarmStart->GetAlignment(),
                params->m_previousMeasure->GetRightBarLine()->GetAlignment(), overlap));
            // Do it now
            params->m_previousMeasure->m_measureAligner.AdjustProportionally(params->m_overlapingHarm);
            params->m_overlapingHarm.clear();
        }
        else {
            // Normal case, both in the same measure
            params->m_overlapingHarm.push_back(std::make_tuple(
                params->m_previousHarmStart->GetAlignment(), this->GetStart()->GetAlignment(), overlap));
        }
    }

    params->m_previousHarmStart = this->GetStart();
    params->m_previousHarmPositioner = harmPositioner;
    params->m_previousMeasure = NULL;

    return FUNCTOR_SIBLINGS;
}

} // namespace vrv
