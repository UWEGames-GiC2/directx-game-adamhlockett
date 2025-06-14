//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <time.h>

//Scarle Headers
#include "GameData.h"
#include "GameState.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"
#include <iostream>

#include "CMOGO.h"
#include <DirectXCollision.h>
#include "Collision.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_11_0)
{
}

// Initialize the Direct3D resources required to run.
void Game::InitMenu(HWND _window, int _width, int _height) {
    m_window = _window;
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);
    CreateDevice();

    CreateResources();

    //seed the random number generator
    srand((UINT)time(NULL));

    //set up keyboard and mouse system
    //documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    m_keyboard = std::make_unique<Keyboard>();
    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(_window);
    m_mouse->SetMode(Mouse::MODE_RELATIVE);
    //Hide the mouse pointer
    ShowCursor(false);

    m_GD = new GameData;
    m_GD->m_GS = GS_PLAY_FPS_CAM;

    m_GD->m_GS = GS_MENU;

    m_DD2D = new DrawData2D();
    m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
    m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"..\\Assets\\italic.spritefont"));
    m_states = new CommonStates(m_d3dDevice.Get());

    //set up DirectXTK Effects system
    m_fxFactory = new EffectFactory(m_d3dDevice.Get());
    //Tell the fxFactory to look to the correct build directory to pull stuff in from
    ((EffectFactory*)m_fxFactory)->SetDirectory(L"..\\Assets");
    //init render system for VBGOs
    VBGO::Init(m_d3dDevice.Get());

    //set audio system
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif
    m_audioEngine = std::make_unique<AudioEngine>(eflags);

    float AR = (float)_width / (float)_height;

    m_cam = new Camera(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
    m_cam->SetPos(Vector3(0.0f, 200.0f, 200.0f));
    m_MenuObjects.push_back(m_cam);

    m_DD = new DrawData;
    m_DD->m_pd3dImmediateContext = nullptr;
    m_DD->m_states = m_states;
    m_DD->m_cam = m_cam;
    m_DD->m_light = m_light;

}

void Game::Initialize(HWND _window, int _width, int _height)
{
    //m_window = _window;
    //m_outputWidth = std::max(_width, 1);
    //m_outputHeight = std::max(_height, 1);
    //
    //CreateDevice();
    //
    //CreateResources();
    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);

    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
    ////seed the random number generator
    //srand((UINT)time(NULL));
    //
    ////set up keyboard and mouse system
    ////documentation here: https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input
    //m_keyboard = std::make_unique<Keyboard>();
    //m_mouse = std::make_unique<Mouse>();
    //m_mouse->SetWindow(_window);
    //m_mouse->SetMode(Mouse::MODE_RELATIVE);
    ////Hide the mouse pointer
    //ShowCursor(false);
    //create GameData struct and populate its pointers
    //m_GD = new GameData;
    //
    //
    //m_GD->m_GS = GS_MENU;
    //when actual game starts
    //m_GD->m_GS = GS_PLAY_FPS_CAM;
    //set up systems for 2D rendering
