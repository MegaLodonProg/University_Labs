#include "WorkersScreen.hpp"
#include <ftxui/component/event.hpp>
#include "Worker.hpp"
#include "Master.hpp"
#include "Workman.hpp"
#include "Foreman.hpp"
#include <sstream>
#include <memory>
#include <iomanip>

using namespace ftxui;

WorkersScreen::WorkersScreen(Brigade& model) : model_(model) {}

Component WorkersScreen::create() {
    auto invisible_btn = Button("", [] {});
    auto invisible_renderer = Renderer(invisible_btn, [] {
        return text("");
    });

    auto main_renderer = Renderer([this]() -> Element {
        if (show_details_) {
            return renderWorkerDetails();
        } else {
            return renderWorkersList();
        }
    });

    auto with_keys = CatchEvent(main_renderer, [this](Event event) -> bool {
        auto workers = model_.get_workers();
        size_t workers_count = workers.size();

        if (workers_count == 0) {
            selected_worker_ = 0;
        } else if (selected_worker_ >= static_cast<int>(workers_count)) {
            selected_worker_ = static_cast<int>(workers_count - 1);
        }

        if (show_details_) {
            if (event == Event::Escape || event == Event::Return) {
                show_details_ = false;
                return true;
            }
        } else {
            if (event == Event::ArrowUp && selected_worker_ > 0) {
                selected_worker_--;
                return true;
            }
            if (event == Event::ArrowDown && selected_worker_ < static_cast<int>(workers_count - 1)) {
                selected_worker_++;
                return true;
            }
            if (event == Event::Return && workers_count > 0) {
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

Element WorkersScreen::renderWorkerDetails() {
    std::vector<Element> els;
    els.push_back(text("Детальная информация о работнике") | bold | color(Color::Cyan));
    els.push_back(separator());

    auto workers = model_.get_workers();

    if (workers.empty()) {
        els.push_back(text("Нет работников (даже бригадира)") | color(Color::Red));
    } else {
        if (selected_worker_ >= static_cast<int>(workers.size())) {
            selected_worker_ = static_cast<int>(workers.size() - 1);
        }

        const auto& worker = workers[selected_worker_];
        els.push_back(hbox({ text("Имя: "), text(worker->get_name()) | bold }));
        els.push_back(hbox({ text("Возраст: "), text(std::to_string(worker->get_age())) }));
        els.push_back(hbox({ text("Должность: "), text(worker->get_role()) | color(Color::Yellow) }));

        if (auto* workman = dynamic_cast<Workman*>(worker)) {
            els.push_back(hbox({ text("Продуктивность: "), text(std::to_string(workman->get_productivity())) }));
        } else if (auto* master = dynamic_cast<Master*>(worker)) {
            els.push_back(hbox({ text("Множитель выработки: "), text(format_double(master->get_multiplier())) }));
        }

        els.push_back(separator());
        els.push_back(text("UUID: " + worker->get_id_string()));
        els.push_back(text("Друзья: " + std::to_string(worker->get_friends().size())));
        els.push_back(text("Враги: " + std::to_string(worker->get_enemies().size())));
    }

    els.push_back(separator());
    els.push_back(text("Enter - вернуться к списку"));
    return vbox(els) | border;
}

Element WorkersScreen::renderWorkersList() {
    std::vector<Element> elements;
    elements.push_back(text("Список работников в бригаде:") | bold | color(Color::White));
    elements.push_back(separator());

    auto workers = model_.get_workers();

    if (workers.empty()) {
        elements.push_back(text("Нет работников (даже бригадира)") | color(Color::RedLight));
        elements.push_back(text("Ошибка инициализации бригады"));
        selected_worker_ = 0;
    } else {
        bool only_foreman = true;
        for (const auto& w : workers) {
            if (!dynamic_cast<Foreman*>(w)) {
                only_foreman = false;
                break;
            }
        }

        if (only_foreman) {
            // Есть бригадир, но нет других работников
            elements.push_back(text("В бригаде только бригадир.") | color(Color::Yellow));
            elements.push_back(text("Нанять рабочих можно в 'Действия' -> 'Нанять рабочего'"));
            selected_worker_ = 0;
        } else {
            // Есть и бригадир, и другие работники
            if (selected_worker_ >= static_cast<int>(workers.size())) {
                selected_worker_ = static_cast<int>(workers.size() - 1);
            }

            for (size_t i = 0; i < workers.size(); ++i) {
                const auto& worker = workers[i];
                std::string worker_text = "• " + worker->get_name() + " (" + std::to_string(worker->get_age()) + " лет) - " + worker->get_role();

                if (auto* workman = dynamic_cast<Workman*>(worker)) {
                    worker_text += " (продуктивность: " + std::to_string(workman->get_productivity()) + ")";
                } else if (auto* master = dynamic_cast<Master*>(worker)) {
                    worker_text += " (множитель: " + format_double(master->get_multiplier()) + ")";
                }

                if (static_cast<int>(i) == selected_worker_) {
                    elements.push_back(text("-> " + worker_text) | color(Color::GreenLight));
                } else {
                    elements.push_back(text("  " + worker_text));
                }
            }
            elements.push_back(separator());
            elements.push_back(text("Используйте стрелки для выбора работника"));
            elements.push_back(text("Enter - показать детали"));
            elements.push_back(text("Всего работников: " + std::to_string(workers.size())));
        }
    }

    return vbox(elements);
}