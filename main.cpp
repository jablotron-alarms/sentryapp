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
    sentry_options_set_dsn(options, "https://6ae9408c92aa4fde82862d32ac9deba5@o504248.ingest.sentry.io/5591753");
    // sentry_options_set_release(options, "testapp@0.1.0");
    sentry_init(options);

    #if 0 // defined(__linux__)
        // https://thomastrapp.com/blog/signal-handler-for-multithreaded-c++/
        // Block signals in this thread and subsequently spawned threads
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGINT);
        sigaddset(&sigset, SIGTERM);
        sigaddset(&sigset, SIGSEGV);
        sigaddset(&sigset, SIGABRT);
        pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

        auto signal_handler = std::async(std::launch::async, [&sigset, &shutdown_requested] {
            int signum = 0;
            sigwait(&sigset, &signum);
            shutdown_requested.store(true);
            return signum;
        });
    #else
        auto signal_handler = std::async(std::launch::async, [] { return 0; });
    #endif

    CLI::App app{"testapp"};

    std::optional<std::string> except{std::nullopt};
    app.add_option("-e,--except", except, "Throw uncaught exception with message");

    bool crash{false};
    app.add_flag("-c,--crash", crash, "Crash");

    std::optional<std::string> log{std::nullopt};
    app.add_option("-l,--log", log, "Log with message");

    bool sleep_indefinitely{false};
    app.add_flag("-s,--sleep", sleep_indefinitely, "Sleep indefinitely");

    CLI11_PARSE(app, argc, argv);

    auto main_worker = std::async(std::launch::async, [log, crash, except, &shutdown_requested] {
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

        while (shutdown_requested.load() == false)
        {
            // do some work
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cerr << ".";
        }

        return shutdown_requested.load();
    });

    // int signum = signal_handler.get();
    // std::cout << "received signal " << signal << std::endl;
    // bool worker_result = main_worker.get();
    // app_cleanup();
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    bool worker_result = main_worker.get();
    return 0;
}