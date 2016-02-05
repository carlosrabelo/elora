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

TEST_CASE("Engine keys start up") {
    elora::Engine engine{"test", 640, 480, 4, 4};
    REQUIRE_FALSE(engine.is_key_down(elora::Key::W));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::A));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::T));
    REQUIRE_FALSE(engine.is_key_down(elora::Key::P));
}
