//https://www.glfw.org/docs/latest/quick.html
//http://graphics.stanford.edu/data/3Dscanrep/

#include "OpenGLCommon.h"
//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "cGlobal.h"

#include <iostream>
#include <fstream>      // C++ file IO (secret: it's a wraper for the c IO)
#include <sstream>      // like a string builder
#include <vector>       // Aka a "smart array"


//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtx/vector_angle.hpp> //glm::angle

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Basic Shader Manager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"

#include "GLWF_CallBacks.h" // keyboard and mouse input

#include "cMesh.h"
#include "cPlayer.h"
#include "cEnemy.h"
#include "cFlockEntity.h"
//#include "sPhsyicsProperties.h"
#include "cPhysics.h"

#include "cLightManager.h"
#include "cLightHelper.h"
#include "TextureManager/cBasicTextureManager.h"
#include "cHiResTimer.h"
#include "cCommand_MoveTo.h"
#include "LuaBrain/cLuaBrain.h"
#include "cParticleSystem.h"
#include "cGrassSystem.h"
#include "cSoftBodyVerlet.h"

#include "cLayoutLoader.h"
#include "AnimationManager.h"

// for the CreateThread(), etc.
#include <Windows.h>	// All things windows
#define WIN32_LEAN_AND_MEAN	

#include "sSoftBodyThreadInfo.h"
DWORD WINAPI UpdateSoftBodyThread(LPVOID lpParameter);
//unsigned long UpdateEntityThread( void* lpParameter );

CRITICAL_SECTION softBodyUpdate;


// Frame Buffer Object (i.e. we render to this instead of the main screen)
#include "FBO/cFBO.h"
// We are going to show what's on the FBO on this quad
cMesh* g_pOffscreenTextureQuad = NULL;


void windowSizeChangedCallback(GLFWwindow* window, int width, int height);

glm::vec3 g_cameraEye = glm::vec3(0.0, 20.0, 50.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

cPlayer* thePlayer = NULL;

double deltaTime = 0.0;

bool thirdPersonView = false;
int animationSpeed = 1;
unsigned long long frameNumber = 0;

// The thread will call this...
void UpdateSoftBody(cSoftBodyVerlet& theSoftBody,
                    double deltaTime);


cVAOManager* g_pMeshManager = NULL;

cBasicTextureManager* g_pTextureManager = NULL;

cMesh* g_pDebugSphereMesh = NULL;
// Used by g_DrawDebugSphere()
GLuint g_DebugSphereMesh_shaderProgramID = 0;

// HACK: I'm using this model as the "particle" model
cMesh* g_pParticleMeshModel = NULL;

cMesh* g_pGrassBladeMeshModel = NULL;

std::vector<double> g_vecLastFrameTimes;

std::vector<cEnemy*> enemyList;
cFlockEntity* flockEntity;

cParticleSystem g_anEmitter;
cParticleSystem g_anEmitter2;

cGrassSystem g_grassPatch;

cSoftBodyVerlet g_SoftBody;
glm::vec3 softBodyForce = glm::vec3(0.0f, -9.0f, 0.0f);
cSoftBodyVerlet g_Wheel;
cSoftBodyVerlet g_Bridge;

// Offscreen frame buffer object
cFBO* g_pFBO_1 = NULL;
cFBO* g_pFBO_2 = NULL;
cFBO* g_depthBuffer = NULL;
std::vector<cFBO*> g_pFBOList;


// Smart array of cMesh object
//std::vector<cMesh> g_vecMeshesToDraw;
//    ____       _       _                  _          __  __           _               
//   |  _ \ ___ (_)_ __ | |_ ___ _ __ ___  | |_ ___   |  \/  | ___  ___| |__   ___  ___ 
//   | |_) / _ \| | '_ \| __/ _ \ '__/ __| | __/ _ \  | |\/| |/ _ \/ __| '_ \ / _ \/ __|
//   |  __/ (_) | | | | | ||  __/ |  \__ \ | || (_) | | |  | |  __/\__ \ | | |  __/\__ \
//   |_|   \___/|_|_| |_|\__\___|_|  |___/  \__\___/  |_|  |_|\___||___/_| |_|\___||___/
//                            
// This is the list of meshes                                                           
std::vector< cMesh* > g_vec_pMeshesToDraw;
// This is the list of physical properties 
// This has a pointer to the matching mesh
//std::vector< sPhsyicsProperties* > g_vec_pPhysicalProps;
cPhysics* g_pPhysics = NULL;


// Returns NULL if not found
cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

cLightManager* g_pTheLights = NULL;

std::vector<cCommand_MoveTo> g_vecAnimationCommands;

//std::vector<sPhsyicsProperties*> g_vecThingsThePhysicsThingPaysAtte;
// 
int g_selectedMesh = 0;
int g_selectedLight = 0;

// layout maker
cLayoutLoader* layout;

// animation controll system
AnimationManager* g_Animations;

// Function signature
bool SaveVectorSceneToFile(std::string saveFileName);

bool LoadModels(void);

//void DoPhysicUpdate(double deltaTime);

void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

void DrawLightDebugSpheres(glm::mat4 matProjection, glm::mat4 matView,
                           GLuint shaderProgramID);

float getRandomFloat(float a, float b);

void LoadTheRobotronModels(GLuint shaderProgram);

// HACK:
float g_HeightAdjust = 10.0f;
glm::vec2 g_UVOffset = glm::vec2(0.0f, 0.0f);

// This is in the global space as an example, 
// mainly so that we can run scripts when we press keys
cLuaBrain g_LuaBrain;

// Silly Lua script binding example
void ChangeTaylorSwiftTexture(std::string newTexture)
{
//    pGroundMesh->friendlyName = "Ground";
//    pGroundMesh->textureName[0] = "TaylorSwift_Eras_Poster.bmp";

    cMesh* pGround = g_pFindMeshByFriendlyName("Ground");
    if ( pGround )
    {
//        pGround->textureName[0] = "TaylorSwift_Eras_Poster.bmp";
        pGround->textureName[0] = newTexture;   // "SpidermanUV_square.bmp";
    }
    return;
}

// Here the same sort of thing, but that Lua can call
int lua_ChangeTaylorSwiftTexture(lua_State* L)
{
//    ChangeTaylorSwiftTexture("SpidermanUV_square.bmp");

    // Look up whatever was passed on the stack
    // https://www.lua.org/pil/24.2.2.html:
    // "The first element in the stack (that is, the element that was pushed first) 
    //    has index 1, the next one has index 2, and so on."

    const char* bitMapName = lua_tostring(L, 1);
    std::string sBitMapName(bitMapName);

    ChangeTaylorSwiftTexture(sBitMapName);

    return 0;
}

// Here the same sort of thing, but that Lua can call
int lua_AddSerialMoveObjectCommand(lua_State* L)
{
    // We have to decide what we are typing into Lua.
    // It's 100% up to us
    // 
    // How about:
    // AddMoveCommand('bathtub', Dx, Dy, Dz, timeInSeconds)
    //
    // The first string is the friendly name
    // The Dx, etc. is "Destination" X,Y,Z



    std::string MeshFriendlyName(lua_tostring(L, 1));       // 'bathtub'
    cMesh* pBathTub = g_pFindMeshByFriendlyName(MeshFriendlyName);

    // https://pgl.yoyo.org/luai/i/lua_Number
    // All Lua numbers are stored as doubles
    // typedef double lua_Number;

    glm::vec3 destinationXYZ;
    destinationXYZ.x = (float)lua_tonumber(L, 2);     // Dx (destination X)
    destinationXYZ.y = (float)lua_tonumber(L, 3);     // Dy (destination Y)
    destinationXYZ.z = (float)lua_tonumber(L, 4);     // Dz (destination Z)
    float timeInSeconds = (float)lua_tonumber(L, 5);   // timeInSeconds

//    cCommand_MoveTo moveBathTub(pBathTub,
//                                pBathTub->getDrawPosition(),
//                                glm::vec3(50.0f, 0.0f, 0.0f),
//                                10.0f);

    cCommand_MoveTo moveBathTub(pBathTub,
                                pBathTub->getDrawPosition(),
                                destinationXYZ,
                                timeInSeconds);
 
    ::g_vecAnimationCommands.push_back(moveBathTub);


    return 0;
}

// 
int lua_GetMeshPositionByFriendlyName(lua_State* L)
{
    // Example Lua script might be:
    // 
    // bIsValid, x, y, z = GetMeshPositionByFriendlyName('bathtub');
    // 
    // 1st parameter indicates if we found this object or not
    // 
    // Returns the x, y, z location

    std::string MeshFriendlyName(lua_tostring(L, 1));       // 'bathtub'
    cMesh* pBathTub = g_pFindMeshByFriendlyName(MeshFriendlyName);

    if (pBathTub)
    {
        // 1st parameter: indicate if this object was found
        lua_pushboolean(L, true);
        lua_pushnumber( L, pBathTub->getDrawPosition().x);
        lua_pushnumber( L, pBathTub->getDrawPosition().y );
        lua_pushnumber( L, pBathTub->getDrawPosition().z );
        // Tell Lua how many things got pushed onto the stack
        return 4;
    }
    
    // Didn't find it
    lua_pushboolean(L, false);

    // We returned 1 value, a "false"
    return 1;
}

int lua_SetMeshPositionByFriendlyName(lua_State* L)
{
    // Example Lua script might be
    // SetMeshPositionByFriendlyName('bathtub', x, y, z )

    std::string MeshFriendlyName(lua_tostring(L, 1));       // 'bathtub'
    cMesh* pBathTub = g_pFindMeshByFriendlyName(MeshFriendlyName);

    glm::vec3 newLocationXYZ;
    newLocationXYZ.x = (float)lua_tonumber(L, 2);     // Dx (destination X)
    newLocationXYZ.y = (float)lua_tonumber(L, 3);     // Dy (destination Y)
    newLocationXYZ.z = (float)lua_tonumber(L, 4);     // Dz (destination Z)

    pBathTub->drawPosition = newLocationXYZ;

    return 0;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void TestSort(void);


// The drawing passes

// Draws the original screen (soft cloth, t-swizzle, skybox, etc.)
void DrawPass_1(GLuint shaderProgramID, GLFWwindow* pWindow, cHiResTimer* p_HRTimer,
                int screenWidth, int screenHeight,
                glm::vec3 cameraEye, glm::vec3 cameraTarget, bool shadowMap);

// Only draws the TV.
void DrawPass_2(GLuint shaderProgramID, GLFWwindow* pWindow,
                int screenWidth, int screenHeight);

// Only a full screen quad
void DrawPass_FSQ(GLuint shaderProgramID, GLFWwindow* pWindow,
                  int screenWidth, int screenHeight);

void DrawPass_GaussianBlur(GLuint shaderProgramID, GLFWwindow* pWindow,
    int screenWidth, int screenHeight, int fboNumber, bool firstIt, bool horizontal);

void DrawPass_Depth(GLuint shaderProgramID, GLFWwindow* pWindow, cHiResTimer* p_HRTimer,
    int screenWidth, int screenHeight, glm::vec3 sceneEye, glm::vec3 sceneTarget);

void DrawPass_Dilate(GLuint shaderProgramID, GLFWwindow* pWindow,
    int screenWidth, int screenHeight);

void DrawPass_DOF(GLuint shaderProgramID, GLFWwindow* pWindow,
    int screenWidth, int screenHeight);


int main(void)
{
    std::cout << "Loading Game..." << std::endl;

    GLFWwindow* window;
//    GLuint vertex_buffer; //, vertex_shader, fragment_shader;//v , program;
//    GLint mvp_location;// , vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetWindowSizeCallback(window, windowSizeChangedCallback);
    glfwSetCursorPosCallback(window, cursor_position_callback);      

    glfwMakeContextCurrent(window);
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor and lock it to the screen

//    glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
//    FARPROC gl_glShaderSourceFucntion = GetProcAddress(NULL, "glShaderSource");
//    gl_glShaderSourceFucntion(...);

    // Create an offscreen frame buffer object
    ::g_pFBO_1 = new cFBO();
    std::string FBOError;
    if ( ! ::g_pFBO_1->init(1920, 1080, FBOError) )
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }


    // Create an offscreen frame buffer object
    ::g_pFBO_2 = new cFBO();
    if ( ! ::g_pFBO_2->init(1920, 1080, FBOError) )
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }
    ::g_depthBuffer = new cFBO();
    if ( ! ::g_depthBuffer->initShadowFBO(2048, 2048, FBOError) )
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }

    cFBO* newFBO1 = new cFBO();
    if (!newFBO1->init(1920, 1080, FBOError))
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }
    ::g_pFBOList.push_back(newFBO1);
    
    cFBO* newFBO2 = new cFBO();
    if (!newFBO2->init(1920, 1080, FBOError))
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }
    ::g_pFBOList.push_back(newFBO2);
    
    cFBO* newFBO3= new cFBO();
    if (!newFBO3->init(1920, 1080, FBOError))
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }
    ::g_pFBOList.push_back(newFBO3);
    
    cFBO* newFBO4 = new cFBO();
    if (!newFBO4->init(1920, 1080, FBOError))
    {
        std::cout << "Error creating FBO: " << FBOError << std::endl;
    }
    else
    {
        std::cout << "FBO created OK." << std::endl;
    }
    ::g_pFBOList.push_back(newFBO4);


#pragma region AllTheLoadingCarp

    cHiResTimer* p_HRTimer = new cHiResTimer(60);


    cShaderManager* pShaderThing = new cShaderManager();
    pShaderThing->setBasePath("assets/shaders");

    cShaderManager::cShader vertexShader;
    vertexShader.fileName = "vertexShader01.glsl";

    // Add a geometry shader
    cShaderManager::cShader geometryShader;
    geometryShader.fileName = "geometryPassThrough.glsl";


    cShaderManager::cShader fragmentShader;
    fragmentShader.fileName = "fragmentShader01.glsl";

    cShaderManager::cShader shadowVert;
    shadowVert.fileName = "shadowVertexShader01.glsl";

    cShaderManager::cShader shadowFrag;
    shadowFrag.fileName = "shadowFragmentShader01.glsl";

   // if ( ! pShaderThing->createProgramFromFile("shader01", vertexShader, fragmentShader ) )
    if ( ! pShaderThing->createProgramFromFile("shader01", vertexShader, geometryShader, fragmentShader))
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << pShaderThing->getLastError();
        return -1;
    }
    
    if ( ! pShaderThing->createProgramFromFile("shadowShader01", shadowVert, geometryShader, shadowFrag) )
    {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << pShaderThing->getLastError();
        return -1;
    }

    GLuint shaderProgramID = pShaderThing->getIDFromFriendlyName("shader01");
    GLuint shadowShaderProgramID = pShaderThing->getIDFromFriendlyName("shadowShader01");



    // Set the debug shader ID we're going to use
    ::g_DebugSphereMesh_shaderProgramID = shaderProgramID;

    ::g_pMeshManager = new cVAOManager();

    ::g_pMeshManager->setBasePath("assets/models");

    std::string fileSceneName = "scene1.txt";
    std::string basepathForScene = "assets/layout/";
    layout = new cLayoutLoader();
    layout->setBasepath(basepathForScene);
    if (!layout->loadSourceFile(fileSceneName))
    {
        std::cout << "Error: Couldn't open scene file" << std::endl;
        return -1;
    }
    layout->loadLayout();

    sModelDrawInfo tempModelInfo;
    for (int i = 0; i < layout->filesToLoad.size(); i++)
    {
        if (layout->filesToLoad[i] != "camera")
        {
            ::g_pMeshManager->LoadModelIntoVAO(layout->filesToLoad[i], tempModelInfo, shaderProgramID);
            // std::cout << "Loaded: h[" << tempModelInfo.numberOfVertices << " vertices" << std::endl;

        }
        else if (layout->filesToLoad[i] == "camera")
        {
            ::g_cameraEye = layout->modelPositions[i];
            ::g_cameraTarget = layout->modelRotation[i];
        }

    }

    // START OF: FOR THE SOFT BODY OBJECTS
    sModelDrawInfo bunnyDrawingInfo;
