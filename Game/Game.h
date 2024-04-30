//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include <list>
#include "CommonStates.h"
#include "../DirectXTK/Inc/Effects.h" //this clashes with a SDK file so must explitily state it
#include "Keyboard.h"
#include "Mouse.h"
#include "Audio.h"
#include "CMOGO.h"
#include <string>
#include <iostream>
#include <math.h>
#include "Terrain.h"
#include "Projectile.h"
#include "Player.h"

using std::list;
using namespace std;

// Forward declarations
struct GameData;
struct DrawData;
struct DrawData2D;
class GameObject;
class GameObject2D;
class Camera;
class TPSCamera;
class FPSCamera;
class Light;
class Sound;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND _window, int _width, int _height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int _width, int _height);

    // Properties
    void GetDefaultSize( int& _width, int& _height ) const noexcept;

    //void FireProjectile();

    void Update(DX::StepTimer const& _timer);

    //bool m_start_hand_anim = false;

private:

    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    void Fire();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

    
    float m_hand_anim_timer = 0;
    float m_hand_anim_end_time = 1.5;
    int collision_count = 0;
    float platform_offset = 50.0f;

    float camera_bob_counter = 0;
    int camera_bob_multiplier = 1;
    Vector2 p_current_pos;
    Vector2 p_last_pos;

    //Scarle Added stuff
    GameData* m_GD = NULL;			//Data to be shared to all Game Objects as they are ticked
    DrawData* m_DD = NULL;			//Data to be shared to all 3D Game Objects as they are drawn
    DrawData2D* m_DD2D = NULL;	    //Data to be passed by game to all 2D Game Objects via Draw 

    //Basic 3D renderers
    std::shared_ptr<Player> pPlayer = NULL;
    std::shared_ptr<Camera> m_cam = NULL; //principle camera
    std::shared_ptr<TPSCamera> m_TPScam = NULL;//TPS cam
    std::shared_ptr<FPSCamera> m_FPScam = NULL;//FPS cam
    Light* m_light = NULL; //base light

    //required for the CMO model rendering system
    DirectX::CommonStates* m_states = NULL;
    DirectX::IEffectFactory* m_fxFactory = NULL;

    //basic keyboard and mouse input system
    void ReadInput(); //Get current Mouse and Keyboard states
    std::unique_ptr<DirectX::Keyboard> m_keyboard;
    std::unique_ptr<DirectX::Mouse> m_mouse;

    std::vector<std::shared_ptr<GameObject>> m_GameObjects; //data structure to hold pointers to the 3D Game Objects
    //list<GameObject2D*> m_GameObjects2D; //data structure to hold pointers to the 2D Game Objects 
    //std::vector<std::shared_ptr<GameObject>> m_GameObjects;
    std::vector<std::shared_ptr<GameObject2D>> m_GameObjects2D;

    std::vector<std::shared_ptr<CMOGO>> m_ColliderObjects;
    std::vector<std::shared_ptr<CMOGO>> m_PhysicsObjects;
    std::vector<std::shared_ptr<CMOGO>> m_ProjectileObjects;
    std::vector<std::shared_ptr<Terrain>> platforms;
    std::vector<std::shared_ptr<Projectile>> m_projectiles;

    int platform_count = 5;

    void CheckCollision();
    void CheckProjectileCollision();
    
    //sound stuff
	//This uses a simple system, but a better pipeline can be used using Wave Banks
	//See here: https://github.com/Microsoft/DirectXTK/wiki/Creating-and-playing-sounds Using wave banks Section
    std::unique_ptr<DirectX::AudioEngine> m_audioEngine;
    list<Sound*>m_Sounds;
};
