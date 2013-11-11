/*
* ==============================================================================
*  Name        : OOMHandler.h
*  Part of     : Webcore / Platform
*  Interface   :
*  Description :
*  Version     : %Version%
*
*    Copyright (c) 2006, Nokia Corporation
*    All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions
*   are met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in
*        the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the Nokia Corporation nor the names of its
*        contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*   USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*   DAMAGE.
*
*
* ==============================================================================
*/

#ifndef __OOMSTOPPER_H__
#define __OOMSTOPPER_H__

#include <e32def.h>
#include "MemoryManager.h"

namespace WebCore {

/*
 * This handles stopping things when OOM occurs, by stopping page loading.
 * The StopScheduler has two OOM modes, Normal and Emergency.  Normal mode 
 * calls all low priority stoppers.  Emergency mode calls all high priority stoppers.
 * (For some reason, emergency mode never seems to occur).   This stopper is low 
 * priority because we want to always stop page loading whenever OOM happens
 * (not just in emergency OOM situations).
 */
class OOMStopper : public MOOMStopper
{
public:
    OOMStopper();
    virtual ~OOMStopper();

    void Stop();

    // Priority is low because we always want to stop page loading when we're low on memory; not just during an emergency memory situation
    TOOMPriority Priority()     { return EOOM_PriorityLow; }
};

}

#endif // END OF FILE
