#include "vulkan_device.hpp"

#include "config.hpp"

#include <SDL2/SDL_vulkan.h>


VulkanDevice::VulkanDevice(SDL_Window* window) {
    createVulkanInstance(window);
    pickPhysicalDevice();
}

VulkanDevice::~VulkanDevice() {
    vkDestroyInstance(m_instance, nullptr);
}


void VulkanDevice::createVulkanInstance(SDL_Window* window) {
    // get extensions
    unsigned nRequiredExtensions = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &nRequiredExtensions, nullptr)) {
        throw std::runtime_error("failed to get the number of required instance extensions");
    }
    std::vector<const char*> requiredExtensionNames(nRequiredExtensions);
    if (!SDL_Vulkan_GetInstanceExtensions(window, &nRequiredExtensions, requiredExtensionNames.data())) {
        throw std::runtime_error("failed to get the names of the required instance extensions");
    }

    // get layers (none so far)
    unsigned nRequiredLayers = 0;
    std::vector<const char*> requiredLayerNames(nRequiredLayers);

    // construct the application info
    const auto applicationName = Config::get<std::string>("application.name");
    const auto versionMajor = Config::get<int>("application.version.major");
    const auto versionMinor = Config::get<int>("application.version.minor");
    const auto versionPatch = Config::get<int>("application.version.patch");
    const unsigned version = VK_MAKE_VERSION(versionMajor, versionMinor, versionPatch);
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
    // get all available physical devices
    unsigned nPhysicalDevices = 0;
    if (vkEnumeratePhysicalDevices(m_instance, &nPhysicalDevices, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to enumerate phyiscal devices");
    }
    std::vector<VkPhysicalDevice> physicalDevices(nPhysicalDevices);
    if (vkEnumeratePhysicalDevices(m_instance, &nPhysicalDevices, physicalDevices.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to retrieve the list of physical devices");
    }

    // choose the most suitable physical device by assigning a score to each one
    std::vector<int> physicalDeviceScores;
    for (const auto& device : physicalDevices) {
        int score = 0;

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);

        // rate deviceType
        // these scores are quite arbitrary as of now
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
    }
    const auto bestScore = std::max_element(physicalDeviceScores.begin(), physicalDeviceScores.end());
    m_physicalDevice = physicalDevices[std::distance(physicalDeviceScores.begin(), bestScore)];
}

