#define UPQ 100
#define DOWNQ 101
#define MSGSIZE 200
struct mymsg{
    long    mtype;     /* Message type. */
    char    mtext[MSGSIZE];  /* Message text. */
};
typedef struct mymsg msg;