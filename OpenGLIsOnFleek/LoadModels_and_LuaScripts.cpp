#include "cMesh.h"
//#include "sPhsyicsProperties.h"
#include "cVAOManager/cVAOManager.h"
#include "cPhysics.h"
#include <vector>
#include "LuaBrain/cLuaBrain.h"
#include "cParticleSystem.h"
#include "cGrassSystem.h"
#include "cLayoutLoader.h"
#include <sstream>
#include <iostream>

extern std::vector< cMesh* > g_vec_pMeshesToDraw;
//extern std::vector< sPhsyicsProperties* > g_vec_pPhysicalProps;
extern cPhysics* g_pPhysics;    //

extern cMesh* g_pDebugSphereMesh;

extern cVAOManager* g_pMeshManager;

extern cLayoutLoader* layout; 

// From main.cpp
extern cLuaBrain g_LuaBrain;
// Silly function binding example
void ChangeTaylorSwiftTexture(void);

// Our one and only emitter (for now, maybe)
extern cParticleSystem g_anEmitter;
extern cParticleSystem g_anEmitter2;

extern cGrassSystem g_grassPatch;


extern cMesh* g_pParticleMeshModel;

extern cMesh* g_pGrassBladeMeshModel;

extern cMesh* g_pOffscreenTextureQuad;



float getRandomFloat(float a, float b);

void LoadLuaScripts(void)
{
//    g_LuaBrain.


    return;
}


