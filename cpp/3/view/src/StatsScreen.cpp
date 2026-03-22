#include "StatsScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <string>
#include <sstream>

using namespace ftxui;
using namespace boost::uuids;

StatsScreen::StatsScreen(Brigade& model) : model_(model) {}

Component StatsScreen::createWillCompleteForm(std::shared_ptr<int> active_form) {
    auto msg = std::make_shared<std::string>("");

    auto execute = Button("Проверить завершение", [=] {
        *msg = "";
        try {
            bool will_complete = model_.will_all_zones_be_completed_today();
            if (will_complete) {
                *msg = "Все участки будут завершены сегодня!";
            } else {
                *msg = "Не все участки будут завершены сегодня.";
            }
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ execute, back }), [=] {
        auto els = std::vector<Element>{
            text("Проверка завершения задач") | bold | color(Color::Cyan),
            separator(),
            text("Этот расчёт покажет, будут ли все участки завершены сегодня."),
            separator(),
            execute->Render(),
            back->Render(),
            separator()
        };
        if (!msg->empty()) {
            bool is_error = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(is_error ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component StatsScreen::createProblematicZoneForm(std::shared_ptr<int> active_form) {
    auto msg = std::make_shared<std::string>("");

    auto execute = Button("Найти проблемный участок", [=] {
        *msg = "";
        try {
            uuid zone_id = model_.get_most_problematic_zone();
            WorkZone* zone = model_.get_work_zone(zone_id);
            size_t days = model_.get_zone_days_remaining(zone_id);
            *msg = "Самый проблемный участок - \n"
                   "UUID: " + zone->get_id_string() + " \n"
                   "Дней до завершения: " + std::to_string(days);
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ execute, back }), [=] {
        auto els = std::vector<Element>{
            text("Самый проблемный участок") | bold | color(Color::Yellow),
            separator(),
            text("Участок с наибольшим количеством дней до завершения."),
            separator(),
            execute->Render(),
            back->Render(),
            separator()
        };
        if (!msg->empty()) {
            bool is_error = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(is_error ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component StatsScreen::createSimulateDayForm(std::shared_ptr<int> active_form) {
    auto msg = std::make_shared<std::string>("");

    auto execute = Button("Моделировать рабочий день", [=] {
        *msg = "";
        try {
            model_.simulate_work_day();
            *msg = "Рабочий день смоделирован! \n"
                   "Остатки работы обновлены. \n"
                   "Случайные отношения добавлены.";
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ execute, back }), [=] {
        auto els = std::vector<Element>{
            text("Моделирование рабочего дня") | bold | color(Color::Magenta),
            separator(),
            text("Обновляет остатки работы и добавляет случайные отношения."),
            separator(),
            execute->Render(),
            back->Render(),
            separator()
        };
        if (!msg->empty()) {
            bool is_error = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(is_error ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component StatsScreen::create() {
    auto active_form = std::make_shared<int>(0);

    auto will_complete_form = createWillCompleteForm(active_form);
    auto problematic_form = createProblematicZoneForm(active_form);
    auto simulate_form = createSimulateDayForm(active_form);

    auto will_complete_btn = Button("Завершение задач сегодня", [=] { *active_form = 1; });
    auto problematic_btn = Button("Самый проблемный участок", [=] { *active_form = 2; });
    auto simulate_btn = Button("Моделировать рабочий день", [=] { *active_form = 3; });

    auto actions_list = Container::Vertical({
        will_complete_btn,
        problematic_btn,
        simulate_btn
    });

    return Container::Tab({
        actions_list,
        will_complete_form,
        problematic_form,
        simulate_form
    }, active_form.get()) | border;
}