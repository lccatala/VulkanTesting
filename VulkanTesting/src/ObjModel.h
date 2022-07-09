#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct VertexObj
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 TexCoord;
};


struct ObjModel
{
	uint32_t nbIndices{0};
	uint32_t nbVertices{0};
	VkBuffer vertexBuffer;    // Device buffer of all 'Vertex'
	VkBuffer indexBuffer;     // Device buffer of the indices forming triangles
	VkBuffer matColorBuffer;  // Device buffer of array of 'Wavefront material'
	VkBuffer matIndexBuffer;  // Device buffer of array of 'Wavefront material'
};
