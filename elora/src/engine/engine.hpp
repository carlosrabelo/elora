#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace elora {

enum class Key {
    A,
    D,
    R,
    F,
    W,
    S,
    T,
    P,
    Left,
    Right,
    Up,
    Down,
    Escape,
    Space,
    Count
};

class Engine {
public:
    Engine(std::string app_name, int width, int height, int pixel_width, int pixel_height);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void init();
    void shutdown();
    bool is_initialized() const;
    bool is_running() const;
    bool run(std::function<void()> on_frame = {});

    // Drawing writes the back buffer only. The window is unchanged until present().
    void clear(std::uint32_t color);
    void put_pixel(int x, int y, std::uint32_t color);
    void draw_line(int x0, int y0, int x1, int y1, std::uint32_t color);
    void fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, std::uint32_t color);
    void stroke_triangle(int x0, int y0, int x1, int y1, int x2, int y2, std::uint32_t color);
    std::uint32_t pixel(int x, int y) const;
    void present();

    const std::string& app_name() const;
    int width() const;
    int height() const;
    int pixel_width() const;
    int pixel_height() const;
    int buffer_width() const;
    int buffer_height() const;
    std::string title() const;

    bool is_key_down(Key key) const;

private:
    struct Graphics;

    bool open_window();
    void close_window();
    void process_events(bool block);

    std::string app_name_;
    int width_;
    int height_;
    int pixel_width_;
    int pixel_height_;
    bool initialized_{false};
    bool running_{false};
    std::vector<std::uint32_t> back_buffer_;
    std::unique_ptr<Graphics> graphics_;
    std::array<bool, static_cast<std::size_t>(Key::Count)> keys_{};
};

}  // namespace elora