//    m_DD2D = new DrawData2D();
//    m_DD2D->m_Sprites.reset(new SpriteBatch(m_d3dContext.Get()));
//    m_DD2D->m_Font.reset(new SpriteFont(m_d3dDevice.Get(), L"..\\Assets\\italic.spritefont"));
//    m_states = new CommonStates(m_d3dDevice.Get());
//
//    //set up DirectXTK Effects system
//    m_fxFactory = new EffectFactory(m_d3dDevice.Get());
//    //Tell the fxFactory to look to the correct build directory to pull stuff in from
//    ((EffectFactory*)m_fxFactory)->SetDirectory(L"..\\Assets");
//    //init render system for VBGOs
//    VBGO::Init(m_d3dDevice.Get());
//
//    //set audio system
//    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
//#ifdef _DEBUG
//    eflags = eflags | AudioEngine_Debug;
//#endif
//    m_audioEngine = std::make_unique<AudioEngine>(eflags);
    //create a set of dummy things to show off the engine

    for (size_t i = 0; i < 10; i++) {
        std::shared_ptr<Projectile> proj = std::make_shared<Projectile>("capsule", m_d3dDevice.Get(), m_fxFactory);
        proj->SetActive(false);
        m_GameObjects.push_back(proj);
        m_ProjectileObjects.push_back(proj);
    }

    pPlayer = std::make_shared<Player>("steve", m_d3dDevice.Get(), m_fxFactory);
    //pPlayer->m_name = "player";
    //pPlayer->isRendered = false;
    m_GameObjects.push_back(pPlayer);
    m_PhysicsObjects.push_back(pPlayer);
    pPlayer.get()->projectiles = m_ProjectileObjects;

    //create a base light
    std::shared_ptr<Light> m_light = std::make_shared<Light>(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
    m_GameObjects.push_back(m_light);


    //find how big my window is to correctly calculate my aspect ratio
    //float AR = (float)_width / (float)_height;

    //example basic 3D stuff
    //std::shared_ptr<Terrain> platform = std::make_shared<Terrain>("floor_4x4_free", m_d3dDevice.Get(), m_fxFactory, Vector3(0.0f, -10.0f, 0.0f), 0.0f, 0.0f, 0.0f, Vector3(2.0f,20.0f,2.0f));
    //m_GameObjects.push_back(platform);
    //m_ColliderObjects.push_back(platform);
    //std::shared_ptr<Terrain> platform = std::make_shared<Terrain>("round platform", m_d3dDevice.Get(), m_fxFactory, Vector3(0.0f, -10.0f, 0.0f), 0.0f, 0.0f, 0.0f, Vector3(10.0f, 20.0f, 10.0f));
    //m_GameObjects.push_back(platform);
    //m_ColliderObjects.push_back(platform);
    //for (int i = 0; i < platform_count; i++) {
    //    std::shared_ptr<Terrain> p = std::make_shared<Terrain>("round platform", m_d3dDevice.Get(), m_fxFactory, Vector3(i * (platform_offset) + 100.0f, -100.0f, i * (platform_offset) + 100.0f), 0.0f, 0.0f, 0.0f, Vector3(10.0f, 10.0f, 10.0f));
    //    //platforms.push_back(p);
    //    m_GameObjects.push_back(p);
    //    m_ColliderObjects.push_back(p);
    //}
    //for (std::shared_ptr<Terrain> plat : platforms) {
    //}
    ////L-system like tree
    //m_GameObjects.push_back(new Tree(4, 4, .6f, 10.0f * Vector3::Up, XM_PI / 6.0f, "JEMINA vase -up", m_d3dDevice.Get(), m_fxFactory));
    ////Vertex Buffer Game Objects
    //FileVBGO* terrainBox = new FileVBGO("terrainTex", m_d3dDevice.Get());
    //m_GameObjects.push_back(terrainBox);
    //std::shared_ptr<FileVBGO> box = std::make_shared<FileVBGO>("cube", m_d3dDevice.Get());
    //box->SetPos(Vector3(0.0f, 0.0f, -100.0f));
    //box->SetPitch(XM_PIDIV4);
    //box->SetScale(20.0f);
    //m_GameObjects.push_back(box);
    //m_ColliderObjects.push_back(box);
    //std::shared_ptr<VBCube> cube = std::make_shared<VBCube>();
    //cube->init(11, m_d3dDevice.Get());
    //cube->SetPos(Vector3(100.0f, 0.0f, 0.0f));
    //cube->SetScale(4.0f);
    //m_GameObjects.push_back(cube);
    //m_ColliderObjects.push_back(cube);
    //VBSpike* spikes = new VBSpike();
    //spikes->init(11, m_d3dDevice.Get());
    //spikes->SetPos(Vector3(0.0f, 0.0f, 100.0f));
    //spikes->SetScale(4.0f);
    //m_GameObjects.push_back(spikes);
    //VBSpiral* spiral = new VBSpiral();
    //spiral->init(11, m_d3dDevice.Get());
    //spiral->SetPos(Vector3(-100.0f, 0.0f, 0.0f));
    //spiral->SetScale(4.0f);
    //m_GameObjects.push_back(spiral);
    //VBPillow* pillow = new VBPillow();
    //pillow->init(11, m_d3dDevice.Get());
    //pillow->SetPos(Vector3(-100.0f, 0.0f, -100.0f));
    //pillow->SetScale(4.0f);
    //m_GameObjects.push_back(pillow);
    //VBSnail* snail = new VBSnail(m_d3dDevice.Get(), "shell", 150, 0.98f, 0.09f * XM_PI, 0.4f, Color(1.0f, 0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f));
    //snail->SetPos(Vector3(-100.0f, 0.0f, 100.0f));
    //snail->SetScale(2.0f);
    //m_GameObjects.push_back(snail);*/
    ////Marching Cubes
    //VBMarchCubes* VBMC = new VBMarchCubes();
    //VBMC->init(Vector3(-8.0f, -8.0f, -17.0f), Vector3(8.0f, 8.0f, 23.0f), 60.0f * Vector3::One, 0.01, m_d3dDevice.Get());
    //VBMC->SetPos(Vector3(100, 0, -100));
    //VBMC->SetPitch(-XM_PIDIV2);
    //VBMC->SetScale(Vector3(3, 3, 1.5));
    //m_GameObjects.push_back(VBMC);*/

    //create a base camera
    m_cam = std::make_shared<Camera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
    m_cam->SetPos(Vector3(0.0f, 200.0f, 200.0f));
    m_GameObjects.push_back(m_cam);

    //add a secondary camera
    m_TPScam = std::make_shared<TPSCamera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 100.0f, 50.0f));
    m_GameObjects.push_back(m_TPScam);

    //add first person camera
    m_FPScam = std::make_shared<FPSCamera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 0.0f, 0.1f));
    m_GameObjects.push_back(m_FPScam);

    //test all GPGOs
    /*float* params = new float[3];
    params[0] = 10.f;  params[1] = 20.0f; params[2] = 30.f;
    GPGO* pGPGO = new GPGO(m_d3dContext.Get(), GPGO_BOX, (float*)&Colors::Azure, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, -100.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = params[1] = 20.0f; params[2] = (size_t)32;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_CONE, (float*)&Colors::Navy,params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, -70.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 15.0f;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_CUBE, (float*)&Colors::SeaGreen, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, -40.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = params[1] = 20.0f; params[2] = (size_t)32;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_CYLINDER, (float*)&Colors::OliveDrab, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, -10.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 15.0f;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_DODECAHEDRON, (float*)&Colors::OrangeRed,params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 20.f));
    m_GameObjects.push_back(pGPGO);
    params[0] =  15.0f; params[1] = (size_t)3;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_GEOSPHERE, (float*)&Colors::BlueViolet, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 50.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 20;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_ICOSAHEDRON, (float*)&Colors::DodgerBlue, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 80.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 20;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_OCTAHEDRON, (float*)&Colors::PaleTurquoise, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 110.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 15.0f; params[1] = (size_t)16;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_SPHERE, (float*)&Colors::LawnGreen, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 140.0));
    m_GameObjects.push_back(pGPGO);
    params[0] = 15.0f; params[1] = (size_t)8;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_TEAPOT, (float*)&Colors::YellowGreen, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 170.0f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 20;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_TETRAHEDRON, (float*)&Colors::Firebrick, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 200.f));
    m_GameObjects.push_back(pGPGO);
    params[0] = 30.0f; params[1] = 10.0f; params[2] = (size_t)32;
    pGPGO = new GPGO(m_d3dContext.Get(), GPGO_TORUS, (float*)&Colors::Aquamarine, params);
    pGPGO->SetPos(Vector3(-50.0f, 10.0f, 230.f));
    m_GameObjects.push_back(pGPGO);*/

    //create DrawData struct and populate its pointers
    m_DD = new DrawData;
    m_DD->m_pd3dImmediateContext = nullptr;
    m_DD->m_states = m_states;
    m_DD->m_cam = m_cam.get();
    m_DD->m_light = m_light.get();

    //example basic 2D stuff
    std::shared_ptr<ImageGO2D> hand = std::make_shared<ImageGO2D>("hand", m_d3dDevice.Get());
    Vector2 hand_image_origin(875, 875);
    hand->SetOrigin(hand_image_origin);
    hand->SetScale(Vector2::One);
    hand->SetPos(/*200.0f * Vector2::One*/Vector2(_width, _height));
    hand->m_name = "hand";
    m_GameObjects2D.push_back(hand);

    timer = std::make_shared<TextGO2D>("60.0s");
    timer->SetActive(false);
    //timer->SetPos(Vector2(20, 20));
    //timer->SetColour(Color((float*)&Colors::Tomato));
    m_GameObjects2D.push_back(timer);

    GeneratePlatformsRegular(Vector3(10.0f,-10.0f,10.0f), 100.0f, 4);
    GeneratePlatformsRandom(Vector3(100.0f, -20.0f, 250.0f), 10);

    pPlayer.get()->SetYaw(-27000);

    help1 = std::make_shared<TextGO2D>("WASD TO MOVE, LEFT MOUSE TO SHOOT");
    m_GameObjects2D.push_back(help1);
    help_vec.push_back(help1);

    help2 = std::make_shared<TextGO2D>("SPACE TO JUMP OR DOUBLE JUMP");
    help2->SetPos(Vector2(0.0f, 50.0f));
    m_GameObjects2D.push_back(help2);
    help_vec.push_back(help2);

    help3 = std::make_shared<TextGO2D>("R TO CHANGE CAMERA");
    help3->SetPos(Vector2(0.0f, 100.0f));
    m_GameObjects2D.push_back(help3);
    help_vec.push_back(help3);

    help4 = std::make_shared<TextGO2D>("shoot the targets for points");
    help4->SetPos(Vector2(0.0f, 150.0f));
    m_GameObjects2D.push_back(help4);
    help_vec.push_back(help4);

    help5 = std::make_shared<TextGO2D>("see how many points you can get in a minute");
    help5->SetPos(Vector2(0.0f, 200.0f));
    m_GameObjects2D.push_back(help5);
    help_vec.push_back(help5);

    help6 = std::make_shared<TextGO2D>("reach the end of the platforms to generate more");
    help6->SetPos(Vector2(0.0f, 250.0f));
    m_GameObjects2D.push_back(help6);
    help_vec.push_back(help6);

    help7 = std::make_shared<TextGO2D>("ENTER TO PLAY");
    help7->SetPos(Vector2(0.0f, 300.0f));
    m_GameObjects2D.push_back(help7);
    help_vec.push_back(help7);

    end1 = std::make_shared<TextGO2D>("TIMES UP, YOUR SCORE WAS:");
    end1->SetActive(false);
    m_GameObjects2D.push_back(end1);
    end_vec.push_back(end1);

    end2 = std::make_shared<TextGO2D>("00");
    end2->SetActive(false);
    end2->SetPos(Vector2(0.0f, 50.0f));
    m_GameObjects2D.push_back(end2);
    end_vec.push_back(end2);

    end3 = std::make_shared<TextGO2D>("ENTER TO PLAY AGAIN");
    end3->SetActive(false);
    end3->SetPos(Vector2(0.0f, 100.0f));
    m_GameObjects2D.push_back(end3);
    end_vec.push_back(end3);

    //ImageGO2D* bug_test = new ImageGO2D("bug_test", m_d3dDevice.Get());
    //bug_test->SetPos(300.0f * Vector2::One);
    //m_GameObjects2D.push_back(bug_test);
    /*TextGO2D* text = new TextGO2D("Test Text");
    text->SetPos(Vector2(100, 10));
    text->SetColour(Color((float*)&Colors::Yellow));
    m_GameObjects2D.push_back(text);*/
    //Test Sounds
    //Loop* loop = new Loop(m_audioEngine.get(), "NightAmbienceSimple_02");
    //loop->SetVolume(0.1f);
    //loop->Play();
    //m_Sounds.push_back(loop);
    //
    //TestSound* TS = new TestSound(m_audioEngine.get(), "Explo1");
    //m_Sounds.push_back(TS);

}

