#pragma once

#include "vulkan/vulkan.h"
#include "SDL.h"
#include <vector>

#define DEBUG_MODE 1

#define MAX_VK_EXTENSIONS 10
#define MAX_VK_EXTENSION_NAME_SIZE 256

class VulkanApp {

public:
	void run();

private:

	struct SwapChainDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamily {
		uint32_t index;
		bool exists = false;
	};

	struct QueueFamilies {
		QueueFamily graphicsFamily;
		QueueFamily presentFamily;

		bool isComplete() {
			return graphicsFamily.exists && presentFamily.exists;
		}
	};

	void init();
	void mainLoop();
	void cleanup();

	void createInstance();
	void createSurface();
	bool checkValidationSupport();
	void createSwapChain();
	void getNecessaryInstanceExtensions(char** extensionNamesBuffer, int* numExtensions);
	VkResult setupDebugCallbacks();
	VkResult destroyCallbacks();
	void populateCreateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void pickPhysicalDevice();
	void createLogicalDevice();
	QueueFamilies getQueueFamiliesForPhysDevice(const VkPhysicalDevice& device);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
	SwapChainDetails querySwapChainSupport(const VkPhysicalDevice& device);
	bool isPhysDevSuitable(const VkPhysicalDevice& device);
	VkSurfaceFormatKHR chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats);
	VkPresentModeKHR chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
	VkExtent2D chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createImageViews();
	void createGraphicsPipeline();
	void createRenderPass();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	void createFrameBuffers();
	void createCommandPool();

	const char* validationLayers[1] = { "VK_LAYER_KHRONOS_validation" };
	int numValidationLayers = 1;
	const char* extraNecessaryInstanceExts[1] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	int numNecessaryInstanceExts = 1;
	const char* extraNecessaryDevExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	int numNecessaryDevExts = 1;

#if DEBUG_MODE == 1
	bool includeValidation = true;
#else
	bool includeValidation = false;
#endif

	SDL_Window* window;
	int windowWidth = 600;
	int windowHeight = 600;
	bool running = true;
	VkInstance vkInstance = VK_NULL_HANDLE;
	VkSurfaceKHR vkSurface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages = {};
	std::vector<VkImageView> swapChainImageViews = {};
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFrameBuffers;
	VkCommandPool commandPool;
};
