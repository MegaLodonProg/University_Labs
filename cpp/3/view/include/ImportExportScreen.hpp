#pragma once

#include <ftxui/component/component.hpp>
#include "../../model/include/Brigade.hpp"

class ImportExportScreen {
private:
    Brigade& model_;

public:
    ImportExportScreen(Brigade& model);
    ftxui::Component create();

private:
    ftxui::Component createExportForm(std::shared_ptr<int> active_form, std::shared_ptr<std::string> global_msg);
    ftxui::Component createImportForm(std::shared_ptr<int> active_form, std::shared_ptr<std::string> global_msg);
};