void Game::ReInitialize()
{
    //create a set of dummy things to show off the engine

    for (size_t i = 0; i < 10; i++) {
        std::shared_ptr<Projectile> proj = std::make_shared<Projectile>("capsule", m_d3dDevice.Get(), m_fxFactory);
        proj->SetActive(false);
        m_GameObjects.push_back(proj);
        m_ProjectileObjects.push_back(proj);
    }

    pPlayer = std::make_shared<Player>("steve", m_d3dDevice.Get(), m_fxFactory);
    //pPlayer->m_name = "player";
    //pPlayer->isRendered = false;
    m_GameObjects.push_back(pPlayer);
    m_PhysicsObjects.push_back(pPlayer);
    pPlayer.get()->projectiles = m_ProjectileObjects;

    //create a base light
    std::shared_ptr<Light> m_light = std::make_shared<Light>(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
    m_GameObjects.push_back(m_light);

    //find how big my window is to correctly calculate my aspect ratio
    float AR = (float)m_outputWidth / (float)m_outputHeight;
    //create a base camera
    m_cam = std::make_shared<Camera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, Vector3::UnitY, Vector3::Zero);
    m_cam->SetPos(Vector3(0.0f, 200.0f, 200.0f));
    m_GameObjects.push_back(m_cam);

    //add a secondary camera
    m_TPScam = std::make_shared<TPSCamera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 100.0f, 50.0f));
    m_GameObjects.push_back(m_TPScam);

    //add first person camera
    m_FPScam = std::make_shared<FPSCamera>(0.25f * XM_PI, AR, 1.0f, 10000.0f, pPlayer, Vector3::UnitY, Vector3(0.0f, 0.0f, 0.1f));
    m_GameObjects.push_back(m_FPScam);


    //example basic 2D stuff
    std::shared_ptr<ImageGO2D> hand = std::make_shared<ImageGO2D>("hand", m_d3dDevice.Get());
    Vector2 hand_image_origin(875, 875);
    hand->SetOrigin(hand_image_origin);
    hand->SetScale(Vector2::One);
    hand->SetPos(/*200.0f * Vector2::One*/Vector2(m_outputWidth, m_outputHeight));
    hand->m_name = "hand";
    m_GameObjects2D.push_back(hand);

    timer = std::make_shared<TextGO2D>("60.0s");
    timer->SetActive(false);
    m_GameObjects2D.push_back(timer);

    GeneratePlatformsRegular(Vector3(10.0f, -10.0f, 10.0f), 100.0f, 4);
    GeneratePlatformsRandom(Vector3(100.0f, -20.0f, 250.0f), 10);

    pPlayer.get()->SetYaw(-27000);

    help1 = std::make_shared<TextGO2D>("WASD TO MOVE, LEFT MOUSE TO SHOOT");
    m_GameObjects2D.push_back(help1);
    help_vec.push_back(help1);

    help2 = std::make_shared<TextGO2D>("SPACE TO JUMP OR DOUBLE JUMP");
    help2->SetPos(Vector2(0.0f, 50.0f));
    m_GameObjects2D.push_back(help2);
    help_vec.push_back(help2);

    help3 = std::make_shared<TextGO2D>("R TO CHANGE CAMERA");
    help3->SetPos(Vector2(0.0f, 100.0f));
    m_GameObjects2D.push_back(help3);
    help_vec.push_back(help3);

    help4 = std::make_shared<TextGO2D>("shoot the targets for points");
    help4->SetPos(Vector2(0.0f, 150.0f));
    m_GameObjects2D.push_back(help4);
    help_vec.push_back(help4);

    help5 = std::make_shared<TextGO2D>("see how many points you can get in a minute");
    help5->SetPos(Vector2(0.0f, 200.0f));
    m_GameObjects2D.push_back(help5);
    help_vec.push_back(help5);

    help6 = std::make_shared<TextGO2D>("reach the end of the platforms to generate more");
    help6->SetPos(Vector2(0.0f, 250.0f));
    m_GameObjects2D.push_back(help6);
    help_vec.push_back(help6);

    help7 = std::make_shared<TextGO2D>("ENTER TO PLAY");
    help7->SetPos(Vector2(0.0f, 300.0f));
    m_GameObjects2D.push_back(help7);
    help_vec.push_back(help7);

    end1 = std::make_shared<TextGO2D>("TIMES UP, YOUR SCORE WAS:");
    end1->SetActive(false);
    m_GameObjects2D.push_back(end1);
    end_vec.push_back(end1);

    end2 = std::make_shared<TextGO2D>("00");
    end2->SetActive(false);
    end2->SetPos(Vector2(0.0f, 50.0f));
    m_GameObjects2D.push_back(end2);
    end_vec.push_back(end2);

    end3 = std::make_shared<TextGO2D>("ENTER TO PLAY AGAIN");
    end3->SetActive(false);
    end3->SetPos(Vector2(0.0f, 100.0f));
    m_GameObjects2D.push_back(end3);
    end_vec.push_back(end3);
}

