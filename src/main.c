#include <xmc_gpio.h>

#define TICKS_PER_SECOND 1000

#define BUTTON1 P1_14
#define BUTTON2 P1_15
#define LED1 P1_1
#define LED2 P1_0

#define DOT 100
#define DASH 3*DOT

#define SPACE_LETTERS_PERIOD 3*DOT
#define SPACE_WORDS_PERIOD 7*DOT
#define SPACE_SIGNALS_PERIOD 1*DOT

#define PAUSE 5000
#define DUD 100

#define NUM_NUMBERS 10
#define MESSAGE_SIZE 9

void SysTick_Handler(void);
void Initialize_GPIOS();
int Button_Event_Rise(short int* CurrentButtonState, short int* LastButtonState);
void TurnOnLed();
void TurnOffLed();
void ResetTicks();
void Handle_Button_ONE_Events();
void Handle_Button_TWO_Events();
void Eval_Time_B1_Presses();
void Upd_Time_B1_Presses();
void Parse_Time_B1_Presses();
int EndOfIndOrTotalTrans(short int CurrentLetterIndex, int SizeOfCurrentSignal);
void UpdateCurSignalValues(char* cur_signal, int* cur_signal_on_time, int* cur_signal_wait_time, 
                           int* cur_signal_len, int* cur_signal_total_size);

volatile int LED_IS_ON = 1;

volatile int TICKS_ON = 1000;
volatile int TICKS_WAIT = 1000;
volatile uint32_t cur_ticks_on = 0;
volatile uint32_t cur_ticks_wait = 0;
volatile uint32_t global_ticks = 0;

volatile int b1_num_presses = 0;
volatile uint32_t b1_time_marks[2] = { 0 };

volatile int MORSE_TRANSMISSION_ONGOING = 0;
volatile int END_OF_IND_TRANSMISSION = 0;
volatile int STRING_ONGOING = 0;
volatile int NUMBS_ONGOING = 0;
volatile int TIME_ELAPSED_BTWN_B1S = 0;

volatile short int cur_lett = 0;
volatile short int cur_periods = 0;
volatile short int cur_num = 0;
volatile short int cur_num_array[10] = { 0 };
volatile short int cur_num_array_size = 0;

typedef struct morse_key{
    char c;
    char* morse_code;
    int on_times[5];
    int wait_times[5];
    int size;
}morse_key_t;

morse_key_t MESSAGE[MESSAGE_SIZE] = {
    {   'I', ".."   ,   {DOT, DOT}              , {SPACE_SIGNALS_PERIOD, SPACE_WORDS_PERIOD},                                                   2   },
    {   'C', "-.-." ,   {DASH, DOT, DASH, DOT}  , {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},     4   },
    {   'A', ".-"   ,   {DOT, DASH}             , {SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},                                                 2   },
    {   'N', "-."   ,   {DASH, DOT}             , {SPACE_SIGNALS_PERIOD, SPACE_WORDS_PERIOD},                                                   2   },
    {   'M', "--"   ,   {DASH, DASH}            , {SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},                                                 2   },
    {   'O', "---"  ,   {DASH, DASH, DASH}      , {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},                           3   },
    {   'R', ".-."  ,   {DOT, DASH, DOT}       , {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},                           3   },
    {   'S', "...." ,   {DOT, DOT, DOT}         , {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},                           3   },
    {   'E', "."    ,   {DOT}                   , {DUD},                                                                                        1   }
};

morse_key_t NUMBERS[NUM_NUMBERS] = {
    {   '0', "-----"   ,   {DASH, DASH, DASH, DASH, DASH}   ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '1', ".----"   ,   {DOT, DASH, DASH, DASH, DASH}    ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '2', ".----"   ,   {DOT, DOT, DASH, DASH, DASH}     ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '3', ".----"   ,   {DOT, DOT, DOT, DASH, DASH}      ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '4', ".----"   ,   {DOT, DOT, DOT, DOT, DASH}       ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '5', "....."   ,   {DOT, DOT, DOT, DOT, DOT}        ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '6', "-...."   ,   {DASH, DOT, DOT, DOT, DOT}       ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '7', "--..."   ,   {DASH, DASH, DOT, DOT, DOT}      ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '8', "---.."   ,   {DASH, DASH, DASH, DOT, DOT}     ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   },
    {   '9', "----."   ,   {DASH, DASH, DASH, DASH, DOT}    ,       {SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_SIGNALS_PERIOD, SPACE_LETTERS_PERIOD},         5   }
};

