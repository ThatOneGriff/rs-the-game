#ifndef INIT_QUIT_H
#define INIT_QUIT_H

#define SDL_FLAGS (SDL_INIT_VIDEO)

#define SAVE_DATA_PATH "./player.savedata"
#define SAVE_DATA_LINES 6


/* Predef */

void init(int *const exit_code);
void quit(void);
void read_data(void);
void save_data(void);
[[ noreturn ]]
void program_exit(const int exit_code);


#endif /// INIT_QUIT_H