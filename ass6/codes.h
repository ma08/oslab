#define MSGSIZE 5000
#define MSGQ 101
#define SEM1 121
#define SEM2 131
#define SEM3 141
#define SHMPID 121
#define SHMMSG 131
struct mymsg{
    long    mtype;     /* Message type. */
    char    mtext[MSGSIZE];  /* Message text. */
};
typedef struct mymsg msg;