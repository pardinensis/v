#include "app.hpp"

#include "common_includes.hpp"
#include "config.hpp"
#include "vulkan_device.hpp"

#include <SDL2/SDL.h>

#include <functional>
#include <stack>


struct App::Impl {
private:
    SDL_Window* m_window;
    std::stack<std::function<void()>> m_cleanupStack;
    std::unique_ptr<VulkanDevice> m_device;

public:
    Impl() {
        // init sdl
        SDL_Init(SDL_INIT_VIDEO);
        m_cleanupStack.push(SDL_Quit);

        // create window
        uint32_t flags = SDL_WINDOW_VULKAN;
        if (Config::get<bool>("window.fullscreen", false)) flags |= SDL_WINDOW_FULLSCREEN;
        if (Config::get<bool>("window.fullscreen_desktop", false)) flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        m_window = SDL_CreateWindow(
            /* title  */ Config::get<std::string>("application.name").c_str(),
            /* x      */ SDL_WINDOWPOS_CENTERED,
            /* y      */ SDL_WINDOWPOS_CENTERED,
            /* width  */ Config::get<int>("window.width", 640),
            /* height */ Config::get<int>("window.height", 480),
            /* flags  */ flags
        );
        if (m_window == nullptr) {
            throw std::runtime_error(std::string("failed to create window: ") + SDL_GetError());
        }
        m_cleanupStack.push(std::bind(SDL_DestroyWindow, m_window));

        // create the vulkan device
        m_device = std::make_unique<VulkanDevice>(m_window);
        m_cleanupStack.push([this](){ m_device.reset(); });
    }

    ~Impl() {
        while (!m_cleanupStack.empty()) {
            m_cleanupStack.top()();
            m_cleanupStack.pop();
        }
    }

    void run() {
        SDL_Event event;
        bool running = true;
        while (running) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
                }
            }
        }
    }
};


App::App() {
    m_pImpl = std::make_unique<App::Impl>();
}

App::~App() {
}

void App::run() {
    m_pImpl->run();
}
