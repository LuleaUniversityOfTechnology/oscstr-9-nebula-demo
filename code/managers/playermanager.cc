//------------------------------------------------------------------------------
//  playermanager.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "application/stdneb.h"
#include "playermanager.h"
#include "input/inputserver.h"
#include "graphics/cameracontext.h"
#include "visibility/visibilitycontext.h"
#include "imgui.h"
#include "dynui/im3d/im3dcontext.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "basegamefeature/managers/entitymanager.h"
#include "input/mouse.h"
#include "renderutil/mouserayutil.h"
#include "game/api.h"
#include "properties/input.h"

namespace Demo
{

__ImplementSingleton(PlayerManager)

//------------------------------------------------------------------------------
/**
*/
Game::ManagerAPI
PlayerManager::Create()
{
    n_assert(!PlayerManager::HasInstance());
    Singleton = n_new(PlayerManager);

    Game::ManagerAPI api;
    api.OnActivate = &PlayerManager::OnActivate;
    api.OnBeginFrame = &PlayerManager::OnBeginFrame;
    api.OnFrame = &PlayerManager::OnFrame;
    return api;
}

//------------------------------------------------------------------------------
/**
*/
void
PlayerManager::Destroy()
{
    n_assert(PlayerManager::HasInstance());
    n_delete(Singleton);
}

//------------------------------------------------------------------------------
/**
*/

void
PlayerManager::OnActivate()
{
    auto view = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    auto stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();

    auto const windowId = Base::DisplayDeviceBase::Instance()->GetMainWindow();
    auto const displayMode = CoreGraphics::WindowGetDisplayMode(windowId);
    SizeT width = displayMode.GetWidth();
    SizeT height = displayMode.GetHeight();

    Game::EntityCreateInfo playerCreateInfo;
    playerCreateInfo.templateId = Game::GetTemplateId("Player/player"_atm);
    playerCreateInfo.immediate = true;
    Singleton->playerEntity = Game::CreateEntity(playerCreateInfo);

    GraphicsFeature::Camera camera = Game::GetProperty<GraphicsFeature::Camera>(Singleton->playerEntity, Game::GetPropertyId("Camera"_atm));
    Demo::TopdownCamera topdownCamera = Game::GetProperty< Demo::TopdownCamera>(Singleton->playerEntity, Game::GetPropertyId("TopdownCamera"_atm));
    camera.aspectRatio = (float)width / (float)height;
    camera.viewHandle = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultViewHandle();
    Game::SetProperty<GraphicsFeature::Camera>(Singleton->playerEntity, Game::GetPropertyId("Camera"_atm), camera);

    Singleton->topdownCamUtil.Setup(topdownCamera.height,topdownCamera.pitch, topdownCamera.yaw);
}

//------------------------------------------------------------------------------
/**
*/
void
PlayerManager::OnBeginFrame()
{
    auto& io = ImGui::GetIO();
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        Singleton->topdownCamUtil.SetForwardsKey(io.KeysDown[Input::Key::W]);
        Singleton->topdownCamUtil.SetBackwardsKey(io.KeysDown[Input::Key::S]);
        Singleton->topdownCamUtil.SetRightStrafeKey(io.KeysDown[Input::Key::D]);
        Singleton->topdownCamUtil.SetLeftStrafeKey(io.KeysDown[Input::Key::A]);
        Singleton->topdownCamUtil.SetUpKey(io.KeysDown[Input::Key::Q]);
        Singleton->topdownCamUtil.SetDownKey(io.KeysDown[Input::Key::E]);
        Singleton->topdownCamUtil.SetMouseMovement({ -io.MouseDelta.x, -io.MouseDelta.y });
        Singleton->topdownCamUtil.SetAccelerateButton(io.KeyShift);
        Singleton->topdownCamUtil.SetRotateButton(io.MouseDown[Input::MouseButton::RightButton]);
        Singleton->topdownCamUtil.SetMovementSpeed(0.1f);
        Singleton->topdownCamUtil.Update();
    }

    //Math::mat4 worldTransform = Game::GetProperty(Singleton->playerEntity, Game::GetPropertyId("WorldTransform"_atm));
    Game::SetProperty<Math::mat4>(Singleton->playerEntity, Game::GetPropertyId("WorldTransform"_atm), Singleton->topdownCamUtil.GetTransform());
}

//------------------------------------------------------------------------------
/**
*/
void
PlayerManager::OnFrame()
{
    
}

//------------------------------------------------------------------------------
/**
*/
PlayerManager::PlayerManager()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
PlayerManager::~PlayerManager()
{
    // empty
}

} // namespace Game


