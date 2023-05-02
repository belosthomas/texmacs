#include "Application.hpp"
#include "Utils/ArgsParser.hpp"
#include "Utils/PlatformDependant.hpp"
#include "tm_window.hpp"

//#include <QtWebView>

using namespace texmacs;

int main(int argc, char** argv) {


    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::system_clock::duration elapsed_seconds;

    // add start and elapsed_seconds
    std::chrono::time_point<std::chrono::system_clock> test = start + elapsed_seconds;

    // Find all the available Guile implementations
    initialize_schemes();

    // Enable high dpi scaling for Qt < 5.6. This does nothing for Qt >= 5.6.
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

    // Create the application
    texmacs::Application app = texmacs::Application(argc, argv);

    // Qt documentation says that QtWebView must be initialized before the QApplication
    // QtWebView::initialize();

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

    argsParser.option({"-h", "--help"}, "Print this help message", [&argsParser](std::vector<std::string> &, int) {
        argsParser.printHelp();
        cout << "\nPlease report bugs to <bugs@texmacs.org>\n";
        cout << "\n";
        exit(0);
        return 0;
    });

   // argsParser.option({"-s", "--silent"}, "Suppress information messages", showInformationMessages, false);
    argsParser.option({"-V", "--verbose"}, "Show some informative messages", debug_status, DEBUG_FLAG_VERBOSE);
    argsParser.option({"-d", "--debug"}, "For debugging purposes", debug_status, DEBUG_FLAG_STD);
    argsParser.option({"--debug-event"}, "", debug_status, DEBUG_FLAG_EVENTS);
    argsParser.option({"--debug-io"}, "", debug_status, DEBUG_FLAG_IO);
    argsParser.option({"--debug-bench"}, "", debug_status, DEBUG_FLAG_BENCH);
    argsParser.option({"--debug-history"}, "", debug_status, DEBUG_FLAG_HISTORY);
    argsParser.option({"--debug-qt"}, "", debug_status, DEBUG_FLAG_QT);
    argsParser.option({"--debug-qt-widgets"}, "", debug_status, DEBUG_FLAG_QT_WIDGETS);
    argsParser.option({"--debug-keyboard"}, "", debug_status, DEBUG_FLAG_KEYBOARD);
    argsParser.option({"--debug-packrat"}, "", debug_status, DEBUG_FLAG_PACKRAT);
    argsParser.option({"--debug-flatten"}, "", debug_status, DEBUG_FLAG_FLATTEN);
    argsParser.option({"--debug-parser"}, "", debug_status, DEBUG_FLAG_PARSER);
    argsParser.option({"--debug-correct"}, "", debug_status, DEBUG_FLAG_CORRECT);
    argsParser.option({"--debug-convert"}, "", debug_status, DEBUG_FLAG_CONVERT);
    argsParser.option({"--debug-remote"}, "", debug_status, DEBUG_FLAG_REMOTE);
    argsParser.option({"--debug-live"}, "", debug_status, DEBUG_FLAG_LIVE);
    argsParser.option({"--debug-all"}, "", [](std::vector<std::string> &args, int pos) {
        debug(DEBUG_FLAG_EVENTS, true);
        debug(DEBUG_FLAG_STD, true);
        debug(DEBUG_FLAG_IO, true);
        debug(DEBUG_FLAG_HISTORY, true);
        debug(DEBUG_FLAG_BENCH, true);
        debug(DEBUG_FLAG_QT, true);
        debug(DEBUG_FLAG_QT_WIDGETS, true);
        return 0;
    });

    //argsParser.option({"--disable-error-recovery"}, "", disable_error_recovery);
    //argsParser.option({"-fn", "--font"}, "Set the default TeX font", the_default_font);
    argsParser.optionVector2D({"-ws", "--windowSize"}, "Set the size of the window in pixel (example : 1920x1080)",
                              geometry_w, geometry_h, 'x');
    argsParser.optionVector2D({"-wp", "--windowPosition"}, "Set the position of the window in pixel (example: 10x20)",
                              geometry_x, geometry_y, 'x');

    argsParser.option({"-b", "--initialize-buffer"}, "Specify scheme buffers initialization file",
                      [](std::vector<std::string> &args, int pos) {
                          if (pos + 1 >= args.size()) {
                              return -1;
                          }
                          tm_init_buffer_file = url_system(args[pos + 1].c_str());
                          return 1;
                      });

    argsParser.option({"-i", "--initialize"}, "Specify scheme initialization file",
                      [](std::vector<std::string> &args, int pos) {
                          if (pos + 1 >= args.size()) {
                              return -1;
                          }
                          tm_init_file = url_system(args[pos + 1].c_str());
                          return 1;
                      });

    argsParser.option({"-v", "--version"}, "Display current TeXmacs version",
                      [](std::vector<std::string> &args, int pos) {
                          cout << "\n";
                          cout << "TeXmacs version " << TEXMACS_VERSION << "\n";
                          cout << "SVN version " << TEXMACS_REVISION << "\n";
                          cout << TEXMACS_COPYRIGHT << "\n";
                          cout << "\n";
                          exit(0);
                          return 0;
                      });

    argsParser.option({"-p", "--path"}, "Get the TeXmacs path", [](std::vector<std::string> &args, int pos) {
        cout << get_env("TEXMACS_PATH") << "\n";
        exit(0);
        return 0;
    });

    argsParser.option({"-bp", "--binpath"}, "", [](std::vector<std::string> &args, int pos) {
        cout << get_env("TEXMACS_BIN_PATH") << "\n";
        exit(0);
        return 0;
    });

    argsParser.option({"-q", "--quit"}, "Shortcut for -x \"(quit-TeXmacs)\"",
                      [](std::vector<std::string> &args, int pos) {
                          my_init_cmds = my_init_cmds * " (quit-TeXmacs)";
                          return 0;
                      });

    argsParser.option({"-r", "--reverse"}, "Reverse video mode", [](std::vector<std::string> &args, int pos) {
        set_reverse_colors(true);;
        return 0;
    });

    argsParser.option({"--no-retina"}, "", [](std::vector<std::string> &args, int pos) {
        retina_manual = true;
        retina_factor = 1;
        retina_zoom = 1;
        retina_icons = 1;
        retina_scale = 1.0;
        return 0;
    });

    argsParser.option({"R", "--retina"}, "", [](std::vector<std::string> &args, int pos) {
        retina_manual = true;
#ifdef MACOSX_EXTENSIONS
        retina_factor= 2;
        retina_zoom  = 1;
        retina_scale = 1.4;
#else
        retina_factor = 1;
        retina_zoom = 2;
        retina_scale = (tm_style_sheet == "" ? 1.0 : 1.6666);
#endif
        retina_icons = 2;
        return 0;
    });

    argsParser.option({"--no-retina-icons"}, "", [](std::vector<std::string> &args, int pos) {
        retina_iman = true;
        retina_icons = 1;
        return 0;
    });

    argsParser.option({"--retina-icons"}, "", [](std::vector<std::string> &args, int pos) {
        retina_iman = true;
        retina_icons = 2;
        return 0;
    });

    argsParser.option({"-c", "--convert"}, "Convert file 'i' into file 'o'",
                      [](std::vector<std::string> &args, int pos) {
                          if (pos + 2 >= args.size()) {
                              return -1;
                          }
                          url in("$PWD", args[pos + 1].c_str());
                          url out("$PWD", args[pos + 2].c_str());
                          my_init_cmds = my_init_cmds * " " *
                                         "(load-buffer " * scm_quote(as_string(in)) * " :strict) " *
                                         "(export-buffer " * scm_quote(as_string(out)) * ")";
                          return 2;
                      });

    argsParser.option({"-x", "--execute"}, "Execute scheme command", [](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            return -1;
        }
        my_init_cmds = (my_init_cmds * " ") * args[pos + 1].c_str();
        return 1;
    });

    /*argsParser.option({"--server"}, "", [](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            return -1;
        }
        start_server_flag = true;
        return 1;
    });

    argsParser.option({"-0c", "--no-char-clipping"}, "TeX characters bitmap clipping off", char_clip, false);
    argsParser.option({"+0c", "--char-clipping"}, "TeX characters bitmap clipping on (default)", char_clip, true);

    argsParser.option({"--build-manual"}, "", [](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            return -1;
        }
        extra_init_cmd << "(build-manual " << scm_quote(args[pos + 1].c_str()) << " delayed-quit)";
        return 1;
    });

    argsParser.option({"--reference-suite"}, "", [](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            return -1;
        }
        extra_init_cmd << "(build-ref-suite " << scm_quote(args[pos + 1].c_str()) << " delayed-quit)";
        return 1;
    });

    argsParser.option({"--test-suite"}, "", [](std::vector<std::string> &args, int pos) {
        if (pos + 1 >= args.size()) {
            return -1;
        }
        extra_init_cmd << "(run-test-suite " << scm_quote(args[pos + 1].c_str()) << "delayed-quit)";
        return 1;
    });
*/

    argsParser.parse(argc, argv);

    app.showSchemeImplementationChooserWidget();

    // Execute the application
    return app.exec();
}