bool LoadModels(void)
{
    LoadLuaScripts();

    ::g_pDebugSphereMesh = new cMesh();
//    ::g_pDebugSphereMesh->meshName = "Sphere_1_unit_Radius.ply";
    ::g_pDebugSphereMesh->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    ::g_pDebugSphereMesh->bIsWireframe = true;
    ::g_pDebugSphereMesh->bDoNotLight = true;
    ::g_pDebugSphereMesh->setUniformDrawScale(1.0f);
    ::g_pDebugSphereMesh->bIsVisible = false;
    ::g_pDebugSphereMesh->friendlyName = "DEBUG_SPHERE";
    // Note: we are NOT adding this to the vector of meshes

    for (int i = 0; i < layout->modelPositions.size(); i++)
    {
        if (layout->filesToLoad[i] != "camera")
        {
            cMesh* pTempMesh = new cMesh();
            pTempMesh->meshName = layout->filesToLoad[i];

            pTempMesh->drawPosition = layout->modelPositions[i];
            pTempMesh->adjustRoationAngleFromEuler(layout->modelRotation[i]);
            pTempMesh->setUniformDrawScale(layout->modelScale[i]);

            pTempMesh->textureName[0] = "Texture_01_A.bmp";
            pTempMesh->textureRatios[0] = 1.0f;


            /*if (layout->filesToLoad[i] == "Sphere_1_unit_Radius.ply")
            {
                pTempMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                pTempMesh->bUseDebugColours = true;
            }*/
            if (layout->filesToLoad[i] == "SM_Env_GrassPatch_01.ply")
            {
                if ((int)getRandomFloat(0.0f, 10.0f) == 5)
                {
                    pTempMesh->textureName[1] = "sand.bmp";
                    pTempMesh->textureRatios[0] = getRandomFloat(0.7f, 1.0f);
                    pTempMesh->textureRatios[1] = 1.0f - pTempMesh->textureRatios[0];
                }
            }
            if (layout->filesToLoad[i] == "Big_Flat_Mesh_256x256_12_5_xyz_n_rgba_uv.ply")
            {
                pTempMesh->friendlyName = "ground";
                pTempMesh->textureName[0] = "sand.bmp";
                pTempMesh->textureName[1] = "landShape.bmp";
                pTempMesh->textureRatios[1] = 0.0f;
                pTempMesh->textureRatios[0] = 1.0f;

            }
            if (layout->filesToLoad[i] == "SM_Item_Bottle_01.ply")
            {
                pTempMesh->alpha_transparency = 0.7f;
            }
            if (layout->filesToLoad[i] == "SM_Item_Bottle_02.ply")
            {
                pTempMesh->alpha_transparency = 0.6f;
            }
            if (layout->filesToLoad[i] == "SM_Item_Bottle_03.ply")
            {
                pTempMesh->alpha_transparency = 0.8f;
            }
            if (layout->filesToLoad[i] == "SM_Item_Bottle_04.ply")
            {
                pTempMesh->alpha_transparency = 0.68f;
            }
            if (layout->filesToLoad[i] == "Big_Flat_Mesh_256x256_04_8K_xyz_n_rgba_uv.ply")//"Big_Flat_Mesh_256x256_12_5_xyz_n_rgba_uv2.ply")
            {
                pTempMesh->alpha_transparency = 0.9f;
                pTempMesh->textureName[1] = "fire.bmp";
                pTempMesh->textureName[0] = "flames.bmp";
                pTempMesh->textureRatios[1] = 1.0f;
                pTempMesh->textureRatios[0] = 0.0f;
                pTempMesh->friendlyName = "flames";
            }

            ::g_vec_pMeshesToDraw.push_back(pTempMesh);
        }

    }

    cMesh* pPlayer = new cMesh();
    pPlayer->meshName = "Ch44_nonPBR.dae";
   // pPlayer->meshName = "Rumba_Dancing.dae";
    //pPlayer->meshName = "dancing_vampire.dae";
    //pPlayer->meshName = "spiderManLegoFullAdjusted.ply";
    //pPlayer->meshName = "sphereWithManyVertices2.ply";
    pPlayer->friendlyName = "player";
    pPlayer->hasBones = true;
    pPlayer->setUniformDrawScale(0.1f);
    pPlayer->drawPosition = glm::vec3(0.0f, 3.4049f, 0.0f);
    //pPlayer->adjustRoationAngleFromEuler(glm::vec3(-1.5708f, -1.5708f, 0.0f));
    pPlayer->textureName[0] = "alien.bmp";
    pPlayer->textureName[1] = "alienNormal.bmp";
    //pPlayer->hasNormalMapSecondTexture = true;
    //pPlayer->textureName[0] = "Blue.bmp";
   // pPlayer->textureName[0] = "SpidermanUV_square.bmp";
    pPlayer->textureRatios[0] = 1.0f;
    //pPlayer->bIsVisible = false;

    sPhsyicsProperties* playerPhys = new sPhsyicsProperties();
    playerPhys->shapeType = sPhsyicsProperties::SPHERE;
    playerPhys->friendlyName = "player";
    playerPhys->setShape(new sPhsyicsProperties::sSphere(10.0f));
    playerPhys->inverse_mass = 1.0f;
    playerPhys->acceleration.y = -5.0f;
    playerPhys->pTheAssociatedMesh = pPlayer;
    playerPhys->meshOffset = glm::vec3(0.0f, -10.0f, 0.0f);

    
    ::g_pPhysics->AddShape(playerPhys);

    ::g_vec_pMeshesToDraw.push_back(pPlayer);

  
    cMesh* pSpaceShip = new cMesh();
    pSpaceShip->meshName = "spaceShip.ply";
    pSpaceShip->setUniformDrawScale(20.0f);
    pSpaceShip->textureName[0] = "Pixel_Large.bmp";
    pSpaceShip->textureRatios[0] = 1.0f;
    pSpaceShip->drawPosition = glm::vec3(20.0f, 30.0f, 100.0f);
    pSpaceShip->setDrawOrientation(glm::vec3(glm::radians(-70.0f), glm::radians(200.0f), 0.0f));
    //pSpaceShip->adjustRoationAngleFromEuler(glm::vec3(0.0f, 0.0f, glm::radians(180.0f)));
    ::g_vec_pMeshesToDraw.push_back(pSpaceShip);

    cMesh* pQuad = new cMesh();
    //pQuad->meshName = "grass.obj";
    pQuad->meshName = "StonePortal1.obj";
    //pQuad->friendlyName = "quad";
    pQuad->setUniformDrawScale(1.0f);
   // pQuad->bUseDebugColours = true;
   // pQuad->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    pQuad->drawPosition = glm::vec3(-40.0f, 37.2570f, -53.0f);
    pQuad->textureName[0] = "StonePortal1_Base_Diffuse.bmp";
    pQuad->textureName[1] = "StonePortal1_Base_Normal.bmp";
    //pQuad->textureName[1] = "smokeInvert.bmp";
    pQuad->textureRatios[0] = 1.0f;
    //pQuad->bDoNotLight = true;
    pQuad->hasNormalMapSecondTexture = true;
    ::g_vec_pMeshesToDraw.push_back(pQuad);

    
    for (int i = 0; i < 30; i++)
    {
        cMesh* tree = new cMesh();
        if (i % 3 == 0)
        {
            tree->meshName = "tree1.ply";
            tree->textureName[0] = "tree1base.bmp";
            tree->textureName[1] = "tree1norm.bmp";

        }
        else if (i % 3 == 1)
        {
            tree->meshName = "tree2.ply";

            tree->textureName[0] = "tree2base.bmp";
            tree->textureName[1] = "tree2norm.bmp";
        }
        else 
        {
            tree->meshName = "tree3.ply";
            tree->textureName[0] = "tree3base.bmp";
            tree->textureName[1] = "tree3norm.bmp";
        }
        
        int random1 = rand() % 2;
        int random2 = rand() % 2;
        random1 = (random1 == 0) ? 1 : -1;
        random2 = (random2 == 0) ? 1 : -1;

        tree->drawPosition = glm::vec3((rand() % 301 + 200) * random1, 0.0f, (rand() % 301 + 200) * random2);

        tree->setUniformDrawScale(10.0f);
        tree->textureRatios[0] = 1.0f;
        //tree->hasNormalMapSecondTexture = true;
        ::g_vec_pMeshesToDraw.push_back(tree);
    }

    cMesh* pBoneyWall = new cMesh();
    pBoneyWall->meshName = "BonyWall1.obj";
    pBoneyWall->setUniformDrawScale(1.0f);
    pBoneyWall->drawPosition = glm::vec3(0.0f, 40.0f, 0.0f);
    pBoneyWall->textureName[0] = "BonyWall1_BaseDark_Diffuse.bmp";
    pBoneyWall->textureName[1] = "BonyWall1_BaseDark_Normal.bmp";
    pBoneyWall->textureRatios[0] = 1.0f;
    pBoneyWall->hasNormalMapSecondTexture = true;
    //::g_vec_pMeshesToDraw.push_back(pBoneyWall);
    
    cMesh* StoneArch = new cMesh();
    StoneArch->meshName = "StoneArchLarge_Obj.obj";
    StoneArch->setUniformDrawScale(1.0f);
    StoneArch->drawPosition = glm::vec3(-200.0f, 30.0f, 0.0f);
    StoneArch->setDrawOrientation(glm::vec3(0.0f, glm::radians(70.0f), 0.0f));
    StoneArch->textureName[0] = "StoneArchLarge_Base-Diffuse.bmp";
    StoneArch->textureName[1] = "StoneArchLarge_Base-Normal.bmp";
    StoneArch->textureRatios[0] = 1.0f;
    StoneArch->hasNormalMapSecondTexture = true;
    ::g_vec_pMeshesToDraw.push_back(StoneArch);
    
    cMesh* pQuad2 = new cMesh();
    //pQuad2->meshName = "grass.obj";
    //pQuad2->meshName = "Big_Flat_Mesh_256x256_12_5_xyz_n_rgba_uv.ply";
    pQuad2->meshName = "planeObject2.obj";
    //pQuad2->meshName = "Quad_1_sided_aligned_on_XY_plane.ply";
    //pQuad2->friendlyName = "quad";
    pQuad2->setUniformDrawScale(1.0f);
   // pQuad2->bUseDebugColours = true;
   // pQuad2->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    pQuad2->drawPosition = glm::vec3(-150.0f, 0.0f, -53.0f);
    pQuad2->textureName[0] = "brickwall.bmp";
    pQuad2->textureName[1] = "brickwall_normal.bmp";
    //pQuad2->textureName[1] = "smokeInvert.bmp";
    pQuad2->textureRatios[0] = 1.0f;
    //pQuad2->bDoNotLight = true;
    //pQuad2->setDrawOrientation(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
    pQuad2->hasNormalMapSecondTexture = true;
    //::g_vec_pMeshesToDraw.push_back(pQuad2);

    cMesh* pBathtub = new cMesh();
//    pBathtub->meshName = "bathtub_xyz_n_rgba_uv.ply";
//    pBathtub->meshName = "bathtub_xyz_n_rgba_uv_x3_size.ply";
    pBathtub->meshName = "camera.ply";
    pBathtub->friendlyName = "bathtub";
    pBathtub->setUniformDrawScale(1.0f);
    pBathtub->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
    pBathtub->drawPosition = glm::vec3(-50.0f, 20.0f, 0.0f);
    // 
    pBathtub->textureName[0] = "Yellow.bmp";
    pBathtub->textureRatios[0] = 1.0f;

    pBathtub->drawPosition.x = 60.0f;

//    pBathtub->bIsVisible = false;

    //::g_vec_pMeshesToDraw.push_back(pBathtub);

    cMesh* pCamera = new cMesh();
    //    pCamera->meshName = "bathtub_xyz_n_rgba_uv.ply";
    //    pCamera->meshName = "bathtub_xyz_n_rgba_uv_x3_size.ply";
    pCamera->meshName = "camera.ply";
    //pCamera->friendlyName = "bathtub";
    pCamera->setUniformDrawScale(1.0f);
    pCamera->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), glm::radians(-180.0f), 0.0f));
    pCamera->drawPosition = glm::vec3(0.0f, 20.0f, 185.0f);
    // 
    pCamera->textureName[0] = "Yellow.bmp";
    pCamera->textureRatios[0] = 1.0f;

    

    //    pCamera->bIsVisible = false;

    ::g_vec_pMeshesToDraw.push_back(pCamera);

    //cMesh* tv1 = new cMesh();
    //tv1->meshName = "RetroTV.edited.bodyonly.ply";
    //tv1->friendlyName = "tv1";
    //tv1->textureName[0] = "Yellow.bmp";
    //tv1->textureRatios[0] = 1.0f;
    //tv1->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), glm::radians(45.0f), 0.2f));
    //tv1->setUniformDrawScale(0.15f);
    //tv1->drawPosition = glm::vec3(-40.0f, 2.0f, 20.0f);
    //::g_vec_pMeshesToDraw.push_back(tv1);

    //cMesh* screen1 = new cMesh();
    //screen1->meshName = "RetroTV.edited.screenonly.ply";
    //screen1->friendlyName = "screen1";
    ////screen1->textureName[0] = "Yellow.bmp";
    ////screen1->textureRatios[0] = 1.0f;
    //screen1->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), glm::radians(45.0f), 0.2f));
    //screen1->setUniformDrawScale(0.15f);
    //screen1->drawPosition = glm::vec3(-40.0f, 2.0f, 20.0f);
    //::g_vec_pMeshesToDraw.push_back(screen1);
    //
    //cMesh* tv2 = new cMesh();
    //tv2->meshName = "RetroTV.edited.bodyonly.ply";
    //tv2->friendlyName = "tv2";
    //tv2->textureName[0] = "Yellow.bmp";
    //tv2->textureRatios[0] = 1.0f;
    //tv2->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), glm::radians(75.0f), -0.147f));
    //tv2->setUniformDrawScale(0.1f);
    //tv2->drawPosition = glm::vec3(-50.0f, 2.0f, 35.0f);
    //::g_vec_pMeshesToDraw.push_back(tv2);

    //cMesh* screen2 = new cMesh();
    //screen2->meshName = "RetroTV.edited.screenonly.ply";
    //screen2->friendlyName = "screen2";
    //screen2->textureName[1] = "hud1.bmp";
    //screen2->useOtherTextures = true;
    //screen2->textureRatios[1] = 1.0f;
    ////screen2->bDoNotLight = true;
    //screen2->setRotationFromEuler(glm::vec3(glm::radians(-90.0f), glm::radians(75.0f), -0.147f));
    //screen2->setUniformDrawScale(0.1f);
    //screen2->drawPosition = glm::vec3(-50.0f, 2.0f, 35.0f);
    //::g_vec_pMeshesToDraw.push_back(screen2);


    cMesh* cube1 = new cMesh();
    //animatedXwing->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    //cube1->meshName = "xwing3.ply";
    cube1->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    cube1->friendlyName = "xwingog";
    cube1->setUniformDrawScale(1.0f);
    cube1->bDoNotLight = true;
    //cube1->drawPosition = glm::vec3(0.0f, 10.0f, 0.0f);
    cube1->textureName[0] = "Texture_01_A.bmp";
    cube1->textureRatios[0] = 1.0f;
    cube1->hasBones = true;
    //animatedXwing->bUseDebugColours = true;
    cube1->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

    Animation* newAnimation = new Animation();
    newAnimation->m_Time = 100.0;
    newAnimation->m_Length = 8.0;
    newAnimation->m_Loop = true;
    newAnimation->m_PositionKeyFrames.push_back(PositionKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 0.0));
    newAnimation->m_PositionKeyFrames.push_back(PositionKeyFrame(glm::vec3(-20.0f, 0.0f, 0.0f), 1.0));
    newAnimation->m_PositionKeyFrames.push_back(PositionKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 2.0));
    newAnimation->m_PositionKeyFrames.push_back(PositionKeyFrame(glm::vec3(20.0f, 0.0f, 0.0f), 3.0));
    newAnimation->m_PositionKeyFrames.push_back(PositionKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 4.0));
    
    Animation* newAnimation2 = new Animation();
    newAnimation2->m_Time = 100.0;
    newAnimation2->m_Length = 8.0;
    newAnimation2->m_Loop = true;
    newAnimation2->m_RotationKeyFrames.push_back(RotationKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f));
    newAnimation2->m_RotationKeyFrames.push_back(RotationKeyFrame(glm::vec3(0.0f, 3.14159f, 0.0f), 4.0f));
    newAnimation2->m_RotationKeyFrames.push_back(RotationKeyFrame(glm::vec3(0.0f, 0.0f, 0.0f), 8.0f));

    Animation* newAnimation3 = new Animation();
    newAnimation3->m_Time = 100.0;
    newAnimation3->m_Length = 8.0;
    newAnimation3->m_Loop = true;
    newAnimation3->m_ScaleKeyFrames.push_back(ScaleKeyFrame(glm::vec3(1.0f, 1.0f, 1.0f), 0.0));
    newAnimation3->m_ScaleKeyFrames.push_back(ScaleKeyFrame(glm::vec3(2.0f, 2.0f, 2.0f), 4.0));
    newAnimation3->m_ScaleKeyFrames.push_back(ScaleKeyFrame(glm::vec3(1.0f, 1.0f, 1.0f), 8.0));
    
        
    cMesh* cube2 = new cMesh();
    //cube2->meshName = "xwing3.ply";
    cube2->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    cube2->friendlyName = "xwing1";
    cube2->setUniformDrawScale(1.0f);
    //Xwing->hasBones = true;
    cube2->bIsVisible = false;
    cube2->m_Animation = newAnimation2;

    cube1->m_Animation = newAnimation2;
    //cube1->vec_pChildMeshes.push_back(cube2); 
    //::g_vec_pMeshesToDraw.push_back(cube1);


    //cMesh* cube3 = new cMesh();
    ////animatedXwing->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    ////animatedXwing->meshName = "xwing3.ply";
    //cube3->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    //cube3->friendlyName = "xwingog";
    //cube3->setUniformDrawScale(1.0f);
    //cube3->bDoNotLight = true;
    ////animatedXwing->drawPosition = glm::vec3(0.0f, 10.0f, 0.0f);
    //cube3->textureName[0] = "Texture_01_A.bmp";
    //cube3->textureRatios[0] = 1.0f;
    //cube3->hasBones = true;

    //cMesh* cube4 = new cMesh();
    ////Xwing->meshName = "xwing3.ply";
    //cube4->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    //cube4->friendlyName = "xwing1";
    //cube4->setUniformDrawScale(1.0f);
    ////Xwing->hasBones = true;
    //cube4->bIsVisible = false;
    //cube4->m_Animation = newAnimation;

    //cube3->m_Animation = newAnimation;
    //cube3->vec_pChildMeshes.push_back(cube4);
    //::g_vec_pMeshesToDraw.push_back(cube3);


    //cMesh* cube5 = new cMesh();
    ////animatedXwing->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    ////animatedXwing->meshName = "xwing3.ply";
    //cube5->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    //cube5->friendlyName = "xwingog";
    //cube5->setUniformDrawScale(1.0f);
    //cube5->bDoNotLight = true;
    //cube5->drawPosition = glm::vec3(0.0f, -10.0f, 0.0f);
    //cube5->textureName[0] = "Texture_01_A.bmp";
    //cube5->textureRatios[0] = 1.0f;
    //cube5->hasBones = true;

    //cMesh* cube6 = new cMesh();
    ////Xwing->meshName = "xwing3.ply";
    //cube6->meshName = "Cube_1x1x1_xyz_n_rgba_for_Verlet.ply";
    //cube6->friendlyName = "xwing1";
    //cube6->setUniformDrawScale(1.0f);
    ////Xwing->hasBones = true;
    //cube6->bIsVisible = false;
    //cube6->m_Animation = newAnimation3;

    //cube5->m_Animation = newAnimation3;
    //cube5->vec_pChildMeshes.push_back(cube6);
    //::g_vec_pMeshesToDraw.push_back(cube5);
