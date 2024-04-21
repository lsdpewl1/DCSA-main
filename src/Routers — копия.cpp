#include "Routers.hpp"
#include "Utility.hpp"

// Middleware для базовой аутентификации
void basicAuthMiddleware(HttpRequest* req, HttpResponse* resp, std::function<void()> next) {
    auto basic_auth = req->GetHeader("Authorization");

    if (!basic_auth.empty()) {
        auto splited_header = utils::Split(basic_auth, " ");

        if (splited_header.size() == 2 && splited_header.front() == "Basic") {
            auto decode = utils::DecodeBase64(splited_header.back());
            auto splited_auth = utils::Split(decode, ":");

            if (splited_auth.size() == 2) {
                std::string username = splited_auth.front();
                std::string password = splited_auth.back();

                // Здесь вы должны проверить имя пользователя и пароль
                if (checkUserCredentials(username, password)) {
                    // Если они действительны, вызовите next()
                    next();
                } else {
                    // Если они недействительны, отправьте ответ 401 Unauthorized
                    resp->status_code = 401;
                    resp->SetBody("Unauthorized");
                }
            }
        }
    } else {
        resp->status_code = 401;
        resp->SetBody("Unauthorized");
    }
}

void route::RegisterResources(hv::HttpService &router)
{
    router.GET("/hello", [](HttpRequest *req, HttpResponse *resp)
    {
        auto basic_auth = req->GetHeader("Authorization");

        if (!basic_auth.empty())
        {
            auto splited_header = utils::Split(basic_auth, " ");

            if (splited_header.size() == 2 && splited_header.front() == "Basic")
            {
                auto decode = utils::DecodeBase64(splited_header.back());
                auto splited_auth = utils::Split(decode, ":");

                if (splited_auth.size() == 2)
                {
                    std::cout << splited_auth.front() << " : " << splited_auth.back() << std::endl;
                }
            }
        }

        nlohmann::json response
        {
            {"msg", "Hello world"}
        };

        resp->SetBody(response.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

    router.POST("/hello", [](HttpRequest *req, HttpResponse *resp)
    {
        nlohmann::json request;
        nlohmann::json response;

        try
        {
            request = nlohmann::json::parse(req->body);
        }
        catch(const std::exception& e)
        {
            response["error"] = "Invalid JSON";
            resp->SetBody(response.dump());
            resp->content_type = APPLICATION_JSON;
            return 400;
        }
        
        resp->SetBody(request.dump());
        resp->content_type = APPLICATION_JSON;

        return 200;
    });

}