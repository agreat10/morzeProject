#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Network.hpp>

#include <ClientWebSocketThread.h>

#include <thread>

#include <map>
#include <string>

std::map<char, std::string> alp {
        {'A', "*-"},{'B', "*--"},{'C', "-*-*"},{'D', "-**"},{'E', "*"},{'F', "**-*"},{'G', "--*"},{'H', "****"},{'I', "**"},
        {'J', "*---"},{'K', "-*-"},{'L', "*-**"},{'M', "--"},{'N', "-*"},{'O', "---"},{'P', "*--*"},{'Q', "--*-"},{'R', "*-*"},
        {'S', "***"},{'T', "-"},{'U', "**-"},{'V', "***-"},{'W', "*--"},{'X', "-**-"},{'Y', "-*--"},{'Z', "--**"}
};

bool finishedFlag = true;
char bufferFromTextInput[1024];
std::string message_;
sf::Clock clock1;

void display(ImVec2 size={})
{
    ImGui::Begin("Morze", nullptr, ImGuiWindowFlags_NoMove + ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoTitleBar);
    ImGui::SetWindowPos({0, 0});
    ImGui::SetWindowSize(size);
    if(finishedFlag)
    {
        ImGui::InputText("Input text", bufferFromTextInput, IM_ARRAYSIZE(bufferFromTextInput));
        if(ImGui::Button("Send"))
        {
            std::string tmp = bufferFromTextInput;
            for(auto item: tmp)
            {
                auto pos = alp.find(std::toupper(item));
                if(pos!=alp.end())
                {
                    message_ += " ";
                    message_ += pos->second;

                }
            }
            SendMessage(message_);
            message_.clear();

        }
        if(ImGui::BeginTable("Alphabet", 4, ImGuiCol_TableBorderLight + ImGuiCol_TableBorderStrong))
        {
            ImGui::SetWindowFontScale(1.2f);
            for(const auto &item: alp)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::string(std::string(1,item.first) + " => " + item.second).c_str());                
            }
            ImGui::SetWindowFontScale(1.0f);
            ImGui::EndTable();

        }
    }
    ImGui::End();
}

int main()
{
    // Нужно для запуска потока с сервером
    memset(&info, 0, sizeof info);
    info.port = 41235;
    info.protocols = protocols;
    context = lws_create_context(&info);
    std::thread t1(run);
    t1.detach();

    sf::RenderWindow window(sf::VideoMode(800, 480), "Arduino UDP");
    window.setFramerateLimit(60);

    if(ImGui::SFML::Init(window))
    {
        sf::Clock deltaClock;
        while (window.isOpen())
        {
            sf::Event event{};
            while (window.pollEvent(event))
            {
                ImGui::SFML::ProcessEvent(window, event);
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }
            ImGui::SFML::Update(window, deltaClock.restart());

            display({(float)window.getSize().x, (float)window.getSize().y});

            window.clear();
            ImGui::SFML::Render(window);
            window.display();
        }
        ImGui::SFML::Shutdown();
    }

    running_ = false;
    lws_context_destroy(context);
    return 0;
}
