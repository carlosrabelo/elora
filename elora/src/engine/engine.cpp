#include <engine/engine.hpp>

#include <chrono>
#include <cstddef>

namespace elora {

void Engine::init() {
    const int bw = buffer_width();
    const int bh = buffer_height();
    const auto count = static_cast<std::size_t>(bw) * static_cast<std::size_t>(bh);
    back_buffer_.assign(count, 0);
    dt_ = 0;
    fps_ = 0;
    initialized_ = true;
}

void Engine::shutdown() {
    running_ = false;
    keys_.fill(false);
    close_window();
    back_buffer_.clear();
    initialized_ = false;
}

bool Engine::is_initialized() const {
    return initialized_;
}

bool Engine::is_running() const {
    return running_;
}

bool Engine::run(std::function<void()> on_frame) {
    if (!initialized_) {
        return false;
    }
    if (!open_window()) {
        return false;
    }
    running_ = true;
    if (!on_frame) {
        present();
        while (running_) {
            process_events(true);
        }
        return true;
    }

    using clock = std::chrono::steady_clock;
    constexpr float max_dt = 0.1f;
    last_tick_ = clock::now();
    dt_ = 0;
    fps_ = 0;
    while (running_) {
        process_events(false);
        if (!running_) {
            break;
        }
        const auto now = clock::now();
        dt_ = std::chrono::duration<float>(now - last_tick_).count();
        if (dt_ > max_dt) {
            dt_ = max_dt;
        }
        last_tick_ = now;
        if (dt_ > 0.0f) {
            const float instant = 1.0f / dt_;
            fps_ = (fps_ <= 0.0f) ? instant : (fps_ * 0.9f + instant * 0.1f);
        }
        on_frame();
        present();
    }
    return true;
}

float Engine::delta_time() const {
    return dt_;
}

float Engine::fps() const {
    return fps_;
}

const std::string& Engine::app_name() const {
    return app_name_;
}

int Engine::width() const {
    return width_;
}

int Engine::height() const {
    return height_;
}

int Engine::pixel_width() const {
    return pixel_width_;
}

int Engine::pixel_height() const {
    return pixel_height_;
}

int Engine::buffer_width() const {
    return width_ / pixel_width_;
}

int Engine::buffer_height() const {
    return height_ / pixel_height_;
}

std::string Engine::title() const {
    return app_name_ + " " + std::to_string(width_) + "x" + std::to_string(height_);
}

}  // namespace elora
