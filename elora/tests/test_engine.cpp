#include <catch.hpp>
#include <engine/engine.hpp>

TEST_CASE("Engine starts uninitialized") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    REQUIRE_FALSE(engine.is_initialized());
}

TEST_CASE("Engine init sets initialized") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    REQUIRE(engine.is_initialized());
}

TEST_CASE("Engine shutdown clears initialized") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.shutdown();
    REQUIRE_FALSE(engine.is_initialized());
}

TEST_CASE("Engine stores the given app name") {
    elora::Engine engine{"My App", 640, 480, 4, 4};
    REQUIRE(engine.app_name() == "My App");
}

TEST_CASE("Engine stores the given window size") {
    elora::Engine engine{"test", 320, 240, 4, 4};
    REQUIRE(engine.width() == 320);
    REQUIRE(engine.height() == 240);
}

TEST_CASE("Engine title includes name and size") {
    elora::Engine engine{"3D Demo", 640, 480, 4, 4};
    REQUIRE(engine.title() == "3D Demo 640x480");
}

TEST_CASE("Engine pixel size scales the framebuffer") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    REQUIRE(engine.pixel_width() == 4);
    REQUIRE(engine.pixel_height() == 4);
    REQUIRE(engine.buffer_width() == 160);
    REQUIRE(engine.buffer_height() == 120);
}

TEST_CASE("Engine pixel width and height are independent") {
    elora::Engine engine{"test", 640, 480, 8, 4};
    REQUIRE(engine.buffer_width() == 80);
    REQUIRE(engine.buffer_height() == 120);
}

TEST_CASE("Engine put_pixel writes to the framebuffer") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.put_pixel(10, 20, 0x00ff00);
    REQUIRE(engine.pixel(10, 20) == 0x00ff00);
}

TEST_CASE("Engine present without a window leaves the back buffer") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.put_pixel(4, 5, 0xabcdef);
    engine.present();
    REQUIRE(engine.pixel(4, 5) == 0xabcdef);
}

TEST_CASE("Engine put_pixel ignores out of bounds") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.put_pixel(-1, 0, 0xffffff);
    engine.put_pixel(0, -1, 0xffffff);
    engine.put_pixel(160, 0, 0xffffff);
    engine.put_pixel(0, 120, 0xffffff);
    REQUIRE(engine.pixel(0, 0) == 0);
}

TEST_CASE("Engine clear fills the framebuffer") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.clear(0x112233);
    REQUIRE(engine.pixel(0, 0) == 0x112233);
    REQUIRE(engine.pixel(159, 119) == 0x112233);
}

TEST_CASE("Engine fill_triangle covers interior pixels") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.fill_triangle(0, 0, 20, 0, 0, 20, 0xff0000);
    REQUIRE(engine.pixel(2, 2) == 0xff0000);
    REQUIRE(engine.pixel(1, 1) == 0xff0000);
}

TEST_CASE("Engine fill_triangle leaves outside pixels") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.fill_triangle(0, 0, 20, 0, 0, 20, 0xff0000);
    REQUIRE(engine.pixel(19, 19) == 0);
}

TEST_CASE("Engine fill_triangle skips degenerate") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.fill_triangle(5, 5, 5, 5, 10, 5, 0xffffff);
    REQUIRE(engine.pixel(5, 5) == 0);
}

TEST_CASE("Engine draw_line marks endpoints") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.draw_line(2, 3, 8, 3, 0x00ff00);
    REQUIRE(engine.pixel(2, 3) == 0x00ff00);
    REQUIRE(engine.pixel(5, 3) == 0x00ff00);
    REQUIRE(engine.pixel(8, 3) == 0x00ff00);
}

TEST_CASE("Engine stroke_triangle draws edges") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    engine.init();
    engine.stroke_triangle(0, 0, 10, 0, 0, 10, 0xffffff);
    REQUIRE(engine.pixel(0, 0) == 0xffffff);
    REQUIRE(engine.pixel(10, 0) == 0xffffff);
    REQUIRE(engine.pixel(0, 10) == 0xffffff);
    REQUIRE(engine.pixel(5, 0) == 0xffffff);
}

TEST_CASE("Engine keys start up") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    REQUIRE_FALSE(engine.is_key_down(elora::Key::W));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::A));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::T));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::P));
}
