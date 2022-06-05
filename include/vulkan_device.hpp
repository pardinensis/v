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

    VkInstance m_vkInstance = nullptr;
    VkPhysicalDevice m_vkPhysicalDevice = nullptr;
    VkDevice m_vkDevice = nullptr;
    VkCommandPool m_vkCommandPool = nullptr;

    std::optional<uint32_t> m_graphicsQueueFamily = {};
};
