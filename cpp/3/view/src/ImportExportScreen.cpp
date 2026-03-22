
#include "ImportExportScreen.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <../../third_party/json.hpp>

using namespace ftxui;
using json = nlohmann::json;

ImportExportScreen::ImportExportScreen(Brigade& model) : model_(model) {}

ftxui::Component ImportExportScreen::create() {
    auto active_form = std::make_shared<int>(0); // 0 = main, 1 = export, 2 = import
    auto global_msg = std::make_shared<std::string>("");

    auto export_form = createExportForm(active_form, global_msg);
    auto import_form = createImportForm(active_form, global_msg);

    auto export_btn = Button("Экспортировать в файл", [=] { *active_form = 1; });
    auto import_btn = Button("Импортировать из файла", [=] { *active_form = 2; });

    auto buttons_container = Container::Vertical({
        export_btn,
        import_btn
    });

    auto main_view = Renderer(buttons_container, [=] {
        std::vector<Element> elements = {
            text("Импорт / Экспорт данных") | bold | color(Color::Cyan),
            separator(),
            text("Выберите действие:"),
            text("")
        };

        elements.push_back(export_btn->Render());
        elements.push_back(import_btn->Render());
        elements.push_back(separator());

        if (!global_msg->empty()) {
            bool is_error = global_msg->find("Ошибка:") == 0;
            elements.push_back(text(*global_msg) | color(is_error ? Color::Red : Color::Green) | bold);
        }

        return vbox(elements) | border;
    });

    return Container::Tab({
        main_view,
        export_form,
        import_form
    }, active_form.get());
}

ftxui::Component ImportExportScreen::createExportForm(
    std::shared_ptr<int> active_form,
    std::shared_ptr<std::string> global_msg
) {
    auto filename = std::make_shared<std::string>("brigade.json");

    auto submit_btn = Button("Экспортировать", [=] {
        *global_msg = "";
        try {
            model_.save_to_file(*filename);
            *global_msg = "Успех: данные сохранены в файл: " + *filename;
            *active_form = 0;
        } catch (const std::exception& e) {
            *global_msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back_btn = Button("<- Назад", [=] { *active_form = 0; });

    auto input = Input(filename.get(), "Имя файла");
    auto buttons = Container::Horizontal({ submit_btn, back_btn });

    return Renderer(Container::Vertical({ input, buttons }), [=] {
        return vbox({
            text("Экспорт состояния бригады") | bold | color(Color::Green),
            separator(),
            text("Имя файла:"), input->Render(),
            separator(),
            hbox({ submit_btn->Render(), text(" "), back_btn->Render() }),
            separator(),
            text(*global_msg) | color(global_msg->find("Ошибка:") == 0 ? Color::Red : Color::Green)
        }) | border;
    });
}

ftxui::Component ImportExportScreen::createImportForm(
    std::shared_ptr<int> active_form,
    std::shared_ptr<std::string> global_msg
) {
    auto filename = std::make_shared<std::string>("brigade.json");

    auto submit_btn = Button("Импортировать", [=] {
        *global_msg = "";
        try {
            model_.load_from_file(*filename);
            *global_msg = "Успех: данные загружены из файла: " + *filename;
            *active_form = 0;
        } catch (const std::exception& e) {
            *global_msg = "Ошибка: " + std::string(e.what());
        }
    });

    auto back_btn = Button("<- Назад", [=] { *active_form = 0; });

    auto input = Input(filename.get(), "Имя файла");
    auto buttons = Container::Horizontal({ submit_btn, back_btn });

    return Renderer(Container::Vertical({ input, buttons }), [=] {
        return vbox({
            text("Импорт состояния бригады") | bold | color(Color::Blue),
            separator(),
            text("Имя файла:"), input->Render(),
            separator(),
            hbox({ submit_btn->Render(), text(" "), back_btn->Render() }),
            separator(),
            text(*global_msg) | color(global_msg->find("Ошибка:") == 0 ? Color::Red : Color::Green)
        }) | border;
    });
}