#include "Application.hpp"
#include "Utils/ArgsParser.hpp"

#include <QtWebView>

int main(int argc, char** argv) {
    // Qt documentation says that QtWebView must be initialized before the QApplication
    QtWebView::initialize();

    // Find all the available Guile implementations
    register_all_scheme();

    // Create the application
    auto app = texmacs::Application(argc, argv);

    // Enable high dpi scaling for Qt < 5.6. This does nothing for Qt >= 5.6.
    app.setAttribute(Qt::AA_EnableHighDpiScaling);

    // Parse the command line arguments
    texmacs::utils::ArgsParser argsParser;

    argsParser.option({"-gi", "--guile-implementation"}, "Choose the Guile Implementation", [&app](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            std::cout << "List of available Guile implementations:" << std::endl;
            auto allSchemes = get_scheme_factories();
            if (allSchemes.empty()) {
                std::cout << "No scheme factories registered." << std::endl;
            }
            std::cout << "Available schemes:" << std::endl;
            for (const auto& scheme : allSchemes) {
                std::cout << " - " << scheme << std::endl;
            }
            exit(1);
        }
        app.setWantedSchemeImplementation(args[pos + 1]);
        return 1;
    });

    argsParser.parse(argc, argv);

    // Execute the application
    return app.exec();
}