//
#pragma region enemyInstatiate
    cMesh* pEnemy1 = new cMesh();
    //pEnemy1->meshName = "dancing_vampire.dae";
    pEnemy1->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
   // pEnemy1->meshName = "spaceShip.ply";
   // pEnemy1->meshName = "StonePortal1.obj";
    pEnemy1->friendlyName = "enemy1";
    pEnemy1->setUniformDrawScale(1.0f);
    //pEnemy1->hasBones = true;
    pEnemy1->drawPosition = glm::vec3(5.0f, 10.0f, 0.0f);
    pEnemy1->bDoNotLight = true;
    pEnemy1->bUseDebugColours = true;
    pEnemy1->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy1->textureName[0] = "Texture_01_A.bmp";
    //pEnemy1->textureName[1] = "StonePortal1_Base_Normal.bmp";
    //pEnemy1->hasNormalMapSecondTexture = true;
    pEnemy1->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy1);



    cMesh* pEnemy2 = new cMesh();
    pEnemy2->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy2->friendlyName = "enemy2";
    pEnemy2->setUniformDrawScale(1.0f);
    pEnemy2->drawPosition = glm::vec3(15.0f, 10.0f, 0.0f);
    pEnemy2->bDoNotLight = true;
    pEnemy2->bUseDebugColours = true;
    pEnemy2->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy2->textureName[0] = "Texture_01_A.bmp";
    pEnemy2->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy2);

    cMesh* pEnemy3 = new cMesh();
    pEnemy3->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy3->friendlyName = "enemy3";
    pEnemy3->setUniformDrawScale(1.0f);
    pEnemy3->drawPosition = glm::vec3(25.0f, 10.0f, 0.0f);
    pEnemy3->bDoNotLight = true;
    pEnemy3->bUseDebugColours = true;
    pEnemy3->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy3->textureName[0] = "Texture_01_A.bmp";
    pEnemy3->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy3);

    cMesh* pEnemy4 = new cMesh();
    pEnemy4->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy4->friendlyName = "enemy4";
    pEnemy4->setUniformDrawScale(1.0f);
    pEnemy4->drawPosition = glm::vec3(35.0f, 10.0f, 0.0f);
    pEnemy4->bDoNotLight = true;
    pEnemy4->bUseDebugColours = true;
    pEnemy4->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy4->textureName[0] = "Texture_01_A.bmp";
    pEnemy4->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy4);

    cMesh* pEnemy5 = new cMesh();
    pEnemy5->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy5->friendlyName = "enemy5";
    pEnemy5->setUniformDrawScale(1.0f);
    pEnemy5->drawPosition = glm::vec3(45.0f, 10.0f, 0.0f);
    pEnemy5->bDoNotLight = true;
    pEnemy5->bUseDebugColours = true;
    pEnemy5->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy5->textureName[0] = "Texture_01_A.bmp";
    pEnemy5->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy5);

    cMesh* pEnemy6 = new cMesh();
    pEnemy6->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy6->friendlyName = "enemy6";
    pEnemy6->setUniformDrawScale(1.0f);
    pEnemy6->drawPosition = glm::vec3(55.0f, 10.0f, 0.0f);
    pEnemy6->bDoNotLight = true;
    pEnemy6->bUseDebugColours = true;
    pEnemy6->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy6->textureName[0] = "Texture_01_A.bmp";
    pEnemy6->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy6);
    
    cMesh* pEnemy7 = new cMesh();
    pEnemy7->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy7->friendlyName = "enemy7";
    pEnemy7->setUniformDrawScale(1.0f);
    pEnemy7->drawPosition = glm::vec3(5.0f, 10.0f, 10.0f);
    pEnemy7->bDoNotLight = true;
    pEnemy7->bUseDebugColours = true;
    pEnemy7->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy7->textureName[0] = "Texture_01_A.bmp";
    pEnemy7->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy7);
    
    cMesh* pEnemy8 = new cMesh();
    pEnemy8->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy8->friendlyName = "enemy8";
    pEnemy8->setUniformDrawScale(1.0f);
    pEnemy8->drawPosition = glm::vec3(15.0f, 10.0f, 10.0f);
    pEnemy8->bDoNotLight = true;
    pEnemy8->bUseDebugColours = true;
    pEnemy8->wholeObjectDebugColourRGBA = glm::vec4(1.0f);

    pEnemy8->textureName[0] = "Texture_01_A.bmp";
    pEnemy8->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy8);
    
    cMesh* pEnemy9 = new cMesh();
    pEnemy9->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy9->friendlyName = "enemy9";
    pEnemy9->setUniformDrawScale(1.0f);
    pEnemy9->drawPosition = glm::vec3(25.0f, 10.0f, 10.0f);
    pEnemy9->bDoNotLight = true;
    pEnemy9->bUseDebugColours = true;
    pEnemy9->wholeObjectDebugColourRGBA = glm::vec4(1.0f);


    pEnemy9->textureName[0] = "Texture_01_A.bmp";
    pEnemy9->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy9);
    
    cMesh* pEnemy10 = new cMesh();
    pEnemy10->meshName = "Ch50_nonPBR.dae";//"Ch50_nonPBR.dae";
    pEnemy10->hasBones = true;
    //pEnemy10->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy10->friendlyName = "enemy10";
    pEnemy10->setUniformDrawScale(0.1f);
    pEnemy10->drawPosition = glm::vec3(200.0f, 0.0f, 200.0f);
    //pEnemy10->bDoNotLight = true;
    pEnemy10->bUseDebugColours = true;
    pEnemy10->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);


    pEnemy10->textureName[0] = "Texture_01_A.bmp";
    pEnemy10->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy10);
    
    cMesh* pEnemy11 = new cMesh();
    pEnemy11->meshName = "Ch50_nonPBR.dae";//"Ch50_nonPBR.dae";
    pEnemy11->hasBones = true;
    //pEnemy10->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
    pEnemy11->friendlyName = "enemy11";
    pEnemy11->setUniformDrawScale(0.1f);
    pEnemy11->drawPosition = glm::vec3(-200.0f, 0.0f, 200.0f);
    //pEnemy10->bDoNotLight = true;
    pEnemy11->bUseDebugColours = true;
    pEnemy11->wholeObjectDebugColourRGBA = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);


    pEnemy11->textureName[0] = "Texture_01_A.bmp";
    pEnemy11->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pEnemy11);

