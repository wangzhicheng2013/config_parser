#include "config_management.hpp"
int main() {
    config_management cm;
    if (cm.parse("./test.conf")) {
        cm.dump_config();
        std::string value;
        if (cm.get_value("MEMORY", "B", value)) {
            std::cout << "value = " << value << std::endl;
        }
    }

    return 0;
}