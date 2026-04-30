#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

#define N 8

typedef enum { P_KNIGHT=0, P_ROOK, P_BISHOP, P_QUEEN } PType;
static const char *P_EN[] = {"Knight","Rook","Bishop","Queen"};

typedef enum {
    Q_KNIGHT_MIN,    /* minimum moves for knight A->B                 */
    Q_PIECE_MOVE,    /* give a valid move for a piece                 */
    Q_INTERSECT,     /* square reachable by 2 pieces                  */
    Q_BLOCKED,       /* piece with obstacle, can it reach target?     */
    Q_COUNT_KNIGHT,  /* how many squares can the knight reach?        */
    Q_BISHOP_EVER,   /* can the bishop ever reach this square?        */
    Q_FORK,          /* can the knight attack 2 squares at once?      */
    Q_WHICH_PIECE,   /* which piece makes this move?                  */
    Q_SQUARE_COLOR,  /* color of a square                             */
    Q_BISHOP_ONE,    /* can the bishop reach this square in 1 move?   */
} QType;

typedef struct {
    QType qtype;
    PType ptype, ptype2;
    int   c1,r1, c2,r2, c3,r3;
    bool  bool_ans;
    int   int_ans;
    char  str_ans[32];
} Q;

/* ---- move logic (empty board) ---- */
static bool reaches(PType p, int fc, int fr, int tc, int tr) {
    int dc=abs(tc-fc), dr=abs(tr-fr);
    switch(p) {
        case P_KNIGHT: return (dc==1&&dr==2)||(dc==2&&dr==1);
        case P_ROOK:   return (fc==tc||fr==tr)&&(dc+dr>0);
        case P_BISHOP: return dc==dr&&dc>0;
        case P_QUEEN:  return ((fc==tc||fr==tr)||(dc==dr))&&(dc+dr>0);
    }
    return false;
}

static bool is_dark(int col, int row) { return (col+row)%2==1; }

static int knight_bfs(int c1, int r1, int c2, int r2) {
    if (c1==c2&&r1==r2) return 0;
    int dist[N][N]; memset(dist,-1,sizeof(dist));
    int q[64][2], head=0, tail=0;
    dist[c1][r1]=0; q[tail][0]=c1; q[tail][1]=r1; tail++;
    int dx[]={2,2,-2,-2,1,1,-1,-1}, dy[]={1,-1,1,-1,2,-2,2,-2};
    while (head<tail) {
        int c=q[head][0], r=q[head][1]; head++;
        for (int i=0;i<8;i++) {
            int nc=c+dx[i], nr=r+dy[i];
            if (nc<0||nc>=N||nr<0||nr>=N||dist[nc][nr]!=-1) continue;
            dist[nc][nr]=dist[c][r]+1;
            if (nc==c2&&nr==r2) return dist[nc][nr];
            q[tail][0]=nc; q[tail][1]=nr; tail++;
        }
    }
    return dist[c2][r2];
}

static int count_knight(int c, int r) {
    int cnt=0, dx[]={2,2,-2,-2,1,1,-1,-1}, dy[]={1,-1,1,-1,2,-2,2,-2};
    for (int i=0;i<8;i++) { int nc=c+dx[i],nr=r+dy[i]; if(nc>=0&&nc<N&&nr>=0&&nr<N) cnt++; }
    return cnt;
}

/* ---- notation ---- */
static bool parse_sq(const char *s, int *col, int *row) {
    if (!s||!s[0]||!s[1]) return false;
    int c=tolower((unsigned char)s[0])-'a', r=8-(s[1]-'0');
    if (c<0||c>=N||r<0||r>=N) return false;
    *col=c; *row=r; return true;
}
static void tostr(int col, int row, char *b) { b[0]='a'+col; b[1]='0'+(8-row); b[2]=0; }

/* ---- input helpers ---- */
static char *trim(char *s) {
    while (*s==' '||*s=='\t') s++;
    int l=(int)strlen(s);
    while (l>0&&(s[l-1]==' '||s[l-1]=='\n'||s[l-1]=='\r'||s[l-1]=='\t')) s[--l]=0;
    return s;
}
static void to_lower(char *s) { for(;*s;s++) *s=(char)tolower((unsigned char)*s); }
static bool is_yes(const char *s) { return strcmp(s,"yes")==0||strcmp(s,"y")==0||strcmp(s,"1")==0; }
static bool is_no(const char *s)  { return strcmp(s,"no")==0||strcmp(s,"n")==0||strcmp(s,"0")==0; }

