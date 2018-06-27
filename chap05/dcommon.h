/*     this nibble applies if file already exists                xxxx */

#define OPEN_ACTION_FAIL_IF_EXISTS     0x0000  /* ---- ---- ---- 0000 */
#define OPEN_ACTION_OPEN_IF_EXISTS     0x0001  /* ---- ---- ---- 0001 */
#define OPEN_ACTION_REPLACE_IF_EXISTS  0x0002  /* ---- ---- ---- 0010 */

/*     this nibble applies if file does not exist           xxxx      */

#define OPEN_ACTION_FAIL_IF_NEW        0x0000  /* ---- ---- 0000 ---- */
#define OPEN_ACTION_CREATE_IF_NEW      0x0010  /* ---- ---- 0001 ---- */

/* DosOpen/DosSetFHandState flags */

#define OPEN_ACCESS_READONLY        0x0000  /* ---- ---- ---- -000 */
#define OPEN_ACCESS_WRITEONLY       0x0001  /* ---- ---- ---- -001 */
#define OPEN_ACCESS_READWRITE       0x0002  /* ---- ---- ---- -010 */
#define OPEN_SHARE_DENYREADWRITE    0x0010  /* ---- ---- -001 ---- */
#define OPEN_SHARE_DENYWRITE        0x0020  /* ---- ---- -010 ---- */
#define OPEN_SHARE_DENYREAD         0x0030  /* ---- ---- -011 ---- */
#define OPEN_SHARE_DENYNONE         0x0040  /* ---- ---- -100 ---- */
#define OPEN_FLAGS_NOINHERIT        0x0080  /* ---- ---- 1--- ---- */
#define OPEN_FLAGS_NO_LOCALITY      0x0000  /* ---- -000 ---- ---- */
#define OPEN_FLAGS_SEQUENTIAL       0x0100  /* ---- -001 ---- ---- */
#define OPEN_FLAGS_RANDOM           0x0200  /* ---- -010 ---- ---- */
#define OPEN_FLAGS_RANDOMSEQUENTIAL 0x0300  /* ---- -011 ---- ---- */
#define OPEN_FLAGS_NO_CACHE         0x1000  /* ---1 ---- ---- ---- */
#define OPEN_FLAGS_FAIL_ON_ERROR    0x2000  /* --1- ---- ---- ---- */
#define OPEN_FLAGS_WRITE_THROUGH    0x4000  /* -1-- ---- ---- ---- */
#define OPEN_FLAGS_DASD             0x8000  /* 1--- ---- ---- ---- */


#define SERVER_MODE              1
#define CLIENT_MODE              2
#define SERVER_COLOR             "\n[0;32;40m"
#define CLIENT_COLOR             "\n[0;36;40m"
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

char           achPipeName [MAX_PIPE_NAME_LEN] ;
unsigned short hpPipe ;
char           chToken ;

unsigned short ConnToClient   ( void ) ;
unsigned short ConnToServer   ( void ) ;
unsigned short SendToServer   ( unsigned long ulHiLoCh ) ;
unsigned short RecvFromClient ( unsigned long * pulHiLoCh, unsigned long * pulBytesDone );