//void Game::FireProjectile() 
//{
//    // spawn projectile
//    // cause an update that moves it
//    // do hand animation
//    //std::cout << std::to_string(m_start_hand_anim) + "\n";
//    //std::cout << "fire";
//    m_GD->m_hand_anim = true;
//}

void Game::Fire()
{
    if (m_GD->m_hand_anim == true) {
        m_hand_anim_timer = m_hand_anim_timer + (1 * m_GD->m_dt);
        float calculated_rot = sin(M_PI * (m_hand_anim_timer / m_hand_anim_end_time));
        for (auto g : m_GameObjects2D) {
            if (g.get()->m_name == "hand") {
                g.get()->SetRot(calculated_rot);
            }
        }

        if (m_hand_anim_timer >= m_hand_anim_end_time) {
            m_GD->m_hand_anim = false;
            m_hand_anim_timer = 0;
        }
    }
}

void Game::GeneratePlatformsRegular(Vector3 start_pos, float grid_offset, int p_count)
{
    std::cout << "generate platforms\n";
    int dimension = int(sqrt(p_count));
    //std::cout << std::to_string(dimension) << std::endl;

    for (int i = 0; i < dimension; i++) 
    {
        for (int j = 0; j < dimension; j++) 
        {
            float pos_x = start_pos.x + (grid_offset * i);
            float pos_y = start_pos.y;
            float pos_z = start_pos.z + (grid_offset * j);
            Vector3 pos(pos_x, pos_y, pos_z);
            //std::cout << std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z) + "\n";
            std::shared_ptr<Terrain> platform = std::make_shared<Terrain>("floor_4x4_free", m_d3dDevice.Get(), m_fxFactory, pos, 0.0f, 0.0f, 0.0f, Vector3(4.5f, 20.0f, 4.5f));
            m_GameObjects.push_back(platform);
            m_ColliderObjects.push_back(platform);
        }
    }
}