#pragma endregion 


    cMesh* pGroundMesh = new cMesh();
//    pGroundMesh->meshName = "Terrain_xyz_n_rgba_uv.ply";
//    pGroundMesh->meshName = "Big_Flat_Mesh_256x256_00_132K_xyz_n_rgba_uv.ply";    
   // pGroundMesh->meshName = "Big_Flat_Mesh_256x256_07_1K_xyz_n_rgba_uv.ply";    
    pGroundMesh->meshName = "planeObject2.obj";    
   // pGroundMesh->meshName = "Big_Flat_Mesh_256x256_12_5_xyz_n_rgba_uv.ply";    
    //pGroundMesh->drawPosition.y = -5.0f;
    //pGroundMesh->drawPosition.z = -50.0f;
    pGroundMesh->friendlyName = "Ground";

    //pGroundMesh->bIsWireframe = true;
    //pGroundMesh->bDoNotLight = true;
    //pGroundMesh->wholeObjectDebugColourRGBA = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    //pGroundMesh->bUseDebugColours = true;
    //
//    pGroundMesh->textureName[0] = "NvF5e_height_map.bmp";
//    pGroundMesh->textureName[0] = "Blank_UV_Text_Texture.bmp";
   // pGroundMesh->textureName[0] = "TaylorSwift_Eras_Poster.bmp";
    pGroundMesh->textureName[0] = "sand2Colour.bmp";
    pGroundMesh->textureName[1] = "sand2Normal.bmp";
    //pGroundMesh->textureName[0] = "checkerboard.bmp";
//    pGroundMesh->textureName[0] = "stickers-explosion-texture.bmp";
   // pGroundMesh->textureRatios[0] = 1.0f;
    pGroundMesh->setUniformDrawScale(0.75f);
    pGroundMesh->hasNormalMapSecondTexture = true;

