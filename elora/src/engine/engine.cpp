#include <engine/engine.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <utility>

namespace elora {

struct Engine::Graphics {
    ::Display* display = nullptr;
    ::Window window = 0;
    ::GC gc = {};
    ::XImage* image = nullptr;
    std::vector<std::uint32_t> display_buffer;
    ::Atom wm_delete = 0;
};

Engine::Engine(std::string app_name, int width, int height, int pixel_width, int pixel_height)
    : app_name_(std::move(app_name)),
      width_(width),
      height_(height),
      pixel_width_(pixel_width),
      pixel_height_(pixel_height) {}

Engine::~Engine() {
    shutdown();
}

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

void Engine::clear(std::uint32_t color) {
    std::fill(back_buffer_.begin(), back_buffer_.end(), color);
}

void Engine::put_pixel(int x, int y, std::uint32_t color) {
    const int bw = buffer_width();
    const int bh = buffer_height();
    if (x < 0 || y < 0 || x >= bw || y >= bh) {
        return;
    }
    back_buffer_[static_cast<std::size_t>(y) * static_cast<std::size_t>(bw) + static_cast<std::size_t>(x)] = color;
}

void Engine::draw_line(int x0, int y0, int x1, int y1, std::uint32_t color) {
    const int dx = std::abs(x1 - x0);
    const int sx = x0 < x1 ? 1 : -1;
    const int dy = -std::abs(y1 - y0);
    const int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    for (;;) {
        put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        const int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Engine::stroke_triangle(int x0, int y0, int x1, int y1, int x2, int y2, std::uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

namespace {

// 5x7 glyphs; bit 4 is the leftmost column.
const std::uint8_t kFontDigit[10][7] = {
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
    {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F},
    {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E},
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
};
const std::uint8_t kFontF[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
const std::uint8_t kFontP[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
const std::uint8_t kFontS[7] = {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E};

const std::uint8_t* glyph_rows(char c) {
    if (c >= '0' && c <= '9') {
        return kFontDigit[c - '0'];
    }
    if (c == 'F' || c == 'f') {
        return kFontF;
    }
    if (c == 'P' || c == 'p') {
        return kFontP;
    }
    if (c == 'S' || c == 's') {
        return kFontS;
    }
    return nullptr;
}

}  // namespace

void Engine::draw_text(int x, int y, const char* text, std::uint32_t color, int scale) {
    if (text == nullptr || scale < 1) {
        return;
    }
    int cursor = x;
    for (const char* p = text; *p != '\0'; ++p) {
        const std::uint8_t* rows = glyph_rows(*p);
        if (rows != nullptr) {
            for (int row = 0; row < 7; ++row) {
                const std::uint8_t bits = rows[row];
                for (int col = 0; col < 5; ++col) {
                    if ((bits & (0x10 >> col)) == 0) {
                        continue;
                    }
                    const int px = cursor + col * scale;
                    const int py = y + row * scale;
                    for (int sy = 0; sy < scale; ++sy) {
                        for (int sx = 0; sx < scale; ++sx) {
                            put_pixel(px + sx, py + sy, color);
                        }
                    }
                }
            }
        }
        cursor += 6 * scale;
    }
}

float Engine::delta_time() const {
    return dt_;
}

float Engine::fps() const {
    return fps_;
}

namespace {

int edge(int ax, int ay, int bx, int by, int px, int py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

}  // namespace

void Engine::fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2, std::uint32_t color) {
    const int bw = buffer_width();
    const int bh = buffer_height();
    if (bw <= 0 || bh <= 0 || back_buffer_.empty()) {
        return;
    }

    const int area = edge(x0, y0, x1, y1, x2, y2);
    if (area == 0) {
        return;
    }

    const int min_x = std::max(0, std::min(x0, std::min(x1, x2)));
    const int min_y = std::max(0, std::min(y0, std::min(y1, y2)));
    const int max_x = std::min(bw - 1, std::max(x0, std::max(x1, x2)));
    const int max_y = std::min(bh - 1, std::max(y0, std::max(y1, y2)));

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            const int w0 = edge(x1, y1, x2, y2, x, y);
            const int w1 = edge(x2, y2, x0, y0, x, y);
            const int w2 = edge(x0, y0, x1, y1, x, y);
            const bool inside = (area > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0)
                                           : (w0 <= 0 && w1 <= 0 && w2 <= 0);
            if (inside) {
                put_pixel(x, y, color);
            }
        }
    }
}

std::uint32_t Engine::pixel(int x, int y) const {
    const int bw = buffer_width();
    const int bh = buffer_height();
    if (x < 0 || y < 0 || x >= bw || y >= bh) {
        return 0;
    }
    return back_buffer_[static_cast<std::size_t>(y) * static_cast<std::size_t>(bw) + static_cast<std::size_t>(x)];
}

void Engine::present() {
    // Sole path from back buffer to the window. Drawing APIs never call X11.
    if (!graphics_ || graphics_->display == nullptr || graphics_->image == nullptr) {
        return;
    }

    const int bw = buffer_width();
    const int bh = buffer_height();
    auto* dst = graphics_->display_buffer.data();
    for (int y = 0; y < bh; ++y) {
        for (int x = 0; x < bw; ++x) {
            const std::uint32_t color =
                back_buffer_[static_cast<std::size_t>(y) * static_cast<std::size_t>(bw) +
                             static_cast<std::size_t>(x)];
            for (int py = 0; py < pixel_height_; ++py) {
                const int dy = y * pixel_height_ + py;
                auto* row = dst + static_cast<std::size_t>(dy) * static_cast<std::size_t>(width_);
                for (int px = 0; px < pixel_width_; ++px) {
                    row[x * pixel_width_ + px] = color;
                }
            }
        }
    }

    XPutImage(graphics_->display, graphics_->window, graphics_->gc, graphics_->image, 0, 0, 0, 0,
              static_cast<unsigned int>(width_), static_cast<unsigned int>(height_));
    XFlush(graphics_->display);
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

bool Engine::is_key_down(Key key) const {
    const auto index = static_cast<std::size_t>(key);
    if (index >= static_cast<std::size_t>(Key::Count)) {
        return false;
    }
    return keys_[index];
}

bool Engine::open_window() {
    if (graphics_ && graphics_->display != nullptr) {
        return true;
    }

    graphics_ = std::make_unique<Graphics>();
    graphics_->display = XOpenDisplay(nullptr);
    if (graphics_->display == nullptr) {
        graphics_.reset();
        return false;
    }

    const int screen = DefaultScreen(graphics_->display);
    ::Window root = RootWindow(graphics_->display, screen);

    XSetWindowAttributes attrs{};
    attrs.background_pixel = BlackPixel(graphics_->display, screen);
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | FocusChangeMask;

    graphics_->window = XCreateWindow(graphics_->display, root, 0, 0, static_cast<unsigned int>(width_),
                                      static_cast<unsigned int>(height_), 0, DefaultDepth(graphics_->display, screen),
                                      InputOutput, DefaultVisual(graphics_->display, screen),
                                      CWBackPixel | CWEventMask, &attrs);

    XSizeHints hints{};
    hints.flags = PMinSize | PMaxSize | PBaseSize;
    hints.min_width = hints.max_width = hints.base_width = width_;
    hints.min_height = hints.max_height = hints.base_height = height_;
    XSetWMNormalHints(graphics_->display, graphics_->window, &hints);

    const std::string window_title = title();
    XStoreName(graphics_->display, graphics_->window, window_title.c_str());

    graphics_->wm_delete = XInternAtom(graphics_->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(graphics_->display, graphics_->window, &graphics_->wm_delete, 1);

    graphics_->gc = XCreateGC(graphics_->display, graphics_->window, 0, nullptr);

    graphics_->display_buffer.assign(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_), 0);
    graphics_->image =
        XCreateImage(graphics_->display, DefaultVisual(graphics_->display, screen),
                     static_cast<unsigned int>(DefaultDepth(graphics_->display, screen)), ZPixmap, 0,
                     reinterpret_cast<char*>(graphics_->display_buffer.data()), static_cast<unsigned int>(width_),
                     static_cast<unsigned int>(height_), 32, 0);
    if (graphics_->image == nullptr) {
        close_window();
        return false;
    }

    XMapWindow(graphics_->display, graphics_->window);
    XFlush(graphics_->display);
    return true;
}

void Engine::close_window() {
    if (!graphics_) {
        return;
    }

    if (graphics_->image != nullptr) {
        graphics_->image->data = nullptr;
        XDestroyImage(graphics_->image);
        graphics_->image = nullptr;
    }
    if (graphics_->gc != 0 && graphics_->display != nullptr) {
        XFreeGC(graphics_->display, graphics_->gc);
        graphics_->gc = {};
    }
    if (graphics_->window != 0 && graphics_->display != nullptr) {
        XDestroyWindow(graphics_->display, graphics_->window);
        graphics_->window = 0;
    }
    if (graphics_->display != nullptr) {
        XCloseDisplay(graphics_->display);
        graphics_->display = nullptr;
    }
    graphics_.reset();
}

void Engine::process_events(bool block) {
    if (!graphics_ || graphics_->display == nullptr) {
        return;
    }

    auto apply_key = [this](KeySym sym, bool down) {
        switch (sym) {
        case XK_a:
        case XK_A:
            keys_[static_cast<std::size_t>(Key::A)] = down;
            break;
        case XK_d:
        case XK_D:
            keys_[static_cast<std::size_t>(Key::D)] = down;
            break;
        case XK_r:
        case XK_R:
            keys_[static_cast<std::size_t>(Key::R)] = down;
            break;
        case XK_f:
        case XK_F:
            keys_[static_cast<std::size_t>(Key::F)] = down;
            break;
        case XK_w:
        case XK_W:
            keys_[static_cast<std::size_t>(Key::W)] = down;
            break;
        case XK_s:
        case XK_S:
            keys_[static_cast<std::size_t>(Key::S)] = down;
            break;
        case XK_t:
        case XK_T:
            keys_[static_cast<std::size_t>(Key::T)] = down;
            break;
        case XK_p:
        case XK_P:
            keys_[static_cast<std::size_t>(Key::P)] = down;
            break;
        case XK_c:
        case XK_C:
            keys_[static_cast<std::size_t>(Key::C)] = down;
            break;
        case XK_Left:
            keys_[static_cast<std::size_t>(Key::Left)] = down;
            break;
        case XK_Right:
            keys_[static_cast<std::size_t>(Key::Right)] = down;
            break;
        case XK_Up:
            keys_[static_cast<std::size_t>(Key::Up)] = down;
            break;
        case XK_Down:
            keys_[static_cast<std::size_t>(Key::Down)] = down;
            break;
        case XK_space:
            keys_[static_cast<std::size_t>(Key::Space)] = down;
            break;
        case XK_Escape:
            keys_[static_cast<std::size_t>(Key::Escape)] = down;
            if (down) {
                running_ = false;
            }
            break;
        case XK_q:
        case XK_Q:
            if (down) {
                running_ = false;
            }
            break;
        default:
            break;
        }
    };

    auto handle = [this, &apply_key](XEvent& event) {
        switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0) {
                present();
            }
            break;
        case ClientMessage:
            if (static_cast<Atom>(event.xclient.data.l[0]) == graphics_->wm_delete) {
                running_ = false;
            }
            break;
        case FocusOut:
            keys_.fill(false);
            break;
        case KeyPress:
            apply_key(XLookupKeysym(&event.xkey, 0), true);
            break;
        case KeyRelease: {
            if (XEventsQueued(graphics_->display, QueuedAfterReading) > 0) {
                XEvent next;
                XPeekEvent(graphics_->display, &next);
                if (next.type == KeyPress && next.xkey.time == event.xkey.time &&
                    next.xkey.keycode == event.xkey.keycode) {
                    break;
                }
            }
            apply_key(XLookupKeysym(&event.xkey, 0), false);
            break;
        }
        default:
            break;
        }
    };

    if (block) {
        XEvent event;
        XNextEvent(graphics_->display, &event);
        handle(event);
    }
    while (running_ && XPending(graphics_->display) > 0) {
        XEvent event;
        XNextEvent(graphics_->display, &event);
        handle(event);
    }
}

}  // namespace elora