void Game::GeneratePlatformsRandom(Vector3 start_pos, int platform_count)
{
    //if (platforms.size() > 12) {
    //    for (int i = 0; i < platforms.size() - 12; i++) {
    //        platforms.erase(platforms.begin() + i);
    //    }
    //}

    std::cout << "generate random platforms\n";
    for (int i = 0; i < platform_count; i++) {
        // make random offset
        int offset_x = -50 + (rand() % 101);
        int offset_z = (i * 50) + (-25 + (rand() % 25));
        float pos_x = start_pos.x + offset_x;
        float pos_y = start_pos.y;
        float pos_z = start_pos.z + offset_z;
        Vector3 pos(pos_x, pos_y, pos_z);
        std::shared_ptr<Terrain> platform = std::make_shared<Terrain>("round platform", m_d3dDevice.Get(), m_fxFactory, pos, 0.0f, 0.0f, 0.0f, Vector3(10.0f, 10.0f, 10.0f));
        m_GameObjects.push_back(platform);
        m_ColliderObjects.push_back(platform);
        platforms.push_back(platform);
        end_platform = platform;
    }
    // spawn enemy on end_platform
    float pos_x = end_platform.get()->GetPos().x;
    float pos_y = end_platform.get()->GetPos().y + 50.0f;
    float pos_z = end_platform.get()->GetPos().z + 200.0f;
    Vector3 pos(pos_x, pos_y, pos_z);
    std::shared_ptr<Terrain> target = std::make_shared<Terrain>("Archery Target", m_d3dDevice.Get(), m_fxFactory, pos, 0.0f, 9.5f, 0.0f, Vector3(0.06f, 0.06f, 1.0f));
    //target->SetYaw(20);
    target->isTarget = true;
    m_GameObjects.push_back(target);
    m_ColliderObjects.push_back(target);
}