//    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_res2_xyz_n_rgba_trivialUV_offset_Y.ply",
    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_res3_xyz_n_rgba_trivialUV_offset_Y.ply",
                                   bunnyDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << bunnyDrawingInfo.numberOfVertices << " vertices" << std::endl;

    sModelDrawInfo verletCubeDrawingInfo;
    ::g_pMeshManager->LoadModelIntoVAO("Cube_1x1x1_xyz_n_rgba_for_Verlet.ply",
                                       verletCubeDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << verletCubeDrawingInfo.numberOfVertices << " vertices" << std::endl;

    sModelDrawInfo bathtubDrawingInfo;
//    ::g_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba_uv.ply",
//    ::g_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba_uv_x3_size.ply",
    ::g_pMeshManager->LoadModelIntoVAO("bathtub_xyz_n_rgba_uv_x3_size_Offset_in_Y.ply",
                                   bathtubDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << bathtubDrawingInfo.numberOfVertices << " vertices" << std::endl;

    sModelDrawInfo softBodyMesh;
    ::g_pMeshManager->LoadModelIntoVAO("Grid_10x10_vertical_in_y.ply", softBodyMesh, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("Grid_10x10_XZ_plane_at_origin.ply", softBodyMesh, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("Grid_100x100_vertical_in_y.ply", softBodyMesh, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("Grid_100x100_XZ_plane_at_origin.ply", softBodyMesh, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("camera.ply", softBodyMesh, shaderProgramID);

    // END OF: Soft body objects

    sModelDrawInfo terrainDrawingInfo;
    sModelDrawInfo terrainDrawingInfo2;
//    ::g_pMeshManager->LoadModelIntoVAO("Terrain_xyz_n_rgba.ply",
    ::g_pMeshManager->LoadModelIntoVAO("Terrain_xyz_n_rgba_uv.ply",
                                   terrainDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << terrainDrawingInfo.numberOfVertices << " vertices" << std::endl;

    ::g_pMeshManager->LoadModelIntoVAO("Big_Flat_Mesh_256x256_00_132K_xyz_n_rgba_uv.ply",
                                   terrainDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << terrainDrawingInfo.numberOfVertices << " vertices" << std::endl;

    ::g_pMeshManager->LoadModelWithAssimp("Big_Flat_Mesh_256x256_12_5_xyz_n_rgba_uv.ply",
                                   terrainDrawingInfo2, shaderProgramID);
    std::cout << "Loaded: " << terrainDrawingInfo2.numberOfVertices << " vertices" << std::endl;

    ::g_pMeshManager->LoadModelIntoVAO("Big_Flat_Mesh_256x256_07_1K_xyz_n_rgba_uv.ply",
                                   terrainDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << terrainDrawingInfo.numberOfVertices << " vertices" << std::endl;


    // LOD Bunny Rabbits:
    //bun_zipper_xyz_n_rgba_uv.ply        // 200,000 vertices     70,000 faces
    //bun_zipper_res2_xyz_n_rgba.ply      //  19,000 vertices     16,000 faces
    //bun_zipper_res3_xyz_n_rgba_uv.ply   //  11,000 vertices      3,800 faces
    //bun_zipper_res4_xyz_n_rgba_uv.ply   //   2,800 vertices      960 faces

    sModelDrawInfo bunnyTemp;
    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_xyz_n_rgba_uv.ply", bunnyTemp, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_res2_xyz_n_rgba_uv.ply", bunnyTemp, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_res3_xyz_n_rgba_uv.ply", bunnyTemp, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("bun_zipper_res4_xyz_n_rgba_uv.ply", bunnyTemp, shaderProgramID);

    //::g_pMeshManager->LoadModelIntoVAO("xwing3.ply", bunnyTemp, shaderProgramID);




    sModelDrawInfo HilbertRampDrawingInfo;
    ::g_pMeshManager->LoadModelIntoVAO("spiderManLegoFullAdjusted.ply",
                                       HilbertRampDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << HilbertRampDrawingInfo.numberOfVertices << " vertices" << std::endl;

//    sModelDrawInfo gridDrawingInfo;
//    ::g_pMeshManager->LoadModelIntoVAO("Flat_Grid_100x100.ply",
//                                   gridDrawingInfo, shaderProgramID);
//    std::cout << "Loaded: " << gridDrawingInfo.numberOfVertices << " vertices" << std::endl;

    sModelDrawInfo sphereDrawingInfo;
//    ::g_pMeshManager->LoadModelIntoVAO("Sphere_1_unit_Radius.ply",
    ::g_pMeshManager->LoadModelIntoVAO("Sphere_1_unit_Radius_xyz_n_rgba_uv.ply",
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelIntoVAO("spaceShip.ply",
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelIntoVAO("tree1.ply",
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelIntoVAO("tree2.ply",
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelIntoVAO("tree3.ply",
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    
    sModelDrawInfo grassInfo;
    ::g_pMeshManager->LoadModelWithAssimp("grass.obj", grassInfo, shaderProgramID);
    std::cout << "Grass Loaded: " << grassInfo.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelWithAssimp("spaceShip.obj", grassInfo, shaderProgramID);
    std::cout << "Grass Loaded: " << grassInfo.numberOfVertices << " vertices" << std::endl;



    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("xwing3.ply", 
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded xwing: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;

    ::g_pMeshManager->setBasePath("assets/models/alien");
    sModelDrawInfo alienModel;
    std::vector<std::string> alienAnimations;
    alienAnimations.push_back("Idle.dae");
    alienAnimations.push_back("Running.dae");
    alienAnimations.push_back("Walking Backwards.dae");
    alienAnimations.push_back("Walking.dae");
    alienAnimations.push_back("Jumping.dae");
    alienAnimations.push_back("Right Strafe Walking.dae");
    alienAnimations.push_back("Left Strafe Walking.dae");
    alienAnimations.push_back("Punching.dae");
    ::g_pMeshManager->LoadModelWithAssimpMultipleAnimations("Ch44_nonPBR.dae", alienAnimations,
        alienModel, shaderProgramID, shadowShaderProgramID);
   
    ::g_pMeshManager->setBasePath("assets/models/enemy");
    sModelDrawInfo enemyModel;
    std::vector<std::string> enemyAnimations;
    enemyAnimations.push_back("Running Crawl.dae");
    enemyAnimations.push_back("Low Crawl.dae");
    ::g_pMeshManager->LoadModelWithAssimpMultipleAnimations("Ch50_nonPBR.dae", enemyAnimations,
        enemyModel, shaderProgramID, shadowShaderProgramID);
    std::cout << "Loaded vampire: " << enemyModel.numberOfVertices << " vertices" << std::endl;
    /* sModelDrawInfo alienShadow;
    ::g_pMeshManager->LoadModelWithAssimpMultipleAnimations("Ch44_nonPBR.dae", alienAnimations,
        alienShadow, shadowShaderProgramID);
    std::cout << "Loaded vampire: " << alienModel.numberOfVertices << " vertices" << std::endl;
    */
    ::g_pMeshManager->setBasePath("assets/models");
   /* ::g_pMeshManager->LoadModelWithAssimp("Silly_Dancing.dae", 
                                   sphereDrawingInfo, shaderProgramID);
    std::cout << "Loaded vampire: " << sphereDrawingInfo.numberOfVertices << " vertices" << std::endl;
    */

    sModelDrawInfo rumbaModel;
    std::vector<std::string> animationsForRumba;
    animationsForRumba.push_back("Dancing.dae");
    animationsForRumba.push_back("Rumba_Dancing.dae");
   
   /* ::g_pMeshManager->LoadModelWithAssimpMultipleAnimations("Ch14_nonPBR.dae", animationsForRumba,
            rumbaModel, shaderProgramID);
    std::cout << "Loaded vampire: " << rumbaModel.numberOfVertices << " vertices" << std::endl;

    ::g_pMeshManager->LoadAnimationFile("Dancing.dae", rumbaModel);*/

    
   /*  ::g_pMeshManager->LoadModelWithAssimp("Dancing.dae", 
                                    rumbaModel, shaderProgramID);
    std::cout << "Loaded vampire: " << rumbaModel.numberOfVertices << " vertices" << std::endl;
    */
    ::g_pMeshManager->LoadModelWithAssimp("Rumba_Dancing.dae", 
                                    rumbaModel, shaderProgramID);
    std::cout << "Loaded rabbit: " << rumbaModel.numberOfVertices << " vertices" << std::endl;
    
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("Cube_1x1x1_xyz_n_rgba_for_Verlet.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("sphereWithManyVertices.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("sphereWithManyVertices2.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("dblSphere2.ply",
        sphereDrawingInfo, shaderProgramID);
    /*::g_pMeshManager->LoadModelIntoVAOWithManualBone("plank.ply",
        sphereDrawingInfo, shaderProgramID);
    */::g_pMeshManager->LoadModelIntoVAOWithManualBone("bridge.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("bridge2.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("bridge3.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAOWithManualBone("bird.ply",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("planeObject.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("planeObject2.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("StonePortal1.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("StoneArchLarge_Obj.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("BonyWall1.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("tree1.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("tree2.obj",
        sphereDrawingInfo, shaderProgramID);
    ::g_pMeshManager->LoadModelWithAssimp("tree3.obj",
        sphereDrawingInfo, shaderProgramID);

    

//    sModelDrawInfo Flat_1x1_planeDrawingInfo;
//    ::g_pMeshManager->LoadModelIntoVAO("Flat_1x1_plane.ply",
//                                       Flat_1x1_planeDrawingInfo, shaderProgramID);
//    std::cout << "Loaded: " << Flat_1x1_planeDrawingInfo.numberOfVertices << " vertices" << std::endl;

    // Spiderman
    sModelDrawInfo spiderMan;
    //::g_pMeshManager->LoadModelIntoVAO("spiderManLegoFullAdjusted.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("Prop_Skeleton.ply", spiderMan, shaderProgramID);


    ::g_pMeshManager->setBasePath("assets/models/LEGO_Spiderman");
    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_head_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_head_xyz_n_rgba_uv_at_Origin.ply", spiderMan, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_body_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Hips_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Left_arm_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Left_hand_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Right_arm_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Right_hand_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);

    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Left_leg_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("legospiderman_Right_leg_xyz_n_rgba_uv.ply", spiderMan, shaderProgramID);

        
    ::g_pMeshManager->setBasePath("assets/models/Imposter_Shapes");
    ::g_pMeshManager->LoadModelIntoVAO("Quad_2_sided_aligned_on_XY_plane.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("Quad_1_sided_aligned_on_XY_plane.ply", spiderMan, shaderProgramID);

    // The retro TV
    ::g_pMeshManager->setBasePath("assets/models/Retro_TV");
    ::g_pMeshManager->LoadModelIntoVAO("RetroTV.edited.bodyonly.ply", spiderMan, shaderProgramID);
    ::g_pMeshManager->LoadModelIntoVAO("RetroTV.edited.screenonly.ply", spiderMan, shaderProgramID);


    sModelDrawInfo xWingFighter;
    ::g_pMeshManager->setBasePath("assets/models/TieFighter");
    ::g_pMeshManager->LoadModelIntoVAO("Tie-Fighter_cleaned.ply", xWingFighter, shaderProgramID);

    sModelDrawInfo wheel;
    ::g_pMeshManager->setBasePath("assets/models");
    ::g_pMeshManager->LoadModelIntoVAO("Wheel_15.ply", wheel, shaderProgramID);


    // ... and so on

    ::g_pTextureManager = new cBasicTextureManager();

    ::g_pTextureManager->SetBasePath("assets/textures"); 
    if ( ::g_pTextureManager->Create2DTextureFromBMPFile("TaylorSwift_Eras_Poster.bmp", true) )
    {
        std::cout << "Loaded the Taylor Swift texture" << std::endl;
    }
    else
    {
        std::cout << "ERROR: no Taylor Swift!!" << std::endl;
    }
    //
    ::g_pTextureManager->Create2DTextureFromBMPFile("Blank_UV_Text_Texture.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("SpidermanUV_square.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Water_Texture_01.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("taylor-swift-jimmy-fallon.bmp", true);
    // Load the HUGE height map
    ::g_pTextureManager->Create2DTextureFromBMPFile("NvF5e_height_map.bmp", true);
    
    // Using this for discard transparency mask
    ::g_pTextureManager->Create2DTextureFromBMPFile("FAKE_Stencil_Texture_612x612.bmp", true);

    // Using this for discard transparency mask
    ::g_pTextureManager->Create2DTextureFromBMPFile("Yellow.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Blue.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Green.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("wood.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("hud1.bmp", true);

    ::g_pTextureManager->Create2DTextureFromBMPFile("checkerboard.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Texture_01_A.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("mousetext.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("alien.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("alienNormal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("smoke.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("smokeInvert.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("brickBase.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("brickNormal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("brickwall.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("brickwall_normal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("sandNormal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("sandColour.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("sand2Normal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("sand2Colour.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("Pixel_Large.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("StonePortal1_Base_Diffuse.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("StonePortal1_Base_Normal.bmp", true);
    
    ::g_pTextureManager->Create2DTextureFromBMPFile("StoneArchLarge_Base-Diffuse.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("StoneArchLarge_Base-Normal.bmp", true);
    
    ::g_pTextureManager->Create2DTextureFromBMPFile("BonyWall1_BaseDark_Diffuse.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("BonyWall1_BaseDark_Normal.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree1base.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree1norm.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree2base.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree2norm.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree3base.bmp", true);
    ::g_pTextureManager->Create2DTextureFromBMPFile("tree3norm.bmp", true);



    // Explosion for the imposter, billboard quad particle
    // https://www.google.com/url?sa=i&url=https%3A%2F%2Fpixers.ca%2Fstickers%2Fexplosion-texture-FO30094132&psig=AOvVaw12NxuJHVqxwc7btMc0pG3O&ust=1706290535512000&source=images&cd=vfe&opi=89978449&ved=0CBMQjRxqFwoTCNCdp-WJ-YMDFQAAAAAdAAAAABAD
    ::g_pTextureManager->Create2DTextureFromBMPFile("stickers-explosion-texture.bmp", true);
    // Load a cube map
    ::g_pTextureManager->SetBasePath("assets/textures/CubeMaps");
    std::string errors;
    ::g_pTextureManager->CreateCubeTextureFromBMPFiles("space",
                                                       "SpaceBox_right1_posX.bmp",
                                                       "SpaceBox_left2_negX.bmp",
                                                       "SpaceBox_top3_posY.bmp",
                                                       "SpaceBox_bottom4_negY.bmp",
                                                       "SpaceBox_front5_posZ.bmp",
                                                       "SpaceBox_back6_negZ.bmp",
                                                       true,
                                                       errors);

    ::g_pTextureManager->CreateCubeTextureFromBMPFiles("SunnyDay",
                                                       "TropicalSunnyDayLeft2048.bmp",
                                                       "TropicalSunnyDayRight2048.bmp",
                                                       "TropicalSunnyDayUp2048.bmp",
                                                       "TropicalSunnyDayDown2048.bmp",
                                                       "TropicalSunnyDayFront2048.bmp",
                                                       "TropicalSunnyDayBack2048.bmp",
                                                       true,
                                                       errors);
    
    // interstellar skybox
    // https://opengameart.org/content/interstellar-skybox
    ::g_pTextureManager->CreateCubeTextureFromBMPFiles("interstellar",
                                                       "interstellar_lf.bmp",
                                                       "interstellar_rt.bmp",
                                                       "interstellar_up.bmp",
                                                       "interstellar_dn.bmp",
                                                       "interstellar_bk.bmp",
                                                       "interstellar_ft.bmp",
                                                       true,
                                                       errors);

   
    // This handles the phsyics objects
    ::g_pPhysics = new cPhysics();

    ::g_pPhysics->setVAOManager(::g_pMeshManager);

    // 
    LoadModels();

    // *************************************
   // player init

    cMesh* playerMesh = ::g_pFindMeshByFriendlyName("player");

    if (playerMesh)
    {
        thePlayer = new cPlayer(playerMesh);
        thePlayer->speed = 30.0f;
        thePlayer->moveDir = glm::vec3(0.0f);
        thePlayer->thePhysics = ::g_pPhysics->findShapeByFriendlyName("player");
    }

//    //**************************************
#pragma region enemyStuff

    cEnemy* enemy1 = new cEnemy(::g_pFindMeshByFriendlyName("enemy1"), eEnemyBehaviour::Wander);
    //enemy1->UpdateBehaviour(eEnemyBehaviour::Flee);
    enemy1->target = thePlayer;
    enemy1->forward = glm::vec3(0.0f, 0.0f, 1.0f);
    enemy1->speed = 5.0f;
    enemyList.push_back(enemy1);

    cEnemy* enemy2 = new cEnemy(::g_pFindMeshByFriendlyName("enemy2"), eEnemyBehaviour::Wander);
    //enemy2->UpdateBehaviour(eEnemyBehaviour::Seek);
    enemy2->target = thePlayer;
    enemy2->forward = glm::vec3(0.0f, 0.0f, -1.0f);
    enemy2->speed = 5.0f;
    enemyList.push_back(enemy2);

    cEnemy* enemy3 = new cEnemy(::g_pFindMeshByFriendlyName("enemy3"), eEnemyBehaviour::Wander);
    //enemy3->UpdateBehaviour(eEnemyBehaviour::Evade);
    enemy3->target = thePlayer;
    enemy3->forward = glm::vec3(1.0f, 0.0f, 0.0f);
    enemy3->speed = 5.0f;
    enemyList.push_back(enemy3);

    cEnemy* enemy4 = new cEnemy(::g_pFindMeshByFriendlyName("enemy4"), eEnemyBehaviour::Wander);
    //enemy4->UpdateBehaviour(eEnemyBehaviour::Pursue);
    enemy4->target = thePlayer;
    enemy4->forward = glm::vec3(-1.0f, 0.0f, 0.0f);
    enemy4->speed = 5.0f;
    enemyList.push_back(enemy4);

    cEnemy* enemy5 = new cEnemy(::g_pFindMeshByFriendlyName("enemy5"), eEnemyBehaviour::Wander);
    //enemy5->UpdateBehaviour(eEnemyBehaviour::Approach);
    enemy5->target = thePlayer;
    enemy5->forward = glm::vec3(0.0f, 0.0f, 1.0f);
    enemy5->speed = 5.0f;
    enemyList.push_back(enemy5);

    cEnemy* enemy6 = new cEnemy(::g_pFindMeshByFriendlyName("enemy6"), eEnemyBehaviour::Wander);
    //enemy6->UpdateBehaviour(eEnemyBehaviour::PathFollowing);
    enemy6->target = thePlayer;
    enemy6->forward = glm::vec3(0.0f, 0.0f, -1.0f);
    enemy6->speed = 5.0f;
    enemyList.push_back(enemy6);
    
    cEnemy* enemy7 = new cEnemy(::g_pFindMeshByFriendlyName("enemy7"), eEnemyBehaviour::Wander);
    //enemy7->UpdateBehaviour(eEnemyBehaviour::Wander);
    //enemy7->wanderData.radiusTwo = 0.5f;
    enemy7->target = thePlayer;
    enemy7->forward = glm::vec3(1.0f, 0.0f, 0.0f);
    enemy7->speed = 5.0f;
    enemyList.push_back(enemy7);

    cEnemy* enemy8 = new cEnemy(::g_pFindMeshByFriendlyName("enemy8"), eEnemyBehaviour::Wander);
    //enemy8->UpdateBehaviour(eEnemyBehaviour::Wander);
   // enemy8->wanderData.radiusTwo = 4.0f;
    enemy8->target = thePlayer;
    enemy8->forward = glm::vec3(-1.0f, 0.0f, 0.0f);
    enemy8->speed = 5.0f;
    enemyList.push_back(enemy8);

    cEnemy* enemy9 = new cEnemy(::g_pFindMeshByFriendlyName("enemy9"), eEnemyBehaviour::Wander);
    //enemy9->UpdateBehaviour(eEnemyBehaviour::Wander);
    //enemy9->wanderData.decisionTime = 10.0f;
    enemy9->target = thePlayer;
    enemy9->forward = glm::vec3(0.0f, 0.0f, 1.0f);
    enemy9->speed = 5.0f;
    enemyList.push_back(enemy9);
    
    cEnemy* enemy10 = new cEnemy(::g_pFindMeshByFriendlyName("enemy10"), eEnemyBehaviour::PathFollowing);
    //enemy10->UpdateBehaviour(eEnemyBehaviour::Wander);
    //enemy10->wanderData.decisionTime = 10.0f;
    std::vector<glm::vec3> points;
    points.push_back(glm::vec3(200.0f, 0.0f, 200.0f));
    points.push_back(glm::vec3(300.0f, 0.0f, 0.0f));
    points.push_back(glm::vec3(200.0f, 0.0f, -200.0f));
    points.push_back(glm::vec3(0.0f, 0.0f, -300.0f));
    points.push_back(glm::vec3(-200.0f, 0.0f, -200.0f));
    points.push_back(glm::vec3(-300.0f, 0.0f, 0.0f));
    points.push_back(glm::vec3(-200.0f, 0.0f, 200.0f));
    points.push_back(glm::vec3(0.0f, 0.0f, 300.0f));
    enemy10->pathData.pointsOnPath = points;
    enemy10->target = thePlayer;
    enemy10->forward = glm::vec3(0.0f, 0.0f, -1.0f);
    enemy10->speed = 10.0f;
    enemyList.push_back(enemy10);
    cEnemy* enemy11 = new cEnemy(::g_pFindMeshByFriendlyName("enemy11"), eEnemyBehaviour::PathFollowing);
    //enemy10->UpdateBehaviour(eEnemyBehaviour::Wander);
    //enemy10->wanderData.decisionTime = 10.0f;
    
    enemy11->pathData.pointsOnPath = points;
    enemy11->target = thePlayer;
    enemy11->forward = glm::vec3(0.0f, 0.0f, -1.0f);
    enemy11->speed = 10.0f;
    enemyList.push_back(enemy11);

#pragma endregion 

    ::g_SoftBody.acceleration = glm::vec3(0.0f, -5.0f, 0.0f);

    {


        sModelDrawInfo softBodyObjectDrawingInfo;
        if ( ::g_pMeshManager->FindDrawInfoByModelName("sphereWithManyVertices2.ply", softBodyObjectDrawingInfo) )
       // if ( ::g_pMeshManager->FindDrawInfoByModelName("spiderManLegoFullAdjusted.ply", softBodyObjectDrawingInfo) )
        //if ( ::g_pMeshManager->FindDrawInfoByModelName("sphereWithManyVertices.ply", softBodyObjectDrawingInfo) )
       // if ( ::g_pMeshManager->FindDrawInfoByModelName("dblSphere2.ply", softBodyObjectDrawingInfo) )
        {

            // Move the soft body here
            glm::mat4 matTransform = glm::mat4(1.0f);
//            glm::mat4 matTransform = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, 50.0f, 0.0f) );

            matTransform = glm::translate(matTransform, glm::vec3(0.0f, 50.0f, 0.0f));

             
            matTransform = glm::scale(matTransform,
                                      glm::vec3(3.0f));


            ::g_SoftBody.CreateSoftBody(softBodyObjectDrawingInfo, matTransform);
            
            for (int i = 0; i < 17; i++)
            {
                glm::vec3 vector(0.0f);
                ::g_SoftBody.vec_pPlankPositions.push_back(vector);
            }
//            

            // Add bracing constraints
            // With the 'bun_zipper_res3_xyz_n_rgba_trivialUV_offset_Y' model, 
            //  the triangles are around the same size, and the rest lengths
            //  seem to be around 1.2 to 1.5, so we'll pick 2.0.
            // NOTE: You would likely do this manually. 
            //::g_SoftBody.CreateRandomBracing(4000, 0.5f, 4.0f);
            ::g_SoftBody.CreateRandomBracing(5000, 0.001f, 2.0f);
            //::g_SoftBody.CreateRandomBracing(10000, 0.5f, 1.5f);

            std::cout << "Created softbody OK." << std::endl;
        }
    }

    ::g_Bridge.acceleration = glm::vec3(0.0f, -9.0f, 0.0f);
    sModelDrawInfo bridgeDrawInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName("bridge3.ply", bridgeDrawInfo))
    {
        glm::mat4 matTransform = glm::mat4(1.0f);
        //            glm::mat4 matTransform = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, 50.0f, 0.0f) );

        matTransform = glm::translate(matTransform, glm::vec3(0.0f, 25.0f, 0.0f));

       /* matTransform = glm::rotate(matTransform,
            glm::radians(90.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));*/
       /* matTransform = glm::rotate(matTransform,
            glm::radians(45.0f),
            glm::vec3(0.0f, 0.0f, 1.0f));*/
        matTransform = glm::scale(matTransform,
            glm::vec3(6.0f, 1.0f, 5.0f));//20.0f, 1.0f, 20.0f));
        ::g_Bridge.CreateSoftBody(bridgeDrawInfo, matTransform);
        ::g_Bridge.CreateRandomBracing(10, 2.0f);
    }

    // Spawn thread to handle the soft body
    sSoftBodyThreadInfo SBIParams;
    SBIParams.p_theSoftBody = &::g_SoftBody;
    SBIParams.force = &softBodyForce;
    SBIParams.desiredUpdateTime = 1.0 / 60.0;   // 60FPS
    SBIParams.bIsAlive = true;
    SBIParams.blob = true;


    // Maybe set this to false and right before the while, set it to true
    SBIParams.bRun = true;

    sSoftBodyThreadInfo* pSBI = &SBIParams;

    InitializeCriticalSection(&::softBodyUpdate);

    DWORD ThreadId = 0;
    HANDLE threadHandle = 0;
    threadHandle = CreateThread(
        NULL,					// lpThreadAttributes,
        0,						// dwStackSize,
        UpdateSoftBodyThread,		// lpStartAddress,
        (void*)pSBI,				//  lpParameter,
        0,						// dwCreationFlags (0 or CREATE_SUSPENDED)
        &ThreadId); // lpThreadId

    sSoftBodyThreadInfo bridgeParams;
    bridgeParams.p_theSoftBody = &::g_Bridge;
    bridgeParams.desiredUpdateTime = 1.0 / 120.0;
    bridgeParams.bIsAlive = true;
    bridgeParams.bRun = true;

    sSoftBodyThreadInfo* pBridgeParams = &bridgeParams;

    DWORD ThreadId2 = 0;
    HANDLE threadHandle2 = 0;
    threadHandle2 = CreateThread(
        NULL,					// lpThreadAttributes,
        0,						// dwStackSize,
        UpdateSoftBodyThread,		// lpStartAddress,
        (void*)pBridgeParams,				//  lpParameter,
        0,						// dwCreationFlags (0 or CREATE_SUSPENDED)
        &ThreadId2); // lpThreadId

    // And a squishy wheel
    sModelDrawInfo wheelDrawInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName("Wheel_15.ply", wheelDrawInfo))
    {
        // Move the soft body here
        glm::mat4 matTransform = glm::mat4(1.0f);
//            glm::mat4 matTransform = glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, 50.0f, 0.0f) );

        matTransform = glm::translate(matTransform, glm::vec3(0.0f, 10.0f, 0.0f));

        matTransform = glm::rotate(matTransform,
                                  glm::radians(-90.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
//        matTransform = glm::rotate(matTransform,
//                                   glm::radians(45.0f),
//                                   glm::vec3(0.0f, 0.0f, 1.0f));

        matTransform = glm::scale(matTransform, glm::vec3(15.0f, 15.0f, 15.0f));


        ::g_Wheel.CreateSoftBody(wheelDrawInfo, matTransform);

//        std::vector<unsigned int> vecAxleParticles;
//        ::g_Wheel.findCentreVerticesOfWheel(vecAxleParticles);
        // When the wheel is at the origin, the three vertices along the x axis are:
        //  43
        // 113
        // 167

        ::g_Wheel.acceleration = glm::vec3(0.0f, -9.0f, 0.0f);
//        ::g_Wheel.CreateRandomBracing(200, 1.0f);
        ::g_Wheel.CreateWheelBracing();

        std::cout << "Created wheel softbody OK." << std::endl;    
    }

//    LoadTheRobotronModels(shaderProgramID);

#pragma region lightsstuff 
    ::g_pTheLights = new cLightManager();
    // 
    ::g_pTheLights->SetUniformLocations(shaderProgramID);

    
    for (int i = 0; i < 9; i++)
    {


        ::g_pTheLights->theLights[i].param2.x = 1.0f;   // Turn on
        //    ::g_pTheLights->theLights[0].param1.x = 0.0f;   // 0 = point light
        ::g_pTheLights->theLights[i].param1.x = 0.0f;   // 1 = spot light
        // With spots, set direction (relative)
        ::g_pTheLights->theLights[i].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
        //	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
        ::g_pTheLights->theLights[i].param1.y = 15.0f;
        ::g_pTheLights->theLights[i].param1.z = 25.0f;


        ::g_pTheLights->theLights[i].position.x = -46.0f;
        ::g_pTheLights->theLights[i].position.y = 23.0f;
        ::g_pTheLights->theLights[i].position.z = -26.0f;



        // How "bright" the lights is
        // Really the opposite of brightness.
        //  how dim do you want this
        ::g_pTheLights->theLights[i].atten.x = 0.0f;        // Constant attenuation
        ::g_pTheLights->theLights[i].atten.y = 0.25f;        // Linear attenuation
        ::g_pTheLights->theLights[i].atten.z = 0.1f;        // Quadratic attenuation

    }
    //::g_pTheLights->theLights[1].param2.x = 1.0f;   // Turn on
    ////    ::g_pTheLights->theLights[0].param1.x = 0.0f;   // 0 = point light
    //::g_pTheLights->theLights[1].param1.x = 0.0f;   // 1 = spot light
    //// With spots, set direction (relative)
    //::g_pTheLights->theLights[1].diffuse = glm::vec4(1.0f, 0.67f, 0.0f, 0.0f);
    //::g_pTheLights->theLights[1].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    ////	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
    //::g_pTheLights->theLights[1].param1.y = 15.0f;
    //::g_pTheLights->theLights[1].param1.z = 25.0f;


    //::g_pTheLights->theLights[1].position.x = 7.65f;
    //::g_pTheLights->theLights[1].position.y = 11.8f;
    //::g_pTheLights->theLights[1].position.z = 16.2f;



    //// How "bright" the lights is
    //// Really the opposite of brightness.
    ////  how dim do you want this
    //::g_pTheLights->theLights[1].atten.x = 0.0f;        // Constant attenuation
    //::g_pTheLights->theLights[1].atten.y = 0.5f;        // Linear attenuation
    //::g_pTheLights->theLights[1].atten.z = 0.01f;        // Quadratic attenuation


    //::g_pTheLights->theLights[2].param2.x = 1.0f;   // Turn on
    ////    ::g_pTheLights->theLights[0].param1.x = 0.0f;   // 0 = point light
    //::g_pTheLights->theLights[2].param1.x = 0.0f;   // 1 = spot light
    //// With spots, set direction (relative)
    //::g_pTheLights->theLights[2].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    //::g_pTheLights->theLights[2].direction = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
    ////	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
    //::g_pTheLights->theLights[2].param1.y = 15.0f;
    //::g_pTheLights->theLights[2].param1.z = 25.0f;


    //::g_pTheLights->theLights[2].position.x = -75.0f;
    //::g_pTheLights->theLights[2].position.y = 15.0f;
    //::g_pTheLights->theLights[2].position.z = -56.0f;



    //// How "bright" the lights is
    //// Really the opposite of brightness.
    ////  how dim do you want this
    //::g_pTheLights->theLights[2].atten.x = 0.0f;        // Constant attenuation
    //::g_pTheLights->theLights[2].atten.y = 0.25f;        // Linear attenuation
    //::g_pTheLights->theLights[2].atten.z = 0.01f;        // Quadratic attenuation


    // Light #1 is a directional light 
    ::g_pTheLights->theLights[9].param2.x = 1.0f;   // Turn on
    ::g_pTheLights->theLights[9].param1.x = 2.0f;   // Directional
    ::g_pTheLights->theLights[9].position = glm::vec4(48.f, 114.f, -174.f, 1.0f);

    // Direction with respect of the light.
    ::g_pTheLights->theLights[9].direction = glm::normalize(glm::vec4(-0.5f, -1.0f, -1.0f, 1.0f));
    //    float lightBrightness = 0.7f;
    ::g_pTheLights->theLights[9].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ::g_pTheLights->theLights[9].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


   

#pragma endregion  

// *************************************************************************************
    // These are the commands we are going to process
//    std::vector<cCommand_MoveTo> vecAnimationCommands;
//
//    cMesh* pBathTub = g_pFindMeshByFriendlyName("bathtub");
//
//    cCommand_MoveTo moveBathTub(pBathTub,
//                                pBathTub->getDrawPosition(),
//                                glm::vec3(50.0f, 0.0f, 0.0f),
//                                10.0f);
//    ::g_vecAnimationCommands.push_back(moveBathTub);
//
//
//    cCommand_MoveTo moveBathTub2(pBathTub,
//                                pBathTub->getDrawPosition(),
//                                glm::vec3(-30.0f, 10.0f, 16.0f),
//                                5.0f);
//    ::g_vecAnimationCommands.push_back(moveBathTub2);
//
// *************************************************************************************

#pragma endregion AllTheLoadingCarp

    

    //cFlockEntity::sFlockEntityDef flockDef; 
    //std::vector<cMesh*> flockMembers; 
    //for (int i = 0; i < 30; i++)
    //{
    //    cMesh* flock = new cMesh();
    //    flock->meshName = "bird.ply";
    //    flock->setUniformDrawScale(0.1f);
    //    //flock->drawPosition.x = 5.0f * i;
    //    flock->bUseDebugColours = true;
    //    flock->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    //    flockMembers.push_back(flock);
    //    ::g_vec_pMeshesToDraw.push_back(flock);
    //}

    //flockDef.theMeshes = flockMembers;
    //flockDef.maxBoundary = glm::vec3(30.0f, 30.0f, 30.0f);
    //flockDef.minBoundary = glm::vec3(-30.0f, 10.0f, -30.0f);
    //flockDef.neighboutRadius = 6.0f;
    //flockDef.dangerRadius = 4.0f;
    //flockDef.separationFactor = 0.6f;
    //flockDef.cohesionFactor = 0.7f;
    //flockDef.alignmentFactor = 0.6f;
    //flockDef.flockCount = flockDef.theMeshes.size();
    //

    //flockEntity = new cFlockEntity(flockDef); 


    while (!glfwWindowShouldClose(window))
    {
        
        float nearPlane = 0.1f;
        float farPlane = 900.0f;
        float shadowBox = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-shadowBox, shadowBox, -shadowBox, shadowBox, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(500.0f * glm::vec3(-0.5f,1.0f, 1.0f) + thePlayer->theMesh->drawPosition + thePlayer->lookingAt * 50.0f//glm::vec3(::g_pTheLights->theLights[4].position.x, ::g_pTheLights->theLights[4].position.y, ::g_pTheLights->theLights[4].position.z)
            , thePlayer->theMesh->drawPosition + thePlayer->lookingAt * 50.0f/*glm::vec3(0.0f)*/, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        glUseProgram(shadowShaderProgramID);

        GLint lightSpaceMatrix_UL = glGetUniformLocation(shadowShaderProgramID, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceMatrix_UL, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        {
            glBindFramebuffer(GL_FRAMEBUFFER, ::g_depthBuffer->ID);
            glViewport(0, 0, ::g_depthBuffer->width, (float)::g_depthBuffer->height);

//            glViewport(0, 0, width, height);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ::g_depthBuffer->clearBuffers(true, true);

            
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
            glm::vec3 scene_1_CameraEye = ::g_cameraEye;
            glm::vec3 scene_1_CameraTarget =::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;

            DrawPass_1(shadowShaderProgramID, window, p_HRTimer, ::g_depthBuffer->width, ::g_depthBuffer->height,
                scene_1_CameraEye, scene_1_CameraTarget, true);
        }

// STARTOF: Pass #1   
        // Draw original scene

        {
            glUseProgram(shaderProgramID);

            GLint lightSpaceMatrix_UL = glGetUniformLocation(shaderProgramID, "lightSpaceMatrix");
            glUniformMatrix4fv(lightSpaceMatrix_UL, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            GLint textureUnitNumber = 35;
            glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
            glBindTexture(GL_TEXTURE_2D, ::g_depthBuffer->colourTexture_0_ID);
            GLint shadowMap_UL = glGetUniformLocation(shaderProgramID, "shadowMap");
            glUniform1i(shadowMap_UL, textureUnitNumber);



            //glUseProgram(shaderProgramID); 
            glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBO_1->ID);
            float ratio;
            int width, height;
//            glfwGetFramebufferSize(window, &width, &height);
//            ratio = width / (float)height;
            ratio = ::g_pFBO_1->width / (float)::g_pFBO_1->height;
            glViewport(0, 0, ::g_pFBO_1->width, (float)::g_pFBO_1->height);

//            glViewport(0, 0, width, height);
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ::g_pFBO_1->clearBuffers(true, true);

//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
            glm::vec3 scene_1_CameraEye = ::g_cameraEye;
            glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;

            DrawPass_1(shaderProgramID, window, p_HRTimer, ::g_pFBO_1->width, ::g_pFBO_1->height,
                       scene_1_CameraEye, scene_1_CameraTarget, false); 
        }
        
        


 //gaussian blur
        {
            bool horizontal = true;
            bool firstIt = true;
            int amount = 20;
            for (int i = 0; i < amount; i++)
            {

                glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOList[i % 2]->ID);
                float ratio;
                int width, height;
                ratio = ::g_pFBOList[i % 2]->width / (float) ::g_pFBOList[i % 2]->height;
                glViewport(0, 0, ::g_pFBOList[i % 2]->width, (float)::g_pFBOList[i % 2]->height);
               ::g_pFBOList[i % 2]->clearBuffers(true, true);
                glm::vec3 scene_1_CameraEye = ::g_cameraEye;
                glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;

                DrawPass_GaussianBlur(shaderProgramID, window, ::g_pFBOList[i % 2]->width, ::g_pFBOList[i % 2]->height, (i+1) % 2, firstIt, horizontal);

                horizontal = !horizontal;
                if (firstIt)
                {
                    firstIt = false;
                }
            }
        }
        //draw passes

//        // night vision pass
//        {
//            glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOList[1]->ID);
//            float ratio;
//            int width, height;
////            glfwGetFramebufferSize(window, &width, &height);
////            ratio = width / (float)height;
//            ratio = ::g_pFBOList[1]->width / (float)::g_pFBOList[1]->height;
//            glViewport(0, 0, ::g_pFBOList[1]->width, (float)::g_pFBOList[1]->height);
//
////            glViewport(0, 0, width, height);
////            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            ::g_pFBOList[1]->clearBuffers(true, true);
//
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
////            glm::vec3 scene_1_CameraEye = ::g_cameraEye;
// //           glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;
//
//           // GLint bUsesNightvision_UL = glGetUniformLocation(shaderProgramID, "bUseNightvision");
//           // glUniform1f(bUsesNightvision_UL, (GLfloat)GL_TRUE);
//
//            DrawPass_1(shaderProgramID, window, p_HRTimer, ::g_pFBOList[1]->width, ::g_pFBOList[1]->height,
//                       scene_1_CameraEye, scene_1_CameraTarget); 
//
//            //glUniform1f(bUsesNightvision_UL, (GLfloat)GL_FALSE);
//        }
//        
//        
//
//        
//        
//        //g_pFindMeshByFriendlyName("screen2")->textureIsFromFBO = true;
//        //g_pFindMeshByFriendlyName("screen2")->FBOTextureNumber = ::g_pFBOList[1]->colourTexture_0_ID;
//        //******************************************DOF
//        {//regular draw pass
//            glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOList[0]->ID);
//            float ratio;
//            int width, height;
////            glfwGetFramebufferSize(window, &width, &height);
////            ratio = width / (float)height;
//            ratio = ::g_pFBOList[0]->width / (float)::g_pFBOList[0]->height;
//            glViewport(0, 0, ::g_pFBOList[0]->width, (float)::g_pFBOList[0]->height);
//
////            glViewport(0, 0, width, height);
////            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            ::g_pFBOList[0]->clearBuffers(true, true);
//
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
////            glm::vec3 scene_1_CameraEye = ::g_cameraEye;
////            glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;
//
//            DrawPass_1(shaderProgramID, window, p_HRTimer, ::g_pFBOList[0]->width, ::g_pFBOList[0]->height,
//                       scene_1_CameraEye, scene_1_CameraTarget); 
//        }
   //draw passes

//        {//depth draw pass
//            glBindFramebuffer(GL_FRAMEBUFFER, ::g_depthBuffer->ID);
//            float ratio;
//            int width, height;
////            glfwGetFramebufferSize(window, &width, &height);
////            ratio = width / (float)height;
//            ratio = ::g_depthBuffer->width / (float)::g_depthBuffer->height;
//            glViewport(0, 0, ::g_depthBuffer->width, (float)::g_depthBuffer->height);
//
////            glViewport(0, 0, width, height);
////            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            ::g_depthBuffer->clearBuffers(true, true);
//
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
//           // glm::vec3 scene_1_CameraEye = ::g_cameraEye;
//           // glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;
//
//            DrawPass_Depth(shaderProgramID, window, p_HRTimer, ::g_depthBuffer->width, ::g_depthBuffer->height,
//                       scene_1_CameraEye, scene_1_CameraTarget); 
//        }
//        
//        {//dilation draw pass
//            glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOList[2]->ID);
//            float ratio;
//            int width, height;
////            glfwGetFramebufferSize(window, &width, &height);
////            ratio = width / (float)height;
//            ratio = ::g_pFBOList[2]->width / (float)::g_pFBOList[2]->height;
//            glViewport(0, 0, ::g_pFBOList[2]->width, (float)::g_pFBOList[2]->height);
//
////            glViewport(0, 0, width, height);
////            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            ::g_pFBOList[2]->clearBuffers(true, true);
//
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
//           // glm::vec3 scene_1_CameraEye = ::g_cameraEye;
//           // glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;
//
//            DrawPass_Dilate(shaderProgramID, window, ::g_pFBOList[2]->width, ::g_pFBOList[2]->height); 
//        }
//        
//        {//dof draw pass
//            glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOList[3]->ID);
//            float ratio;
//            int width, height;
////            glfwGetFramebufferSize(window, &width, &height);
////            ratio = width / (float)height;
//            ratio = ::g_pFBOList[3]->width / (float)::g_pFBOList[3]->height;
//            glViewport(0, 0, ::g_pFBOList[3]->width, (float)::g_pFBOList[3]->height);
//
////            glViewport(0, 0, width, height);
////            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            ::g_pFBOList[3]->clearBuffers(true, true);
//
//            glm::vec3 scene_1_CameraEye = glm::vec3(0.0, 20.0, 181.0f);
//            glm::vec3 scene_1_CameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);
//           // glm::vec3 scene_1_CameraEye = ::g_cameraEye;
//           // glm::vec3 scene_1_CameraTarget = ::g_cameraEye + ::g_cameraFront;//::g_cameraTarget;
//
//            DrawPass_DOF(shaderProgramID, window, ::g_pFBOList[3]->width, ::g_pFBOList[3]->height); 
//        }
        //cMesh* quad = ::g_pFindMeshByFriendlyName("quad");
        //glm::vec3 camDir = glm::normalize(::g_cameraEye - quad->drawPosition);
        //float angleY = atan2(camDir.x, camDir.z);
        ////float angleX = atan2(-camDir.y, sqrt(camDir.x * camDir.x + camDir.z * camDir.z));

        //glm::quat rotate = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f));//* glm::angleAxis(angleX, glm::vec3(1.0f, 0.0f, 0.0f));

        //quad->setDrawOrientation(rotate);//glm::quatLookAt(camDir, glm::vec3(0.0f, 1.0f, 0.0f)));
        //quad->adjustRoationAngleFromEuler(glm::vec3(0.0f, 1.570796f, 0.0f));
        //// g_pFindMeshByFriendlyName("screen1")->textureIsFromFBO = true;
       // g_pFindMeshByFriendlyName("screen1")->FBOTextureNumber = ::g_pFBOList[3]->colourTexture_0_ID;

       /* ::g_SoftBody.VerletUpdate(1.0 / 60.0);
        ::g_SoftBody.ApplyCollision(1.0 / 60.0);
        ::g_SoftBody.SatisfyConstraints();
        ::g_SoftBody.UpdateVertexPositions();
        ::g_SoftBody.UpdateNormals();*/
#pragma region PlankInstatiate
        cMesh* plank1 = ::g_pFindMeshByFriendlyName("plank1");
        //sPhsyicsProperties* pPlank = ::g_pPhysics->findShapeByFriendlyName("plank1");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(4, 7);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(4, 7);
            ::g_SoftBody.vec_pPlankPositions[0] = plank1->drawPosition; 
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank2");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank2");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(5, 6);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(5, 6);
            ::g_SoftBody.vec_pPlankPositions[1] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank3");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank3");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(12, 13);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(12, 13);
            ::g_SoftBody.vec_pPlankPositions[2] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank4");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank4");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(14, 17);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(14, 17);
            ::g_SoftBody.vec_pPlankPositions[3] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank5");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank5"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(16, 19);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(16, 19);
            ::g_SoftBody.vec_pPlankPositions[4] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank6");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank6");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(20, 25);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(20, 25);
            ::g_SoftBody.vec_pPlankPositions[5] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank7");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank7");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(26, 26);
            //pPlank->position.z -= 5.0f;
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(26, 29);
            ::g_SoftBody.vec_pPlankPositions[6] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank8");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank8");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(27, 32);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(27, 32);
            ::g_SoftBody.vec_pPlankPositions[7] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank9");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank9");
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(30, 37);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(30, 37);
            ::g_SoftBody.vec_pPlankPositions[8] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank10");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank10"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(38, 41);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(38, 41);
            ::g_SoftBody.vec_pPlankPositions[9] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank11");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank11"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(39, 42);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(39, 42);
            ::g_SoftBody.vec_pPlankPositions[10] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank12");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank12"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(43, 44);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(43, 44);
            ::g_SoftBody.vec_pPlankPositions[11] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank13");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank13"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(50, 53);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(50, 53);
            ::g_SoftBody.vec_pPlankPositions[12] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank14");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank14"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(51, 57);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(51, 57);
            ::g_SoftBody.vec_pPlankPositions[13] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank15");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank15"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(58, 61);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(58, 61);
            ::g_SoftBody.vec_pPlankPositions[14] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank16");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank16"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(60, 63);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(60, 63);
            ::g_SoftBody.vec_pPlankPositions[15] = plank1->drawPosition;
        }
        plank1 = ::g_pFindMeshByFriendlyName("plank17");
        //pPlank = ::g_pPhysics->findShapeByFriendlyName("plank17"); 
        if (plank1)
        {
            //pPlank->position = g_Bridge.getCenterBetween2Particles(64, 67);
            plank1->drawPosition = g_Bridge.getCenterBetween2Particles(64, 67);
            ::g_SoftBody.vec_pPlankPositions[16] = plank1->drawPosition;
        }

#pragma endregion    
        

        // Only a full screen quad
        {
            // Output directed to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            float ratio;
            int screenWidth, screenHeight;
            glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
            ratio = screenWidth / (float)screenHeight;
//            ratio = ::g_pFBO_1->width / (float)::g_pFBO_1->height;
//            glViewport(0, 0, ::g_pFBO_1->width, (float)::g_pFBO_1->height);

            glViewport(0, 0, screenWidth, screenHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //        ::g_pFBO_1->clearBuffers(true, true);


            DrawPass_FSQ(shaderProgramID, window, screenWidth, screenHeight);
        }



      
        // Here's where we "present" to the screen
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update the title screen
        std::stringstream ssTitle;
        ssTitle << "Camera (x,y,z): "
            << ::g_cameraEye.x << ", "
            << ::g_cameraEye.y << ", "
            << ::g_cameraEye.z << ") "
            << "Light[" << ::g_selectedLight << "]: "
            << ::g_pTheLights->theLights[::g_selectedLight].position.x << ", "
            << ::g_pTheLights->theLights[::g_selectedLight].position.y << ", "
            << ::g_pTheLights->theLights[::g_selectedLight].position.z << "  "
            << "const:" << ::g_pTheLights->theLights[::g_selectedLight].atten.x << " "
            << "linear:" << ::g_pTheLights->theLights[::g_selectedLight].atten.y << " "
            << "quad:" << ::g_pTheLights->theLights[::g_selectedLight].atten.z << " "
            << "inner: " << ::g_pTheLights->theLights[::g_selectedLight].param1.y << " "
            << "outer: " << ::g_pTheLights->theLights[::g_selectedLight].param1.z << " ";

        std::string theTitle = ssTitle.str();

        glfwSetWindowTitle(window, theTitle.c_str() );
    }

    // Delete everything

    DeleteCriticalSection(&::softBodyUpdate);
    SBIParams.bIsAlive = false; 

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Only a full screen quad
void DrawPass_DOF(GLuint shaderProgramID, GLFWwindow* pWindow,
                  int screenWidth, int screenHeight)
{
    float ratio;

    glUseProgram(shaderProgramID);

    //glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Camera is pointing directly at the full screen quad
    glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
    glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


    //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
                FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               100.0f);    // Far (as small)

    glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
                                    FSQ_CameraTarget,
                                    ::g_upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    // Set up the textures for this offscreen quad
    //uniform bool bIsOffScreenTextureQuad;
    GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

    // uniform vec2 screenWidthAndHeight;	// x is width
    GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
    glUniform2f(screenWidthAndHeight_UL, 
                (GLfloat)screenWidth, 
                (GLfloat)screenHeight);


    // Point the FBO from the 1st pass to this texture...

    GLint textureUnitNumber = 65;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[0]->colourTexture_0_ID);
    
    GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
    glUniform1i(textureOffScreen_UL, textureUnitNumber);

    textureUnitNumber = 55;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[2]->colourTexture_0_ID);
            
    GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "dilationTexture"); 
    glUniform1i(texture_00_UL, textureUnitNumber); 
    
    textureUnitNumber = 54;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, ::g_depthBuffer->colourTexture_0_ID);
            
    GLint texture_01_UL = glGetUniformLocation(shaderProgramID, "depthTexture"); 
    glUniform1i(texture_01_UL, textureUnitNumber); 


    GLint bUseDOF_UL = glGetUniformLocation(shaderProgramID, "bUseDOF");
    glUniform1f(bUseDOF_UL, (GLfloat)GL_TRUE);

    //uniform sampler2D textureOffScreen;
    

    cMesh fullScreenQuad;
    //fullScreenQuad.meshName = "Quad_2_sided_aligned_on_XY_plane.ply";
    fullScreenQuad.meshName = "spiderManLegoFullAdjusted.ply";
    //fullScreenQuad.meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";

    //fullScreenQuad.textureName[0] = "hud1.bmp";
    //fullScreenQuad.textureRatios[0] = 1.0f;
    fullScreenQuad.setUniformDrawScale(5.0f);
    fullScreenQuad.drawPosition = glm::vec3(0.0f);
    fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

    DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);
    glUniform1f(bUseDOF_UL, (GLfloat)GL_FALSE);


    return;
}

void DrawPass_FSQ(GLuint shaderProgramID, GLFWwindow* pWindow,
                  int screenWidth, int screenHeight)
{
    float ratio;

    glUseProgram(shaderProgramID);

    //glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Camera is pointing directly at the full screen quad
    glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
    glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


    //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
                FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               100.0f);    // Far (as small)

    glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
                                    FSQ_CameraTarget,
                                    ::g_upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    // Set up the textures for this offscreen quad
    //uniform bool bIsOffScreenTextureQuad;
    GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

    // uniform vec2 screenWidthAndHeight;	// x is width
    GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
    glUniform2f(screenWidthAndHeight_UL, 
                (GLfloat)screenWidth, 
                (GLfloat)screenHeight);
    GLint bGaussianBlur_UL = glGetUniformLocation(shaderProgramID, "bGaussianBlur");
    glUniform1f(bGaussianBlur_UL, (GLfloat)GL_FALSE);
    GLint bHorizontalBlur_UL = glGetUniformLocation(shaderProgramID, "bHorizontalBlur");
    glUniform1f(bHorizontalBlur_UL, (GLfloat)GL_FALSE);


    // Point the FBO from the 1st pass to this texture...

    GLint textureUnitNumber = 70;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
   // glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[1]->colourTexture_0_ID);// ::g_pFBO_1->colourTexture_0_ID);
   //glBindTexture(GL_TEXTURE_2D, ::g_depthBuffer->colourTexture_0_ID);
   // glBindTexture(GL_TEXTURE_2D, ::g_depthBuffer->depthTexture_ID);
    glBindTexture(GL_TEXTURE_2D, ::g_pFBO_1->colourTexture_0_ID);
    //glBindTexture(GL_TEXTURE_2D, ::g_pFBO_1->colourBrightnessTexture_1_ID); 
    //uniform sampler2D textureOffScreen;
    GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
    glUniform1i(textureOffScreen_UL, textureUnitNumber);
    
    
    textureUnitNumber = 71;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[1]->colourTexture_0_ID);

    GLint gaussianTexture_UL = glGetUniformLocation(shaderProgramID, "gaussianTexture");
    glUniform1i(gaussianTexture_UL, textureUnitNumber);

    GLint bAddBloom_UL = glGetUniformLocation(shaderProgramID, "bAddBloom");
    glUniform1f(bAddBloom_UL, (GLfloat)GL_TRUE);
   // glUniform1f(bAddBloom_UL, (GLfloat)GL_FALSE);//TRUE);


    cMesh fullScreenQuad;
    fullScreenQuad.meshName = "Quad_2_sided_aligned_on_XY_plane.ply";
    //fullScreenQuad.meshName = "spiderManLegoFullAdjusted.ply";
    //fullScreenQuad.meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";

    //fullScreenQuad.textureName[0] = "hud1.bmp";
    //fullScreenQuad.textureRatios[0] = 1.0f;
    fullScreenQuad.setUniformDrawScale(10.0f);
    fullScreenQuad.drawPosition = glm::vec3(0.0f);
    //fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

    DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);
    glUniform1f(bAddBloom_UL, (GLfloat)GL_FALSE);


    return;
}
void DrawPass_GaussianBlur(GLuint shaderProgramID, GLFWwindow* pWindow,
                  int screenWidth, int screenHeight, int fboNumber, bool firstIt, bool horizontal)
{
    float ratio;

    glUseProgram(shaderProgramID);

    //glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Camera is pointing directly at the full screen quad
    glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
    glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


    //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
                FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               100.0f);    // Far (as small)

    glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
                                    FSQ_CameraTarget,
                                    ::g_upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    // Set up the textures for this offscreen quad
    //uniform bool bIsOffScreenTextureQuad;
    GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

    // uniform vec2 screenWidthAndHeight;	// x is width
    GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
    glUniform2f(screenWidthAndHeight_UL, 
                (GLfloat)screenWidth, 
                (GLfloat)screenHeight);

    GLint bGaussianBlur_UL = glGetUniformLocation(shaderProgramID, "bGaussianBlur");
    glUniform1f(bGaussianBlur_UL, (GLfloat)GL_TRUE);
    
    GLint bHorizontalBlur_UL = glGetUniformLocation(shaderProgramID, "bHorizontalBlur");
    if (horizontal)
    {
        glUniform1f(bHorizontalBlur_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bHorizontalBlur_UL, (GLfloat)GL_FALSE);
    }
    // Point the FBO from the 1st pass to this texture...

    GLint textureUnitNumber = 70;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    if (firstIt)
    {
        //glBindTexture(GL_TEXTURE_2D, ::g_pFBO_1->colourTexture_0_ID);
        glBindTexture(GL_TEXTURE_2D, ::g_pFBO_1->colourBrightnessTexture_1_ID);
    } 
    else
    {
        glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[fboNumber]->colourTexture_0_ID);
    }
    //uniform sampler2D textureOffScreen;
    GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
    glUniform1i(textureOffScreen_UL, textureUnitNumber);


    cMesh fullScreenQuad;
    fullScreenQuad.meshName = "Quad_2_sided_aligned_on_XY_plane.ply";
    //fullScreenQuad.meshName = "spiderManLegoFullAdjusted.ply";
    //fullScreenQuad.meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";

    //fullScreenQuad.textureName[0] = "hud1.bmp";
    //fullScreenQuad.textureRatios[0] = 1.0f;
    fullScreenQuad.setUniformDrawScale(10.0f);
    fullScreenQuad.drawPosition = glm::vec3(0.0f);
    //fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

    DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);


    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);


    return;
}

void DrawPass_Dilate(GLuint shaderProgramID, GLFWwindow* pWindow,
                  int screenWidth, int screenHeight)
{
    float ratio;

    glUseProgram(shaderProgramID);

    //glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;


    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Camera is pointing directly at the full screen quad
    glm::vec3 FSQ_CameraEye = glm::vec3(0.0, 0.0, 5.0f);
    glm::vec3 FSQ_CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);


    //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
                FSQ_CameraEye.x, FSQ_CameraEye.y, FSQ_CameraEye.z, 1.0f);



//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               100.0f);    // Far (as small)

    glm::mat4 matView = glm::lookAt(FSQ_CameraEye,
                                    FSQ_CameraTarget,
                                    ::g_upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    // Set up the textures for this offscreen quad
    //uniform bool bIsOffScreenTextureQuad;
    GLint bIsOffScreenTextureQuad_UL = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_TRUE);

    // uniform vec2 screenWidthAndHeight;	// x is width
    GLint screenWidthAndHeight_UL = glGetUniformLocation(shaderProgramID, "screenWidthAndHeight");
    glUniform2f(screenWidthAndHeight_UL, 
                (GLfloat)screenWidth, 
                (GLfloat)screenHeight);


    // Point the FBO from the 1st pass to this texture...

    GLint textureUnitNumber = 68;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[0]->colourTexture_0_ID);

    //uniform sampler2D textureOffScreen;
    GLint textureOffScreen_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
    glUniform1i(textureOffScreen_UL, textureUnitNumber);

    GLint dilateTexture_UL = glGetUniformLocation(shaderProgramID, "dilateTexture");
    glUniform1f(dilateTexture_UL, (GLfloat)GL_TRUE);


    cMesh fullScreenQuad;
    //fullScreenQuad.meshName = "Quad_2_sided_aligned_on_XY_plane.ply";
    fullScreenQuad.meshName = "spiderManLegoFullAdjusted.ply";
    //fullScreenQuad.meshName = "legospiderman_head_xyz_n_rgba_uv_at_Origin.ply";

    //fullScreenQuad.textureName[0] = "hud1.bmp";
    //fullScreenQuad.textureRatios[0] = 1.0f;
    fullScreenQuad.setUniformDrawScale(5.0f);
    fullScreenQuad.drawPosition = glm::vec3(0.0f);
    fullScreenQuad.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));

    DrawObject(&fullScreenQuad, glm::mat4(1.0f), shaderProgramID);

    glUniform1f(dilateTexture_UL, (GLfloat)GL_FALSE); 
    glUniform1f(bIsOffScreenTextureQuad_UL, (GLfloat)GL_FALSE);


    return;
}

void DrawPass_Depth(GLuint shaderProgramID, GLFWwindow* pWindow,cHiResTimer* p_HRTimer,
    int screenWidth, int screenHeight,  glm::vec3 sceneEye, glm::vec3 sceneTarget)
{
    GLint bIsDepthTexture_UL = glGetUniformLocation(shaderProgramID, "bIsDepthTexture");

    
    glUniform1f(bIsDepthTexture_UL, (GLfloat)GL_TRUE);
    DrawPass_1(shaderProgramID,
        pWindow,
        p_HRTimer,
        screenWidth, screenHeight,
        sceneEye, sceneTarget, false);

    //GLint bIsDepthTexture_UL = glGetUniformLocation(shaderProgramID, "bIsDepthTexture");


    glUniform1f(bIsDepthTexture_UL, (GLfloat)GL_FALSE);
}

void DrawPass_2(GLuint shaderProgramID, GLFWwindow* pWindow,
                int screenWidth, int screenHeight)
{
    float ratio;

    glUseProgram(shaderProgramID);

    //glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;

    //glViewport(0, 0, width, height);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


// *****************************************************************

    ::g_pTheLights->UpdateUniformValues(shaderProgramID);


// *****************************************************************
        //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
    glUniform4f(eyeLocation_UL,
                ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);



//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               10'000.0f);    // Far (as small)

    glm::mat4 matView = glm::lookAt(::g_cameraEye,
                                    ::g_cameraTarget,
                                    ::g_upVector);

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

    //"RetroTV.edited.bodyonly.ply"
    //"RetroTV.edited.screenonly.ply"

// STARTOF: LEFT TV
    glm::vec3 leftTVPosition = glm::vec3(15.0f, -20.0f, 75.0f);

    cMesh tvBody;
    tvBody.meshName = "RetroTV.edited.bodyonly.ply";
    tvBody.textureName[0] = "Yellow.bmp";
    tvBody.textureRatios[0] = 1.0f;
    tvBody.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
//    tvBody.setUniformDrawScale(0.5f);
//    tvBody.setUniformDrawScale(0.5f);
//    tvBody.drawPosition.x = -40.0f;
    tvBody.drawPosition = leftTVPosition;

    DrawObject(&tvBody, glm::mat4(1.0f), shaderProgramID);

    cMesh tvScreen;
    tvScreen.meshName = "RetroTV.edited.screenonly.ply";
    tvScreen.adjustRoationAngleFromEuler(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
//    tvScreen.setUniformDrawScale(0.5f);
//    tvScreen.drawPosition.x = -40.0f;
    tvScreen.drawPosition = leftTVPosition;

    // Set the framebuffer object (from the 1st pass)
    // to the texture on the TV screen
    tvScreen.textureIsFromFBO = true;
    tvScreen.FBOTextureNumber = ::g_pFBO_1->colourTexture_0_ID;
    tvScreen.textureRatios[0] = 1.0f;

    // Brighten up the screen
    tvScreen.bDoNotLight = true;

    DrawObject(&tvScreen, glm::mat4(1.0f), shaderProgramID);
// ENDOF: LEFT TV


 // STARTOF: RIGHT TV
//   tvBody.drawPosition.x = +40.0f;
//
//   DrawObject(&tvBody, glm::mat4(1.0f), shaderProgramID);
//
//   tvScreen.drawPosition.x = +40.0f;
//
//   // Set the framebuffer object (from the 1st pass)
//   // to the texture on the TV screen
//   tvScreen.textureIsFromFBO = true;
//   tvScreen.FBOTextureNumber = ::g_pFBO_2->colourTexture_0_ID;
//   tvScreen.textureRatios[0] = 1.0f;
//
//   // Brighten up the screen
//   tvScreen.bDoNotLight = true;
//
//   DrawObject(&tvScreen, glm::mat4(1.0f), shaderProgramID);
// ENDOF: RIGHT TV


    return;
}

void DrawPass_1(GLuint shaderProgramID, 
                GLFWwindow* pWindow, 
                cHiResTimer* p_HRTimer,
                int screenWidth, int screenHeight,
                glm::vec3 sceneEye, glm::vec3 sceneTarget, bool shadowMap)
{
#pragma region viewStuff
    // Switch the "main" shader
//        shaderProgramID = pShaderThing->getIDFromFriendlyName("shader01");
//        glUseProgram(shaderProgramID);

    float ratio;
//    int width, height;

    glUseProgram(shaderProgramID);

//    glfwGetFramebufferSize(pWindow, &width, &height);
    ratio = screenWidth / (float)screenHeight;

//    glViewport(0, 0, width, height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // While drawing a pixel, see if the pixel that's already there is closer or not?
    glEnable(GL_DEPTH_TEST);
    
    // (Usually) the default - does NOT draw "back facing" triangles
    glEnable(GL_CULL_FACE);
    if (shadowMap)
    {
        glCullFace(GL_FRONT);
    }
    else
    {
        glCullFace(GL_BACK);
    }
    
   


// *****************************************************************

    ::g_pTheLights->UpdateUniformValues(shaderProgramID);


// *****************************************************************
        //uniform vec4 eyeLocation;
    GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");



    glUniform4f(eyeLocation_UL,
                sceneEye.x, sceneEye.y, sceneEye.z, 1.0f);

    

//       //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    glm::mat4 matProjection = glm::perspective(0.6f,
                                               ratio,
                                               0.1f,        // Near (as big)
                                               10'000.0f);    // Far (as small)

    glm::mat4 matView; /*= glm::lookAt(sceneEye,
                                    sceneTarget,
                                    glm::vec3(0.0f, 1.0f, 1.0f));*/
                                    // first person
    if (!thirdPersonView)
    {
        matView = glm::lookAt(sceneEye,
            sceneTarget, 
            ::g_upVector);
    }

    // third person
    else
    {
        glm::vec3 lookAtVec = thePlayer->theMesh->drawPosition;
        lookAtVec.y += 10.0f;
        matView = glm::lookAt(sceneEye,
             lookAtVec,//thePlayer->theMesh->drawPosition,
            ::g_upVector);
    }

    GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
    glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

    GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
    glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));


#pragma endregion
//        // Redirect the output to the FBO (NOT the screen)
//        glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBO_1->ID );
//        // Remember to clear the FBO
//        ::g_pFBO_1->clearBuffers(true, true);
//        //::g_pFBO_1->clearColourBuffer(0);
//        //::g_pFBO_1->clearDepthBuffer();

       
// stores all opaque objects at the front of the vector
    int pos = 0;
    for (int i = 0; i < ::g_vec_pMeshesToDraw.size(); i++)
    {
        if (::g_vec_pMeshesToDraw[i]->alpha_transparency >= 1.0f)
        {
            std::swap(::g_vec_pMeshesToDraw[i], ::g_vec_pMeshesToDraw[pos]);
            pos++;
        }
    }

    // bubble sort for transparency objects
    for (int i = 0; i < ::g_vec_pMeshesToDraw.size() - 1; i++)
    {
        for (int j = 0; j < ::g_vec_pMeshesToDraw.size() - i - 1; j++)
        {
            if (::g_vec_pMeshesToDraw[j]->alpha_transparency >= 1.0f)
                continue;

            float distanceOne = glm::distance(::g_vec_pMeshesToDraw[j]->drawPosition, ::g_cameraEye);
            float distanceTwo = glm::distance(::g_vec_pMeshesToDraw[j + 1]->drawPosition, ::g_cameraEye);

            if (distanceOne < distanceTwo)
                std::swap(::g_vec_pMeshesToDraw[j], ::g_vec_pMeshesToDraw[j + 1]);
        }
    }

    //if (frameValue > 5.0f) // loop animation
    //{
    //    frameValue = 0.0f;
    //    frameNumber = 0;
    //}
    
    //         mat4x4_identity(m);
    
    
        // Draw the skybox
    {
        // HACK: I'm making this here, but hey...
        cMesh theSkyBox;
        theSkyBox.meshName = "Sphere_1_unit_Radius_xyz_n_rgba_uv.ply";
        theSkyBox.setUniformDrawScale(10.0f);

        theSkyBox.setUniformDrawScale(5'000.0f);
        theSkyBox.setDrawPosition(::g_cameraEye);
        //            theSkyBox.bIsWireframe = true;

                    // Depth test
        //            glDisable(GL_DEPTH_TEST);       // Writes no matter what
                    // Write to depth buffer (depth mask)
        //            glDepthMask(GL_FALSE);          // Won't write to the depth buffer

                    // uniform bool bIsSkyBox;
        GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgramID, "bIsSkyBox");
        glUniform1f(bIsSkyBox_UL, (GLfloat)GL_TRUE);

        // The normals for this sphere are facing "out" but we are inside the sphere
        glCullFace(GL_FRONT);

        DrawObject(&theSkyBox, glm::mat4(1.0f), shaderProgramID);

        glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);

        // Put the culling back to "normal" (back facing are not drawn)
        glCullFace(GL_BACK);
    }

// *********************************************************************
        // Draw all the objects
    frameNumber += 1 * animationSpeed;  
    float frameValue = ((float)frameNumber / 250.0f);
    double deltaTime = p_HRTimer->getFrameTime();   

    for (unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++)
    {
        cMesh* pCurrentMesh = ::g_vec_pMeshesToDraw[index];

        //if (pCurrentMesh->m_Animation != nullptr)
        //{
        //    ::g_Animations->Update(pCurrentMesh);
        //    // updates animation times

        //    if (pCurrentMesh->m_Animation->m_Loop)
        //    {
        //        pCurrentMesh->m_Animation->m_Time = fmod(frameValue, pCurrentMesh->m_Animation->m_Length);
        //    }
        //    else
        //    {
        //        pCurrentMesh->m_Animation->m_Time = frameValue;
        //    }

        //    
        //    cMesh* rootAnimationMesh = pCurrentMesh;
        //    glm::mat4 boneMatrices[5];
        //    for (int i = 0; i < 5; i++)
        //    {
        //        boneMatrices[i] = glm::mat4(1.0f);
        //    }

        //    glm::mat4 matModelParent = glm::mat4(1.0f); 
        //    int boneMatrixIndex = 0;
        //    while (rootAnimationMesh != nullptr)
        //    {
        //       // std::cout << boneMatrixIndex << " bone mesh index " << rootAnimationMesh->friendlyName << std::endl;
        //        
        //         // Translation
        //        glm::mat4 matTranslate = glm::translate(matModelParent,//glm::mat4(1.0f),//
        //            rootAnimationMesh->drawPosition);
        //        

        //        // Now we are all bougie, using quaternions
        //        glm::mat4 matRotation = glm::mat4(rootAnimationMesh->get_qOrientation()); 

        //        // Scaling matrix
        //        glm::mat4 matScale = glm::scale(glm::mat4(1.0f),//matModelParent,//
        //            rootAnimationMesh->drawScale);

        //        //std::cout << boneMatrixIndex << std::endl;

        //        boneMatrices[boneMatrixIndex] = matTranslate * matRotation * matScale; //matModel;
        //        matModelParent = matTranslate * matRotation * matScale;// boneMatrices[boneMatrixIndex];
        //        boneMatrixIndex++;

        //        rootAnimationMesh = rootAnimationMesh->vec_pChildMeshes.size() > 0 ? rootAnimationMesh->vec_pChildMeshes[0] : nullptr;
        //    }

        //    for (int i = 0; i < 5; i++)
        //    {
        //        std::stringstream boneMatrix;
        //        boneMatrix << "boneMatrices[" << i << "]";
        //        GLint boneMatricesUL = glGetUniformLocation(shaderProgramID, boneMatrix.str().c_str()); 
        //        glUniformMatrix4fv(boneMatricesUL, 1, GL_FALSE, glm::value_ptr(boneMatrices[i]));

        //    }
        //}
        sModelDrawInfo modelInfo;
        ::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshName, modelInfo);
        if (modelInfo.CharacterAnimations.size() != 0)
        {
            if (pCurrentMesh->meshName == thePlayer->theMesh->meshName)
            {
                if (thePlayer->animationPlaying != thePlayer->prevAnimation)
                {
                    if (!pCurrentMesh->hasBlend)
                    {
                        ::g_Animations->BlendAnimations(pCurrentMesh, thePlayer->prevAnimation, thePlayer->animationPlaying,
                            &modelInfo, deltaTime, thePlayer->lookingAt);
                        
                    }
                    ::g_Animations->UpdateCharacter(pCurrentMesh, -1, &modelInfo, deltaTime, thePlayer->lookingAt);
                    if (pCurrentMesh->blendAnimation->currentTime >= pCurrentMesh->blendAnimation->duration)
                    {
                        thePlayer->prevAnimation = thePlayer->animationPlaying;
                        //std::cout << modelInfo.BlendAnimation->currentTime << ", " << modelInfo.BlendAnimation->duration << std::endl;
                        pCurrentMesh->hasBlend = false;
                    }
                }
                else
                {
                    ::g_Animations->UpdateCharacter(pCurrentMesh, thePlayer->animationPlaying, &modelInfo, deltaTime, thePlayer->lookingAt);
                }
            }
            else if (pCurrentMesh->friendlyName == "enemy10" || pCurrentMesh->friendlyName == "enemy11")
            {
                ::g_Animations->UpdateCharacter(pCurrentMesh, 1, &modelInfo, deltaTime, enemyList[9]->forward);
                
            }
            auto boneInfo = modelInfo.BoneInfoVec;
            std::vector<glm::mat4> boneMatrices;
            //int headBone = modelInfo.FindBoneInfoByName("mixamorig_Neck");
            for (int i = 0; i < boneInfo.size(); i++)
            {
                /*if (i == headBone)
                {
                    boneMatrices.push_back(boneInfo[i].FinalTransformation * glm::toMat4(glm::quatLookAt(thePlayer->lookingAt, glm::vec3(0.0f, 1.0f, 0.0f))));
                    continue;
                }*/
                boneMatrices.push_back(boneInfo[i].FinalTransformation);
            }
       
            GLint boneMatricesUL = glGetUniformLocation(shaderProgramID, "boneMatrices");
            glUniformMatrix4fv(boneMatricesUL, boneMatrices.size(), GL_FALSE, glm::value_ptr(boneMatrices[0]));
        }

        if (pCurrentMesh->bIsVisible)
        {
            glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix 

            if (pCurrentMesh->friendlyName == "screen2")
            {
                GLint bUsesNightvision_UL = glGetUniformLocation(shaderProgramID, "bUseNightvision");
                glUniform1f(bUsesNightvision_UL, (GLfloat)GL_TRUE);
                DrawObject(pCurrentMesh, matModel, shaderProgramID);
                glUniform1f(bUsesNightvision_UL, (GLfloat)GL_FALSE);
            }
            /*else if (pCurrentMesh->friendlyName == "screen1")
            {
                GLint textureUnitNumber = 72;
                glActiveTexture(GL_TEXTURE0 + textureUnitNumber); 
                glBindTexture(GL_TEXTURE_2D, ::g_pFBOList[0]->colourTexture_0_ID);
                
                            
                GLint colourTexture_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
                glUniform1i(colourTexture_UL, textureUnitNumber);
                
                GLint bUseDOF_UL = glGetUniformLocation(shaderProgramID, "bUseDOF");
                glUniform1f(bUseDOF_UL, (GLfloat)GL_TRUE);
                
                GLint focalPosition_UL = glGetUniformLocation(shaderProgramID, "DOFPosition");
                glUniform4f(focalPosition_UL, 0.0f, 2.0f, 0.0f, 1.0f);
                            
                GLint dofValues_UL = glGetUniformLocation(shaderProgramID, "DOFValues");
                glUniform4f(dofValues_UL, 5.0f, 3.0f, 1.0f, 1.0f);

                DrawObject(pCurrentMesh, matModel, shaderProgramID);

                glUniform1f(bUseDOF_UL, (GLfloat)GL_FALSE); 
            }*/
            if (pCurrentMesh->friendlyName == "Ground")
            {
                for (int i = -9; i < 10; i++)
                {
                    for (int j = -9; j < 10; j++)
                    {
                        pCurrentMesh->drawPosition = glm::vec3(0.0f + i * 192.0f, 0.0f, 0.0f + j * 192.0f);
                        DrawObject(pCurrentMesh, matModel, shaderProgramID);
                    }
                }
            }
            else
            {
                
                DrawObject(pCurrentMesh, matModel, shaderProgramID);
                
            }
        }//if (pCurrentMesh->bIsVisible)

    }

    //for ( unsigned int index
    // *********************************************************************



// This is very old OpenGL "Immediate Mode"
// Don't use this for your usual stuff, but it's very handy 
//   for debug stuff (where you're not worried about speed)
// So I guess you CAN mix old and new stuff
//        glBegin(GL_QUADS);
//            glColor3f(1.0f, 1.0f, 1.0f);
//            glVertex3i(-10, 10.0, 0.0);
//            glVertex3i(+10, 10.0, 0.0);
//            glVertex3i(+10, 10.0, -10.0);
//            glVertex3i(-10, 10.0, -10.0);
//        glEnd();


    
//        std::cout << deltaTime << std::endl;


// ***********************************************************************
    if (!::g_vecAnimationCommands.empty())
    {
        ::g_vecAnimationCommands[0].Update(deltaTime);

        // Done? 
        if (::g_vecAnimationCommands[0].isDone())
        {
            // Erase the item from vector
            // Is "slow" in that it has to copy the vector again
            ::g_vecAnimationCommands.erase(::g_vecAnimationCommands.begin());
        }
    }//if (!vecAnimationCommands.empty())
// ***********************************************************************


//
    ::g_pPhysics->Update(deltaTime); 

    if (!enemyList.empty())
    {
        for (unsigned int index = 0; index < enemyList.size(); index++)
        {
            enemyList[index]->Update(deltaTime);
            
            if (index <= 8)
            {
                ::g_pTheLights->theLights[index].position = glm::vec4(enemyList[index]->theMesh->drawPosition, 0.0f);
            }
        }
    }

   // flockEntity->Update(deltaTime);
    

        // Update the particles
    ::g_anEmitter.UpdateSmoke(deltaTime);
    ::g_anEmitter2.UpdateSmoke(deltaTime);

    std::vector< cParticleSystem::cParticleRender > vecParticles_to_draw;


    ::g_grassPatch.Update(deltaTime);
    //::g_DrawDebugSphere(::g_anEmitter.getPosition(), 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    for (cBlade& curBlade : ::g_grassPatch.m_Blades)
    {
        glm::mat4 matModel = glm::mat4(1.0f);

        ::g_pGrassBladeMeshModel->drawPosition = curBlade.position;
        ::g_pGrassBladeMeshModel->wholeObjectDebugColourRGBA = curBlade.colour;

        glm::vec3 camDir = glm::normalize(::g_cameraEye - curBlade.position);
        float angleY = atan2(camDir.x, camDir.z);

        glm::quat rotate = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f));
        ::g_pGrassBladeMeshModel->setDrawOrientation(rotate);
        ::g_pGrassBladeMeshModel->adjustRoationAngleFromEuler(glm::vec3(0.0f, 1.570796f, 0.0f));

        ::g_pGrassBladeMeshModel->drawScale = curBlade.scale;

       /* glm::vec3 camDir = glm::normalize(::g_pGrassBladeMeshModel->drawPosition - ::g_cameraEye);
        float angleY = atan2(camDir.x, camDir.z);
        float angleX = atan2(-camDir.y, sqrt(camDir.x * camDir.x + camDir.z * camDir.z));

        glm::quat rotate = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(angleX, glm::vec3(1.0f, 0.0f, 0.0f));

        ::g_pGrassBladeMeshModel->setDrawOrientation(rotate);*/

        DrawObject(::g_pGrassBladeMeshModel, matModel, shaderProgramID);
    }

    ::g_anEmitter.getParticleList(vecParticles_to_draw);

    for (int i = 0; i < vecParticles_to_draw.size() - 1; i++)
    {
        for (int j = 0; j < vecParticles_to_draw.size() - i - 1; j++)
        {
            float distanceOne = glm::distance(vecParticles_to_draw[j].position, ::g_cameraEye);
            float distanceTwo = glm::distance(vecParticles_to_draw[j + 1].position, ::g_cameraEye);

            if (distanceOne < distanceTwo)
            {
                std::swap(vecParticles_to_draw[j], vecParticles_to_draw[j + 1]);
            }
        }
    }

    for (cParticleSystem::cParticleRender& curParticle : vecParticles_to_draw)
    {
//            ::g_DrawDebugSphere(curParticle.position, 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

        ::g_pParticleMeshModel->setUniformDrawScale(1.0f);

        ::g_pParticleMeshModel->drawPosition = curParticle.position;
        //::g_pParticleMeshModel->setDrawOrientation(curParticle.orientation);
        glm::vec3 camDir = glm::normalize(::g_cameraEye - ::g_pParticleMeshModel->drawPosition);
        float angleY = atan2(camDir.x, camDir.z);
        float angleX = atan2(-camDir.y, sqrt(camDir.x * camDir.x + camDir.z * camDir.z));

        glm::quat rotate = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(angleX, glm::vec3(1.0f, 0.0f, 0.0f));
        ::g_pParticleMeshModel->setDrawOrientation(rotate);

        ::g_pParticleMeshModel->drawScale = curParticle.scaleXYZ;
       
        ::g_pParticleMeshModel->alpha_transparency = curParticle.colourChange.w;
        
//            ::g_pParticleMeshModel->setDrawOrientation(glm::vec3(0.0f, 3.141f/2.0f, 0.0f));

        DrawObject(::g_pParticleMeshModel, matModel, shaderProgramID);
    }

    vecParticles_to_draw.clear();

    ::g_anEmitter2.getParticleList(vecParticles_to_draw);

    for (int i = 0; i < vecParticles_to_draw.size() - 1; i++)
    {
        for (int j = 0; j < vecParticles_to_draw.size() - i - 1; j++)
        {
            float distanceOne = glm::distance(vecParticles_to_draw[j].position, ::g_cameraEye);
            float distanceTwo = glm::distance(vecParticles_to_draw[j + 1].position, ::g_cameraEye);

            if (distanceOne < distanceTwo)
            {
                std::swap(vecParticles_to_draw[j], vecParticles_to_draw[j + 1]);
            }
        }
    }

    for (cParticleSystem::cParticleRender& curParticle : vecParticles_to_draw)
    {
//            ::g_DrawDebugSphere(curParticle.position, 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

        ::g_pParticleMeshModel->setUniformDrawScale(1.0f);

        ::g_pParticleMeshModel->drawPosition = curParticle.position;
        //::g_pParticleMeshModel->setDrawOrientation(curParticle.orientation);
        glm::vec3 camDir = glm::normalize(::g_cameraEye - ::g_pParticleMeshModel->drawPosition);
        float angleY = atan2(camDir.x, camDir.z);
        float angleX = atan2(-camDir.y, sqrt(camDir.x * camDir.x + camDir.z * camDir.z));

        glm::quat rotate = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(angleX, glm::vec3(1.0f, 0.0f, 0.0f));
        ::g_pParticleMeshModel->setDrawOrientation(rotate);

        ::g_pParticleMeshModel->drawScale = curParticle.scaleXYZ;
       
        ::g_pParticleMeshModel->alpha_transparency = curParticle.colourChange.w;
        
//            ::g_pParticleMeshModel->setDrawOrientation(glm::vec3(0.0f, 3.141f/2.0f, 0.0f));

        DrawObject(::g_pParticleMeshModel, matModel, shaderProgramID);
    }

    
   

   // The soft body bathtub

//    ::g_SoftBody.VerletUpdate(deltaTime);
//    // 
//    ::g_SoftBody.ApplyCollision(deltaTime);
//    // 
//    ::g_SoftBody.SatisfyConstraints();
//
//   // Update the mesh in the VAO based on the current particle positions
//    ::g_SoftBody.UpdateVertexPositions();
//    ::g_SoftBody.UpdateNormals();//    
    

//    UpdateSoftBody(::g_SoftBody, deltaTime);

//    sSoftBodyThreadInfo SBIParams;
//    SBIParams.p_theSoftBody = &::g_SoftBody;
//    SBIParams.deltaTime = deltaTime;
//
//    sSoftBodyThreadInfo* pSBI = &SBIParams;
//    
//    UpdateEntityThread( (void*)pSBI); 
    ::g_pMeshManager->UpdateVertexBuffers(::g_SoftBody.m_ModelVertexInfo.meshName, 
        ::g_SoftBody.m_ModelVertexInfo, 
        shaderProgramID); 
    
    ::g_pMeshManager->UpdateVertexBuffers(::g_Bridge.m_ModelVertexInfo.meshName, 
        ::g_Bridge.m_ModelVertexInfo, 
        shaderProgramID);  
// for the softbody corners    
    /*for (cSoftBodyVerlet::sParticle* pCurParticle : ::g_SoftBody.vec_pParticles)
    {
        ::g_DrawDebugSphere(pCurParticle->position, 1.5f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    }*/
    
    //int it = 0;
    //for (cSoftBodyVerlet::sParticle* pCurParticle : ::g_Bridge.vec_pParticles)
    //{
    //    if (it == 65 || it == 9 )//|| it == 8 || it == 9 || it == 67 || it == 66 || it == 65 || it == 64)
    //    {
    //        ::g_DrawDebugSphere(pCurParticle->position, 2.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    //    }
    //    else
    //    {
    //        ::g_DrawDebugSphere(pCurParticle->position, 2.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    //        
    //    }
    //    
    //    it++;
    //}



    // Update vertex positions in mesh (in VAO aka on the GPU)
    // g_SoftBody->GetModelDrawInfo();
//    pVertics* pLocalArray = ::g_SoftBody.m_ModelVertexInfo.pVertices;
//    ::g_pMeshManager->UpdateVertexBuffers(::g_SoftBody.m_ModelVertexInfo.meshName,
//                                          ::g_SoftBody.m_ModelVertexInfo,
 //                                         shaderProgramID);

    // Draw actual mesh
//    {
//        glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
//
//        cMesh softBodyMesh;
//        softBodyMesh.meshName = ::g_SoftBody.m_ModelVertexInfo.meshName;
//        softBodyMesh.textureName[0] = "SpidermanUV_square.bmp";
//        softBodyMesh.textureRatios[0] = 1.0f;
//
//        glDisable(GL_CULL_FACE);
//        DrawObject(&softBodyMesh, matModel, shaderProgramID);
//        glEnable(GL_CULL_FACE);
//
//    }


    //  Draw the wheel based on the soft body
    ::g_Wheel.VerletUpdate(deltaTime);
    // 
    ::g_Wheel.ApplyCollision(deltaTime);
    // 
    ::g_Wheel.SatisfyConstraints();

   // Update the mesh in the VAO based on the current particle positions
    ::g_Wheel.UpdateVertexPositions();
    ::g_Wheel.UpdateNormals();//   

    ::g_pMeshManager->UpdateVertexBuffers(::g_Wheel.m_ModelVertexInfo.meshName,
                                          ::g_Wheel.m_ModelVertexInfo,
                                          shaderProgramID);


    /*for (cSoftBodyVerlet::sParticle* pCurParticle : ::g_Wheel.vec_pParticles)
    {
        ::g_DrawDebugSphere(pCurParticle->position, 0.1f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }*/

    // Draw actual mesh
    //{
    //    glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix

    //    cMesh softBodyMesh;
    //    softBodyMesh.meshName = ::g_Wheel.m_ModelVertexInfo.meshName;
    //    softBodyMesh.textureName[0] = "SpidermanUV_square.bmp";
    //    softBodyMesh.textureRatios[0] = 1.0f;

    //    glDisable(GL_CULL_FACE);
    //    DrawObject(&softBodyMesh, matModel, shaderProgramID);
    //    glEnable(GL_CULL_FACE);
    //}

    // TEST: See if we can shift a mesh
//    sModelDrawInfo gridMesh;
//    if ( ::g_pMeshManager->FindDrawInfoByModelName("Grid_100x100_vertical_in_y.ply", gridMesh) )
//    if ( ::g_pMeshManager->FindDrawInfoByModelName("bathtub_xyz_n_rgba_uv_x3_size_Offset_in_Y.ply", gridMesh) )
//    {
//        for ( unsigned int index = 0; index != gridMesh.numberOfVertices; index++ )
//        {
//            gridMesh.pVertices[index].x += 0.1f;
//        }
//    }
//    // Now update the mesh in the VAO
//    ::g_pMeshManager->UpdateVAOBuffers("Grid_100x100_vertical_in_y.ply", gridMesh, shaderProgramID);
 //   ::g_pMeshManager->UpdateVAOBuffers("bathtub_xyz_n_rgba_uv_x3_size_Offset_in_Y.ply", gridMesh, shaderProgramID);
    


//        // Redirect the output back to the screen
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//        glfwGetFramebufferSize(window, &width, &height);
//        ratio = width / (float)height;
//
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // While drawing a pixel, see if the pixel that's already there is closer or not?
//        glEnable(GL_DEPTH_TEST);
//        // (Usually) the default - does NOT draw "back facing" triangles
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);



//        //uniform bool bIsOffScreenTextureQuad;
//        GLuint bIsOffScreenTextureQuad_ID = glGetUniformLocation(shaderProgramID, "bIsOffScreenTextureQuad");
//        glUniform1f(bIsOffScreenTextureQuad_ID, (GLfloat)GL_TRUE);
//
//        // Connect the FBO colour texture to this texture
//        {
//            GLint textureUnitNumber = 50;
//            glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
//            GLuint Texture01 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
//           // Get texture number from the FBO colour texture
//           glBindTexture(GL_TEXTURE_2D, ::g_pFBO_1->colourTexture_0_ID);
//           GLint texture_01_UL = glGetUniformLocation(shaderProgramID, "textureOffScreen");
//           glUniform1i(texture_01_UL, textureUnitNumber);
//       }
//
//       DrawObject(::g_pOffscreenTextureQuad, glm::mat4(1.0f), shaderProgramID);
//
//       glUniform1f(bIsOffScreenTextureQuad_ID, (GLfloat)GL_FALSE);



    return;
}// void DrawPass_1(

// Returns NULL if not found
cMesh* g_pFindMeshByFriendlyName(std::string friendlyNameToFind)
{
    for ( unsigned int index = 0; index != ::g_vec_pMeshesToDraw.size(); index++ )
    {
        if ( ::g_vec_pMeshesToDraw[index]->friendlyName == friendlyNameToFind )
        {
            // Found it
            return ::g_vec_pMeshesToDraw[index];
        }
    }
    // Didn't find it
    return NULL;
}

void DrawLightDebugSpheres(glm::mat4 matProjection, glm::mat4 matView,
                           GLuint shaderProgramID)
{
    if ( ! ::g_drawDebugLightSpheres )
    {
        return;
    }

    // Draw concentric spheres to indicate light position and attenuation

    // Small white sphere where the light is
    ::g_DrawDebugSphere(::g_pTheLights->theLights[g_selectedLight].position,
                        0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    cLightHelper lightHelper;

    // vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
    float constantAtten = ::g_pTheLights->theLights[g_selectedLight].atten.x;
    float linearAtten = ::g_pTheLights->theLights[g_selectedLight].atten.y;
    float quadAtten = ::g_pTheLights->theLights[g_selectedLight].atten.z;


    // Draw a red sphere at 75% brightness
    {
        float distAt75Percent = lightHelper.calcApproxDistFromAtten(0.75f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        ::g_DrawDebugSphere(::g_pTheLights->theLights[g_selectedLight].position,
                            distAt75Percent, 
                            glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));
    }


    // Draw a green sphere at 50% brightness
    {
        float distAt50Percent = lightHelper.calcApproxDistFromAtten(0.50f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        ::g_DrawDebugSphere(::g_pTheLights->theLights[g_selectedLight].position,
                            distAt50Percent,
                            glm::vec4(0.0f, 0.5f, 0.0f, 1.0f));
    }

    // Draw a yellow? sphere at 25% brightness
    {
        float distAt25Percent = lightHelper.calcApproxDistFromAtten(0.25f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        ::g_DrawDebugSphere(::g_pTheLights->theLights[g_selectedLight].position,
                            distAt25Percent,
                            glm::vec4(0.50f, 0.5f, 0.0f, 1.0f));
    }

    // Draw a blue sphere at 1% brightness
    {
        float distAt_5Percent = lightHelper.calcApproxDistFromAtten(0.01f, 0.01f, 100000.0f,
                                                                    constantAtten, linearAtten, quadAtten, 50);

        ::g_DrawDebugSphere(::g_pTheLights->theLights[g_selectedLight].position,
                            distAt_5Percent,
                            glm::vec4(0.0f, 0.0f, 0.5f, 1.0f));
    }


    return;
}

void SetUpTextures(cMesh* pCurrentMesh, GLuint shaderProgramID)
{
//    GLuint Texture00 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[0]);
//    if (Texture00 == 0)
//    {
//        Texture00 = ::g_pTextureManager->getTextureIDFromName("Blank_UV_Text_Texture.bmp");
//    }
//
//    // We are just going to pick texture unit 5 (for no reason, just as an example)
//    //    glActiveTexture(GL_TEXTURE5);       // #5 TEXTURE UNIT
//    glActiveTexture(GL_TEXTURE0 + 5);       // #5 TEXTURE UNIT
//    glBindTexture(GL_TEXTURE_2D, Texture00);
//
//    //uniform sampler2D texture_00;
//    GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "texture_00");
//    glUniform1i(texture_00_UL, 5);     // <- 5, an integer, because it's "Texture Unit #5"
//    // ***************************************************************

//    uniform sampler2D texture_00;			// 2D meaning x,y or s,t or u,v
//    uniform sampler2D texture_01;
//    uniform sampler2D texture_02;
//    uniform sampler2D texture_03;
//    uniform sampler2D texture_04;			// 2D meaning x,y or s,t or u,v
//    uniform sampler2D texture_05;
//    uniform sampler2D texture_06;
//    uniform sampler2D texture_07;
//    //... and so on
//    //uniform float textureMixRatio[8];
//    uniform vec4 textureMixRatio_0_3;
//    uniform vec4 textureMixRatio_4_7;

    //    uniform vec4 textureMixRatio_0_3;
//    uniform vec4 textureMixRatio_4_7;

    GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_0_3");
//    GLint textureMixRatio_4_7_UL = glGetUniformLocation(shaderProgramID, "textureMixRatio_4_7");

    glUniform4f(textureMixRatio_0_3_UL,
                pCurrentMesh->textureRatios[0],
                pCurrentMesh->textureRatios[1],
                pCurrentMesh->textureRatios[2],
                pCurrentMesh->textureRatios[3]);
//    glUniform4f(textureMixRatio_4_7_UL,
//                pCurrentMesh->textureRatios[4],
//                pCurrentMesh->textureRatios[5],
//                pCurrentMesh->textureRatios[6],
//                pCurrentMesh->textureRatios[7]);


    // Also set up the height map and discard texture

    {
        // uniform sampler2D heightMapSampler;		// Texture unit 20
        GLint textureUnitNumber = 20;
        GLuint Texture20 = ::g_pTextureManager->getTextureIDFromName("NvF5e_height_map.bmp");
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture20);
        GLint texture_20_UL = glGetUniformLocation(shaderProgramID, "heightMapSampler");
        glUniform1i(texture_20_UL, textureUnitNumber);
    }



    // Is this taking a texture from an FBO??
    if ( pCurrentMesh->textureIsFromFBO)
    {
        GLint textureUnitNumber = 0;
//        GLuint Texture00 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
//        glBindTexture(GL_TEXTURE_2D, Texture00);
        glBindTexture(GL_TEXTURE_2D, pCurrentMesh->FBOTextureNumber);
        GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "texture_00");
        glUniform1i(texture_00_UL, textureUnitNumber);

        // Likely don't want anything else
        if (!pCurrentMesh->useOtherTextures)
        {
            return;
        }
    }

    if (!pCurrentMesh->useOtherTextures)
    {
        GLint textureUnitNumber = 0;
        GLuint Texture00 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture00);
        GLint texture_00_UL = glGetUniformLocation(shaderProgramID, "texture_00");
        glUniform1i(texture_00_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 1;
        GLuint Texture01 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture01);
        GLint texture_01_UL = glGetUniformLocation(shaderProgramID, "texture_01");
        glUniform1i(texture_01_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 2;
        GLuint Texture02 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture02);
        GLint texture_02_UL = glGetUniformLocation(shaderProgramID, "texture_02");
        glUniform1i(texture_02_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 3;
        GLuint Texture03 = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture03);
        GLint texture_03_UL = glGetUniformLocation(shaderProgramID, "texture_03");
        glUniform1i(texture_03_UL, textureUnitNumber);
    }    
    // and so on to however many texture you are using





    // Set up a skybox
    {
        // uniform samplerCube skyBoxTexture;		// Texture unit 30
        GLint textureUnit30 = 30;
        GLuint skyBoxID = ::g_pTextureManager->getTextureIDFromName("interstellar");
        glActiveTexture(GL_TEXTURE0 + textureUnit30);
        // NOTE: Binding is NOT to GL_TEXTURE_2D
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);
        GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramID, "skyBoxTexture");
        glUniform1i(skyBoxSampler_UL, textureUnit30);
    }

    
    return;
}

void DrawObject(cMesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID)
{

   
       // Rotation matrix generation
//    glm::mat4 matRotateX = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.x, // (float)glfwGetTime(),
//                                       glm::vec3(1.0f, 0.0, 0.0f));
//
//
//    glm::mat4 matRotateY = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.y, // (float)glfwGetTime(),
//                                       glm::vec3(0.0f, 1.0, 0.0f));
//
//    glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
//                                       pCurrentMesh->drawOrientation.z, // (float)glfwGetTime(),
//                                       glm::vec3(0.0f, 0.0, 1.0f));
 //         mat4x4_identity(m);
    glm::mat4 matModel = matModelParent;




    // Translation
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
                                            glm::vec3(pCurrentMesh->drawPosition.x,
                                                      pCurrentMesh->drawPosition.y,
                                                      pCurrentMesh->drawPosition.z));


    // Now we are all bougie, using quaternions
    glm::mat4 matRotation = glm::toMat4( pCurrentMesh->get_qOrientation() );


    // Scaling matrix
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                                    glm::vec3(pCurrentMesh->drawScale.x,
                                              pCurrentMesh->drawScale.y,
                                              pCurrentMesh->drawScale.z));
    //--------------------------------------------------------------

    // Combine all these transformation
    matModel = matModel * matTranslate;         // Done last

//    matModel = matModel * matRotateX;
//    matModel = matModel * matRotateY;
//    matModel = matModel * matRotateZ;
    //
    matModel = matModel * matRotation;


    matModel = matModel * matScale;             // Mathematically done 1st

//        m = m * rotateZ;
//        m = m * rotateY;
//        m = m * rotateZ;



   //mat4x4_mul(mvp, p, m);
//    glm::mat4 mvp = matProjection * matView * matModel;

//    GLint mvp_location = glGetUniformLocation(shaderProgramID, "MVP");
//    //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
//    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

    GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));


    // Also calculate and pass the "inverse transpose" for the model matrix
    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    // uniform mat4 matModel_IT;
    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));


    if (pCurrentMesh->bIsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }



