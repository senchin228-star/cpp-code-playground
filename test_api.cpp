#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <initializer_list>

std::string base64_encode(const std::string& in) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');
    return out;
}

std::string read_file(const std::string& filename) {
    std::ifstream f(filename, std::ios::binary);
    if (!f){
         throw std::runtime_error("Could not open file: " + filename);
    }
    f.seekg(0, std::ios::end);
    std::streamsize size = f.tellg();
    if (size < 0) {
        throw std::runtime_error("Could not get size of file: " + filename);
    }
    f.seekg(0, std::ios::beg);
    std::string s(size, '\0');
    f.read(&s[0], size);
    if (f.gcount() != size) {
        throw std::runtime_error("Error reading file: " + filename);
    }
    return s;
}

std::string detect_image_mime_type(const std::string& image_bin) {
    const auto matches = [&image_bin](std::initializer_list<unsigned char> signature) {
        if (image_bin.size() < signature.size()) {
            return false;
        }
        size_t i = 0;
        for (unsigned char byte : signature) {
            if (static_cast<unsigned char>(image_bin[i++]) != byte) {
                return false;
            }
        }
        return true;
    };

    if (matches({0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A})) return "image/png";
    if (matches({0xFF, 0xD8, 0xFF})) return "image/jpeg";
    if (matches({'G', 'I', 'F', '8', '7', 'a'}) || matches({'G', 'I', 'F', '8', '9', 'a'})) return "image/gif";
    if (matches({'R', 'I', 'F', 'F'}) && image_bin.size() >= 12 &&
        image_bin.compare(8, 4, "WEBP") == 0) return "image/webp";
    if (matches({'B', 'M'})) return "image/bmp";
    if (matches({0x49, 0x49, 0x2A, 0x00}) || matches({0x4D, 0x4D, 0x00, 0x2A})) return "image/tiff";

    throw std::runtime_error("Unsupported or unrecognized image format");
}

size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    std::string* out = static_cast<std::string*>(userdata);
    out->append(ptr, size * nmemb);
    return size * nmemb;
}
int main()
{
    const char* api_key = std::getenv("MIXEN_API_KEY");
    if (!api_key) {
        std::cerr << "Error: MIXEN_API_KEY environment variable is not set" << std::endl;
        return 1;
    }
    std::string image_mime_type;
    std::string image_base64;
    std::cout << "Do you want to add a photo? (y/n): ";
    std::string add_photo;
    std::getline(std::cin, add_photo);
    if (add_photo == "y" || add_photo == "Y" || add_photo == "yes" || add_photo == "YES") {
        std::cout << "Enter image filename: ";
        std::string image_filename;
        std::getline(std::cin, image_filename);

        try {
            std::string image_bin = read_file(image_filename);
            image_mime_type = detect_image_mime_type(image_bin);
            image_base64 = base64_encode(image_bin);
        } catch (const std::exception& e) {
            std::cerr << "Error loading image: " << e.what() << std::endl;
            return 1;
        }
    }

    CURL *curl = curl_easy_init();
    if(!curl) {
        std::cerr << "Error initializing curl" << std::endl;
        return 1;
    }
    std::cout << "Enter Your Prompt: ";
    std::string user_input;
    std::getline(std::cin, user_input);

    nlohmann::json request_body;
    request_body["model"] = "gemini-3.8-flash";
    request_body["messages"][0]["role"] = "user";
    request_body["messages"][0]["content"][0]["type"] = "text";
    request_body["messages"][0]["content"][0]["text"] = user_input;
    if (!image_base64.empty()) {
        request_body["messages"][0]["content"][1]["type"] = "image_url";
        request_body["messages"][0]["content"][1]["image_url"]["url"] = "data:" + image_mime_type + ";base64," + image_base64;
    }
    std::string request_body_str = request_body.dump();

    std::string response;

    std::string auth_header = "Authorization: Bearer " + std::string(api_key);
    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.mixen.ai/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body_str.c_str());

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "Error performing curl request" << std::endl;
        std::cerr << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return 1;
    }
    curl_slist_free_all(headers);
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (code != 200) {
        std::cerr << "Error: Received HTTP response code " << code << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }
    try {
        nlohmann::json data = nlohmann::json::parse(response);
        if (!data.contains("choices") || !data["choices"][0].contains("message")) {
            std::cerr << "Unexpected JSON structure" << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }
        if (!data["choices"][0]["message"].contains("content")) {
            std::cerr << "Unexpected JSON structure" << std::endl;
            curl_easy_cleanup(curl);
            return 1;
        }
        std::string content = data["choices"][0]["message"]["content"];
        std::cout << "Response from API: " << content << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
        curl_easy_cleanup(curl);
        return 1;
    }
    curl_easy_cleanup(curl);
    return 0;
}
