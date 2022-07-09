#pragma once

#include <vector>
#include <vulkan/vulkan.h>

struct BlasInput
{
	// Data used to build acceleration structure geometry
	std::vector<VkAccelerationStructureGeometryKHR> AsGeometry;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> AsBuildOffsetInfo;
	VkBuildAccelerationStructureFlagsKHR Flags{0};
};

struct AccelKHR
{
	VkAccelerationStructureKHR Accel = VK_NULL_HANDLE;
	VkBuffer Buffer;
};

struct BuildAccelerationStructure
{
	VkAccelerationStructureBuildGeometryInfoKHR buildInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR};
	VkAccelerationStructureBuildSizesInfoKHR sizeInfo{VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR};
	const VkAccelerationStructureBuildRangeInfoKHR* rangeInfo;
	AccelKHR As;  // result acceleration structure
	AccelKHR AleanupAS;
};

class RaytracingBuilder
{
public:
	void Setup(const VkDevice& device, uint32_t queueIndex);
	void BuildBlas(
		const std::vector<BlasInput>& input,
		VkBuildAccelerationStructureFlagsKHR flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR);
private:
	bool HasFlag(VkFlags item, VkFlags flag) { return (item & flag) == flag; }

	VkDevice m_Device;
	uint32_t m_QueueIndex;
};
