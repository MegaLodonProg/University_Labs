#include "ActionsScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <sstream>

using namespace ftxui;
using namespace boost::uuids;

// Проверка, является ли строка валидным UUID
bool is_valid_uuid(const std::string& s) {
    try {
        uuid u = string_generator()(s);
        return !u.is_nil();
    } catch (...) {
        return false;
    }
}

ActionsScreen::ActionsScreen(Brigade& model) : model_(model) {}

// Действия с работниками

Component ActionsScreen::createHireForm(std::shared_ptr<int> active_form) {
    auto name = std::make_shared<std::string>("");
    auto age = std::make_shared<std::string>("");
    auto value = std::make_shared<std::string>("");
    auto role = std::make_shared<int>(0);
    auto msg = std::make_shared<std::string>("");

    auto name_in = Input(name.get(), "Имя");
    auto age_in = Input(age.get(), "Возраст");
    auto value_in = Input(value.get(), "Значение");
    auto role_sel = Radiobox({{ "Рабочий", "Мастер" }}, role.get());

    auto submit = Button("Нанять", [=] {
        *msg = "";
        try {
            if (name->empty() || age->empty() || value->empty()) {
                *msg = "Ошибка: заполните все поля";
                return;
            }
            int a = std::stoi(*age);
            double v = std::stod(*value);
            if (a <= 0) { *msg = "Ошибка: возраст > 0"; return; }

            std::unique_ptr<Worker> w;
            if (*role == 0) {
                if (v < 1 || v > 10) { *msg = "Ошибка: продуктивность 1-10"; return; }
                w = std::make_unique<Workman>(*name, a, static_cast<unsigned int>(v));
            } else {
                if (v <= 1.0 || v > 5.0) { *msg = "Ошибка: множитель 1.1-5.0"; return; }
                w = std::make_unique<Master>(*name, a, v);
            }

            model_.hire_worker(std::move(w));
            *msg = "Успешно нанят!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ name_in, age_in, value_in, role_sel, Container::Horizontal({ submit, back }) }), [=] {
        std::string label = (*role == 0) ? "Продуктивность (1-10)" : "Множитель (1.1-5.0)";
        auto elements = std::vector<Element>{
            text("Наем работника") | bold | color(Color::Green),
            separator(),
            text("Имя:"), name_in->Render(),
            text("Возраст:"), age_in->Render(),
            text(label + ":"), value_in->Render(),
            text("Роль:"), role_sel->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

Component ActionsScreen::createFireForm(std::shared_ptr<int> active_form) {
    auto id = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");

    auto id_in = Input(id.get(), "UUID работника");
    auto submit = Button("Уволить", [=] {
        *msg = "";
        if (id->empty()) { *msg = "Ошибка: введите UUID"; return; }
        if (!is_valid_uuid(*id)) { *msg = "Ошибка: неверный UUID"; return; }
        try {
            uuid u = string_generator()(*id);
            model_.fire_worker(u);
            *msg = "Успешно уволен!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ id_in, Container::Horizontal({ submit, back }) }), [=] {
        auto elements = std::vector<Element>{
            text("Увольнение") | bold | color(Color::Red),
            separator(),
            text("UUID:"), id_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

Component ActionsScreen::createModifyForm(std::shared_ptr<int> active_form) {
    auto id = std::make_shared<std::string>("");
    auto val = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");

    auto id_in = Input(id.get(), "UUID работника");
    auto val_in = Input(val.get(), "Новое значение");
    auto submit = Button("Изменить", [=] {
        *msg = "";
        if (id->empty() || val->empty()) { *msg = "Ошибка: заполните поля"; return; }
        if (!is_valid_uuid(*id)) { *msg = "Ошибка: неверный UUID"; return; }
        try {
            uuid u = string_generator()(*id);
            double v = std::stod(*val);
            Worker* w = model_.get_worker(u);
            if (dynamic_cast<Workman*>(w)) {
                if (v < 1 || v > 10) { *msg = "Ошибка: продуктивность 1-10"; return; }
                static_cast<Workman*>(w)->set_productivity(static_cast<unsigned int>(v));
            } else if (dynamic_cast<Master*>(w)) {
                if (v <= 1.0 || v > 5.0) { *msg = "Ошибка: множитель 1.1-5.0"; return; }
                static_cast<Master*>(w)->set_multiplier(v);
            } else {
                *msg = "Ошибка: неизвестный тип";
                return;
            }
            *msg = "Параметры изменены!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ id_in, val_in, Container::Horizontal({ submit, back }) }), [=] {
        auto elements = std::vector<Element>{
            text("Изменение параметров") | bold | color(Color::Yellow),
            separator(),
            text("UUID:"), id_in->Render(),
            text("Новое значение:"), val_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

Component ActionsScreen::createFriendshipForm(std::shared_ptr<int> active_form) {
    auto a = std::make_shared<std::string>("");
    auto b = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");

    auto a_in = Input(a.get(), "UUID 1");
    auto b_in = Input(b.get(), "UUID 2");
    auto submit = Button("Добавить дружбу", [=] {
        *msg = "";
        if (a->empty() || b->empty()) { *msg = "Ошибка: оба UUID"; return; }
        if (!is_valid_uuid(*a) || !is_valid_uuid(*b)) { *msg = "Ошибка: неверный UUID"; return; }
        if (*a == *b) { *msg = "Ошибка: нельзя самому себе"; return; }
        try {
            uuid u1 = string_generator()(*a);
            uuid u2 = string_generator()(*b);
            model_.add_friendship(u1, u2);
            *msg = "Дружба добавлена!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ a_in, b_in, Container::Horizontal({ submit, back }) }), [=] {
        auto elements = std::vector<Element>{
            text("Дружба") | bold | color(Color::Blue),
            separator(),
            text("UUID 1:"), a_in->Render(),
            text("UUID 2:"), b_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

Component ActionsScreen::createEnmityForm(std::shared_ptr<int> active_form) {
    auto a = std::make_shared<std::string>("");
    auto b = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");

    auto a_in = Input(a.get(), "UUID 1");
    auto b_in = Input(b.get(), "UUID 2");
    auto submit = Button("Добавить вражду", [=] {
        *msg = "";
        if (a->empty() || b->empty()) { *msg = "Ошибка: оба UUID"; return; }
        if (!is_valid_uuid(*a) || !is_valid_uuid(*b)) { *msg = "Ошибка: неверный UUID"; return; }
        if (*a == *b) { *msg = "Ошибка: нельзя самому себе"; return; }
        try {
            uuid u1 = string_generator()(*a);
            uuid u2 = string_generator()(*b);
            model_.add_enmity(u1, u2);
            *msg = "Вражда добавлена!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ a_in, b_in, Container::Horizontal({ submit, back }) }), [=] {
        auto elements = std::vector<Element>{
            text("Вражда") | bold | color(Color::Red),
            separator(),
            text("UUID 1:"), a_in->Render(),
            text("UUID 2:"), b_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

Component ActionsScreen::createPromoteForm(std::shared_ptr<int> active_form) {
    auto id = std::make_shared<std::string>("");
    auto mult = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");

    auto id_in = Input(id.get(), "UUID рабочего");
    auto mult_in = Input(mult.get(), "Множитель");
    auto submit = Button("Повысить", [=] {
        *msg = "";
        if (id->empty() || mult->empty()) { *msg = "Ошибка: заполните поля"; return; }
        if (!is_valid_uuid(*id)) { *msg = "Ошибка: неверный UUID"; return; }
        try {
            uuid u = string_generator()(*id);
            double m = std::stod(*mult);
            if (m <= 1.0 || m > 5.0) { *msg = "Ошибка: множитель 1.1-5.0"; return; }
            model_.promote_workman_to_master(u, m);
            *msg = "Повышение успешно!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });

    return Renderer(Container::Vertical({ id_in, mult_in, Container::Horizontal({ submit, back }) }), [=] {
        auto elements = std::vector<Element>{
            text("Повышение до мастера") | bold | color(Color::Magenta),
            separator(),
            text("UUID:"), id_in->Render(),
            text("Множитель:"), mult_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            elements.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(elements) | border;
    });
}

// Действия с участками

Component ActionsScreen::createCreateZoneForm(std::shared_ptr<int> active_form) {
    auto volume = std::make_shared<std::string>("100.0");
    auto msg = std::make_shared<std::string>("");
    auto vol_in = Input(volume.get(), "Объём работы");
    auto submit = Button("Создать", [=] {
        *msg = "";
        try {
            double v = std::stod(*volume);
            if (v <= 0) { *msg = "Ошибка: объём > 0"; return; }
            model_.create_work_zone(v);
            *msg = "Участок создан!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });
    return Renderer(Container::Vertical({ vol_in, Container::Horizontal({ submit, back }) }), [=] {
        auto els = std::vector<Element>{
            text("Создание участка") | bold | color(Color::Green),
            separator(),
            text("Объём работы:"), vol_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component ActionsScreen::createModifyZoneForm(std::shared_ptr<int> active_form) {
    auto zone_id = std::make_shared<std::string>("");
    auto new_volume = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");
    auto id_in = Input(zone_id.get(), "UUID участка");
    auto vol_in = Input(new_volume.get(), "Новый объём работы");
    auto submit = Button("Изменить", [=] {
        *msg = "";
        if (zone_id->empty() || new_volume->empty()) { *msg = "Ошибка: заполните поля"; return; }
        if (!is_valid_uuid(*zone_id)) { *msg = "Ошибка: неверный UUID участка"; return; }
        try {
            uuid id = string_generator()(*zone_id);
            double vol = std::stod(*new_volume);
            if (vol <= 0) { *msg = "Ошибка: объём > 0"; return; }
            model_.modify_work_zone(id, vol);
            *msg = "Объём участка изменён!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });
    return Renderer(Container::Vertical({ id_in, vol_in, Container::Horizontal({ submit, back }) }), [=] {
        auto els = std::vector<Element>{
            text("Изменение участка") | bold | color(Color::Yellow),
            separator(),
            text("UUID участка:"), id_in->Render(),
            text("Новый объём:"), vol_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component ActionsScreen::createAssignWorkerForm(std::shared_ptr<int> active_form) {
    auto worker_id = std::make_shared<std::string>("");
    auto zone_id = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");
    auto w_in = Input(worker_id.get(), "UUID работника");
    auto z_in = Input(zone_id.get(), "UUID участка");
    auto submit = Button("Назначить", [=] {
        *msg = "";
        if (worker_id->empty() || zone_id->empty()) { *msg = "Ошибка: оба UUID"; return; }
        if (!is_valid_uuid(*worker_id)) { *msg = "Ошибка: неверный UUID работника"; return; }
        if (!is_valid_uuid(*zone_id)) { *msg = "Ошибка: неверный UUID участка"; return; }
        try {
            uuid w = string_generator()(*worker_id);
            uuid z = string_generator()(*zone_id);
            model_.send_worker_to_zone(w, z);
            *msg = "Работник назначен на участок!";
            *active_form = 0;
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });
    return Renderer(Container::Vertical({ w_in, z_in, Container::Horizontal({ submit, back }) }), [=] {
        auto els = std::vector<Element>{
            text("Назначение работника") | bold | color(Color::Blue),
            separator(),
            text("UUID работника:"), w_in->Render(),
            text("UUID участка:"), z_in->Render(),
            separator(),
            hbox({ submit->Render(), text(" "), back->Render() }),
            separator()
        };
        if (!msg->empty()) {
            bool err = msg->find("Ошибка") != std::string::npos;
            els.push_back(text(*msg) | color(err ? Color::Red : Color::Green));
        }
        return vbox(els) | border;
    });
}

Component ActionsScreen::createZoneProductivityForm(std::shared_ptr<int> active_form) {
    auto zone_id = std::make_shared<std::string>("");
    auto msg = std::make_shared<std::string>("");
    auto id_in = Input(zone_id.get(), "UUID участка");
    auto show = Button("Показать выработку", [=] {
        *msg = "";
        if (zone_id->empty()) { *msg = "Ошибка: введите UUID участка"; return; }
        if (!is_valid_uuid(*zone_id)) { *msg = "Ошибка: неверный UUID"; return; }
        try {
            uuid id = string_generator()(*zone_id);
            double prod = model_.calculate_zone_productivity(id);
            WorkZone* zone = model_.get_work_zone(id);
            double remaining = zone->get_remaining_work();
            size_t days = model_.get_zone_days_remaining(id);

            std::ostringstream oss;
            oss << "Суточная выработка: " << prod << " \n"
                << "Остаток работы: " << remaining << " \n"
                << "Дней до завершения: " << days;
            *msg = oss.str();
        } catch (const std::exception& e) {
            *msg = "Ошибка: " + std::string(e.what());
        }
    });
    auto back = Button("<- Назад", [=] { *active_form = 0; });
    return Renderer(Container::Vertical({ id_in, show, back }), [=] {
        auto els = std::vector<Element>{
            text("Выработка участка") | bold | color(Color::Cyan),
            separator(),
            text("UUID участка:"), id_in->Render(),
            show->Render(),
            back->Render(),
            separator(),
            text(*msg) | color(msg->find("Ошибка") != std::string::npos ? Color::Red : Color::Green)
        };
        return vbox(els) | border;
    });
}

// Объединение всего

Component ActionsScreen::create() {
    auto screen_state = std::make_shared<int>(0); // 0 = main, 1 = workers, 2 = zones
    auto active_worker_form = std::make_shared<int>(0);
    auto active_zone_form = std::make_shared<int>(0);

    // для Работников
    auto hire_form = createHireForm(active_worker_form);
    auto fire_form = createFireForm(active_worker_form);
    auto modify_form = createModifyForm(active_worker_form);
    auto friendship_form = createFriendshipForm(active_worker_form);
    auto enmity_form = createEnmityForm(active_worker_form);
    auto promote_form = createPromoteForm(active_worker_form);

    auto hire_btn = Button("Нанять рабочего", [=] { *active_worker_form = 1; });
    auto fire_btn = Button("Уволить рабочего", [=] { *active_worker_form = 2; });
    auto modify_btn = Button("Изменить параметры", [=] { *active_worker_form = 3; });
    auto friends_btn = Button("Добавить друзей", [=] { *active_worker_form = 4; });
    auto enemies_btn = Button("Добавить врагов", [=] { *active_worker_form = 5; });
    auto promote_btn = Button("Повысить до мастера", [=] { *active_worker_form = 6; });
    auto back_workers = Button("<- Назад", [=] { *screen_state = 0; });

    auto workers_actions = Container::Vertical({
        hire_btn, fire_btn, modify_btn, friends_btn, enemies_btn, promote_btn, back_workers
    });

    auto workers_screen = Container::Tab({
        workers_actions,
        hire_form,
        fire_form,
        modify_form,
        friendship_form,
        enmity_form,
        promote_form
    }, active_worker_form.get()) | border;

    // Для участков
    auto create_zone_form = createCreateZoneForm(active_zone_form);
    auto modify_zone_form = createModifyZoneForm(active_zone_form);
    auto assign_worker_form = createAssignWorkerForm(active_zone_form);
    auto productivity_form = createZoneProductivityForm(active_zone_form);

    auto create_zone_btn = Button("Создать участок", [=] { *active_zone_form = 1; });
    auto modify_zone_btn = Button("Изменить участок", [=] { *active_zone_form = 2; });
    auto assign_worker_btn = Button("Назначить работника", [=] { *active_zone_form = 3; });
    auto productivity_btn = Button("Выработка участка", [=] { *active_zone_form = 4; });
    auto back_zones = Button("<- Назад", [=] { *screen_state = 0; });

    auto zones_actions = Container::Vertical({
        create_zone_btn, modify_zone_btn, assign_worker_btn, productivity_btn, back_zones
    });

    auto zones_screen = Container::Tab({
        zones_actions,
        create_zone_form,
        modify_zone_form,
        assign_worker_form,
        productivity_form
    }, active_zone_form.get()) | border;

    auto to_workers = Button("Действия с работниками", [=] { *screen_state = 1; });
    auto to_zones = Button("Действия с участками", [=] { *screen_state = 2; });

    auto main_screen = Container::Vertical({
        Renderer([] { return text("Управление бригадой") | bold | center; }),
        Renderer([] { return separator(); }),
        Renderer([] { return text("Выберите категорию:"); }),
        to_workers,
        to_zones
    }) | border;

    return Container::Tab({
        main_screen,
        workers_screen,
        zones_screen
    }, screen_state.get());
}