#define SERVER_MODE              1
#define CLIENT_MODE              2
#define SERVER_COLOR             "\n[0;32;40m"
#define CLIENT_COLOR             "\n[0;31;40m"
#define NORMAL_COLOR             "\n[0;37;40m"
#define REMOTE_PIPE              2
#define DISCON_MODE              3
#define BAD_INPUT_ARGS           99
#define MAX_PIPE_NAME_LEN        80
#define MAX_SERV_NAME_LEN        8
#define DEFAULT_PIPE_NAME        "\\PIPE\\MYPIPE"
#define DEFAULT_MAKE_MODE        NP_ACCESS_DUPLEX
#define DEFAULT_PIPE_MODE        NP_WMESG | NP_RMESG | 0x01
#define DEFAULT_OPEN_FLAG        OPEN_ACTION_OPEN_IF_EXISTS
#define DEFAULT_OPEN_MODE        OPEN_FLAGS_WRITE_THROUGH | \
                                 OPEN_FLAGS_FAIL_ON_ERROR | \
                                 OPEN_FLAGS_RANDOM |        \
                                 OPEN_SHARE_DENYNONE |      \
                                 OPEN_ACCESS_READWRITE
#define DEFAULT_OUTB_SIZE        0x1000
#define DEFAULT_INPB_SIZE        0x1000
#define DEFAULT_TIME_OUTV        20000L
#define TOKEN_F2_SWITCH          0x0000003CL
#define TOKEN_F3_DISCON          0x0000003DL
#define RETURN_CHAR              0x0D
#define LINE_FEED_CHAR           0x0A
#define FUNC_KEYS_CHAR           0x00
#define EXTD_KEYS_CHAR           0xE0
#define HAND_SHAKE_LEN           0x08
#define HAND_SHAKE_INP           "pIpEtEsT"
#define HAND_SHAKE_OUT           "PiPeTeSt"
#define HAND_SHAKE_ERROR         101
#define PROGRAM_ERROR            999

CHAR    achPipeName [MAX_PIPE_NAME_LEN] ;
HPIPE   hpPipe ;
CHAR    chToken ;

USHORT BadArgs ( USHORT usNumArgs, PCHAR apchArgs [] ) ;
APIRET ConnToClient ( VOID ) ;
APIRET ConnToServer ( VOID ) ;
APIRET SendToClient ( ULONG ulHiLoCh ) ;
APIRET RecvFromServer ( PULONG pulHiLoCh ) ;
