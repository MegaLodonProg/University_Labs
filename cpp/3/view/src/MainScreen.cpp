#include "MainScreen.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

using namespace ftxui;

MainScreen::MainScreen(Brigade& model)
    : model_(model), workers_screen_(model), work_zones_screen_(model)
    , actions_screen_(model), stats_screen_(model), import_export_screen_(model) {}

Component MainScreen::createUI() {
    // Вкладки с экранами
    auto tab_container = Container::Tab({
        workers_screen_.create(), work_zones_screen_.create(), actions_screen_.create(),stats_screen_.create(), import_export_screen_.create()
    }, &selected_tab);

    // Кнопки переключения вкладок
    auto tab_toggle = Container::Horizontal({
        Button("Работники",   [&] { selected_tab = 0; }),
        Button("Участки",     [&] { selected_tab = 1; }),
        Button("Действия",    [&] { selected_tab = 2; }),
        Button("Статистика",  [&] { selected_tab = 3; }),
        Button("Импорт/Экспорт",  [&] { selected_tab = 4; })
    });

    // Заголовок для приложения
    auto header = Renderer([] {
        return hbox({
            text("Моделирование деятельности бригады") | bold | color(Color::Yellow) | center
        }) | border;
    });

    // Объединение в один контейнер
    return Container::Vertical({
        header,
        tab_toggle,
        tab_container
    });
}