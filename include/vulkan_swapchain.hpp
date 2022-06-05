#pragma once

#include "common_includes.hpp"
#include "vulkan_device.hpp"


class VulkanSwapchain {
public:
    // IMPORTANT: the VulkanDevice object must outlive the VulkanSwapchain object
    VulkanSwapchain(const std::shared_ptr<VulkanDevice>& device);
    ~VulkanSwapchain();

    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain(VulkanSwapchain&&) = delete;
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

private:
    const std::shared_ptr<VulkanDevice>& m_device;
};
