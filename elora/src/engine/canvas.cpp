#include <engine/engine.hpp>
#include <engine/font.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace elora {
namespace {

int edge(int ax, int ay, int bx, int by, int px, int py) {
    return (bx - ax) * (py - ay) - (by - ay) * (px - ax);
}

}  // namespace

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

void Engine::draw_text(int x, int y, const char* text, std::uint32_t color, int scale) {
    if (text == nullptr || scale < 1) {
        return;
    }
    int cursor = x;
    for (const char* p = text; *p != '\0'; ++p) {
        const std::uint8_t* rows = font::glyph_rows(*p);
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

}  // namespace elora