/* ---- generation ---- */
static Q gen_knight_min(void) {
    Q q={0}; q.qtype=Q_KNIGHT_MIN;
    do {
        q.c1=rand()%N; q.r1=rand()%N;
        q.c2=rand()%N; q.r2=rand()%N;
        q.int_ans=knight_bfs(q.c1,q.r1,q.c2,q.r2);
    } while (q.int_ans<2||q.int_ans>5);
    return q;
}

static Q gen_piece_move(void) {
    Q q={0}; q.qtype=Q_PIECE_MOVE;
    q.ptype=(PType)(rand()%4);
    q.c1=rand()%N; q.r1=rand()%N;
    return q;
}

static Q gen_intersect(void) {
    Q q={0}; q.qtype=Q_INTERSECT;
    PType pool[]={P_ROOK,P_ROOK,P_BISHOP,P_BISHOP,P_QUEEN,P_KNIGHT};
    for (int a=0;a<300;a++) {
        q.ptype=pool[rand()%6]; q.ptype2=pool[rand()%6];
        q.c1=rand()%N; q.r1=rand()%N;
        q.c2=rand()%N; q.r2=rand()%N;
        if (q.c1==q.c2&&q.r1==q.r2) continue;
        for (int i=0;i<N;i++) for (int j=0;j<N;j++) {
            if (i==q.c1&&j==q.r1) continue;
            if (i==q.c2&&j==q.r2) continue;
            if (reaches(q.ptype,q.c1,q.r1,i,j)&&reaches(q.ptype2,q.c2,q.r2,i,j)) return q;
        }
    }
    return gen_intersect();
}

static Q gen_blocked(void) {
    Q q={0}; q.qtype=Q_BLOCKED;
    q.ptype=(rand()%2)?P_ROOK:P_BISHOP;
    for (int a=0;a<200;a++) {
        q.c1=rand()%N; q.r1=rand()%N;
        bool blocked=rand()%2;
        int dc=0,dr=0;
        if (q.ptype==P_ROOK) {
            int d=rand()%4;
            if(d==0)dc=1; else if(d==1)dc=-1; else if(d==2)dr=1; else dr=-1;
        } else { dc=(rand()%2)?1:-1; dr=(rand()%2)?1:-1; }
        int dist=3+rand()%4;
        int tc=q.c1+dc*dist, tr=q.r1+dr*dist;
        if (tc<0||tc>=N||tr<0||tr>=N) continue;
        q.c3=tc; q.r3=tr;
        if (blocked) {
            int bd=1+rand()%(dist-1);
            q.c2=q.c1+dc*bd; q.r2=q.r1+dr*bd; q.bool_ans=false;
        } else {
            bool ok=false; int bx=0,by=0;
            for (int t=0;t<50&&!ok;t++) {
                bx=rand()%N; by=rand()%N;
                bool on_path=false;
                for (int d=1;d<=dist;d++) if(bx==q.c1+dc*d&&by==q.r1+dr*d){on_path=true;break;}
                if (!on_path&&!(bx==q.c1&&by==q.r1)) ok=true;
            }
            if (!ok) continue;
            q.c2=bx; q.r2=by; q.bool_ans=true;
        }
        return q;
    }
    return gen_blocked();
}

static Q gen_count_knight(void) {
    Q q={0}; q.qtype=Q_COUNT_KNIGHT;
    q.c1=rand()%N; q.r1=rand()%N;
    q.int_ans=count_knight(q.c1,q.r1); return q;
}

static Q gen_bishop_ever(void) {
    Q q={0}; q.qtype=Q_BISHOP_EVER;
    q.c1=rand()%N; q.r1=rand()%N;
    do { q.c2=rand()%N; q.r2=rand()%N; } while (q.c2==q.c1&&q.r2==q.r1);
    q.bool_ans=(is_dark(q.c1,q.r1)==is_dark(q.c2,q.r2));
    return q;
}