//        glPointSize(10.0f);

    GLint bHasNormalMap_UL = glGetUniformLocation(shaderProgramID, "bHasNormalMap");
    if (pCurrentMesh->hasNormalMapSecondTexture)
    {
        // Set uniform to true
        glUniform1f(bHasNormalMap_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        // Set uniform to false;
        glUniform1f(bHasNormalMap_UL, (GLfloat)GL_FALSE);
    }


        // uniform bool bDoNotLight;
    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (pCurrentMesh->bDoNotLight)
    {
        // Set uniform to true
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        // Set uniform to false;
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
    }

        //uniform bool bUseDebugColour;	
    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");
    if (pCurrentMesh->bUseDebugColours)
    {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
        //uniform vec4 debugColourRGBA;
        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
                    pCurrentMesh->wholeObjectDebugColourRGBA.r,
                    pCurrentMesh->wholeObjectDebugColourRGBA.g,
                    pCurrentMesh->wholeObjectDebugColourRGBA.b,
                    pCurrentMesh->wholeObjectDebugColourRGBA.a);
    }
    else
    {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
    }



    // FOR NOW, hardcode the texture settings
    
    // uniform bool bUseVertexColours;
    GLint bUseVertexColours_UL = glGetUniformLocation(shaderProgramID, "bUseVertexColours");
    glUniform1f(bUseVertexColours_UL, (GLfloat)GL_FALSE);//GL_TRUE);



    SetUpTextures(pCurrentMesh, shaderProgramID);

