#include "vulkan_device.hpp"

#include "config.hpp"

#include <SDL2/SDL_vulkan.h>


VulkanDevice::VulkanDevice(SDL_Window* window) {
    createVulkanInstance(window);
    pickPhysicalDevice();
    createDevice();
    createCommandPool();
}

VulkanDevice::~VulkanDevice() {
    if (m_device != nullptr) {
        vkDestroyDevice(m_device, nullptr);
    }
    if (m_instance != nullptr) {
        vkDestroyInstance(m_instance, nullptr);
    }
}


VkDevice VulkanDevice::getVkDevice() const {
    return m_device;
}

void VulkanDevice::createVulkanInstance(SDL_Window* window) {
    // get extensions
    uint32_t nRequiredExtensions = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &nRequiredExtensions, nullptr)) {
        throw std::runtime_error("failed to get the number of required instance extensions");
    }
    std::vector<const char*> requiredExtensionNames(nRequiredExtensions);
    if (!SDL_Vulkan_GetInstanceExtensions(window, &nRequiredExtensions, requiredExtensionNames.data())) {
        throw std::runtime_error("failed to get the names of the required instance extensions");
    }

    // get layers (none so far)
    uint32_t nRequiredLayers = 0;
    std::vector<const char*> requiredLayerNames(nRequiredLayers);

    // construct the application info
    const auto applicationName = Config::get<std::string>("application.name");
    const auto versionMajor = Config::get<int>("application.version.major");
    const auto versionMinor = Config::get<int>("application.version.minor");
    const auto versionPatch = Config::get<int>("application.version.patch");
    const uint32_t version = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
    const VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = applicationName.c_str(),
        .applicationVersion = version,
        .pEngineName = applicationName.c_str(),
        .engineVersion = version,
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    // construct the instance create info
    const VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = nRequiredLayers,
        .ppEnabledLayerNames = requiredLayerNames.data(),
        .enabledExtensionCount = nRequiredExtensions,
        .ppEnabledExtensionNames = requiredExtensionNames.data(),
    };

    // create the vulkan instance
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vulkan instance");
    }
}

void VulkanDevice::pickPhysicalDevice() {
    assert(m_instance != nullptr);

    // get all available physical devices
    uint32_t nPhysicalDevices = 0;
    if (vkEnumeratePhysicalDevices(m_instance, &nPhysicalDevices, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to enumerate phyiscal devices");
    }
    std::vector<VkPhysicalDevice> physicalDevices(nPhysicalDevices);
    if (vkEnumeratePhysicalDevices(m_instance, &nPhysicalDevices, physicalDevices.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to retrieve the list of physical devices");
    }

    // choose the most suitable physical device by assigning a score to each one
    int bestScore = 0;
    std::optional<VkPhysicalDevice> bestPhysicalDevice = {};
    for (const auto& device : physicalDevices) {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

        // rate deviceType
        // these scores are quite arbitrary as of now
        int score = 0;
        switch (physicalDeviceProperties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                score += 2000;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                score += 3000;
                break;
            default:
                break;
        }

        if (score > bestScore) {
            bestScore = score;
            bestPhysicalDevice = device;
        }
    }

    if (!bestPhysicalDevice.has_value()) {
        throw std::runtime_error("failed to find a suitable physical device");
    }

    m_physicalDevice = bestPhysicalDevice.value();
}


void VulkanDevice::createDevice() {
    assert(m_physicalDevice != nullptr);

    // get all queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, 0);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

    // look for the one with the GRAPHICS_BIT set
    for (size_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueCount > 0 && (queueFamilies[i].queueFlags | VK_QUEUE_GRAPHICS_BIT)) {
            m_graphicsQueueFamily = static_cast<uint32_t>(i);
            break;
        }
    }
    if (!m_graphicsQueueFamily.has_value()) {
        throw std::runtime_error("unable to find a graphics queue family");
    }

    // create the device queue info
    std::vector<float> queuePriorities = { 0.f };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = m_graphicsQueueFamily.value(),
        .queueCount = 1,
        .pQueuePriorities = queuePriorities.data(),
    };

    // create the device itself
    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr,
    };
    if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create device");
    }
}

void VulkanDevice::createCommandPool() {
    assert(m_device != nullptr);
    assert(m_graphicsQueueFamily.has_value());

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = m_graphicsQueueFamily.value(),
    };
    if (vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
}

