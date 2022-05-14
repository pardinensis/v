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

private:
    void createVulkanInstance(SDL_Window* window);
    void pickPhysicalDevice();

    VkInstance m_instance = nullptr;
    VkPhysicalDevice m_physicalDevice = nullptr;
};
