#include <string>
#include <commands.h>

int main(int argc, char** argv) {
    if (argc >= 2) {
        std::string sub_cmd = std::string(argv[1]);
        if (sub_cmd == "init")
            return init_command(argc - 1, &argv[1]);
        else if (sub_cmd == "add")
            return add_command(argc - 1, &argv[1]);
        else if (sub_cmd == "remove")
            return remove_command(argc - 1, &argv[1]);
        else if (sub_cmd == "close")
            return close_command(argc - 1, &argv[1]);
    }
    return base_command(argc, argv);
}