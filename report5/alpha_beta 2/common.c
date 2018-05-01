#include "defs.h"
#include <malloc.h>

#define BONUS 10000

move(i,j,st,mode,depth,judge,bags)
struct state *st,*bags;
int i,j,mode,depth,judge;
{
    register int n;
    struct state *p;



/*
    printf("To move mode=%d i=%d j=%d... \n",mode,i,j);
    print(st);
*/

    if (st->ban[i][j] != EMPTY) return DISABLE;
    if (mode==MAXMODE) st->ban[i][j]=MINE;
    else st->ban[i][j]=YOURS;

    if ((judge==1) && check_closed(st,bags)) {
	st->ban[i][j]=EMPTY;
	return DISABLE;
    }
    if (judge==1) {
/*	printf("putting ...\n"); 
	print(st); */
	put_closed(st,bags);
/*	printf(" with bags\n"); 
	if (bags!=NULL)
	    for (p=bags; p!=NULL; p=p->next)
 		print(p);  
	printf(" with end of bags\n"); */
    }
    st->eval = evaluate(st);
/*    if (st->eval ==BONUS) {
	printf("BONUS");
	print(st);
    } */
    return ENABLE;
}

free_bags(closed)
struct state *closed;
{

    if (closed->next != NULL) free_bags(closed->next);
    free(closed);  
}

put_closed(st,closed)
struct state *st,*closed;
{
    struct state *w,*copy();
    
    w=copy(st);

    if (closed==NULL) {
	printf("Error in put_closed NULL\n"); 
    }
    w->next = closed->next;
    closed->next = w;
}


int check_closed(st,closed)
struct state *st,*closed;
{
    struct state *p;

/*
    printf("checking...\n"); 
    print(st);
    printf(" with Q \n"); 
    if (closed!=NULL)
	for (p=closed; p!=NULL; p=p->next)
	    print(p);  
    printf(" with end of Q \n"); 
*/
    for (p=closed; p!=NULL; p=p->next) {
	    if (eq_state(st,p)) {
/*	    printf("---> Euqal states\n");  */
	    return 1;
	}
    }
    return 0;
}

eq_state(s1,s2)
struct state *s1,*s2;
{
    int tmp[3][3],tmp2[3][3];

