#include <engine/engine.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

bool Engine::is_key_down(Key key) const {
    const auto index = static_cast<std::size_t>(key);
    if (index >= static_cast<std::size_t>(Key::Count)) {
        return false;
    }
    return keys_[index];
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
