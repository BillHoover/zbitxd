void setup();
void loop();
void display();
void redraw();
void key_pressed(char c);
int field_set(const char *label, const char *new_value);
int get_field_value(char *id, char *value);
int get_field_value_by_label(char *label, char *value);
extern int spectrum_plot[];
void remote_execute(char *command);
int remote_update_field(int i, char *text);
void web_get_spectrum(char *buff);
int web_get_console(char *buff, int max);
int remote_audio_output(int16_t *samples);
const char *field_str(char *label);
int field_int(char *label);
int is_in_tx();
void abort_tx();
void enter_qso();
extern int display_freq;

#define FONT_FIELD_LABEL 0
#define FONT_FIELD_VALUE 1
#define FONT_LARGE_FIELD 2
#define FONT_LARGE_VALUE 3
#define FONT_SMALL 4
#define FONT_LOG 5
#define FONT_FT8_RX 6
#define FONT_FT8_TX 7
#define FONT_SMALL_FIELD_VALUE 8
#define FONT_CW_RX 9
#define FONT_CW_TX 10
#define FONT_FLDIGI_RX 11
#define FONT_FLDIGI_TX 12
#define FONT_TELNET 13
#define FONT_FT8_QUEUED 14
#define FONT_FT8_REPLY 15

#define FF_MYCALL 16
#define FF_CALLER 17
#define FF_GRID 18

void enter_qso();
void call_wipe();
void write_console(int style, char *text);
int macro_load(char *filename, char *output);
int macro_exec(int key, char *dest);
void macro_label(int fn_key, char *label);
void macro_list(char *output);
void macro_get_keys(char *output);
void update_log_ed();
void write_call_log();

void tlog(char *id, char *text, int p);
// void tlogf(char * format, ...);

// 006 : Click fix by Beckman
// 007 : FT8 webui fix and coloring
// 008 : Callsign lookup, and grid visited coloring
// 009 : FT8 EXCH field safeguard
// 010 : FT8 callsign and grid decoration
// 011 : FT8 CQ click error fixed
// 012 : FT8 click non CQ fixed
// 013 : FT8 web_q overflow fixed
// 014 : Web GridMap v1, and cur selected Band/Store shown
// 015 : Web Robinson zoom GridMap, mouseover grid info, Logged Seen btn
// 016 : Cleanout and prepare for github sbitx-oz7bx v3.02.0016
#include "version.h"
