#include "cSoftBodyVerlet.h"

#include "OpenGLCommon.h"

// for the CreateThread(), etc.
#include <Windows.h>	// All things windows
#define WIN32_LEAN_AND_MEAN	

#include "sSoftBodyThreadInfo.h"

extern CRITICAL_SECTION softBodyUpdate;


DWORD WINAPI UpdateSoftBodyThread(LPVOID lpParameter)
{

    sSoftBodyThreadInfo* pSBTInfo = (sSoftBodyThreadInfo*)lpParameter;

    double lastFrameTime = glfwGetTime();
    double totalElapsedFrameTime = 0.0;

    DWORD sleepTime_ms = 1;

    while (pSBTInfo->bIsAlive)
    {
        if ( pSBTInfo->bRun )
        {
            // Adjust sleep time based on actual framerate
            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;

            totalElapsedFrameTime += deltaTime;

            if ( totalElapsedFrameTime >= pSBTInfo->desiredUpdateTime )
            {
                // Reset the elapsed time
                totalElapsedFrameTime = 0.0;

                // Update the object
                if (pSBTInfo->blob)
                {
                    if (pSBTInfo->force->x > 0.01f)
                    {
                        pSBTInfo->force->x -= 0.01f;
                    }
                    else if (pSBTInfo->force->x < -0.01f)
                    {
                        pSBTInfo->force->x += 0.01f;
                    }
                    else
                    {
                        pSBTInfo->force->x = 0.0f;
                    }

                    if (pSBTInfo->force->z > 0.01f)
                    {
                        pSBTInfo->force->z -= 0.01f;
                    }
                    else if (pSBTInfo->force->z < -0.01f)
                    {
                        pSBTInfo->force->z += 0.01f;
                    }
                    else
                    {
                        pSBTInfo->force->z = 0.0f;
                    }

                    if (pSBTInfo->force->y > 9.0f)
                    {
                        pSBTInfo->force->y -= 0.01f;
                    }
                    else
                    {
                        pSBTInfo->force->y = -9.0f;
                    }


                    pSBTInfo->p_theSoftBody->acceleration = *pSBTInfo->force;
                }
                else
                {
                    //pSBTInfo->p_theSoftBody->vec_pParticles[0]->position = glm::vec3(-90.0f, 50.0f, -10.0f);
                    //pSBTInfo->p_theSoftBody->vec_pParticles[17]->position = glm::vec3(-90.0f, 50.0f, 10.0f);
                    //pSBTInfo->p_theSoftBody->vec_pParticles[16]->position = glm::vec3(90.0f, 50.0f, -10.0f);
                    //pSBTInfo->p_theSoftBody->vec_pParticles[33]->position = glm::vec3(90.0f, 50.0f, 10.0f);
                    ////pSBTInfo->p_theSoftBody->vec_pParticles[0]->position = glm::vec3(0.0f);

                    pSBTInfo->p_theSoftBody->vec_pParticles[4]->position = glm::vec3(-96.0f, 25.0f, -5.0f);
                    pSBTInfo->p_theSoftBody->vec_pParticles[7]->position = glm::vec3(-96.0f, 25.0f, 5.0f);
                    pSBTInfo->p_theSoftBody->vec_pParticles[67]->position = glm::vec3(96.0f, 25.0f, -5.0f);
                    pSBTInfo->p_theSoftBody->vec_pParticles[64]->position = glm::vec3(96.0f, 25.0f, 5.0f);
                    /*pSBTInfo->p_theSoftBody->vec_pParticles[8]->position = glm::vec3(-96.0f, 29.4881f, -5.0f);
                    pSBTInfo->p_theSoftBody->vec_pParticles[9]->position = glm::vec3(-96.0f, 29.4881f, 5.0f);*/
                }
                /*if (pSBTInfo->force->x > 0.0f)
                {
                    pSBTInfo->p_theSoftBody->getHighestXParticle()->position.x += 0.2f;
                    pSBTInfo->force->x = 0.0f;
                }
                else if (pSBTInfo->force->x < 0.0f)
                {
                    pSBTInfo->p_theSoftBody->getLowestXParticle()->position.x -= 0.2f;
                    pSBTInfo->force->x = 0.0f;
                }
                
                if (pSBTInfo->force->z > 0.0f)
                {
                    pSBTInfo->p_theSoftBody->getHighestZParticle()->position.z += 0.2f;
                    pSBTInfo->force->z = 0.0f;
                }
                else if (pSBTInfo->force->z < 0.0f)
                {
                    pSBTInfo->p_theSoftBody->getLowestZParticle()->position.z -= 0.2f;
                    pSBTInfo->force->z = 0.0f;
                }

                if (pSBTInfo->force->y > 9.0f)
                {
                    pSBTInfo->force->y -= 0.01f;
                }
                else
                {
                    pSBTInfo->force->y = -9.0f;
                }
                
                pSBTInfo->p_theSoftBody->acceleration = *pSBTInfo->force;*/
                

                pSBTInfo->p_theSoftBody->VerletUpdate(pSBTInfo->desiredUpdateTime);
                
                
                if (pSBTInfo->blob)
                {
                    pSBTInfo->p_theSoftBody->SatisfyConstraints();

                    EnterCriticalSection(&softBodyUpdate);
                    pSBTInfo->p_theSoftBody->ApplyCollision(pSBTInfo->desiredUpdateTime);
                    LeaveCriticalSection(&softBodyUpdate);
                }
                else
                {
                    pSBTInfo->p_theSoftBody->SatisfyConstraints(10);
                }
                // These two update the pVertices array
                pSBTInfo->p_theSoftBody->UpdateVertexPositions();
                pSBTInfo->p_theSoftBody->UpdateNormals();

            }

            // Maybe something small like 1ms delay?
            Sleep(0);

        }//if ( pSBTInfo->bRun )
    }//while (pSBTInfo->bIsAlive)

    return 0;
}


