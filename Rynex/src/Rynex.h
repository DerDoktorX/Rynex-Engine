#pragma once

// for use by Rynex Aplication

#include "Rynex/Core/Application.h"
#include "Rynex/Core/Layer.h"
#include "Rynex/Core/Log.h"
#include "Rynex/Core/File.h"
#include "Rynex/Asset/Base/Buffer.h"	// TODO: find a way to move "Buffer.h" in -> "Rynex/Core/..."

#include "Rynex/Core/TimeStep.h"
#include "Rynex/Utils/PlatformUtils.h"

#include "Rynex/Core/Input.h"
#include "Rynex/Core/KeyCodes.h"
#include "Rynex/Core/MouseCodes.h"

#include "Rynex/ImGui/ImGuiLayer.h"

//----- AssetManger ---------------------
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/Base/AssetImporter.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Base/AssetManagerBase.h"
#include "Rynex/Asset/Base/AssetMetadata.h"

#include "Rynex/Asset/Import/SceneImporter.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"

#include "Rynex/Asset/RuntimeAssetManager.h"
#include "Rynex/Asset/EditorAssetManager.h"



//------ entity -------------------------
#include "Rynex/Scene/Scene.h"
#include "Rynex/Scene/Components.h"
#include "Rynex/Scene/Entity.h"
#include "Rynex/Scene/ScriptableEntity.h"

//----- Renderer ------------------------
#include "Rynex/Renderer/Renderer.h"
#include "Rynex/Renderer/Renderer2D.h"
#include "Rynex/Renderer/RenderCommand.h"

#include "Rynex/Renderer/Buffer.h"
#include "Rynex/Renderer/Shader.h"
#include "Rynex/Renderer/Texture.h"
#include "Rynex/Renderer/VertexArray.h"
#include "Rynex/Renderer/Framebuffer.h"

#include "Rynex/Renderer/OrthograficCamera.h"
#include "Rynex/Core/CamerController.h"
//---------------------------------------

//-----EnryPoint-------------------------
//#include "Rynex/Core/EntryPoint.h"
//---------------------------------------