// *********************************************************************
    // Is this using the heigth map?
    // HACK:
    GLint bUseHeightMap_UL = glGetUniformLocation(shaderProgramID, "bUseHeightMap");
    // uniform bool bUseHeightMap;
    if ( pCurrentMesh->friendlyName == "Ground" )
    {
        glUniform1f(bUseHeightMap_UL, (GLfloat)GL_TRUE);

        //uniform float heightScale;
        GLint heightScale_UL = glGetUniformLocation(shaderProgramID, "heightScale");
        glUniform1f(heightScale_UL, ::g_HeightAdjust);
        
        //uniform vec2 UVOffset;
        GLint UVOffset_UL = glGetUniformLocation(shaderProgramID, "UVOffset");
        glUniform2f(UVOffset_UL, ::g_UVOffset.x, ::g_UVOffset.y);


    }
    else
    {
        glUniform1f(bUseHeightMap_UL, (GLfloat)GL_FALSE);
    }

    GLint bHasBones_UL = glGetUniformLocation(shaderProgramID, "bHasBones");
    if (pCurrentMesh->hasBones)
    {
        glUniform1f(bHasBones_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        glUniform1f(bHasBones_UL, (GLfloat)GL_FALSE); 
    } 

    GLint bIsGrass_UL = glGetUniformLocation(shaderProgramID, "bIsGrass");
    GLint timePassed_UL = glGetUniformLocation(shaderProgramID, "timePassed");
    GLint grassPos_UL = glGetUniformLocation(shaderProgramID, "grassPos");
    glUniform1f(timePassed_UL, (float)::g_grassPatch.m_timePassed);
    if (pCurrentMesh->friendlyName == "grassBlade")
    {
        glUniform1f(bIsGrass_UL, (GLfloat)GL_TRUE);

        glUniform4f(grassPos_UL, pCurrentMesh->drawPosition.x,
            pCurrentMesh->drawPosition.y,
            pCurrentMesh->drawPosition.z,
            1.0f);
    }
    else
    {
        glUniform1f(bIsGrass_UL, (GLfloat)GL_FALSE); 
    }

    GLint windForce_UL = glGetUniformLocation(shaderProgramID, "windForce");

    glUniform4f(windForce_UL, ::g_grassPatch.m_GrassInfo.constantForce.x,
        ::g_grassPatch.m_GrassInfo.constantForce.y,
        ::g_grassPatch.m_GrassInfo.constantForce.z,
        1.0f);

// *********************************************************************


// *********************************************************************
    //  Discard texture example
    //    uniform bool bUseDiscardMaskTexture;
    //    uniform sampler2D maskSamplerTexture01;
    {
        GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramID, "bUseDiscardMaskTexture");

        // uniform bool bUseHeightMap;
//        if ( pCurrentMesh->friendlyName == "Ground" )
//        {
//            glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_TRUE);
//
//            //uniform sampler2D maskSamplerTexture01; 	// Texture unit 25
//            GLint textureUnitNumber = 25;
//            GLuint stencilMaskID = ::g_pTextureManager->getTextureIDFromName("FAKE_Stencil_Texture_612x612.bmp");
//            glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
//            glBindTexture(GL_TEXTURE_2D, stencilMaskID);
//
//            GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramID, "maskSamplerTexture01");
//            glUniform1i(bUseDiscardMaskTexture_UL, textureUnitNumber);
//
//        }
//        else
//        {
            glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat)GL_FALSE);