int main(void){

    Initialize_GPIOS();
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);

    //Initalizing variables
    char cur_signal = 'x';
    int cur_signal_on_time = 0;
    int cur_signal_wait_time = 0;
    int cur_signal_len = 0;
    int cur_signal_total_size = 0;

    short int cur_button_one_state = 1;
    short int cur_button_two_state = 1;
    short int last_button_one_state = cur_button_one_state;
    short int last_button_two_state = cur_button_two_state;

    b1_time_marks[0] = 0;
    b1_time_marks[1] = 0;;

    while(1) {
        UpdateCurSignalValues(&cur_signal, &cur_signal_on_time, &cur_signal_wait_time, 
                              &cur_signal_len, &cur_signal_total_size);

        TICKS_ON = cur_signal_on_time;
        TICKS_WAIT = cur_signal_wait_time;

        last_button_one_state = cur_button_one_state;
        last_button_two_state = cur_button_two_state;

        cur_button_one_state = !XMC_GPIO_GetInput(BUTTON1);
        cur_button_two_state = !XMC_GPIO_GetInput(BUTTON2);

        if(Button_Event_Rise(&cur_button_one_state, &last_button_one_state)) {
            Handle_Button_ONE_Events();
        }

        if(Button_Event_Rise(&cur_button_two_state, &last_button_two_state)) {
            Handle_Button_TWO_Events();
        }

        if (END_OF_IND_TRANSMISSION){ //If and individual transmission has ended

            if((EndOfIndOrTotalTrans(cur_lett, cur_signal_total_size) || 
               (EndOfIndOrTotalTrans(cur_num, cur_signal_total_size) && !STRING_ONGOING)) &&
               (EndOfIndOrTotalTrans(cur_periods, cur_signal_len))) { //If its the end of a single component of the MS
                MORSE_TRANSMISSION_ONGOING=0;
                cur_periods=0; //Resets the index of current periods
                cur_lett=0;    //Resets the index of morse letters
                cur_num=0;    //Resets the index of morse numbers
                TurnOffLed();
                ResetTicks();
            }
            else if(EndOfIndOrTotalTrans(cur_periods, cur_signal_len)) { //If its the end of a single component of the MS
                cur_periods=0;
                ++cur_lett;
                ++cur_num;
            }
            else{
                ++cur_periods;
            }

            END_OF_IND_TRANSMISSION = 0;
        }
    }

    return 0;
}

void SysTick_Handler(void)
{
    ++global_ticks;
    if(MORSE_TRANSMISSION_ONGOING) {
        if(LED_IS_ON) {
            cur_ticks_on++;
            if (cur_ticks_on >= TICKS_ON){
                TurnOffLed();
                ResetTicks();
            }
        }
        else{
            cur_ticks_wait++;
            if (cur_ticks_wait >= TICKS_WAIT) {
                TurnOnLed();
                ResetTicks();
                END_OF_IND_TRANSMISSION=1;
            }
        }
    }
}

void ResetTicks()
{
    cur_ticks_on=0;
    cur_ticks_wait=0;
}
void TurnOffLed()
{
    XMC_GPIO_SetOutputLow(LED1);
    LED_IS_ON=0;
}

void TurnOnLed()
{
    XMC_GPIO_SetOutputHigh(LED1);
    LED_IS_ON=1;
}

int Button_Event_Rise(short int* CurrentButtonState, short int* LastButtonState)
{
    int CBS = *CurrentButtonState;
    int LBS = *LastButtonState;
    return CBS && !LBS && !MORSE_TRANSMISSION_ONGOING;
}

void Handle_Button_ONE_Events()
{
    TurnOnLed();
    STRING_ONGOING=1;
    MORSE_TRANSMISSION_ONGOING=1;
    Upd_Time_B1_Presses();
}

void Handle_Button_TWO_Events()
{
    Eval_Time_B1_Presses();
    STRING_ONGOING=0;
    if(!MORSE_TRANSMISSION_ONGOING) {
        TurnOnLed();
        MORSE_TRANSMISSION_ONGOING=1;
    }
}