    if (eq_3_3(s1->ban,s2->ban)) return 1;
    rotate90(s1->ban,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;
    rotate180(s1->ban,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;
    rotate270(s1->ban,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;
    rev(s1->ban,tmp2);
    if (eq_3_3(tmp2,s2->ban)) return 1;
    rotate90(tmp2,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;
    rotate180(tmp2,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;
    rotate270(tmp2,tmp);
    if (eq_3_3(tmp,s2->ban)) return 1;

    return 0;

}


eq_3_3(ban1,ban2)
int ban1[3][3],ban2[3][3];
{
    register int i,j;

    for (i=0; i<3; i++) 
	for (j=0; j<3; j++)
	    if (ban1[i][j]!=ban2[i][j]) return 0;
    return 1;
}    

rev(ban1,ban2)
int ban1[3][3],ban2[3][3];
{
    register int i,j;

    for (i=0; i<3; i++) 
	for (j=0; j<3; j++)
	    ban2[2-i][j]=ban1[i][j];
}    


rotate90(ban1,ban2)
int ban1[3][3],ban2[3][3];
{
    register int i,j;

    for (i=0; i<3; i++) 
	for (j=0; j<3; j++)
	    ban2[j][2-i]=ban1[i][j];
}    

rotate180(ban1,ban2)
int ban1[3][3],ban2[3][3];
{
    register int i,j;

    for (i=0; i<3; i++) 
	for (j=0; j<3; j++)
	    ban2[2-i][2-j]=ban1[i][j];
}    

rotate270(ban1,ban2)
int ban1[3][3],ban2[3][3];
{
    register int i,j;

    for (i=0; i<3; i++) 
	for (j=0; j<3; j++)
	    ban2[2-j][i]=ban1[i][j];
}    


static int mine1,yours1,mine2,yours2,mine3,yours3;

check(st)
struct state *st;
{
    register int i,j;
    int x[8],o[8];

    for (i=0; i<8; i++)
	x[i]=o[i]=0;

    mine1=yours1=mine2=yours2=mine3=yours3=0;
    /*  set mini yoursi */
    for (i=0; i<3; i++)
	for (j=0; j<3; j++) {
            if (st->ban[i][j]==MINE) {o[j]++;x[j]--;};
            if (st->ban[i][j]==YOURS) {x[j]++;o[j]--;};
            if (st->ban[j][i]==MINE) {o[j+3]++;x[j+3]--;};
            if (st->ban[j][i]==YOURS) {x[j+3]++;o[j+3]--;};
    }

    for (i=0; i<3; i++){
            if (st->ban[i][i]==MINE) {o[6]++;x[6]--;};
            if (st->ban[i][i]==YOURS) {x[6]++;o[6]--;};
            if (st->ban[i][2-i]==MINE) {o[7]++;x[7]--;};
            if (st->ban[i][2-i]==YOURS) {x[7]++;o[7]--;};
    }
    for (i=0; i<8; i++){
             switch(o[i]) {
                   case 0: break;
                   case 1: mine1++;break;
                   case 2: mine2++;break;
                   case 3: mine3++;break;
/*                   default: printf("error in check mine %d\n",o[i]);*/
             }
            switch(x[i]) {
                   case 0: break;
                   case 1: yours1++;break;
                   case 2: yours2++;break;
                   case 3: yours3++;break;
/*                   default: printf("error in check yours %d\n",o[i]); */
             }
    }
/*
    printf("Eval\n");
    for (i=0; i<8; i++)
	    printf("o[%d]=%d ",i,o[i]);
    printf("\n");
    for (i=0; i<8; i++)
	    printf("x[%d]=%d ",i,x[i]);
    printf("\n");
    print(st);
    printf("mine1=%d\n",mine1);    
    printf("mine2=%d\n",mine2);    
    printf("mine3=%d\n",mine3);    
    printf("yours1=%d\n",yours1);    
    printf("yours2=%d\n",yours2);    
    printf("yours3=%d\n",yours3);    
*/
    if (mine3>0) {
	return MYWIN;
    }
    if (yours3>0) {
	return YOUWIN;
    }
    if (full(st)) return DRAWN;
    return CONT;
}


evaluate(st)
struct state *st;
{
    switch(check(st)) {
      case MYWIN: 
/*	printf("MYWIN\n"); 
	print(st);   */
	return BONUS;
      case YOUWIN: 
/*	printf("YOUWIN\n");
	print(st);  */
	return -BONUS;
      default: return 7*mine1+31*mine2-15*yours1-63*yours2;
      }
}

static int lev = 0;

int full(st)
struct state *st;
{
    register int i,j;
    
    for (i=0; i<3; i++)
        	for (j=0; j<3; j++) 
	               if (st->ban[i][j] == EMPTY) return 0;
	   return 1;
}

print(st)
struct state *st;
{
    register int i,j;

    printf("------\n");    
    for (i=0; i<3; i++) {
	printf("|");
	for (j=0; j<3; j++) {
	    if (st->ban[i][j] == MINE) printf("o|");
	    else  if (st->ban[i][j] == YOURS) printf("x|");
	    else  printf(" |");
	}
	putchar('\n');
	printf("------\n");
	
    }
    
    printf(" value(%d)\n",st->eval);
    putchar('\n');

}


stfree(st)
struct state *st;
{
    if (st==NULL) return;
    stfree(st->sist);
    stfree(st->daut);
    free(st);
}

#define CELLSIZE sizeof(struct state)

struct state *copy(st)
struct state *st;
{
    register int i,j;
    struct state *new;
    
    if ((new = (struct state *) malloc(CELLSIZE)) == NULL) {
	perror(" No more memory !!");
    }

    new->daut = new->sist = NULL;
    new->moth = st;
    
    for (i=0; i<3; i++)
	for (j=0; j<3; j++) 
	    new->ban[i][j]=st->ban[i][j];

    new->eval = 0;
    new->next = NULL;
    return new;
}

struct state *init()
{
    int i,j;
    struct state *st;
    
    if ((st = (struct state *) malloc(CELLSIZE)) == NULL) {
	perror(" No more memory !!");
    }

    for (i=0; i<3; i++)
	for (j=0; j<3; j++) 
	    st->ban[i][j]=EMPTY;

    st->eval =  0;
    st->moth = st-> sist = st->daut = NULL;
    st->next = NULL;
    return st;
}


