#pragma once

#include "common_includes.hpp"

#include <SDL2/SDL.h>

class VulkanDevice {
public:
    VulkanDevice(SDL_Window* window);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice(VulkanDevice&&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;
    VulkanDevice& operator=(VulkanDevice&&) = delete;

    VkDevice getVkDevice() const;

private:
    void createVulkanInstance(SDL_Window* window);
    void pickPhysicalDevice();
    void createDevice();
    void createCommandPool();

    VkInstance m_instance = nullptr;
    VkPhysicalDevice m_physicalDevice = nullptr;
    VkDevice m_device = nullptr;
    VkCommandPool m_commandPool = nullptr;

    std::optional<uint32_t> m_graphicsQueueFamily = {};
};