//        }
    }
// *********************************************************************
    


    // ALPHA Transparency
    // is this even transparent?
    if (pCurrentMesh->alpha_transparency < 1.0f)
    {
        //yes
        glEnable(GL_BLEND);
        // this is what makes it alpha transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        // turn this off
        glDisable(GL_BLEND);
    }

    GLint transparencyAlpha = glGetUniformLocation(shaderProgramID, "transparencyAlpha");
    glUniform1f(transparencyAlpha, pCurrentMesh->alpha_transparency);

    GLint bIsSmokeParticle_UL = glGetUniformLocation(shaderProgramID, "bIsSmokeParticle");
        
    if (pCurrentMesh->friendlyName == "smokeParticle")
    {
        //yes
        glEnable(GL_BLEND);
        // this is what makes it alpha transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniform1f(bIsSmokeParticle_UL, (GLfloat)GL_TRUE);
    }
    else
    {
        // turn this off
        glDisable(GL_BLEND);

        glUniform1f(bIsSmokeParticle_UL, (GLfloat)GL_FALSE); 
    }


    std::string meshToDraw = pCurrentMesh->meshName;

    // See if there is any LOD info in this mesh? 
    if ( ! pCurrentMesh->vecLODs.empty() )
    {
        // Yes, so choose the LOD based on distance from camera

        float distToCamera = glm::distance(pCurrentMesh->drawPosition, g_cameraEye);

        // Choose mesh based on distance
        for ( cMesh::sLODInfo &curLOD : pCurrentMesh->vecLODs )
        {
            if ( distToCamera < curLOD.minDistance )
            {
                meshToDraw = curLOD.meshName;
            }
        }

    }//if ( ! pCurrentMesh->vecLODs.empty() )

    sModelDrawInfo modelInfo;
    if (::g_pMeshManager->FindDrawInfoByModelName(meshToDraw, modelInfo))
    {
        // Found it!!!

        glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
        glDrawElements(GL_TRIANGLES,
                       modelInfo.numberOfIndices,
                       GL_UNSIGNED_INT,
                       0);
        glBindVertexArray(0); 			            // disable VAO (and everything else)

    }

    // Are there any child meshes on this mesh
    // std::vector<cMesh*> vec_pChildMeshes;

   /* glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
                                            glm::vec3( 
                                                1.0f / pCurrentMesh->drawScale.x,
                                                1.0f / pCurrentMesh->drawScale.y,
                                                1.0f / pCurrentMesh->drawScale.z));

    matModel = matModel * matRemoveScaling;*/

   for ( cMesh* pChild : pCurrentMesh->vec_pChildMeshes )
   {

       // Notice we are passing the "parent" (already transformed) matrix
       // NOT an identiy matrix

       // if you are using scaling, you can "undo" the scaling
       // i.e. add the opposite of the scale the parent had

       if (pChild->bIsVisible)
       {
           DrawObject(pChild, matModel, shaderProgramID);
       }

   }//for ( cMesh* pChild 



    return;
}