// Executes the basic game loop.
void Game::Tick()
{
    if (m_GD->m_GS == GS_MENU) {
        m_timer.Tick([&]() 
        {
            MenuUpdate(m_timer);
        });
    }
    else if (m_GD->m_GS == GS_PLAY_MAIN_CAM || 
        m_GD->m_GS == GS_PLAY_FPS_CAM || 
        m_GD->m_GS == GS_PLAY_TPS_CAM)
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

void Game::MenuUpdate(DX::StepTimer const& _timer) 
{
    float elapsedTime = float(_timer.GetElapsedSeconds());
    m_GD->m_dt = elapsedTime;

    if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            // We lost the audio device!
        }
    }
    else
    {
        //update sounds playing
        for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
        {
            (*it)->Tick(m_GD);
        }
    }

    ReadInput();

    m_GD->cursorShowing = true;

    if (m_GD->cursorShowing) ShowCursor(true);
    else ShowCursor(false);

    for (list<GameObject*>::iterator it = m_MenuObjects.begin(); it != m_MenuObjects.end(); it++)
    {
        (*it)->Tick(m_GD);
    }

    for (list<GameObject2D*>::iterator it = m_MenuObjects2D.begin(); it != m_MenuObjects2D.end(); it++)
    {
        (*it)->Tick(m_GD);
    }
}

