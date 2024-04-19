#pragma once

#include "cSoftBodyVerlet.h"


struct sSoftBodyThreadInfo
{
    cSoftBodyVerlet* p_theSoftBody;
    glm::vec3* force;
    // This is the ideal framerate we want
    double desiredUpdateTime = 0.0;
    // Stuff to control the thread (which never exits)
    bool bRun = false;	// if false, thread is idle
    bool bIsAlive = true;
    bool blob = false;
    DWORD sleepTime;	// Wait time for sleeping
};