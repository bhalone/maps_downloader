#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <curl/curl.h>

namespace fs = std::filesystem;

// Функция для скачивания файла с помощью libcurl
static size_t WriteData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool downloadFile(const std::string& url, const std::string& filepath) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filepath.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        return (res == CURLE_OK);
    }
    return false;
}

int main() {
    std::cout << "This .exe will download map files" << std::endl;
    std::cout << "Press any key to start" << std::endl;
    std::cin.get();

    std::string path = "csgo/maps/";
    for (const auto & entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".mapcfg") {
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
                // Убедитесь, что путь существует или создайте его
                fs::create_directories(savepath);

                // Скачивание файла
                if (downloadFile(url, savepath + filename)) {
                    std::cout << "Downloaded: " << filename << std::endl;
                } else {
                    std::cerr << "Failed to download: " << filename << std::endl;
                }
            }
        }
    }
    std::cout << "Press any key to exit" << std::endl;
    std::cin.get();
    return 0;
}