void g_DrawDebugSphere(glm::vec3 position, float scale, glm::vec4 colourRGBA)
{
    // Save the debug sphere state
    bool OLD_isVisible = ::g_pDebugSphereMesh->bIsVisible;
    glm::vec3 OLD_position = ::g_pDebugSphereMesh->drawPosition;
    glm::vec3 OLD_scale = ::g_pDebugSphereMesh->drawScale;
    glm::vec4 OLD_colours = ::g_pDebugSphereMesh->wholeObjectDebugColourRGBA;

    ::g_pDebugSphereMesh->bIsVisible = true;
    ::g_pDebugSphereMesh->drawPosition = position;
    ::g_pDebugSphereMesh->setUniformDrawScale(scale);
    ::g_pDebugSphereMesh->bDoNotLight = true;
    ::g_pDebugSphereMesh->bUseDebugColours = true;
    ::g_pDebugSphereMesh->wholeObjectDebugColourRGBA = colourRGBA;

   
    DrawObject(::g_pDebugSphereMesh, glm::mat4(1.0f), ::g_DebugSphereMesh_shaderProgramID);

    ::g_pDebugSphereMesh->bIsVisible = OLD_isVisible;
    ::g_pDebugSphereMesh->drawPosition = OLD_position;
    ::g_pDebugSphereMesh->drawScale = OLD_scale;
    ::g_pDebugSphereMesh->wholeObjectDebugColourRGBA = OLD_colours;

    return;
}

