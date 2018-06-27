#define MAX_INPUT_ARGS           2
#define MAX_CLIENTS              5
#define SERVER_MODE              1
#define CLIENT_MODE              2
#define DISCON_MODE              3
#define BAD_INPUT_ARGS           99
#define DEFAULT_QUE_NAME         "\\QUEUES\\MYQUEUE"
#define DEFAULT_SEM_NAME         "\\SEM32\\EVENTQUE"
#define DEFAULT_PAGE_SIZE        4096
#define DEFAULT_QUE_FLAG         QUE_FIFO | QUE_CONVERT_ADDRESS
#define DEFAULT_SEG_NAME         "\\SHAREMEM\\MYQUEUE.SHR"
#define DEFAULT_SEG_FLAG         PAG_WRITE | PAG_COMMIT
#define TOKEN_F2_SWITCH          0x0000003CL
#define TOKEN_F3_DISCON          0x0000003DL
#define RETURN_CHAR              0x0D
#define LINE_FEED_CHAR           0x0A
#define FUNC_KEYS_CHAR           0x00
#define EXTD_KEYS_CHAR           0xE0
#define CLEAR_HI_WORD            0x0000FFFFL
#define ULONG_NULL               0L
#define PROGRAM_ERROR            999

#define MAX_COLOR_LEN            14

char    SERVER_COLOR  [MAX_COLOR_LEN] = {10,13,27,91,48,59,51,50,59,52,48,109,0};/* "\n[0;32;40m"*/
char    WHITE_COLOR  [MAX_COLOR_LEN] = {10,13,27,91,48,59,51,55,59,52,48,109,0};/* "\n[0;37;40m"*/
char    CLIENT0_COLOR [MAX_COLOR_LEN] = {27,91,48,59,51,49,59,52,55,109,0};/* "[0;31;40m"*/
char    CLIENT1_COLOR [MAX_COLOR_LEN] = {27,91,48,59,51,50,59,52,55,109,0};/* "[0;32;40m"*/
char    CLIENT2_COLOR [MAX_COLOR_LEN] = {27,91,48,59,51,51,59,52,55,109,0};/* "[0;33;40m"*/
char    CLIENT3_COLOR [MAX_COLOR_LEN] = {27,91,48,59,51,54,59,52,55,109,0};/* "[0;34;40m"*/
char    CLIENT4_COLOR [MAX_COLOR_LEN] = {27,91,48,59,51,53,59,52,55,109,0};/* "[0;35;40m"*/


typedef struct  _MYQUEUESTRUC {
   BYTE szColor [MAX_COLOR_LEN] ;
   PID ulPid ;
} MYQUEUESTRUC, * PMYQUEUESTRUC ;

HQUEUE        hqQueue ;
USHORT        usClientIndex = MAX_CLIENTS ;
PVOID         pvData ;
HEV           hsmSem ;
PMYQUEUESTRUC pmqsClient ;
CHAR          chToken = 0 ;
CHAR          aachColors [MAX_CLIENTS] [MAX_COLOR_LEN] ;
VOID APIENTRY CleanUp ( ULONG  ulTermCode );/* ExitList routines must be declared with VOID APIENTRY*/