void UpdateSoftBody(cSoftBodyVerlet &theSoftBody,
                    double deltaTime)
{

    theSoftBody.VerletUpdate(deltaTime);
    // 
    theSoftBody.ApplyCollision(deltaTime);
    // 
    theSoftBody.SatisfyConstraints();

    //    for (cSoftBodyVerlet::sParticle* pCurParticle : ::g_SoftBody.vec_pParticles)
    //    {
    //        ::g_DrawDebugSphere(pCurParticle->position, 0.01f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    //    }

        // Update the mesh in the VAO based on the current particle positions
    theSoftBody.UpdateVertexPositions();
    theSoftBody.UpdateNormals();

    return;
}




//sSoftBodyThreadInfo* pSBTInfo = (sSoftBodyThreadInfo*)lpParameter;
//
//double lastFrameTime = glfwGetTime();
//
//DWORD sleepTime_ms = 1;
//
//while (pSBTInfo->bIsAlive)
//{
//    if (pSBTInfo->bRun)
//    {
//        pSBTInfo->p_theSoftBody->VerletUpdate(pSBTInfo->desiredUpdateTime);
//    // 
//        pSBTInfo->p_theSoftBody->ApplyCollision(pSBTInfo->desiredUpdateTime);
//        // 
//        pSBTInfo->p_theSoftBody->SatisfyConstraints();
//
//        //    for (cSoftBodyVerlet::sParticle* pCurParticle : ::g_SoftBody.vec_pParticles)
//        //    {
//        //        ::g_DrawDebugSphere(pCurParticle->position, 0.01f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
//        //    }
//
//            // Update the mesh in the VAO based on the current particle positions
//        pSBTInfo->p_theSoftBody->UpdateVertexPositions();
//        pSBTInfo->p_theSoftBody->UpdateNormals();
//
//        // Adjust sleep time based on actual framerate
//        double currentTime = glfwGetTime();
//        double deltaTime = currentTime - lastFrameTime;
//        lastFrameTime = currentTime;
//
//        // See how much actual time we took
//        // Too fast (delta time too small)
//        if (deltaTime < pSBTInfo->desiredUpdateTime)
//        {
//            sleepTime_ms++;
//        }
//        // To slow (delta time too large)
//        if (deltaTime > pSBTInfo->desiredUpdateTime)
//        {
//            if (sleepTime_ms > 0)
//            {
//                sleepTime_ms--;
//            }
//        }
//
//    }//if ( pSBTInfo->bRun )
//    else
//    {
//        Sleep(sleepTime_ms);
//
//    }//if ( pSBTInfo->bRun )
//}//while (pSBTInfo->bIsAlive)
//
//return 0;
//}
