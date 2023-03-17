#ifndef TEXMACS_UTILS_UTILS_HPP
#define TEXMACS_UTILS_UTILS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>

namespace texmacs {

    namespace utils {

        template <typename T> std::vector<T> splitString(const std::string& s, char delimiter);

        template <> inline std::vector<std::string> splitString<std::string>(const std::string& s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }

        template <> inline std::vector<int> splitString<int>(const std::string& s, char delimiter) {
            std::vector<int> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(std::stoi(token));
            }
            return tokens;
        }

        inline std::string readWholeFile(const std::filesystem::path &path) {
            std::ifstream file(path);
            std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return str;
        }

    }

}

#endif