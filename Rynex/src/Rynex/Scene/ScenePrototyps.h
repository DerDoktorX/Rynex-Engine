#pragma once

#define COMPONENT_SINGLE_MODEL
#define RY_COMPONENT_RELATION_SHIPS_BASED_UUID

namespace Rynex {
	class Entity;
	class Scene;
    class SceneCamera;

	struct CameraComponent;
	struct SpriteRendererComponent;
	struct MaterialComponent;
	struct GeometryComponent;
	struct ScriptComponent;
	struct FrameBufferComponent;
	struct RenderTargetComponent;

	struct MeshComponent;
	struct ModelMangerComponent;
	struct DynamicMeshComponent;
	struct StaticMeshComponent;

	
	struct DirectionLightComponent;
	struct PointLightComponent;
	struct SpotLightComponent;
	struct ParticleComponent;

	struct RelationshipUUIDComponent;

	struct TextComponent;
	struct ViewMatrixComponent;
	struct ProjectionViewMatrixComponent;
	struct InverseProjectionViewMatrixComponent;
	struct WorldViewFrustumComponent;

	struct TransformComponent;

	struct ModelMatrixComponent;
}