void Upd_Time_B1_Presses()
{
    ++b1_num_presses;
    if(b1_num_presses==1) {
        b1_time_marks[0] = global_ticks;
    }
    else{
        b1_time_marks[1] = b1_time_marks[0];
        b1_time_marks[0] = global_ticks;
    }
}

void UpdateCurSignalValues(char* cur_signal, int* cur_signal_on_time, int* cur_signal_wait_time, 
                           int* cur_signal_len, int* cur_signal_total_size)
{
    if(STRING_ONGOING){
        *cur_signal              =   MESSAGE[cur_lett].c;                       //Letter Currently being translated
        *cur_signal_on_time      =   MESSAGE[cur_lett].on_times[cur_periods];   //Period Led On during current letter translation
        *cur_signal_wait_time    =   MESSAGE[cur_lett].wait_times[cur_periods]; //Period Led Off during current letter translation
        *cur_signal_len          =   MESSAGE[cur_lett].size;                    //Number of individual signals that compose current letter translation
        *cur_signal_total_size   =   MESSAGE_SIZE;
    }
    else {
        *cur_signal              =   NUMBERS[cur_num_array[cur_num]].c;
        *cur_signal_on_time      =   NUMBERS[cur_num_array[cur_num]].on_times[cur_periods];
        *cur_signal_wait_time    =   NUMBERS[cur_num_array[cur_num]].wait_times[cur_periods];
        *cur_signal_len          =   NUMBERS[cur_num_array[cur_num]].size;
        *cur_signal_total_size   =   cur_num_array_size;
    }
}
        
void Eval_Time_B1_Presses()
{
    if(b1_num_presses==0){
        TIME_ELAPSED_BTWN_B1S = 0;
        cur_num = 0;
        cur_num_array[0] = 0;
        cur_num_array_size=1;
    }
    else if(b1_num_presses==1) {
        TIME_ELAPSED_BTWN_B1S = b1_time_marks[0];
        Parse_Time_B1_Presses();
    }
    else{
        TIME_ELAPSED_BTWN_B1S = b1_time_marks[0] - b1_time_marks[1];
        Parse_Time_B1_Presses();
    }
}

int EndOfIndOrTotalTrans(short int CurrentLetterIndex, int SizeOfCurrentSignal)
{
    int CLI = CurrentLetterIndex;
    int SOCS = SizeOfCurrentSignal;
    return (CLI == (SOCS - 1)); //Checks if current letter/signal is the last one and if LED is ON
}

void FetchMorseNumIndexs(int num, int tmp_num_array[10])
{
    int i = 0;
    int tmp = 0;
    int rem[10];

    while(1) {
        tmp = num;
        tmp /= 10;
        if (!(tmp < 1)){
            rem[i] = num % 10;
            ++i;      
            num = tmp;
        }
        else{
            rem[i] = num;
            ++i;      
            break;
        }
    }

    for(int j=0; j<i; ++j) tmp_num_array[j] = rem[j];

    cur_num_array_size = i;
}

void Parse_Time_B1_Presses()
{
    int parsed_num = TIME_ELAPSED_BTWN_B1S;

    int tmp_num_array[10] = { 0 };
    FetchMorseNumIndexs(parsed_num, tmp_num_array);

    int i = 0;
    for(int j=cur_num_array_size-1; j>=0; --j) {
        cur_num_array[i] = tmp_num_array[j];
        ++i;
    }

    cur_num = 0;
}

void Initialize_GPIOS()
{
    XMC_GPIO_CONFIG_t button_one_config;
    button_one_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    button_one_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    button_one_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;

    XMC_GPIO_CONFIG_t button_two_config;
    button_two_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    button_two_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    button_two_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;

    XMC_GPIO_CONFIG_t led_config;
    led_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
    led_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    led_config.output_strength = XMC_GPIO_OUTPUT_STRENGTH_MEDIUM;

    XMC_GPIO_Init(LED1, &led_config);                   //Initialize LED_1
    XMC_GPIO_Init(BUTTON1, &button_one_config);         //Initialize button1 
    XMC_GPIO_Init(BUTTON2, &button_two_config);         //Initialize button2
}

