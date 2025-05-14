#pragma once

#include <Windows.h>

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include <map>
#include <functional>
#include <bitset>
#include <unordered_set>


using namespace std;
using namespace chrono;
using Microsoft::WRL::ComPtr;

#include "Utility/Delegate.h"
#include "Utility/String.h"
#include "Utility/Path.h"


#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>


#include <directxtk/SimpleMath.h>
#include <dxgi.h>    // DXGIFactory
#include <dxgi1_4.h> // DXGIFactory4
#include <DirectXTex.h>


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma



using namespace DirectX;
using namespace DirectX::SimpleMath;



#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>



#include "Definition.h"
#include "Systems/D3D.h"

#include "Render/Buffers.h"
#include "Render/Texture.h"
#include "Render/Material.h"
#include "Render/Shader.h"

#include "MathLibrary/FTransform.h"
#include "Render/VertexData.h"


#include "Timer/Timer.h"
#include"Timer/TimerManager.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Manager/TickTaskManager.h"

#include "Editor/Application/Gui.h"
#include "Editor/Application/EditorApplication.h"
