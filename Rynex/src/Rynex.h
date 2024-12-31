#pragma once

// for use by Rynex Aplication
#include "Rynex/Core/Base.h"

#include "Rynex/Core/Application.h"
#include "Rynex/Core/Layer.h"
#include "Rynex/Core/Log.h"
#include "Rynex/Core/BufferData.h"
#include "Rynex/Core/Assert.h"

#include "Rynex/Core/TimeStep.h"


#include "Rynex/Core/Input.h"
#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/MouseCodes.h"

#include "Rynex/ImGui/ImGuiLayer.h"

// Platform header
#include "Rynex/Utils/PlatformUtils.h"

//----- AssetManger ---------------------
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/Base/AssetImporter.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Base/AssetManagerBase.h"
#include "Rynex/Asset/Base/AssetMetadata.h"

#include "Rynex/Asset/Import/SceneImporter.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"
#include "Rynex/Asset/Import/FramebufferImporter.h"

#include "Rynex/Asset/RuntimeAssetManager.h"
#include "Rynex/Asset/EditorAssetManager.h"
// #include "Rynex/Asset/EditorAssetManegerThreade.h"


//------ entity -------------------------
#include "Rynex/Scene/Scene.h"
#include "Rynex/Scene/Components.h"
#include "Rynex/Scene/Entity.h"
#include "Rynex/Scene/ScriptableEntity.h"

//----- Renderer ------------------------
#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"
#include "Rynex/Renderer/RenderCommand.h"

#include "Rynex/Renderer/Text/Font.h"
#include "Rynex/Renderer/Objects/Model.h"
#include "Rynex/Renderer/Objects/Mesh.h"
#include "Rynex/Renderer/Objects/Geomtrys.h"

#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Renderer/API/VertexArray.h"
#include "Rynex/Renderer/API/Framebuffer.h"

#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/CamerController.h"
//---------------------------------------

//-----EnryPoint-------------------------
// #include "Rynex/Core/EntryPoint.h"
//---------------------------------------