static Q gen_fork(void) {
    Q q={0}; q.qtype=Q_FORK;
    q.bool_ans=rand()%2;
    q.c1=rand()%N; q.r1=rand()%N;
    int dx[]={2,2,-2,-2,1,1,-1,-1}, dy[]={1,-1,1,-1,2,-2,2,-2};
    int valid[8][2]; int nv=0;
    for (int i=0;i<8;i++) {
        int nc=q.c1+dx[i], nr=q.r1+dy[i];
        if (nc>=0&&nc<N&&nr>=0&&nr<N) { valid[nv][0]=nc; valid[nv][1]=nr; nv++; }
    }
    if (nv<2) { q.c1=3; q.r1=3; nv=8; for(int i=0;i<8;i++){valid[i][0]=q.c1+dx[i];valid[i][1]=q.r1+dy[i];} }
    if (q.bool_ans) {
        int i=rand()%nv, j; do{j=rand()%nv;}while(j==i);
        q.c2=valid[i][0]; q.r2=valid[i][1];
        q.c3=valid[j][0]; q.r3=valid[j][1];
    } else {
        int i=rand()%nv;
        q.c2=valid[i][0]; q.r2=valid[i][1];
        for (int a=0;a<100;a++) {
            int nc=rand()%N, nr=rand()%N;
            if (!reaches(P_KNIGHT,q.c1,q.r1,nc,nr)) { q.c3=nc; q.r3=nr; break; }
        }
    }
    return q;
}

static Q gen_which_piece(void) {
    Q q={0}; q.qtype=Q_WHICH_PIECE;
    for (int a=0;a<200;a++) {
        q.c1=rand()%N; q.r1=rand()%N;
        q.c2=rand()%N; q.r2=rand()%N;
        if (q.c1==q.c2&&q.r1==q.r2) continue;
        int dc=abs(q.c2-q.c1), dr=abs(q.r2-q.r1);
        if ((dc==1&&dr==2)||(dc==2&&dr==1)) { strcpy(q.str_ans,"knight"); return q; }
        if (q.c1==q.c2||q.r1==q.r2)        { strcpy(q.str_ans,"rook");   return q; }
        if (dc==dr)                         { strcpy(q.str_ans,"bishop"); return q; }
        if (rand()%3==0)                    { strcpy(q.str_ans,"none");   return q; }
    }
    return gen_which_piece();
}

static Q gen_square_color(void) {
    Q q={0}; q.qtype=Q_SQUARE_COLOR;
    q.c1=rand()%N; q.r1=rand()%N;
    q.bool_ans=is_dark(q.c1,q.r1); return q;
}

static Q gen_bishop_one(void) {
    Q q={0}; q.qtype=Q_BISHOP_ONE;
    q.c1=rand()%N; q.r1=rand()%N;
    q.bool_ans=rand()%2;
    if (q.bool_ans) {
        int dirs[4][2]={{1,1},{1,-1},{-1,1},{-1,-1}};
        for (int d=0;d<4;d++) for (int dist=1;dist<N;dist++) {
            int nc=q.c1+dirs[d][0]*dist, nr=q.r1+dirs[d][1]*dist;
            if (nc>=0&&nc<N&&nr>=0&&nr<N) { q.c2=nc; q.r2=nr; return q; }
        }
    } else {
        for (int a=0;a<100;a++) {
            int nc=rand()%N, nr=rand()%N;
            if (nc==q.c1&&nr==q.r1) continue;
            if (abs(nc-q.c1)!=abs(nr-q.r1)) { q.c2=nc; q.r2=nr; return q; }
        }
    }
    return gen_bishop_one();
}

static Q gen_question(void) {
    switch(rand()%20) {
        case 0: case 1: case 2: return gen_knight_min();
        case 3: case 4:         return gen_piece_move();
        case 5: case 6: case 7: return gen_intersect();
        case 8: case 9: case 10: return gen_blocked();
        case 11: case 12:       return gen_count_knight();
        case 13: case 14:       return gen_bishop_ever();
        case 15: case 16:       return gen_fork();
        case 17:                return gen_which_piece();
        case 18:                return gen_square_color();
        default:                return gen_bishop_one();
    }
}

