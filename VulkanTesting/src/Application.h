#pragma once

#pragma warning(push)
#pragma warning(disable: 26812) // Disable warnings for unscoped enums in Vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma warning(pop)

#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <array>
#include <optional>

struct Vertex
{
	glm::vec2 Position;
	glm::vec3 Color;

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; // Index of binding in array of bindings
		bindingDescription.stride = sizeof(Vertex); // Bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to the next data entry after each vertex
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		// Position
		attributeDescriptions[0].binding = 0; // From which binding is the per-vertex data coming
		attributeDescriptions[0].location = 0; // Reference to 'location' directive in vertex shader input
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // Commonly used format for vec2 data
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		// Color
		attributeDescriptions[1].binding = 0; 
		attributeDescriptions[1].location = 1; 
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // Commonly used format for vec3 data
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}
};

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
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	// GPU/CPU synchronization
	std::vector<VkSemaphore> m_ImageAvailableSemaphores, m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;

	bool m_FramebufferResized = false;

	const std::vector<Vertex> m_Vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> m_Indices = {
		0, 1, 2, 2, 3, 0
	};

	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
};