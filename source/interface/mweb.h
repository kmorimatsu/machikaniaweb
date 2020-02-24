// Prototypes
extern const unsigned int _html5_func[];

/*
	Macros for HTML5 functions
*/
#define html5func(x) \
	asm volatile("addu $v1,$ra,$zero"); \
	asm volatile("ori $v0,$zero," x ); \
	asm volatile("jal _html5_func")
//*/
#define HTML5FUNC_ps2readkey "0"
#define HTML5FUNC_FindFirst  "1"
#define HTML5FUNC_FindNext   "2"
#define HTML5FUNC_FSmkdir    "3"
#define HTML5FUNC_FSgetcwd   "4"
#define HTML5FUNC_FSchdir    "5"
#define HTML5FUNC_FSfwrite   "6"
#define HTML5FUNC_FSremove   "7"
#define HTML5FUNC_FSrename   "8"
#define HTML5FUNC_FSfeof     "9"
#define HTML5FUNC_FSftell    "10"
#define HTML5FUNC_FSfseek    "11"
#define HTML5FUNC_FSfread    "12"
#define HTML5FUNC_FSrewind   "13"
#define HTML5FUNC_FSfclose   "14"
#define HTML5FUNC_FSfopen    "15"
#define HTML5FUNC_FSInit     "16"
#define HTML5FUNC_set_palette   "256"
#define HTML5FUNC_set_bgcolor   "257"
#define HTML5FUNC_set_videomode "258"
