#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <curl/curl.h>
#include <windows.h>
#include <iomanip>
#include <cstdint>

namespace fs = std::filesystem;


static size_t WriteData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}



bool downloadFile(const std::string& url, const std::string& filepath) {
    CURL* curl;
    FILE* fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        std::cout << "Starting download from: " << url << std::endl;
        fp = fopen(filepath.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        return (res == CURLE_OK);
    }
    return false;
}

int main() {
    HWND hwnd = GetConsoleWindow();
    std::wstring title = L"Maps Downloader";
    SetWindowText(hwnd, title.c_str());
    std::cout << "This .exe will download map files." << std::endl;
    std::cout << "Press enter to start." << std::endl;
    std::cin.get();

    std::string path = "csgo/maps/";
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".mapcfg") {
            std::cout << "Parsing mapcfg: " << entry.path().filename() << std::endl;
            std::ifstream mapConfig(entry.path());
            std::string line;
            std::string url, savepath, filename;

            while (getline(mapConfig, line)) {
                if (line.find("url = ") != std::string::npos) {
                    url = line.substr(line.find("url = ") + 6);
                } else if (line.find("path = ") != std::string::npos) {
                    savepath = line.substr(line.find("path = ") + 7);
                } else if (line.find("filename = ") != std::string::npos) {
                    filename = line.substr(line.find("filename = ") + 11);
                }
            }

            if (!url.empty() && !savepath.empty() && !filename.empty()) {
                // Creating savepath dir if it does not exists
                fs::create_directories(savepath);
                // Some debug info
                std::cout << "URL: " << url << std::endl;
                std::cout << "Save path: " << savepath << std::endl;
                std::cout << "Filename: " << filename << std::endl;

                std::string fullFilePath = savepath + filename;
                if (fs::exists(fullFilePath)) {
                    std::cout << "Map " << filename << " is already downloaded" << std::endl << std::endl;
                    continue;
                }
                // Downloading file
                if (!fs::exists(fullFilePath)) {
                    if (downloadFile(url, fullFilePath)) {
                        std::cout << "Downloaded: " << filename << std::endl;
                    }
                    else {
                        std::cerr << "Failed to download: " << filename << std::endl;
                    }
                }

            }
            std::cout << "Done parsing mapcfg: " << entry.path().filename() << std::endl << std::endl;
        }
    }
    std::cout << "Press enter to exit." << std::endl;
    std::cin.get();
    return 0;
}