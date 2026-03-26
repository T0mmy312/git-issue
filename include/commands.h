#ifndef COMMANDS_H
#define COMMANDS_H

int base_command(int argc, char** argv);

int init_command(int argc, char** argv);
int add_command(int argc, char** argv);
int remove_command(int argc, char** argv);
int close_command(int argc, char** argv);

#endif // COMMANDS_H