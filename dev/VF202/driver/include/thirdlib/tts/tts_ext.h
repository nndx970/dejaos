#ifndef TTS_EXT_H
#define TTS_EXT_H

#ifdef __cplusplus
extern "C"{
#endif

typedef enum {
    GREETING_CLOSE = -1,	/*output without greetings*/
    GREETING_AHEAD,			/*output with greetings ahead*/
    GREETING_END			/*output with greetings at end*/
} greeting_positon_e;

typedef enum {
    GREET_AFTERNOON = 0,			/*下午好*/
    GREET_EVENING,				/*晚上好*/
    GREET_GREETING,				/*请通行*/
    GREET_MORNING,				/*早上好*/
    GREET_OFF_WORK,				/*下班打卡成功*/
    GREET_OFF_WORK_2,				/*辛苦了*/
    GREET_TEM_ABNORMAL,			/*体温异常*/
    GREET_TEM_NORMAL,				/*体温正常，请通行*/
    GREET_WORK,					/*打卡成功*/
    GREET_WORK_2,					/*上班打卡成功*/
    GREET_TEM_NORMAL_SHORT          /*体温正常*/
} greeting_list_e;

typedef struct greet_param_s{
    int language_type;          /* 0:Chinese; 1:English */
    int greeting_num;           /* actual greeting number */
    int* greeting_id;           /* id of each greeting */
    greeting_positon_e pos;
    //greeting_list_e index;
} greet_param_t;

void* tts_init();
void tts_getWavData(void* hdl, char *str, char *out_buf, int *out_size, int buf_size, greet_param_t *p_greet_param);
//void tts_deinit(void* hdl);

#ifdef __cplusplus
}
#endif

#endif