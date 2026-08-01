#pragma once

#define COMPONENT_SINGLE_MODEL
#define RY_COMPONENT_RELATION_SHIPS_BASED_UUID

namespace Rynex {
	class Entity;
	class Scene;

	struct CameraComponent;
	struct SpriteRendererComponent;
	struct MaterialComponent;
	struct GeomtryComponent;
	struct ScriptComponent;
	struct FrameBufferComponent;
	struct RenderTargetComponent;

	struct MeshComponent;
	struct ModelMangerComponent;
	struct DynamicMeshComponent;
	struct StaticMeshComponent;

	
	struct DrirectionleLigthComponent;
	struct PointLigthComponent;
	struct SpotLigthComponent;
	struct ParticelComponente;

#ifdef RY_COMPONENT_RELATION_SHIPS_BASED_UUID
	struct RealtionShipUUIDComponent;
#else
	struct RealtionShipComponent
#endif

	struct TextComponent;
	struct ViewMatrixComponent;
	struct ProjtionViewMatrixComponent;
	struct InverseProjtionViewMatrixComponent;
	struct WorldViewFustrumComponent;

	struct TransformComponent;

#ifdef COMPONENT_SINGLE_MODEL
	struct ModelMatrixComponent;
#else
	struct ModelMatrixLocaleComponent;
	struct ModelMatrixGlobleComponent;
#endif
}