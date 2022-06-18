#pragma once

#include "vulkan/vulkan.h"
#include "SDL.h"

#define DEBUG_MODE 1

#define MAX_VK_EXTENSIONS 10
#define MAX_VK_EXTENSION_NAME_SIZE 256

class VulkanApp {

public:
	void run();

private:

	struct QueueuFamily {
		uint32_t index;
		bool exists = false;
	};

	struct QueueFamilies {
		QueueuFamily graphicsFamily;
	};

	void init();
	void mainLoop();
	void cleanup();

	void createInstance();
	bool checkedValidationSupport();
	void getNecessaryExtensions(char** extensionNamesBuffer, int* numExtensions);
	VkResult setupDebugCallbacks();
	VkResult destroyCallbacks();
	void populateCreateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void pickPhysicalDevice();
	QueueFamilies findQueueFamiliesForDevice(const VkPhysicalDevice& device);

	const char* validationLayer = "VK_LAYER_KHRONOS_validation";
	int numValidationLayers = 1;

	const char* extraNecessaryExtensions[1] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
	int numExtraNecessaryExtensions = 1;

#if DEBUG_MODE == 1
	bool includeValidation = true;
#else
	bool includeValidation = false;
#endif

	SDL_Window* window;
	bool running = true;
	VkInstance vkInstance = VK_NULL_HANDLE;
	VkSurfaceKHR vkSurface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger;
};