//    pGroundMesh->textureName[1] = "taylor-swift-jimmy-fallon.bmp";
//    pGroundMesh->textureRatios[1] = 0.0f;

    sPhsyicsProperties* pGroundPhys = new sPhsyicsProperties();
    pGroundPhys->shapeType = sPhsyicsProperties::AABB;
    pGroundPhys->friendlyName = "ground";
   // pGroundPhys->inverse_mass = -1.0f;
    pGroundPhys->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-500.0f, -10.0f, -500.0f), glm::vec3(500.0f, 0.0f, 500.0f)));
    ::g_pPhysics->AddShape(pGroundPhys);
    ::g_vec_pMeshesToDraw.push_back(pGroundMesh);
    
    cMesh* pMesh = new cMesh();
    pMesh->meshName = "planeObject2.obj";
    pMesh->textureName[0] = "sand2Colour.bmp";
    pMesh->textureName[1] = "sand2Normal.bmp";
    pMesh->setUniformDrawScale(0.75f);
    pMesh->hasNormalMapSecondTexture = true;
    pMesh->drawPosition.x = 192.0f;
    //pMesh->drawPosition.y = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(pMesh);


    cMesh* pZeroYMesh = new cMesh();
    pZeroYMesh->meshName = "Big_Flat_Mesh_256x256_00_132K_xyz_n_rgba_uv.ply";
    pZeroYMesh->friendlyName = "pZeroYMesh";
    pZeroYMesh->drawPosition.z = -50.0f;
    pZeroYMesh->drawPosition.y = 2.0f;
    pZeroYMesh->bIsWireframe = true;
    pZeroYMesh->bDoNotLight = true;
    pZeroYMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    pZeroYMesh->bUseDebugColours = true;
   // ::g_vec_pMeshesToDraw.push_back(pZeroYMesh);




    const float MAX_SPHERE_LOCATION = 30.0f;
    const float MAX_VELOCITY = 1.0f;

    // Make a bunch of spheres...
    const unsigned int NUMBER_OF_SPHERES = 0;
    for (unsigned int count = 0; count != NUMBER_OF_SPHERES; count++)
    {
        cMesh* pSphereMesh = new cMesh();
//        pSphereMesh->meshName = "Sphere_1_unit_Radius.ply";
        pSphereMesh->meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
        pSphereMesh->bUseDebugColours = true;
        pSphereMesh->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

        pSphereMesh->friendlyName = "Sphere";

        pSphereMesh->setUniformDrawScale(5.0f);

        // Add drawing mesh to the things to draw
        ::g_vec_pMeshesToDraw.push_back(pSphereMesh);

        // Now the physical object to match this
        sPhsyicsProperties* pSpherePhysProps = new sPhsyicsProperties();        // HEAP

        //pSpherePhysProps->velocity.y = getRandomFloat(0.0f, MAX_VELOCITY);
        pSpherePhysProps->velocity.x = 1.0f; //getRandomFloat(-MAX_VELOCITY, MAX_VELOCITY);
        //pSpherePhysProps->velocity.z = getRandomFloat(-MAX_VELOCITY, MAX_VELOCITY);

        pSpherePhysProps->acceleration.y = (-9.81f / 5.0f);

        //pSpherePhysProps->position.x = -10.0f;                      // getRandomFloat(-MAX_SPHERE_LOCATION, MAX_SPHERE_LOCATION);
        //pSpherePhysProps->position.z = -10.0f;                        // getRandomFloat(-MAX_SPHERE_LOCATION, MAX_SPHERE_LOCATION);
        //pSpherePhysProps->position.y = MAX_SPHERE_LOCATION; //  getRandomFloat(10.0f, MAX_SPHERE_LOCATION + 20.0f);
        pSpherePhysProps->position = glm::vec3(0.0f, 50.0f, 0.0f);
        // Mass of 10 somethings? kg?
        pSpherePhysProps->inverse_mass = 1.0f / 10.0f;


        pSpherePhysProps->shapeType = sPhsyicsProperties::SPHERE;

        // The rendered graphical object that moves with this physics object
        pSpherePhysProps->pTheAssociatedMesh = pSphereMesh;

//        sPhsyicsProperties::sSphere* pTemp = new sPhsyicsProperties::sSphere(1.0f);
//        pSpherePhysProps->setShape(pTemp);

        pSpherePhysProps->setShape( new sPhsyicsProperties::sSphere(5.0f) );
        ::g_pPhysics->AddShape(pSpherePhysProps);


    }//for ( unsigned int count...





//    float spiderScale = 10.0f;
//
//    cMesh* pSpiderMan_body = new cMesh();
//    pSpiderMan_body->setUniformDrawScale(spiderScale);
//    pSpiderMan_body->friendlyName = "SpiderManBody";
//    pSpiderMan_body->meshName = "legospiderman_body_xyz_n_rgba.ply";
////    pSpiderMan_body->drawOrientation.x = glm::radians(-90.0f);
////    pSpiderMan_body->drawOrientation.z = glm::radians(-90.0f);
//
//    pSpiderMan_body->adjustRoationAngleFromEuler( glm::vec3(-90.0f, 0.0f, 0.0f) );
////    pSpiderMan_body->adjustRoationAngleFromEuler( glm::vec3(0.0f, 0.0f, -90.0f) );
//
//
//
//    ::g_vec_pMeshesToDraw.push_back(pSpiderMan_body);
//
//    // Now add these as child meshes
//    cMesh* pSpiderMan_head = new cMesh();
//    pSpiderMan_head->setUniformDrawScale(spiderScale);
//    pSpiderMan_head->meshName = "legospiderman_head_xyz_n_rgba.ply";
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_head);
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManHead);
//
//    cMesh* pSpiderMan_Hips = new cMesh();
//    pSpiderMan_Hips->setUniformDrawScale(spiderScale);
//    pSpiderMan_Hips->meshName = "legospiderman_Hips_xyz_n_rgba.ply";
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Hips);
//
//    cMesh* pSpiderMan_Left_arm = new cMesh();
//    pSpiderMan_Left_arm->meshName = "legospiderman_Left_arm_xyz_n_rgba.ply";
//    pSpiderMan_Left_arm->setUniformDrawScale(spiderScale);
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Left_arm);
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManLeft_arm);
//
//    cMesh* pSpiderMan_Left_hand = new cMesh();
//    pSpiderMan_Left_hand->meshName = "legospiderman_Left_hand_xyz_n_rgba.ply";
//    pSpiderMan_Left_hand->setUniformDrawScale(spiderScale);
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Left_hand);
//
//    cMesh* pSpiderMan_Right_arm = new cMesh();
//    pSpiderMan_Right_arm->meshName = "legospiderman_Right_arm_xyz_n_rgba.ply";
//    pSpiderMan_Right_arm->setUniformDrawScale(spiderScale);
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Right_arm);
//
//    cMesh* pSpiderMan_Right_hand = new cMesh();
//    pSpiderMan_Right_hand->meshName = "legospiderman_Right_hand_xyz_n_rgba.ply";
//    pSpiderMan_Right_hand->setUniformDrawScale(spiderScale);
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Right_hand);
//
//    cMesh* pSpiderMan_Left_leg = new cMesh();
//    pSpiderMan_Left_leg->setUniformDrawScale(spiderScale);
//    pSpiderMan_Left_leg->meshName = "legospiderman_Left_leg_xyz_n_rgba.ply";
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Left_leg);
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManLeft_leg);
//
//    cMesh* pSpiderMan_Right_leg = new cMesh();
//    pSpiderMan_Right_leg->setUniformDrawScale(spiderScale);
//    pSpiderMan_Right_leg->meshName = "legospiderman_Right_leg_xyz_n_rgba.ply";
//    pSpiderMan_body->vec_pChildMeshes.push_back(pSpiderMan_Right_leg);
//
//
//
//
////    cMesh* pSpiderManHead = new cMesh();
////    pSpiderManHead->setUniformDrawScale(spiderScale);
////    pSpiderManHead->meshName = "legospiderman_head_xyz_n_rgba.ply";
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManHead);
////
////    cMesh* pSpiderManLeft_arm = new cMesh();
////    pSpiderManLeft_arm->meshName = "legospiderman_Left_arm_xyz_n_rgba.ply";
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManLeft_arm);
////
////    cMesh* pSpiderManLeft_leg = new cMesh();
////    pSpiderManLeft_leg->setUniformDrawScale(spiderScale);
////    pSpiderManLeft_leg->meshName = "legospiderman_Left_leg_xyz_n_rgba.ply";
////    ::g_vec_pMeshesToDraw.push_back(pSpiderManLeft_leg);

    ::g_pGrassBladeMeshModel = new cMesh();
    ::g_pGrassBladeMeshModel->setUniformDrawScale(100.0f);
    ::g_pGrassBladeMeshModel->meshName = "grass.obj";
    ::g_pGrassBladeMeshModel->friendlyName = "grassBlade";
    ::g_pGrassBladeMeshModel->bUseDebugColours = true;
    //::g_pGrassBladeMeshModel->bDoNotLight = true;

    cGrassSystem::sGrassInfo grassPatch;
    grassPatch.numOfBlades = 30;
    grassPatch.separationDistance = 0.5f;
    grassPatch.minBound = glm::vec3(20.0f, 0.0f, 20.0f);
    grassPatch.maxBound = glm::vec3(-20.0f, 0.0f, -20.0f);
    grassPatch.scaleDifferenceMin = glm::vec3(100.0f);
    grassPatch.scaleDifferenceMax = glm::vec3(150.0f);
    //grassPatch.colourMin = glm::vec4(2.0f / 255.0f, 77.0f / 255.0f, 22.0f / 255.0f, 1.0f);
    //grassPatch.colourMax = glm::vec4(18.0f / 255.0f, 219.0f / 255.0f, 71.0f / 255.0f, 1.0f);
    grassPatch.colourMin = glm::vec4(128.0f / 255.0f, 112.0f / 255.0f, 9.0f / 255.0f, 1.0f);
    grassPatch.colourMax = glm::vec4(250.0f / 255.0f, 222.0f / 255.0f, 40.0f / 255.0f, 1.0f);
    grassPatch.constantForce = glm::vec3(2.0f, 0.0f, 0.0f);

    ::g_grassPatch.InitializeGrass(grassPatch);


    ::g_pParticleMeshModel = new cMesh();
    ::g_pParticleMeshModel->setUniformDrawScale(1.0f);
