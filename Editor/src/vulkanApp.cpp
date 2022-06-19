#include "vulkanApp.h"
#include "SDL_vulkan.h"
#include <iostream>
#include <fstream>

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

	// SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);
	window = SDL_CreateWindow("vulkan window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, windowFlags);

	createInstance();
	if (setupDebugCallbacks() != VK_SUCCESS) {
		throw std::exception("could not set up debug callbacks");
	}
	createSurface();

	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffers();
	createCommandPool();
}

void VulkanApp::createCommandPool() {

}

void VulkanApp::createFrameBuffers() {
	// create a framebuffer that accounts for each swap chain image view as its color attachment
	swapChainFrameBuffers.resize(swapChainImageViews.size());

	for (int i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
		swapChainImageViews[i]
		};
		VkFramebufferCreateInfo createInfo{};
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = attachments;
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = renderPass;
		createInfo.width = swapChainExtent.width;
		createInfo.height = swapChainExtent.height;
		createInfo.layers = 1;

		if (vkCreateFramebuffer(device, &createInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void VulkanApp::createRenderPass() {
	VkAttachmentDescription colorAttachmentDesc{};
	colorAttachmentDesc.format = swapChainImageFormat;
	colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	// load and store operations for color/depth attachments
	colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	// load and store operations for stencil attachments
	colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// layout of image before render pass
	colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// layout of image after render pass
	colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachmentDesc;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VulkanApp::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());
	for (int i = 0; i < swapChainImageViews.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::exception("could not create image view");
		}
	}
}

void VulkanApp::createSwapChain() {
	SwapChainDetails swapChainDetails = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapChainFormat(swapChainDetails.formats);
	swapChainImageFormat = surfaceFormat.format;
	VkPresentModeKHR presentMode = chooseSwapChainPresentMode(swapChainDetails.presentModes);
	swapChainExtent = chooseSwapChainExtent(swapChainDetails.capabilities);
	uint32_t imageCount = swapChainDetails.capabilities.minImageCount + 1;
	if (swapChainDetails.capabilities.maxImageCount > 0) {
		imageCount = fmin(imageCount, swapChainDetails.capabilities.maxImageCount);
	}
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.imageExtent = swapChainExtent;
	createInfo.surface = vkSurface;
	createInfo.presentMode = presentMode;
	createInfo.minImageCount = imageCount;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// handles how images from swap chain are shared amongst different queue families
	QueueFamilies queueFamilies = getQueueFamiliesForPhysDevice(physicalDevice);
	uint32_t indicies[2] = { queueFamilies.graphicsFamily.index, queueFamilies.presentFamily.index };
	if (queueFamilies.graphicsFamily.index == queueFamilies.presentFamily.index) {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = indicies;
	}

	createInfo.preTransform = swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::exception("could not create swapchain");
	}

	uint32_t swapChainImgCount;
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImgCount, nullptr);
	swapChainImages.resize(swapChainImgCount);
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImgCount, swapChainImages.data());
}

void VulkanApp::createSurface() {
	if (SDL_Vulkan_CreateSurface(window, vkInstance, &vkSurface) == 0) {
		throw std::exception("could not create vulkan surface");
	}
}

void VulkanApp::pickPhysicalDevice() {
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0) {
		throw std::exception("could not find GPUs that support Vulkan");
	}

	VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
	vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices);

	for (int i = 0; i < physicalDeviceCount; i++) {
		VkPhysicalDevice& device = physicalDevices[i];
		if (isPhysDevSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}
	physicalDevice = physicalDevices[0];
	delete[] physicalDevices;
}

bool VulkanApp::isPhysDevSuitable(const VkPhysicalDevice& device) {
	VulkanApp::QueueFamilies queueFamilies = getQueueFamiliesForPhysDevice(device);
	if (queueFamilies.isComplete() && checkDeviceExtensionSupport(device)) {
		SwapChainDetails swapChainDetails = querySwapChainSupport(device);
		if (swapChainDetails.formats.size() > 0 && swapChainDetails.presentModes.size() > 0) {
			return true;
		}
	}
	return false;
}

