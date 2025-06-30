#pragma once

#include <Windows.h>

#include <chrono>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory>
#include <wrl/client.h>
#include <map>
#include <functional>
#include <bitset>
#include <unordered_set>
#include <array>


using namespace std;
using namespace chrono;
using Microsoft::WRL::ComPtr;



#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <directxtk/SimpleMath.h>
#include <dxgi.h>    // DXGIFactory
#include <dxgi1_4.h> // DXGIFactory4
#include <DirectXTex.h>
#include <fp16.h>

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

#include "Libraries/MathLibrary/FTransform.h"
#include "Definition.h"

#include "Render/RenderDefinition.h"
#include "Render/Mesh/Buffers.h"
#include "Render/Resource/Texture.h"
#include "Render/Resource/Material.h"
#include "Render/Pipeline/FPSO.h"
#include "Render/Pipeline/FGlobalPSO.h"
#include "Render/Resource/FSceneView.h"
#include "Render/Mesh/VertexData.h"



#include "Libraries/FileSystemHeaders.h"
#include "Libraries/Utility/Delegate.h"

#include "Libraries/Timer/Timer.h"
#include "Libraries/Timer/TimerManager.h"
#include "Frameworks/Manager/TickTaskManager.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"

#include "Systems/D3D.h"

#include "Editor/Application/Gui.h"
#include "Editor/Application/EditorApplication.h"
