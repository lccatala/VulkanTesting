#pragma once

#pragma warning(push)
#pragma warning(disable: 26812) // Disable warnings for unscoped enums in Vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma warning(pop)

#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Depth ranges from 0 to 1 instead of -1 to 1 (Vulkan vs OpenGL)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <array>
#include <optional>
#include <chrono>

struct UniformBufferObject
{
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TextureCoordinates;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; // Index of binding in array of bindings
		bindingDescription.stride = sizeof(Vertex); // Bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to the next data entry after each vertex
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		// Position
		attributeDescriptions[0].binding = 0; // From which binding is the per-vertex data coming
		attributeDescriptions[0].location = 0; // Reference to 'location' directive in vertex shader input
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // Commonly used format for vec2 data
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		// Color
		attributeDescriptions[1].binding = 0; 
		attributeDescriptions[1].location = 1; 
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // Commonly used format for vec3 data
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		// Color
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TextureCoordinates);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const
	{
		return Position == other.Position && Color == other.Color && TextureCoordinates == other.TextureCoordinates;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return
				((hash<glm::vec3>()(vertex.Position) ^
				 (hash<glm::vec3>()(vertex.Color) << 1)) >> 1) ^
				 (hash<glm::vec2>()(vertex.TextureCoordinates) << 1);
		}
	};
}

struct QueueFamilyIndices
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class Application
{
public:
	void Run();
private:
	void InitWindow();
	
	void InitVulkan();
	void CreateInstance();
	bool CheckValidationLayerSupport();
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	void CreateLogicalDevice();
	void CreateSurface();
	void CreateSwapchain();
	void CreateImageViews();
	SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateRenderPass();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	static std::vector<char> ReadFile(const std::string& filename);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);
	void CreateSyncObjects();
	void CreateVertexBuffer();
	void CreateBuffer(
		VkDeviceSize size, 
		VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkBuffer& buffer, 
		VkDeviceMemory& deviceMemory);
	void CreateIndexBuffer();
	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void UpdateUniformBuffer(uint32_t currentImage);
	void CreateTextureImage();
	void CreateTextureImageView();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void CreateImage(
		uint32_t width, 
		uint32_t height, 
		uint32_t mipLevels,
		VkSampleCountFlagBits numSamples,
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image, 
		VkDeviceMemory& imageMemory);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void CreateTextureSampler();
	void CreateDepthResources();
	void CreateColorResources();
	VkFormat FindDepthFormat();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	bool HasStencilComponent(VkFormat format);
	void LoadModel();
	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	VkSampleCountFlagBits GetMaxUsableSampleCount();

	void MainLoop();
	void DrawFrame();

	void Cleanup();

	void RecreateSwapchain();
	void CleanupSwapchain();

	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

	const uint32_t m_MaxFramesInFlight = 2;
	uint32_t m_CurrentFrame = 0;

	// Window
	GLFWwindow* m_Window;
	const uint32_t m_WindowWidth = 800;
	const uint32_t m_WindowHeight = 600;

	VkInstance m_VkInstance;

	// Validation layers
	const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
	const bool m_EnableValidationLayers = false;
#else
	const bool m_EnableValidationLayers = true;
#endif

	// Physical device
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

	// Logical device
	VkDevice m_Device;

	// Handle to graphics queue
	VkQueue m_GraphicsQueue;

	// Window surface
	VkSurfaceKHR m_WindowSurface;

	// Present queue
	VkQueue m_PresentQueue;

	// Swapchain
	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkSwapchainKHR m_Swapchain;
	std::vector<VkImage> m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;
	VkFormat m_SwapchainImageFormat;
	VkExtent2D m_SwapchainExtent;
	std::vector<VkFramebuffer> m_SwapchainFramebuffers;

	VkRenderPass m_RenderPass;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	// GPU/CPU synchronization
	std::vector<VkSemaphore> m_ImageAvailableSemaphores, m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;

	bool m_FramebufferResized = false;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBufferMemories;

	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;

	uint32_t m_MipLevels;
	VkImage m_TextureImage;
	VkDeviceMemory m_TextureImageMemory;
	VkImageView m_TextureImageView;
	VkSampler m_TextureSampler;
	
	// Depth buffer
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;

	// Model
	const std::string m_ModelPath = "resources/models/viking_room.obj";
	const std::string m_TexturePath = "resources/textures/viking_room.png";
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;

	// Multisampling
	VkSampleCountFlagBits m_MsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage m_ColorImage;
	VkDeviceMemory m_ColorImageMemory;
	VkImageView m_ColorImageView;
};