#include <string>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

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
    CURL *curl = curl_easy_init();
    if(!curl) {
        std::cerr << "Error initializing curl" << std::endl;
        return 1;
    }

    nlohmann::json request_body;
    request_body["model"] = "gemini-3.8-flash";
    nlohmann::json message;
    message["role"] = "user";
    message["content"] = "Hello, how are you?";
    request_body["messages"] = nlohmann::json::array();
    request_body["messages"].push_back(message);
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
