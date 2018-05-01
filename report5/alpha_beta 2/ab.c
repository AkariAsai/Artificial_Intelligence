#include "defs.h"
static struct state *st0;

main()
{

    struct state *init(),*bag1;

    st0=init();


/*    st0->ban[0][1]=MINE;  */
/*    st0->ban[1][1]=YOURS; */


    bag1=init();
    expand(st0,MAXDEPTH,MAXMODE,MININT,MAXINT,1,bag1);
/*    expand(st0,MAXDEPTH,MINMODE,MININT,MAXINT,1,bag1); */ 
    free_bags(bag1); 
    printf("Showing ... \n");
    show(st0,0);
    printf("Playing ... \n");
    playhand(st0,MAXMODE);
}

show(st,d)
struct state *st;
int d;
{
    struct state *w;

    printf("Depth -> %d\n",d);
    print(st);
    if (st->daut!=NULL) 
	for (w=st->daut; w!=NULL; w=w->sist)
	        show(w,d+1);

}


expand(st,depth,mode,alpha,beta,judge,bags)
struct state *st,*bags;
int depth, mode,alpha,beta,judge;
{
    register int i,j;
    struct state *w, *copy(),*init();
    struct state *newbags;
    struct state *newbags2;


    if (st==NULL) return;
    if (depth<=0 || check(st)!=CONT) return;

    printf("Expanding depth=%d mode=%d... \n",depth,mode);
    print(st);

    st->eval = (mode==MINMODE) ? MAXINT : MININT;
    if (st->daut==NULL) 
	for(i=0; i<3; i++) 
	    for(j=0; j<3; j++) {
		switch (move(i, j, w=copy(st),mode,depth,judge,bags)) {
		case DISABLE:
		    stfree(w);
		    continue;
		case ENABLE:
		    printf("ENABLE with i=%d,j=%d\n",i,j); 
		    print(w);
		    newbags=init();
		    expand(w,depth-1,TOGGLE(mode),alpha,beta,judge,newbags);
		    free_bags(newbags); 
		    w->sist = st->daut;
		    st->daut = w;

		    if ((mode==MINMODE && w->eval<st->eval) ||
			(mode==MAXMODE && w->eval>st->eval))
			st->eval = w->eval;
		    if (mode==MINMODE && st->eval<=alpha) {
			printf("Alpha cut %d =< %d\n",st->eval,alpha);
			print(st);
			return;
		    }
		    if (mode==MAXMODE && st->eval>=beta) {
			printf("Beta cut %d >= %d\n",st->eval,beta);
			print(st);
			return;
		    }
		    if (mode==MINMODE) beta = st->eval;
		    if (mode==MAXMODE) alpha = st->eval;
		}
	    } 
    else 
	for (w=st->daut; w!=NULL; w=w->sist) {
	    newbags2=init();
/*	    expand(w,depth-1,TOGGLE(mode),judge,newbags2); */
	    expand(w,1,TOGGLE(mode),judge,newbags2);
	    free_bags(newbags2); 
	}
}



playhand(st,mode)
struct state *st;
int mode;
{
    struct state *w,*t;
    register int i=0;

    for (; st!=NULL; st=st->daut) {
	if (check(st)!=CONT) {
	    switch(check(st)) {
		case MYWIN: printf("I win\n"); break;
		case YOUWIN: printf("You win\n"); break;
		case DRAWN: printf("Draw\n"); break;
		}
	    break;
	}
	for (w=st->daut, st->daut=NULL; w!=NULL; w=t) {
	    t=w->sist;
	    w->sist = NULL;
	    if (w->eval==st->eval && st->daut==NULL)
		st->daut =w;
	    else stfree(w);
	}
	stfree(st->daut->daut);
	st->daut->daut=NULL;
	mode = TOGGLE(mode);
	expand(st->daut,1,mode,MININT,MAXINT,0,NULL);
/*	expand(st->daut,MAXDEPTH,mode,MININT,MAXINT,0,NULL); */
    }
    printf("Answering...\n",i);
    for (w=st0; w!=NULL; w=w->daut) {
	printf("(%d)\n",i);
	print(w);
	i++;
    }
}