VkSurfaceFormatKHR VulkanApp::chooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}
	return formats[0];
}

VkPresentModeKHR VulkanApp::chooseSwapChainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& presentMode : presentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanApp::chooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int pixelsWidth;
		int pixelsHeight;
		SDL_Vulkan_GetDrawableSize(window, &pixelsWidth, &pixelsHeight);
		VkExtent2D extent;
		extent.width = fmin(fmax(pixelsWidth, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
		extent.height = fmin(fmax(pixelsHeight, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);
		return extent;
	}
}

bool VulkanApp::checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
	uint32_t availableExtCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtCount, nullptr);
	VkExtensionProperties* availableExt = new VkExtensionProperties[availableExtCount];
	vkEnumerateDeviceExtensionProperties(device, nullptr, &availableExtCount, availableExt);

	int remainingNecessaryExts = numNecessaryDevExts;

	for (int i = 0; i < availableExtCount; i++) {
		for (int j = 0; j < numNecessaryDevExts; j++) {
			if (strcmp(availableExt[i].extensionName, extraNecessaryDevExtensions[j]) == 0) {
				remainingNecessaryExts -= 1;
				break;
			}
		}
	}

	delete[] availableExt;
	return (remainingNecessaryExts == 0);
}

VulkanApp::QueueFamilies VulkanApp::getQueueFamiliesForPhysDevice(const VkPhysicalDevice& physicalDevice) {
	QueueFamilies families;
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties* queueProperties = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueProperties);

	for (int i = 0; i < queueFamilyCount; i++) {
		if (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			families.graphicsFamily.exists = true;
			families.graphicsFamily.index = i;
		}
		VkBool32 supportsSurface;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vkSurface, &supportsSurface);
		if (supportsSurface) {
			families.presentFamily.exists = true;
			families.presentFamily.index = i;
		}
	}

	delete[] queueProperties;

	return families;
}

void VulkanApp::populateCreateDebugUtilsMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

VkResult VulkanApp::setupDebugCallbacks() {
	if (!includeValidation) {
		return VK_SUCCESS;
	}
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateCreateDebugUtilsMessenger(createInfo);
	auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
	if (vkCreateDebugUtilsMessengerEXT) {
		vkCreateDebugUtilsMessengerEXT(vkInstance, &createInfo, nullptr, &debugMessenger);
		return VK_SUCCESS;
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

VkResult VulkanApp::destroyCallbacks() {
	auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT) {
		vkDestroyDebugUtilsMessengerEXT(vkInstance, debugMessenger, nullptr);
		return VK_SUCCESS;
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanApp::createLogicalDevice() {
	QueueFamilies queueFamilies = getQueueFamiliesForPhysDevice(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfos[2] = {};
	int numCreateInfos;
	uint32_t queueFamilyIndicies[2] = {};

	if (queueFamilies.graphicsFamily.index != queueFamilies.presentFamily.index) {
		queueFamilyIndicies[0] = queueFamilies.graphicsFamily.index;
		queueFamilyIndicies[1] = queueFamilies.presentFamily.index;
		numCreateInfos = 2;
	}
	else {
		queueFamilyIndicies[0] = queueFamilies.graphicsFamily.index;
		numCreateInfos = 1;
	}

	float queuePriority = 1.0f;
	for (int i = 0; i < numCreateInfos; i++) {
		queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[i].queueFamilyIndex = queueFamilyIndicies[i];
		queueCreateInfos[i].queueCount = 1;
		queueCreateInfos[i].pQueuePriorities = &queuePriority;
	}

	VkPhysicalDeviceFeatures physicalDeviceFeatures{};

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
	deviceCreateInfo.queueCreateInfoCount = numCreateInfos;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	deviceCreateInfo.enabledExtensionCount = 0;

	if (includeValidation && checkValidationSupport()) {
		deviceCreateInfo.ppEnabledLayerNames = validationLayers;
		deviceCreateInfo.enabledLayerCount = numValidationLayers;
	}

	deviceCreateInfo.enabledExtensionCount = numNecessaryDevExts;
	deviceCreateInfo.ppEnabledExtensionNames = extraNecessaryDevExtensions;

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::exception("could not create device");
	}

	vkGetDeviceQueue(device, queueFamilies.graphicsFamily.index, 0, &graphicsQueue);
	vkGetDeviceQueue(device, queueFamilies.presentFamily.index, 0, &presentQueue);
}

VulkanApp::SwapChainDetails VulkanApp::querySwapChainSupport(const VkPhysicalDevice& device) {
	SwapChainDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vkSurface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, nullptr);
	details.formats.resize(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vkSurface, &formatCount, details.formats.data());

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, nullptr);

	details.presentModes.resize(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vkSurface, &presentModeCount, details.presentModes.data());

	return details;
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
	getNecessaryInstanceExtensions(extensionNamePtrs, &extensionCount);

	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.enabledExtensionCount = extensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = extensionNamePtrs;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	if (includeValidation) {
		if (checkValidationSupport()) {
			instanceCreateInfo.ppEnabledLayerNames = validationLayers;
			instanceCreateInfo.enabledLayerCount = numValidationLayers;
		}

		populateCreateDebugUtilsMessenger(createInfo);
		instanceCreateInfo.pNext = &createInfo;
	}

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::exception("could not create vulkan instance");
	}
}

