#include <iostream>
#include <string>
#include <optional>
#include <cstring>
#include <cstdlib>
#include <exception>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>

#include <CLI/CLI.hpp>
#include <sentry.h>

static void *invalid_mem = (void *)1;

static void trigger_crash()
{
    std::memset((char *)invalid_mem, 1, 100);
}

void app_cleanup()
{
    static bool exited = false;

    if (exited) {
        return;
    }
    
    exited = true;
    std::cout << "cleaning up" << std::endl;
    sentry_shutdown();
    std::cout << "done cleaning up" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "starting up" << std::endl;

    std::atomic<bool> shutdown_requested{false};

    sentry_options_t *options = sentry_options_new();
    // sentry_options_set_release(options, "testapp@0.1.0");
    sentry_init(options);

    CLI::App app{"testapp"};

    std::optional<std::string> except{std::nullopt};
    app.add_option("-e,--except", except, "Throw uncaught exception with message");

    bool crash{false};
    app.add_flag("-c,--crash", crash, "Crash");

    std::optional<std::string> log{std::nullopt};
    app.add_option("-l,--log", log, "Log with message");

    CLI11_PARSE(app, argc, argv);

    if (log)
    {
        std::cout << "log " << log.value() << std::endl;
        sentry_capture_event(sentry_value_new_message_event(
            /*   level */ SENTRY_LEVEL_INFO,
            /*  logger */ "custom",
            /* message */ log.value().data()
        ));
    }

    if (crash)
    {
        std::cout << "crash" << std::endl;
        trigger_crash();
    }

    if (except)
    {
        std::cout << "except " << except.value() << std::endl;
        throw std::runtime_error(except.value());
    }

    sentry_shutdown();
    return 0;
}