/* ---- validation ---- */
static bool check(const Q *q, const char *raw) {
    char s[64]; strncpy(s,raw,63); s[63]=0; to_lower(s);
    int ac, ar;
    switch(q->qtype) {
        case Q_KNIGHT_MIN:   return atoi(s)==q->int_ans;
        case Q_PIECE_MOVE:   return parse_sq(s,&ac,&ar)&&reaches(q->ptype,q->c1,q->r1,ac,ar);
        case Q_INTERSECT:
            if (!parse_sq(s,&ac,&ar)) return false;
            if (ac==q->c1&&ar==q->r1) return false;
            if (ac==q->c2&&ar==q->r2) return false;
            return reaches(q->ptype,q->c1,q->r1,ac,ar)&&reaches(q->ptype2,q->c2,q->r2,ac,ar);
        case Q_BLOCKED:
            return (is_yes(s)&&q->bool_ans)||(is_no(s)&&!q->bool_ans);
        case Q_COUNT_KNIGHT: return atoi(s)==q->int_ans;
        case Q_BISHOP_EVER:
            return (is_yes(s)&&q->bool_ans)||(is_no(s)&&!q->bool_ans);
        case Q_FORK:
            return (is_yes(s)&&q->bool_ans)||(is_no(s)&&!q->bool_ans);
        case Q_WHICH_PIECE:
            return strcmp(s,q->str_ans)==0||
                   (strcmp(q->str_ans,"knight")==0&&strcmp(s,"n")==0)||
                   (strcmp(q->str_ans,"rook")==0&&strcmp(s,"r")==0)||
                   (strcmp(q->str_ans,"bishop")==0&&strcmp(s,"b")==0);
        case Q_SQUARE_COLOR:
            return (strcmp(s,"dark")==0&&q->bool_ans)||(strcmp(s,"light")==0&&!q->bool_ans);
        case Q_BISHOP_ONE:
            return (is_yes(s)&&q->bool_ans)||(is_no(s)&&!q->bool_ans);
    }
    return false;
}

/* ---- print question ---- */
static void print_q(const Q *q) {
    char s1[3],s2[3],s3[3];
    tostr(q->c1,q->r1,s1); tostr(q->c2,q->r2,s2); tostr(q->c3,q->r3,s3);
    printf("------------------------------------------------------------\n");
    switch(q->qtype) {
        case Q_KNIGHT_MIN:
            printf("Knight on %s. What is the minimum number of moves to reach %s?\n",s1,s2);
            break;
        case Q_PIECE_MOVE:
            printf("%s on %s. Give a valid square it can move to.\n",P_EN[q->ptype],s1);
            break;
        case Q_INTERSECT:
            printf("%s on %s  and  %s on %s.\n",P_EN[q->ptype],s1,P_EN[q->ptype2],s2);
            printf("Name a square that BOTH pieces can reach.\n");
            break;
        case Q_BLOCKED:
            printf("%s on %s, obstacle on %s.\n",P_EN[q->ptype],s1,s2);
            printf("Can it reach %s? (yes/no)\n",s3);
            break;
        case Q_COUNT_KNIGHT:
            printf("Knight on %s. How many squares can it reach from there?\n",s1);
            break;
        case Q_BISHOP_EVER:
            printf("Bishop on %s. Can it ever reach square %s? (yes/no)\n",s1,s2);
            break;
        case Q_FORK: {
            char s4[3]; tostr(q->c3,q->r3,s4);
            printf("Knight on %s. Can it attack %s AND %s at the same time? (yes/no)\n",s1,s2,s4);
            break;
        }
        case Q_WHICH_PIECE:
            printf("From %s to %s in one move.\n",s1,s2);
            printf("Which piece can do that? (knight/rook/bishop/none)\n");
            break;
        case Q_SQUARE_COLOR:
            printf("What color is square %s? (light/dark)\n",s1);
            break;
        case Q_BISHOP_ONE:
            printf("Bishop on %s. Can it reach %s in ONE move? (yes/no)\n",s1,s2);
            break;
    }
}

/* ---- main ---- */
int main(void) {
    srand((unsigned)time(NULL));
    int score=0, questions=0;
    bool running=true;

    printf("=== Chess Quiz ===\n");
    printf("Type 'quit' to exit.\n\n");

    while (running) {
        Q q=gen_question();
        print_q(&q);
        bool first=true;

        while (running) {
            printf("> "); fflush(stdout);
            char buf[64];
            if (!fgets(buf,sizeof(buf),stdin)) { running=false; break; }
            char *s=trim(buf);
            if (!*s) continue;
            if (!strcmp(s,"quit")||!strcmp(s,"q")) { running=false; break; }
            to_lower(s);
            if (check(&q,s)) {
                questions++;
                if (first) score++;
                printf("Correct! Score: %d/%d\n\n",score,questions);
                break;
            }
            first=false;
            printf("Incorrect. Try again.\n");
        }
    }

    printf("\nDone! %d/%d correct on first try.\n",score,questions);
    return 0;
}
