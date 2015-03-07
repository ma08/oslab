#define UPQ 100
#define DOWNQ 101

struct mymsg{
    long    mtype;     /* Message type. */
    char    mtext[100];  /* Message text. */
};
typdef struct mymsg msg;