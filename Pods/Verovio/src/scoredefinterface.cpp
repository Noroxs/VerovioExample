/////////////////////////////////////////////////////////////////////////////
// Name:        scoredefinterface.cpp
// Author:      Laurent Pugin
// Created:     2015
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "scoredefinterface.h"

//----------------------------------------------------------------------------

#include <assert.h>

//----------------------------------------------------------------------------

#include "layerelement.h"
#include "staff.h"
#include "vrv.h"

namespace vrv {

//----------------------------------------------------------------------------
// ScoreDefInterface
//----------------------------------------------------------------------------

ScoreDefInterface::ScoreDefInterface()
    : Interface(), AttLyricStyle(), AttMeasureNumbers(), AttMidiTempo(), AttMultinumMeasures()
{
    RegisterInterfaceAttClass(ATT_LYRICSTYLE);
    RegisterInterfaceAttClass(ATT_MEASURENUMBERS);
    RegisterInterfaceAttClass(ATT_METERSIGDEFAULTLOG);
    RegisterInterfaceAttClass(ATT_METERSIGDEFAULTVIS);
    RegisterInterfaceAttClass(ATT_MIDITEMPO);
    RegisterInterfaceAttClass(ATT_MULTINUMMEASURES);

    Reset();
}

ScoreDefInterface::~ScoreDefInterface() {}

void ScoreDefInterface::Reset()
{
    ResetLyricStyle();
    ResetMeasureNumbers();
    ResetMidiTempo();
    ResetMultinumMeasures();
}

} // namespace vrv