//    ::g_pParticleMeshModel->meshName = "legospiderman_head_xyz_n_rgba_uv.ply";
//    ::g_pParticleMeshModel->meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";
    ::g_pParticleMeshModel->meshName = "Quad_1_sided_aligned_on_XY_plane.ply";
    ::g_pParticleMeshModel->friendlyName = "smokeParticle";
//    ::g_pParticleMeshModel->textureName[0] = "SpidermanUV_square.bmp";
    ::g_pParticleMeshModel->textureName[0] = "smoke.bmp";
    ::g_pParticleMeshModel->textureName[1] = "smokeInvert.bmp";
    //::g_pParticleMeshModel->textureName[0] = "stickers-explosion-texture.bmp";
    ::g_pParticleMeshModel->textureRatios[0] = 1.0f;
    //
    //::g_pParticleMeshModel->bDoNotLight = true;



    cParticleSystem::sEmitterInfo ballEmitter;
    ballEmitter.emitterPosition = glm::vec3(80.0f, 30.5f, 148.5f);
    //ballEmitter.emitterPosition = glm::vec3(0.0f, 5.0f, 0.0f);
    // A little bit to the sides (x & z)
    // 5.0 to 10.0 veclocity 'up' (in y)
    ballEmitter.initVelocityMin = glm::vec3(-3.0f, 10.0f, -3.0f);
    ballEmitter.initVelocityMax = glm::vec3( 3.0f, 15.0f, 3.0f);

    // Have them tumble (change orientation) a little bit