void VulkanApp::getNecessaryInstanceExtensions(char** extensionNamesBuffer, int* numExtensions) {
	uint32_t extensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
	const char** sdlNecessaryExt = new const char* [extensionCount];
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, sdlNecessaryExt);

	for (int i = 0; i < extensionCount; i++) {
		size_t len = strlen(sdlNecessaryExt[i]);
		const char* ext = sdlNecessaryExt[i];
		std::cout << ext << std::endl;
		char* bufferToWriteTo = extensionNamesBuffer[i];
		strcpy_s(bufferToWriteTo, len + 1, ext);
	}

	delete[] sdlNecessaryExt;

	for (int i = 0; i < numNecessaryInstanceExts; i++) {
		char* bufferToWriteTo = extensionNamesBuffer[extensionCount + i];
		size_t len = strlen(extraNecessaryInstanceExts[i]);
		const char* ext = extraNecessaryInstanceExts[i];
		strcpy_s(bufferToWriteTo, len + 1, ext);
	}
	*numExtensions = extensionCount + numNecessaryInstanceExts;
}

bool VulkanApp::checkValidationSupport() {
	uint32_t valLayerCount;
	vkEnumerateInstanceLayerProperties(&valLayerCount, nullptr);
	VkLayerProperties* properties = new VkLayerProperties[valLayerCount];
	vkEnumerateInstanceLayerProperties(&valLayerCount, properties);

	int numValLayersLeft = numValidationLayers;
	for (int i = 0; i < valLayerCount; i++) {
		for (int j = 0; j < numValidationLayers; j++) {
			if (strcmp(validationLayers[j], properties[i].layerName) == 0) {
				numValLayersLeft -= 1;
				break;
			}
		}
	}

	delete[] properties;
	return (numValLayersLeft == 0);
}

static std::vector<char> readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		std::string excStr = "failed to open file " + fileName;
		throw std::exception(excStr.c_str());
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}


VkShaderModule VulkanApp::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}

void VulkanApp::createGraphicsPipeline() {
	auto vertShaderCode = readFile("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\vulkanVert.spv");
	auto fragShaderCode = readFile("C:\\Sarthak\\programming\\3dFileLoader\\Editor\\src\\shaders\\vulkanFrag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// vertex shader stage
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	// fragment shader stage
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	// vertex input settings
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	// input assembly settings
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = swapChainExtent.width;
	viewport.height = swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = swapChainExtent;

	// viewport settings
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// rasterizer settings
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// configure multisampling, right now it is pretty much disabled
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// color blending settings per framebuffer
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	// color blending settings globally
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::exception("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::exception("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
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
	for (const auto& framebuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	for (const auto& imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(vkInstance, vkSurface, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}