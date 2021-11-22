#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <thread>
#include <mutex>
using namespace std;

//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Effects11d.lib")

//ImGui
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/ImGuizmo.h>
#pragma comment(lib, "ImGui/imgui.lib")

//DirectXTex
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

#define Check(hr) { assert(SUCCEEDED(hr)); }
#define Super __super

#define SafeRelease(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SafeDelete(p){ if(p){ delete (p); (p) = NULL; } }
#define SafeDeleteArray(p){ if(p){ delete [] (p); (p) = NULL; } }

typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR4 Vector4;
typedef D3DXCOLOR Color;
typedef D3DXMATRIX Matrix;
typedef D3DXQUATERNION Quaternion;
typedef D3DXPLANE Plane;


#include "Systems/D3D.h"
#include "Systems/Keyboard.h"
#include "Systems/Mouse.h"
#include "Systems/Time.h"
#include "Systems/Gui.h"


#include "Viewer/Viewport.h"
#include "Viewer/Projection.h"
#include "Viewer/Perspective.h"
#include "Viewer/Orthographic.h"
#include "Viewer/Camera.h"
#include "Viewer/Freedom.h"
#include "Viewer/RenderTarget.h"
#include "Viewer/DepthStencil.h"
#include "Viewer/Fixity.h"

#include "Renders/Shader.h"
#include "Renders/Texture.h"
#include "Renders/Context.h"
#include "Renders/Gizmo.h"
#include "Renders/DebugLine.h"
#include "Renders/Buffers.h"
#include "Renders/Transform.h"
#include "Renders/Lighting.h"
#include "Renders/PerFrame.h"
#include "Renders/Renderer.h"
#include "Renders/Material.h"

#include "Utilities/Math.h"
#include "Utilities/String.h"
#include "Utilities/Path.h"

#include "Meshes/Mesh.h"
#include "Meshes/MeshQuad.h"
#include "Meshes/MeshPlane.h"
#include "Meshes/MeshCube.h"
#include "Meshes/MeshCylinder.h"
#include "Meshes/MeshSphere.h"
#include "Meshes/MeshRender.h"

#include "Model/Model.h"
#include "Model/ModelMesh.h"
#include "Model/ModelRender.h"
#include "Model/ModelClip.h"
#include "Model/ModelAnimator.h"

#include "Environment/Terrain.h"
#include "Environment/CubeSky.h"
#include "Environment/Rain.h"
#include "Environment/Snow.h"
#include "Environment/TerrainLOD.h"
#include "Environment/Sky.h"

#include "Objects/Render2D.h"
#include "Objects/Collider.h"
#include "Objects/Billboard.h"
#include "Objects/PostProcess.h"
#include "Objects/DynamicCube.h"
#include "Objects/Projector.h"
#include "Objects/Shadow.h"
#include "Objects/Frustum.h"

#include "Particle/ParticleData.h"
#include "Particle/Particle.h"