//    ballEmitter.orientationChangeMinRadians = glm::vec3(-0.1f, -0.1f, -0.1f);
//    ballEmitter.orientationChangeMaxRadians = glm::vec3( 0.1f,  0.1f,  0.1f);
    ballEmitter.orientationChangeMinRadians = glm::vec3( 0.0f, 0.0f, -0.1f);
    ballEmitter.orientationChangeMaxRadians = glm::vec3( 0.0f, 0.0f,  0.1f);

    ballEmitter.uniformScaleChangeMin = 5.0f;//0.003f;
    ballEmitter.uniformScaleChangeMax = 10.0f;// 0.003f;

    // A little bit of 'gravity'
    ballEmitter.constantForce = glm::vec3(4.0f, 0.0f, 0.0f);
    // 
    ballEmitter.minLifetime = 10.0f;
    ballEmitter.maxLifetime = 15.0f;
    ballEmitter.minNumParticlesPerUpdate = 1;
    ballEmitter.maxNumParticlesPerUpdate = 3;
    //
    ballEmitter.maxNumParticles = 500;

    ::g_anEmitter.InitializeParticles(ballEmitter);
    ::g_anEmitter.Enable();
    ::g_anEmitter.Show_MakeVisible();
    
    ballEmitter.emitterPosition = glm::vec3(7.0, 32.0f, 178.0f);
    ::g_anEmitter2.InitializeParticles(ballEmitter);
    ::g_anEmitter2.Enable();
    ::g_anEmitter2.Show_MakeVisible();


    // What's on the FBO??
    ::g_pOffscreenTextureQuad = new cMesh();
    ::g_pOffscreenTextureQuad->meshName = "Quad_2_sided_aligned_on_XY_plane.ply";
    ::g_pOffscreenTextureQuad->setUniformDrawScale(100.0f);
    ::g_pOffscreenTextureQuad->bDoNotLight = true;
    ::g_pOffscreenTextureQuad->bIsVisible = true;
    ::g_pOffscreenTextureQuad->drawPosition.x = +10.0f;

    ::g_pOffscreenTextureQuad->textureName[0] = "smoke.bmp";
    ::g_pOffscreenTextureQuad->textureRatios[0] = 1.0f;

    cMesh* pBlob = new cMesh();
    pBlob->meshName = "sphereWithManyVertices2.ply";
    //pBlob->meshName = "spiderManLegoFullAdjusted.ply";
   // pBlob->drawPosition.y = 5.0f;
    pBlob->textureName[0] = "Green.bmp";
    pBlob->textureRatios[0] = 0.9f;
    pBlob->textureName[1] = "Blue.bmp";
    pBlob->textureRatios[1] = 0.1f;
    pBlob->friendlyName = "blob";
    //pBlob->alpha_transparency = 0.9f;
    //::g_vec_pMeshesToDraw.push_back(pBlob);

    cMesh* pTieFighter = new cMesh();
    pTieFighter->meshName = "Tie-Fighter_cleaned.ply";
    pTieFighter->textureName[0] = "stickers-explosion-texture.bmp";
    pTieFighter->textureRatios[0] = 1.0f;
    pTieFighter->drawPosition.y = 30.0f;
    pTieFighter->drawPosition.x = 30.0f;
    pTieFighter->friendlyName = "TieFighter";
    //::g_vec_pMeshesToDraw.push_back(pTieFighter);


    cMesh* wheel = new cMesh();
    wheel->meshName = "Wheel_15.ply";
    wheel->textureName[0] = "stickers-explosion-texture.bmp";
    wheel->textureRatios[0] = 1.0f;
    wheel->friendlyName = "Wheel";
    
    cMesh* bridge = new cMesh();
    bridge->meshName = "bridge3.ply";
    bridge->textureName[0] = "stickers-explosion-texture.bmp";
    bridge->textureRatios[0] = 1.0f;
    bridge->friendlyName = "bridge";
    //::g_vec_pMeshesToDraw.push_back(bridge);

    cMesh* plank1 = new cMesh();
    plank1->meshName = "plank.ply";
    plank1->drawPosition.x = -80.0f;
    plank1->setUniformDrawScale(10.0f);
    plank1->friendlyName = "plank1";
    plank1->textureName[0] = "wood.bmp";
    plank1->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank1);

    //sPhsyicsProperties* pPlank1 = new sPhsyicsProperties();
    //pPlank1->shapeType = sPhsyicsProperties::AABB;
    //pPlank1->friendlyName = "plank1";
    // 
    //pPlank1->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pPlank1->inverse_mass = 1.0f / 10.0f; 
    ////pPlank1->acceleration.y = -5.0f;
    //pPlank1->pTheAssociatedMesh = plank1;
    //::g_pPhysics->AddShape(pPlank1);
    //
    cMesh* plank2 = new cMesh();
    plank2->meshName = "plank.ply";
    plank2->drawPosition.x = -70.0f;
    plank2->setUniformDrawScale(10.0f);
    plank2->friendlyName = "plank2";
    plank2->textureName[0] = "wood.bmp";
    plank2->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank2);

    //sPhsyicsProperties* pplank2 = new sPhsyicsProperties();
    //pplank2->shapeType = sPhsyicsProperties::AABB;
    //pplank2->friendlyName = "plank2";
    // 
    //pplank2->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank2->inverse_mass = 1.0f / 10.0f;
    ////pplank2->acceleration.y = -5.0f;
    //pplank2->pTheAssociatedMesh = plank2;
    //::g_pPhysics->AddShape(pplank2);
    //
    cMesh* plank3 = new cMesh();
    plank3->meshName = "plank.ply";
    plank3->drawPosition.x = -60.0f;
    plank3->setUniformDrawScale(10.0f);
    plank3->friendlyName = "plank3";
    plank3->textureName[0] = "wood.bmp";
    plank3->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank3);

    //sPhsyicsProperties* pplank3 = new sPhsyicsProperties();
    //pplank3->shapeType = sPhsyicsProperties::AABB;
    //pplank3->friendlyName = "plank3";
    // 
    //pplank3->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank3->inverse_mass = 1.0f / 10.0f;
    ////pplank3->acceleration.y = -5.0f;
    //pplank3->pTheAssociatedMesh = plank3;
    //::g_pPhysics->AddShape(pplank3);
    
    cMesh* plank4 = new cMesh();
    plank4->meshName = "plank.ply";
    plank4->drawPosition.x = -50.0f;
    plank4->setUniformDrawScale(10.0f);
    plank4->friendlyName = "plank4";
    plank4->textureName[0] = "wood.bmp";
    plank4->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank4);

    //sPhsyicsProperties* pplank4 = new sPhsyicsProperties();
    //pplank4->shapeType = sPhsyicsProperties::AABB;
    //pplank4->friendlyName = "plank4";
    // 
    //pplank4->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank4->inverse_mass = 1.0f / 10.0f;
    ////pplank4->acceleration.y = -5.0f;
    //pplank4->pTheAssociatedMesh = plank4;
    //::g_pPhysics->AddShape(pplank4);


    cMesh* plank5 = new cMesh();
    plank5->meshName = "plank.ply";
    plank5->drawPosition.x = -40.0f;
    plank5->setUniformDrawScale(10.0f);
    plank5->friendlyName = "plank5";
    plank5->textureName[0] = "wood.bmp";
    plank5->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank5);

    //sPhsyicsProperties* pplank5 = new sPhsyicsProperties();
    //pplank5->shapeType = sPhsyicsProperties::AABB;
    //pplank5->friendlyName = "plank5";
    // 
    //pplank5->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank5->inverse_mass = 1.0f / 10.0f;
    ////pplank5->acceleration.y = -5.0f;
    //pplank5->pTheAssociatedMesh = plank5;
    //::g_pPhysics->AddShape(pplank5);

    cMesh* plank6 = new cMesh();
    plank6->meshName = "plank.ply";
    plank6->drawPosition.x = -30.0f;
    plank6->setUniformDrawScale(10.0f);
    plank6->friendlyName = "plank6";
    plank6->textureName[0] = "wood.bmp";
    plank6->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank6);

    //sPhsyicsProperties* pplank6 = new sPhsyicsProperties();
    //pplank6->shapeType = sPhsyicsProperties::AABB;
    //pplank6->friendlyName = "plank6";
    // 
    //pplank6->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank6->inverse_mass = 1.0f / 10.0f;
    ////pplank6->acceleration.y = -5.0f;
    //pplank6->pTheAssociatedMesh = plank6;
    //::g_pPhysics->AddShape(pplank6);

    cMesh* plank7 = new cMesh();
    plank7->meshName = "plank.ply";
    //plank7->drawPosition.x = -20.0f;
    plank7->setUniformDrawScale(10.0f);
    plank7->friendlyName = "plank7";
    plank7->textureName[0] = "wood.bmp";
    plank7->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank7);

    //sPhsyicsProperties* pplank7 = new sPhsyicsProperties();
    //pplank7->shapeType = sPhsyicsProperties::AABB;
    //pplank7->friendlyName = "plank7";
    // 
    //pplank7->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank7->inverse_mass = 1.0f / 10.0f;
    ////pplank7->acceleration.y = -5.0f;
    //pplank7->pTheAssociatedMesh = plank7;
    //::g_pPhysics->AddShape(pplank7);

    cMesh* plank8 = new cMesh();
    plank8->meshName = "plank.ply";
    plank8->drawPosition.x = -10.0f;
    plank8->setUniformDrawScale(10.0f);
    plank8->friendlyName = "plank8";
    plank8->textureName[0] = "wood.bmp";
    plank8->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank8);

    //sPhsyicsProperties* pplank8 = new sPhsyicsProperties();
    //pplank8->shapeType = sPhsyicsProperties::AABB;
    //pplank8->friendlyName = "plank8";
    // 
    //pplank8->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank8->inverse_mass = 1.0f / 10.0f;
    ////pplank8->acceleration.y = -5.0f;
    //pplank8->pTheAssociatedMesh = plank8;
    //::g_pPhysics->AddShape(pplank8);

    cMesh* plank9 = new cMesh();
    plank9->meshName = "plank.ply";
    plank9->drawPosition.x = 0.0f;
    plank9->setUniformDrawScale(10.0f);
    plank9->friendlyName = "plank9";
    plank9->textureName[0] = "wood.bmp";
    plank9->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank9);

    //sPhsyicsProperties* pplank9 = new sPhsyicsProperties();
    //pplank9->shapeType = sPhsyicsProperties::AABB;
    //pplank9->friendlyName = "plank9";
    // 
    //pplank9->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank9->inverse_mass = 1.0f / 10.0f;
    ////pplank9->acceleration.y = -5.0f;
    //pplank9->pTheAssociatedMesh = plank9;
    //::g_pPhysics->AddShape(pplank9);

    cMesh* plank10 = new cMesh();
    plank10->meshName = "plank.ply";
    plank10->drawPosition.x = 10.0f;
    plank10->setUniformDrawScale(10.0f);
    plank10->friendlyName = "plank10";
    plank10->textureName[0] = "wood.bmp";
    plank10->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank10);

    //sPhsyicsProperties* pplank10 = new sPhsyicsProperties();
    //pplank10->shapeType = sPhsyicsProperties::AABB;
    //pplank10->friendlyName = "plank10";
    // 
    //pplank10->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank10->inverse_mass = 1.0f / 10.0f;
    ////pplank10->acceleration.y = -5.0f;
    //pplank10->pTheAssociatedMesh = plank10;
    //::g_pPhysics->AddShape(pplank10);

    cMesh* plank11 = new cMesh();
    plank11->meshName = "plank.ply";
    plank11->drawPosition.x = 20.0f;
    plank11->setUniformDrawScale(10.0f);
    plank11->friendlyName = "plank11";
    plank11->textureName[0] = "wood.bmp";
    plank11->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank11);

    //sPhsyicsProperties* pplank11 = new sPhsyicsProperties();
    //pplank11->shapeType = sPhsyicsProperties::AABB;
    //pplank11->friendlyName = "plank11";
    // 
    //pplank11->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank11->inverse_mass = 1.0f / 10.0f;
    ////pplank11->acceleration.y = -5.0f;
    //pplank11->pTheAssociatedMesh = plank11;
    //::g_pPhysics->AddShape(pplank11);

    cMesh* plank12 = new cMesh();
    plank12->meshName = "plank.ply";
    plank12->drawPosition.x = 30.0f;
    plank12->setUniformDrawScale(10.0f);
    plank12->friendlyName = "plank12";
    plank12->textureName[0] = "wood.bmp";
    plank12->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank12);

    //sPhsyicsProperties* pplank12 = new sPhsyicsProperties();
    //pplank12->shapeType = sPhsyicsProperties::AABB;
    //pplank12->friendlyName = "plank12";
    // 
    //pplank12->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank12->inverse_mass = 1.0f / 10.0f;
    ////pplank12->acceleration.y = -5.0f;
    //pplank12->pTheAssociatedMesh = plank12;
    //::g_pPhysics->AddShape(pplank12);

    cMesh* plank13 = new cMesh();
    plank13->meshName = "plank.ply";
    plank13->drawPosition.x = 40.0f;
    plank13->setUniformDrawScale(10.0f);
    plank13->friendlyName = "plank13";
    plank13->textureName[0] = "wood.bmp";
    plank13->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank13);

    //sPhsyicsProperties* pplank13 = new sPhsyicsProperties();
    //pplank13->shapeType = sPhsyicsProperties::AABB;
    //pplank13->friendlyName = "plank13";
    // 
    //pplank13->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank13->inverse_mass = 1.0f / 10.0f;
    ////pplank13->acceleration.y = -5.0f;
    //pplank13->pTheAssociatedMesh = plank13;
    //::g_pPhysics->AddShape(pplank13);

    cMesh* plank14 = new cMesh();
    plank14->meshName = "plank.ply";
    plank14->drawPosition.x = 50.0f;
    plank14->setUniformDrawScale(10.0f);
    plank14->friendlyName = "plank14";
    plank14->textureName[0] = "wood.bmp";
    plank14->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank14);

    //sPhsyicsProperties* pplank14 = new sPhsyicsProperties();
    //pplank14->shapeType = sPhsyicsProperties::AABB;
    //pplank14->friendlyName = "plank14";
    // 
    //pplank14->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank14->inverse_mass = 1.0f / 10.0f;
    ////pplank14->acceleration.y = -5.0f;
    //pplank14->pTheAssociatedMesh = plank14;
    //::g_pPhysics->AddShape(pplank14);

    cMesh* plank15 = new cMesh();
    plank15->meshName = "plank.ply";
    plank15->drawPosition.x = 60.0f;
    plank15->setUniformDrawScale(10.0f);
    plank15->friendlyName = "plank15";
    plank15->textureName[0] = "wood.bmp";
    plank15->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank15);

    //sPhsyicsProperties* pplank15 = new sPhsyicsProperties();
    //pplank15->shapeType = sPhsyicsProperties::AABB;
    //pplank15->friendlyName = "plank15";
    // 
    //pplank15->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank15->inverse_mass = 1.0f / 10.0f;
    ////pplank15->acceleration.y = -5.0f;
    //pplank15->pTheAssociatedMesh = plank15;
    //::g_pPhysics->AddShape(pplank15);

    cMesh* plank16 = new cMesh();
    plank16->meshName = "plank.ply";
    plank16->drawPosition.x = 70.0f;
    plank16->setUniformDrawScale(10.0f);
    plank16->friendlyName = "plank16";
    plank16->textureName[0] = "wood.bmp";
    plank16->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank16);

    //sPhsyicsProperties* pplank16 = new sPhsyicsProperties();
    //pplank16->shapeType = sPhsyicsProperties::AABB;
    //pplank16->friendlyName = "plank16";
    // 
    //pplank16->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank16->inverse_mass = 1.0f / 10.0f;
    ////pplank16->acceleration.y = -5.0f;
    //pplank16->pTheAssociatedMesh = plank16;
    //::g_pPhysics->AddShape(pplank16);

    cMesh* plank17 = new cMesh();
    plank17->meshName = "plank.ply";
    plank17->drawPosition.x = 80.0f;
    plank17->setUniformDrawScale(10.0f);
    plank17->friendlyName = "plank17";
    plank17->textureName[0] = "wood.bmp";
    plank17->textureRatios[0] = 1.0f;
    ::g_vec_pMeshesToDraw.push_back(plank17);

    //sPhsyicsProperties* pplank17 = new sPhsyicsProperties();
    //pplank17->shapeType = sPhsyicsProperties::AABB;
    //pplank17->friendlyName = "plank17";
    // 
    //pplank17->setShape(new sPhsyicsProperties::sAABB(glm::vec3(-0.4f, -0.05f, -1.0f), glm::vec3(0.4f, 0.05f, 1.0f)));
    //pplank17->inverse_mass = 1.0f / 10.0f;
    ////pplank17->acceleration.y = -5.0f;
    //pplank17->pTheAssociatedMesh = plank17;
    //::g_pPhysics->AddShape(pplank17);



    // Draw a ton of bunnies
