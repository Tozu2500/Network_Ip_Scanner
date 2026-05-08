#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "../network/device.h"
#include "../network/adapter_info.h"
#include "../oui/oui_lookup.h"
#include <vector>

namespace netscanner {

class ConsoleUI {
public:
    ConsoleUI();
    ~ConsoleUI();

    // Main loop ------ show menu, handle input, perform scans
    void run();

private:
    OuiLookup oui_;
    std::vector<AdapterInfo> adapters_;
    std::vector<Device> devices_;
    std::vector<Device> known_devices_;

    void init_console();
    void load_oui_database();
    void detect_adapters();

    void show_banner();
    void show_main_menu();
    void show_adapter_info();
    void show_scan_results();
    void show_intrusion_alerts();

    void do_quick_scan();
    void do_full_scan();
    void do_mark_trusted();
    void do_export_results();

    void set_color(int color);
    void reset_color();
    void clear_screen();
    void pause();
};

}  // namespace netscanner

#endif // CONSOLE_UI_H