#ifndef TEXMACS_UTILS_ARGS_PARSER_HPP
#define TEXMACS_UTILS_ARGS_PARSER_HPP

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>

#include "Utils.hpp"

namespace texmacs {

    namespace utils {

        class ArgsParser {

            struct Option {

                Option(const std::vector<std::string> &_names, const std::string &_description, const std::function<int(std::vector<std::string>&, int)> &_callback) :
                    names(_names), description(_description), callback(_callback) {
                    concatenaedNames = names[0];
                    for (int i = 1; i < names.size(); ++i) {
                        concatenaedNames += ", " + names[i];
                    }
                }
                std::vector<std::string> names;
                std::string concatenaedNames;
                std::string description;
                std::function<int(std::vector<std::string>&, int)> callback;
            };

        public:
            ArgsParser() {

            }

            void option(const std::vector<std::string> &names, const std::string &description, std::function<int(std::vector<std::string>&, int)> callback) {
                mOptions.push_back({names, description, callback});
                for (auto &name : names) {
                    if (mOptionsMap.find(name) != mOptionsMap.end()) {
                        throw std::runtime_error("Option '" + name + "' already exists");
                    }
                    mOptionsMap[name] = mOptions.size() - 1;
                }
            }

            void option(const std::vector<std::string> &names, const std::string &description, bool &variable, bool value = true) {
                option(names, description, [&variable, value](std::vector<std::string>&, int) {
                    variable = value;
                    return 0;
                });
            }

            void option(const std::vector<std::string> &names, const std::string &description, long int &binary, int which) {
                option(names, description, [&binary, which](std::vector<std::string>&, int) {
                    binary = binary | (((long int) 1) << which);
                    return 0;
                });
            }

            template <typename T> void option(const std::vector<std::string> &names, const std::string &description, T &value) {
                option(names, description, [&value](std::vector<std::string>& args, int index) {
                    if (index + 1 >= args.size()) {
                        std::cerr << "Missing value for option '" << args[index] << "'" << std::endl;
                        return -1;
                    }
                    value = args[index + 1].c_str();
                    return 1;
                });
            }

            void optionVector2D(const std::vector<std::string> &names, const std::string &description, int &x, int &y, char delimiter = 'x') {
                option(names, description, [&x, &y, delimiter](std::vector<std::string> &args, int pos) {
                    if (pos + 1 >= args.size()) {
                        return -1;
                    }
                    auto windowPosition = texmacs::utils::splitString<int>(args[pos + 1], delimiter);
                    x = windowPosition[0];
                    y = windowPosition[1];
                    return 1;
                });
            }

            void printHelp() {
                std::cout << "Usage: texmacs [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                int maxNameLength = 0;
                for (auto &option : mOptions) {
                    if (option.description.empty()) {
                        continue;
                    }
                    maxNameLength = std::max(maxNameLength, (int)option.concatenaedNames.size());
                }
                for (auto &option : mOptions) {
                    if (option.description.empty()) {
                        continue;
                    }
                    std::cout << "  ";
                    std::cout << option.concatenaedNames;
                    for (int i = 0; i < maxNameLength - option.concatenaedNames.size(); ++i) {
                        std::cout << " ";
                    }
                    std::cout << "\b\b  " << option.description << std::endl;
                }
            }

            void parse(int argc, char *argv[]) {
                std::vector<std::string> args(argv, argv + argc);
                for (int i = 1; i < args.size(); ++i) {
                    if (args[i][0] == '-') {
                        auto it = mOptionsMap.find(args[i]);
                        if (it == mOptionsMap.end()) {
                            std::cerr << "Unknown option '" << args[i] << "'" << std::endl;
                            printHelp();
                            exit(1);
                        }
                        int result = mOptions[it->second].callback(args, i);
                        if (result < 0) {
                            printHelp();
                            exit(1);
                        }
                        i += result;
                    }
                }
            }


        private:
            std::vector<std::string> mArgs;
            std::vector<Option> mOptions;
            std::unordered_map<std::string, int> mOptionsMap;
        };

    }

}

#endif