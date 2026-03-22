#include <ftxui/component/screen_interactive.hpp>
#include "Brigade.hpp"
#include "Foreman.hpp"
#include "Workman.hpp"
#include "Master.hpp"
#include "MainScreen.hpp"

int main() {
    // Создание модели
    auto foreman = std::make_unique<Foreman>("Иван Бригадиров", 45);
    Brigade brigade(std::move(foreman));

    MainScreen main_screen(brigade);

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto component = main_screen.createUI();

    std::cout << "Интерфейс запущен." << std::endl;
    screen.Loop(component);

    std::cout << "Программа завершена." << std::endl;
    return 0;
}