//    float boxSize = 100.0f;
//    float boxStep = 10.0f;
//
//    std::stringstream ssName;
//
//    int bunnyCount = 0;
//
//    for ( float x = -boxSize; x <= boxSize; x += boxStep )
//    {
//        for ( float y = -boxSize; y <= boxSize; y += boxStep )
//        {
//            for ( float z = -(2.0f * boxSize); z <= 0; z += boxStep )
//            {
//                cMesh* pBunny = new cMesh();
////                pBunny->meshName = "bun_zipper_xyz_n_rgba_uv.ply";
////                pBunny->meshName = "bun_zipper_res4_xyz_n_rgba_uv.ply";
//                pBunny->textureName[0] = "stickers-explosion-texture.bmp";
//                pBunny->textureRatios[0] = 1.0f;
//                pBunny->setUniformDrawScale(30.0f);
//
//                pBunny->drawPosition.x = x;
//                pBunny->drawPosition.y = y;
//                pBunny->drawPosition.z = z;
//
//
//                //bun_zipper_xyz_n_rgba_uv.ply
//                //bun_zipper_res2_xyz_n_rgba_uv.ply
//                //bun_zipper_res3_xyz_n_rgba_uv.ply
//                //bun_zipper_res4_xyz_n_rgba_uv.ply
//
//                // Add some LOD info 
//                pBunny->vecLODs.push_back(cMesh::sLODInfo("bun_zipper_res4_xyz_n_rgba_uv.ply", FLT_MAX));
//                pBunny->vecLODs.push_back(cMesh::sLODInfo("bun_zipper_res3_xyz_n_rgba_uv.ply", 200.0f));
//                pBunny->vecLODs.push_back(cMesh::sLODInfo("bun_zipper_res2_xyz_n_rgba_uv.ply", 75.0f));
//                pBunny->vecLODs.push_back(cMesh::sLODInfo("bun_zipper_xyz_n_rgba_uv.ply", 25.0f));
//
//
//                std::stringstream ssName;
//                ssName << "Bunny_" << (int)x << "_" << (int)y << "_" << (int)z;
//                pBunny->friendlyName = ssName.str();
//
//                g_vec_pMeshesToDraw.push_back(pBunny);
//
//                bunnyCount++;
//            }
//        }
//
//    }
//
//    std::cout << "Bunny count = " << bunnyCount << std::endl;


    return true;
}