// https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float getRandomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void LoadTheRobotronModels(GLuint shaderProgram)
{

    // Load the Robotron: 2084 models

    std::string oldBasePath = ::g_pMeshManager->getBasePath();

    ::g_pMeshManager->setBasePath("Robotron_2084");

    std::vector<std::string> vecRobotronModels;
    vecRobotronModels.push_back("player1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player2_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player3_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player4_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player5_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player6_xyz_rgba_n.ply");
    vecRobotronModels.push_back("player7_xyz_rgba_n.ply");

    vecRobotronModels.push_back("mommy1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("mommy2_xyz_rgba_n.ply");
    vecRobotronModels.push_back("mommy3_xyz_rgba_n.ply");
    //vecRobotronModels.push_back("mommy4.ply");
    //vecRobotronModels.push_back("mommy5.ply");

    vecRobotronModels.push_back("hulk1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("hulk2_xyz_rgba_n.ply");
    vecRobotronModels.push_back("hulk3_xyz_rgba_n.ply");

    vecRobotronModels.push_back("daddy1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("daddy2_xyz_rgba_n.ply");
    //vecRobotronModels.push_back("daddy3.ply");
    //vecRobotronModels.push_back("daddy4.ply");
    //vecRobotronModels.push_back("daddy5.ply");

    vecRobotronModels.push_back("grunt1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("grunt2_xyz_rgba_n.ply");
    vecRobotronModels.push_back("grunt3_xyz_rgba_n.ply");

    //vecRobotronModels.push_back("enforcer1.ply");
    //vecRobotronModels.push_back("enforcer2.ply");
    //vecRobotronModels.push_back("enforcer3.ply");
    //vecRobotronModels.push_back("enforcer4.ply");
    vecRobotronModels.push_back("enforcer5_xyz_rgba_n.ply");
    vecRobotronModels.push_back("enforcer6_xyz_rgba_n.ply");

    vecRobotronModels.push_back("brain1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("brain2_xyz_rgba_n.ply");
    //vecRobotronModels.push_back("brain3.ply");
    //vecRobotronModels.push_back("brain4.ply");
    //vecRobotronModels.push_back("brain5.ply");

    vecRobotronModels.push_back("tank1_xyz_rgba_n.ply");
    vecRobotronModels.push_back("tank2_xyz_rgba_n.ply");
    vecRobotronModels.push_back("tank3_xyz_rgba_n.ply");
    vecRobotronModels.push_back("tank4_xyz_rgba_n.ply");
 //   vecRobotronModels.push_back("tank5_xyz_rgba_n.ply");

    for (std::string modelName : vecRobotronModels)
    {
        ::g_pMeshManager->LoadModelIntoVAO(modelName, shaderProgram);
    }

    // Place them everywhere
    const float DRAW_LIMIT = 500.0f;
    unsigned int numberOfModels = (unsigned int)vecRobotronModels.size();
    for (unsigned int count = 0; count != 100; count++)
    {
        cMesh* pTempMesh = new cMesh();
        pTempMesh->drawPosition.x = getRandomFloat(-DRAW_LIMIT, DRAW_LIMIT);
        pTempMesh->drawPosition.y = getRandomFloat(0.0f, DRAW_LIMIT);
        pTempMesh->drawPosition.z = getRandomFloat(-DRAW_LIMIT, DRAW_LIMIT);
        pTempMesh->bDoNotLight = true;

        pTempMesh->meshName = vecRobotronModels[rand() % numberOfModels];

        ::g_vec_pMeshesToDraw.push_back(pTempMesh);
    }



    ::g_pMeshManager->setBasePath(oldBasePath);

    return;
}
