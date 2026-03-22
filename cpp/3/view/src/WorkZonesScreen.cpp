#include "WorkZonesScreen.hpp"
#include <ftxui/component/event.hpp>
#include <sstream>
#include <memory>
#include <iomanip>

using namespace ftxui;

WorkZonesScreen::WorkZonesScreen(Brigade& model) : model_(model) {}

Component WorkZonesScreen::create() {
    auto invisible_btn = Button("", [] {});
    auto invisible_renderer = Renderer(invisible_btn, [] {
        return text("");
    });

    auto main_renderer = Renderer([this]() -> Element {
        if (show_details_) {
            return renderZoneDetails();
        } else {
            return renderZonesList();
        }
    });

    auto with_keys = CatchEvent(main_renderer, [this](Event event) -> bool {
        std::vector<WorkZone*> zones;
        for (auto it = model_.get_work_zones_begin(); it != model_.get_work_zones_end(); ++it) {
            zones.push_back(it->second.get());
        }
        size_t zones_count = zones.size();

        if (zones_count == 0) {
            selected_zone_ = 0;
        } else if (selected_zone_ >= static_cast<int>(zones_count)) {
            selected_zone_ = static_cast<int>(zones_count - 1);
        }

        if (show_details_) {
            if (event == Event::Escape || event == Event::Return) {
                show_details_ = false;
                return true;
            }
        } else {
            if (event == Event::ArrowUp && selected_zone_ > 0) {
                selected_zone_--;
                return true;
            }
            if (event == Event::ArrowDown && selected_zone_ < static_cast<int>(zones_count - 1)) {
                selected_zone_++;
                return true;
            }
            if (event == Event::Return && zones_count > 0) {
                show_details_ = true;
                return true;
            }
        }
        return false;
    });

    return Container::Vertical({
        with_keys,
        invisible_renderer
    });
}

// Вспомогательная функция: форматирование double с 2 знаками
inline std::string format_double(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

// Вспомогательная функция: получить имя работника по UUID
std::string get_worker_name_or_id(Brigade& model, const boost::uuids::uuid& id) {
    try {
        Worker* w = model.get_worker(id);
        return w->get_name();
    } catch (...) {
        return model.get_worker(id)->get_id_string().substr(0, 8) + "...";
    }
}

Element WorkZonesScreen::renderZoneDetails() {
    std::vector<Element> els;
    els.push_back(text("Детальная информация об участке") | bold | color(Color::Cyan));
    els.push_back(separator());

    std::vector<WorkZone*> zones;
    for (auto it = model_.get_work_zones_begin(); it != model_.get_work_zones_end(); ++it) {
        zones.push_back(it->second.get());
    }

    if (zones.empty()) {
        els.push_back(text("Нет участков") | color(Color::Red));
    } else {
        if (selected_zone_ >= static_cast<int>(zones.size())) {
            selected_zone_ = static_cast<int>(zones.size() - 1);
        }

        WorkZone* zone = zones[selected_zone_];
        double total = zone->get_total_work();
        double remaining = zone->get_remaining_work();
        bool completed = zone->is_completed();

        els.push_back(hbox({ text("UUID: "), text(zone->get_id_string()) | dim }));
        els.push_back(hbox({ text("Объём работы: "), text(format_double(total)) }));
        els.push_back(hbox({ text("Осталось: "), text(format_double(remaining)) | color(completed ? Color::Green : Color::Yellow) }));
        els.push_back(hbox({ text("Статус: "), text(completed ? "Завершён" : "В работе") | color(completed ? Color::Green : Color::Yellow) }));

        els.push_back(separator());
        els.push_back(text("Работники на участке:"));

        const auto& worker_ids = zone->get_workers();
        if (worker_ids.empty()) {
            els.push_back(text("  Никто не прикреплён") | dim);
        } else {
            for (const auto& id : worker_ids) {
                std::string name = get_worker_name_or_id(model_, id);
                els.push_back(text("  • " + name));
            }
        }
    }

    els.push_back(separator());
    els.push_back(text("Enter - вернуться к списку"));
    return vbox(els) | border;
}

Element WorkZonesScreen::renderZonesList() {
    std::vector<Element> elements;
    elements.push_back(text("Список рабочих участков:") | bold | color(Color::White));
    elements.push_back(separator());

    std::vector<WorkZone*> zones;
    for (auto it = model_.get_work_zones_begin(); it != model_.get_work_zones_end(); ++it) {
        zones.push_back(it->second.get());
    }

    if (zones.empty()) {
        elements.push_back(text("Нет созданных участков") | color(Color::RedLight));
        elements.push_back(text("Перейдите в 'Действия' -> 'Создать участок'"));
        selected_zone_ = 0;
    } else {
        if (selected_zone_ >= static_cast<int>(zones.size())) {
            selected_zone_ = static_cast<int>(zones.size() - 1);
        }

        for (size_t i = 0; i < zones.size(); ++i) {
            WorkZone* zone = zones[i];
            std::string line = "Участок #" + std::to_string(i + 1) + ": " + format_double(zone->get_total_work()) + " ед. работы";

            if (static_cast<int>(i) == selected_zone_) {
                elements.push_back(text("-> " + line) | color(Color::GreenLight));
            } else {
                elements.push_back(text("  " + line));
            }
        }

        elements.push_back(separator());
        elements.push_back(text("Используйте стрелки для выбора участка"));
        elements.push_back(text("Enter - показать детали"));
        elements.push_back(text("Всего участков: " + std::to_string(zones.size())));
    }

    return vbox(elements);
}