// Updates the world.
void Game::Update(DX::StepTimer const& _timer)
{
    if (first_frame) {
        m_GD->m_GS = GS_MAIN_MENU;
        first_frame = false;
    }

    //p_current_pos = { pPlayer->GetPos().x , pPlayer.get()->GetPos().y };
    //
    //if (int(p_current_pos.x) != int(p_last_pos.x) || int(p_current_pos.y) != int(p_last_pos.y)) {
    //    //bob camera
    //    camera_bob_counter = camera_bob_counter + m_GD->m_dt;
        m_FPScam.get()->SetDPos(Vector3(0.0f, (sin(camera_bob_counter))/500, 0.1f));
        m_FPScam.get()->SetTargetDisplacement((sin(camera_bob_counter) / 500));
    //    std::cout << std::to_string(m_FPScam.get()->GetPos().y) + "\n";
    //}
    //
    //p_last_pos = { pPlayer.get()->GetPos().x , pPlayer.get()->GetPos().y };
    


    //this will update the audio engine but give us chance to do somehting else if that isn't working
    if (!m_audioEngine->Update())
    {
        if (m_audioEngine->IsCriticalError())
        {
            // We lost the audio device!
        }
    }
    else
    {
        //update sounds playing
        for (list<Sound*>::iterator it = m_Sounds.begin(); it != m_Sounds.end(); it++)
        {
            (*it)->Tick(m_GD);
        }
    }

    ReadInput();
    for (vector<std::shared_ptr<GameObject>>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        (*it)->Tick(m_GD);
    }
    for (vector<std::shared_ptr<GameObject2D>>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
    {
        //OutputDebugStringW(std::to_string((*it)->m_pos.x) + " " + std::to_string((*it)->m_pos.y));
        //std::cout << (std::to_string((*it)->m_origin.x) + " " + std::to_string((*it)->m_origin.y));
        (*it)->Tick(m_GD);
    }



    if (m_GD->m_GS == GS_MAIN_MENU) {
        //set text is active
        for (auto text : help_vec) {
            text.get()->SetActive(true);
        }
        points = 0;
        countdown = 60.0f;
        for (auto text : end_vec) {
            text.get()->SetActive(false);
        }
        pPlayer.get()->SetPos(Vector3(0.0f, 0.0f, 0.0f));
    }

    if (m_GD->m_GS == GS_PLAY_FPS_CAM) {
        //set cover image to not active
        //set text is not active
        timer->SetActive(true);
        for (auto text : help_vec) {
            text.get()->SetActive(false);
        }
    }
    if (m_GD->m_GS == GS_GAME_OVER) {
        timer->SetActive(false);
        end2.get()->SetText(std::to_string(points));
        for (auto text : end_vec) {
            text.get()->SetActive(true);
        }
        pPlayer.get()->SetPos(Vector3(0.0f, 0.0f, 0.0f));
    }

    //std::cout << std::to_string(points) << std::endl;
    //std::cout << std::to_string(int(pPlayer.get()->GetPos().x)) + " " + std::to_string(int(pPlayer.get()->GetPos().z)) + "\n";
    float elapsedTime = float(_timer.GetElapsedSeconds());
    m_GD->m_dt = elapsedTime;
    if(m_GD->m_GS != GS_MAIN_MENU) countdown -= elapsedTime;
    //std::cout << std::to_string(countdown) << std::endl;
    string countdown_string = std::to_string(int(countdown)) + "s";
    timer.get()->SetText(countdown_string);

    if (int(countdown) <= 0) {
        //transition to end screen
        m_GD->m_GS = GS_GAME_OVER;
    }


    //hand animation when firing
    Fire();
    //upon space bar switch camera state
    //see docs here for what's going on: https://github.com/Microsoft/DirectXTK/wiki/Keyboard
    if (m_GD->m_KBS_tracker.pressed.R)
    {
        if (m_GD->m_GS == GS_PLAY_TPS_CAM)
        {
            m_GD->m_GS = GS_PLAY_FPS_CAM;
        }
        else if (m_GD->m_GS == GS_PLAY_FPS_CAM)
        {
            m_GD->m_GS = GS_PLAY_TPS_CAM;
        }
    }
    if (m_GD->cursorShowing) ShowCursor(true);
    else ShowCursor(false);

    //update all objects

    //respawn
    if (pPlayer.get()->GetPos().y < -200.0f) {
        pPlayer.get()->SetPos(Vector3(0.0f, 0.0f, 0.0f));
    }

    CheckCollision();
    CheckProjectileCollision();


}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();
    
    //set immediate context of the graphics device
    m_DD->m_pd3dImmediateContext = m_d3dContext.Get();

    //set which camera to be used
    //m_DD->m_cam = m_cam;
    if (m_GD->m_GS == GS_PLAY_FPS_CAM)
    {
        m_DD->m_cam = m_FPScam.get();
    }
    if (m_GD->m_GS == GS_PLAY_TPS_CAM)
    {
        m_DD->m_cam = m_TPScam.get();
    }

    //update the constant buffer for the rendering of VBGOs
    VBGO::UpdateConstantBuffer(m_DD);

    //Draw 3D Game Obejects
    for (std::vector<std::shared_ptr<GameObject>>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
    {
        if ((*it)->IsActive())
        {
            (*it).get()->Draw(m_DD);
        }
    }

    // Draw sprite batch stuff 
    m_DD2D->m_Sprites->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());
    for (std::vector<std::shared_ptr<GameObject2D>>::iterator it = m_GameObjects2D.begin(); it != m_GameObjects2D.end(); it++)
    {
        if ((*it)->IsActive()) {
            (*it)->Draw(m_DD2D);
        }
    }
    m_DD2D->m_Sprites->End();

    //drawing text screws up the Depth Stencil State, this puts it back again!
    m_d3dContext->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int _width, int _height)
{
    m_outputWidth = std::max(_width, 1);
    m_outputHeight = std::max(_height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& _width, int& _height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    _width = 1280;
    _height = 720;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    //creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    //something missing on the machines in 2Q28
    //this should work!
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}

void Game::ReadInput()
{
    m_GD->m_KBS = m_keyboard->GetState();
    m_GD->m_KBS_tracker.Update(m_GD->m_KBS);
    //quit game on hiting escape
    if (m_GD->m_KBS.Escape)
    {
        ExitGame();
    }
    if (m_GD->m_GS == GS_MAIN_MENU && m_GD->m_KBS.Enter)
    {
        m_GD->m_GS = GS_PLAY_FPS_CAM;
    }
    if (m_GD->m_GS == GS_GAME_OVER && m_GD->m_KBS.Enter) {
        m_ColliderObjects.clear();
        m_PhysicsObjects.clear();
        m_ProjectileObjects.clear();
        platforms.clear();
        m_projectiles.clear();
        m_GameObjects.clear();
        m_GameObjects2D.clear();
        help_vec.clear();
        end_vec.clear();
        ReInitialize();
        m_GD->m_GS = GS_MAIN_MENU;
    }

    m_GD->m_MS = m_mouse->GetState();

    //lock the cursor to the centre of the window
    RECT window;
    GetWindowRect(m_window, &window);
    if (!m_GD->cursorShowing) {
        SetCursorPos((window.left + window.right) >> 1, (window.bottom + window.top) >> 1);
    }
}

void Game::CheckCollision()
{
    //if (m_ColliderObjects.size() > 25) {
    //    for (auto c : m_ColliderObjects) {
    //        if (c == ) {
    //
    //        }
    //    }
    //}

    collision_count = 0;
    for (int i = 0; i < m_PhysicsObjects.size(); i++) for (int j = 0; j < m_ColliderObjects.size(); j++)
    {
        if (m_PhysicsObjects[i]->Intersects(*m_ColliderObjects[j])) //std::cout << "Collision Detected!" << std::endl;
        {
            XMFLOAT3 eject_vect = Collision::ejectionCMOGO(*m_PhysicsObjects[i], *m_ColliderObjects[j]);
            auto pos = m_PhysicsObjects[i]->GetPos();
            m_PhysicsObjects[i]->SetPos(pos - eject_vect);
            if(eject_vect.y < 0) collision_count++;
            if (m_ColliderObjects[j] == end_platform) {
                GeneratePlatformsRandom(Vector3(end_platform.get()->GetPos()), 10);
            }
        }
    }
    if (collision_count > 0) {
        m_GD->gravity_on = false;
        m_GD->m_can_jump = true;
    }
    else {
        m_GD->gravity_on = true;
    }
    //std::cout << std::to_string(collision_count) + "\n";
}

void Game::CheckProjectileCollision() 
{
    for (int i = 0; i < m_ProjectileObjects.size();i++) for (int j = 0; j < m_ColliderObjects.size(); j++) 
    {
        if (m_ProjectileObjects[i]->IsActive() && m_ProjectileObjects[i]->Intersects(*m_ColliderObjects[j])) 
        {
            std::cout << "projectile collision" << std::endl;
            m_ProjectileObjects[i]->SetActive(false);
            if (m_ColliderObjects[j].get()->isTarget) {
                m_ColliderObjects[j].get()->SetActive(false);
                points++;
            }
        }
    }
}