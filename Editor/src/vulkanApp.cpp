#include "vulkanApp.h"
#include "SDL_vulkan.h"
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	std::cout << std::endl;
	return VK_FALSE;
}

void VulkanApp::run() {
	init();
	mainLoop();
	cleanup();
}

void VulkanApp::init() {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		char error[256];
		sprintf_s(error, "Error: %s\n", SDL_GetError());
		throw std::exception(error);
	}

	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	window = SDL_CreateWindow("vulkan window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, windowFlags);

	createInstance();
	if (setupDebugCallbacks() != VK_SUCCESS) {
		throw std::exception("could not set up debug callbacks");
	}

	pickPhysicalDevice();

	if (SDL_Vulkan_CreateSurface(window, vkInstance, &vkSurface) == 0) {
		throw std::exception("could not create vulkan surface");
	}
}

void VulkanApp::pickPhysicalDevice() {
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);

	if (physicalDeviceCount == 0) {
		throw std::exception("could not find GPUs that support Vulkan");
	}

	VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices);

	for (int i = 0; i < physicalDeviceCount; i++) {
		VkPhysicalDeviceProperties properties;
		VkPhysicalDeviceFeatures features;

		VkPhysicalDevice& device = physicalDevices[i];
		vkGetPhysicalDeviceProperties(device, &properties);
		vkGetPhysicalDeviceFeatures(device, &features);

		QueueFamilies queueFamilies = findQueueFamiliesForDevice(device);
		if (queueFamilies.graphicsFamily.exists) {
			physicalDevice = device;
			break;
		}
	}
	physicalDevice = physicalDevices[0];
}

VulkanApp::QueueFamilies VulkanApp::findQueueFamiliesForDevice(const VkPhysicalDevice& device) {
	QueueFamilies families;
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
	VkQueueFamilyProperties* queueProperties = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueProperties);

	for (int i = 0; i < queueFamilyCount; i++) {
		if (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			families.graphicsFamily.exists = true;
			families.graphicsFamily.index = i;
		}
	}

	return families;
}

void VulkanApp::populateCreateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = NULL;
}

VkResult VulkanApp::setupDebugCallbacks() {
	if (!includeValidation) {
		return VK_SUCCESS;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateCreateDebugUtilsMessenger(createInfo);
	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
	if (vkCreateDebugUtilsMessengerEXT) {
		vkCreateDebugUtilsMessengerEXT(vkInstance, &createInfo, NULL, &debugMessenger);
		return VK_SUCCESS;
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

VkResult VulkanApp::destroyCallbacks() {
	auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT) {
		vkDestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, NULL);
		return VK_SUCCESS;
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}


void VulkanApp::createInstance() {

	// optional but maybe allows for version tracking
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Vulkan App";
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

	// get necessary extension names and store them in array
	int extensionCount;
	char* extensionNamePtrs[MAX_VK_EXTENSIONS]{};
	char extensionNames[MAX_VK_EXTENSIONS][MAX_VK_EXTENSION_NAME_SIZE]{};
	for (int i = 0; i < MAX_VK_EXTENSIONS; i++) {
		extensionNamePtrs[i] = extensionNames[i];
	}
	getNecessaryExtensions(extensionNamePtrs, &extensionCount);

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.enabledExtensionCount = extensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = extensionNamePtrs;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	if (includeValidation) {
		if (checkedValidationSupport()) {
			instanceCreateInfo.ppEnabledLayerNames = &validationLayer;
			instanceCreateInfo.enabledLayerCount = numValidationLayers;
		}

		populateCreateDebugUtilsMessenger(createInfo);
		instanceCreateInfo.pNext = &createInfo;
	}

	if (vkCreateInstance(&instanceCreateInfo, NULL, &vkInstance) != VK_SUCCESS) {
		throw std::exception("could not create vulkan instance");
	}
}

void VulkanApp::getNecessaryExtensions(char** extensionNamesBuffer, int* numExtensions) {
	uint32_t extensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL);
	const char** sdlNecessaryExt = new const char* [extensionCount];
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, sdlNecessaryExt);

	for (int i = 0; i < extensionCount; i++) {
		size_t len = strlen(sdlNecessaryExt[i]);
		const char* ext = sdlNecessaryExt[i];
		char* bufferToWriteTo = extensionNamesBuffer[i];
		strcpy_s(bufferToWriteTo, len + 1, ext);
	}

	for (int i = 0; i < numExtraNecessaryExtensions; i++) {
		char* bufferToWriteTo = extensionNamesBuffer[extensionCount + i];
		size_t len = strlen(extraNecessaryExtensions[i]);
		const char* ext = extraNecessaryExtensions[i];
		strcpy_s(bufferToWriteTo, len + 1, ext);
	}
	*numExtensions = extensionCount + numExtraNecessaryExtensions;
}

bool VulkanApp::checkedValidationSupport() {
	uint32_t valLayerCount;
	vkEnumerateInstanceLayerProperties(&valLayerCount, NULL);
	VkLayerProperties* properties = new VkLayerProperties[valLayerCount];
	vkEnumerateInstanceLayerProperties(&valLayerCount, properties);

	std::cout << "valLayerCount: " << valLayerCount << std::endl;
	for (int i = 0; i < valLayerCount; i++) {
		if (strcmp(validationLayer, properties[i].layerName) == 0) {
			return true;
		}
	}

	return false;
}

void VulkanApp::mainLoop() {
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = false;
		}
	}
}

void VulkanApp::cleanup() {
	destroyCallbacks();
	vkDestroySurfaceKHR(vkInstance, vkSurface, NULL);
	vkDestroyInstance(vkInstance, NULL);
}