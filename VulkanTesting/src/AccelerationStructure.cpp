#include "AccelerationStructure.h"

void RaytracingBuilder::BuildBlas(
	const std::vector<BlasInput>& input,
	VkBuildAccelerationStructureFlagsKHR flags)
{
	uint32_t ndBlas = static_cast<uint32_t>(input.size());
	VkDeviceSize asTotalSize{0}; // Memory size for all allocated BLAS
	uint32_t nbCompactions{0}; // Nb of BLAS requesting compaction
	VkDeviceSize maxScratchSize{ 0 }; // Largest scratch size

	// Preparing the information for the acceleration build commands
	std::vector<BuildAccelerationStructure> buildAs(ndBlas);
	for (uint32_t idx = 0; idx < ndBlas; ++idx)
	{
        // Filling partially the VkAccelerationStructureBuildGeometryInfoKHR for querying the build sizes.
		// Other information will be filled in the createBlas (see #2)
		buildAs[idx].buildInfo.type          = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		buildAs[idx].buildInfo.mode          = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		buildAs[idx].buildInfo.flags         = input[idx].Flags | flags;
		buildAs[idx].buildInfo.geometryCount = static_cast<uint32_t>(input[idx].AsGeometry.size());
		buildAs[idx].buildInfo.pGeometries   = input[idx].AsGeometry.data();

		// Build range information
		buildAs[idx].rangeInfo = input[idx].AsBuildOffsetInfo.data();

		// Finding sizes to create acceleration structures and scratch
		std::vector<uint32_t> maxPrimCount(input[idx].AsBuildOffsetInfo.size());
		for (auto tt = 0; tt < input[idx].AsBuildOffsetInfo.size(); tt++)
		{
			maxPrimCount[tt] = input[idx].AsBuildOffsetInfo[tt].primitiveCount;  // Number of primitives/triangles
		}
		vkGetAccelerationStructureBuildSizesKHR(m_Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
												&buildAs[idx].buildInfo, maxPrimCount.data(), &buildAs[idx].sizeInfo);

		// Extra info
		asTotalSize += buildAs[idx].sizeInfo.accelerationStructureSize;
		maxScratchSize = std::max(maxScratchSize, buildAs[idx].sizeInfo.buildScratchSize);
		nbCompactions += HasFlag(buildAs[idx].buildInfo.flags, VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR);

	}
}
void RaytracingBuilder::Setup(const VkDevice& device, uint32_t queueIndex)
{
  m_Device     = device;
  m_QueueIndex = queueIndex;
}
