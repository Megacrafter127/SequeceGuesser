/*
 * main.c
 *
 *  Created on: 25.08.2016
 *      Author: Megacrafter127
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define x 0
#define n (x-1)
#define add (n-1)
#define subtract (add-1)
#define multiply (subtract-1)
#define divide (multiply-1)

#define range (CHAR_MAX-divide)

_Bool debug=0,showFormula=1;

static char char_sink;

char* generate(unsigned int try) {
	struct chain {
		char data;
		struct chain *next;
	} *root=NULL,*tmp=NULL;
	unsigned char cutoff=((RAND_MAX%range)+1)%range,cutoffX=((RAND_MAX%(1-divide))+1)%(1-divide);
	size_t finalLength=0;
	_Bool hasX=0;
	for(size_t length=1;length>0;length--,finalLength++) {
		unsigned int rnd=rand();
		if(!hasX&&length==1) {
			unsigned char cut=finalLength==0?((RAND_MAX%-divide)+1)%-divide:cutoffX;
			while(rnd<cut) rnd=rand();
			rnd-=cut;
			rnd%=finalLength==0?-divide:1-divide;
		} else {
			while(rnd<cutoff) rnd=rand();
			rnd-=cutoff;
			rnd%=range;
		}
		rnd+=divide;
		tmp=malloc(sizeof(struct chain));
		tmp->next=root;
		tmp->data=rnd;
		root=tmp;
		switch(rnd) {
		case x:
		case n:
			hasX=1;
			break;
		case add:
		case subtract:
		case multiply:
		case divide:
			length+=2;
			break;
		}
		if(debug) printf("%5d [%5d/%5d]\n",try,(unsigned int)(finalLength+1),(unsigned int)(length+finalLength));
	}
	char *ret=malloc(finalLength);
	for(size_t i=finalLength;i>0;i--) {
		ret[i-1]=root->data;
		tmp=root;
		root=root->next;
		free(tmp);
	}
	tmp=NULL;
	return ret;
}

size_t getLength(const char* formula) {
	if(!formula) return 0;
	size_t l=0;
	for(size_t length=1,i=0;length>0;i++,length--,l++) {
		if(formula[i]<n) {
			if(formula[i]<-6) return 0;
			length+=2;
		}
	}
	return l;
}

_Bool valid(const char* formula) {
	if(!formula) return 0;
	_Bool hasX=0;
	for(size_t length=1,i=0;length>0;i++,length--) {
		if(formula[i]<0) {
			if(formula[i]<-6) return 0;
			length+=2;
		} else if(formula[i]==0) hasX=1;
	}
	return hasX;
}

char* generateValid(unsigned seed) {
	srand(seed);
	char* ret=NULL;
	unsigned long long try=0;
	do {
		if(!debug) printf(".");
		if(ret) {
			free(ret);
		}
		ret=generate(++try);
	} while(!valid(ret));
	return ret;
}

char eval(const char *formula, const char var, const char time) {
	if(*formula>0) return *formula;
	int result;
	switch(*formula) {
	case x: return var;
	case n: return time;
	case add: return eval(formula+1,var,time)+eval(formula+getLength(formula+1)+1,var,time);
	case subtract: return eval(formula+1,var,time)-eval(formula+getLength(formula+1)+1,var,time);
	case multiply: return eval(formula+1,var,time)*eval(formula+getLength(formula+1)+1,var,time);
	case divide:
		result=eval(formula+getLength(formula+1)+1,var,time);
		return result==0?(eval(formula+1,var,time)<0?CHAR_MIN:CHAR_MAX):eval(formula+1,var,time)/result;
	default:
		printf("Error, %hhu invalid",*formula);
		return 0;
	}
}

void print(const char *formula) {
	if(*formula>0) printf("%hhu",*formula);
	else switch(*formula) {
	case x:
		printf("x");
		break;
	case n:
		printf("n");
		break;
	case add:
		printf("(");
		print(formula+1);
		printf("+");
		print(formula+getLength(formula+1)+1);
		printf(")");
		break;
	case subtract:
		printf("(");
		print(formula+1);
		printf("-");
		print(formula+getLength(formula+1)+1);
		printf(")");
		break;
	case multiply:
		printf("(");
		print(formula+1);
		printf("*");
		print(formula+getLength(formula+1)+1);
		printf(")");
		break;
	case divide:
		printf("(");
		print(formula+1);
		printf("/");
		print(formula+getLength(formula+1)+1);
		printf(")");
		break;
	default:
		printf("Error, %hhu invalid",*formula);
		break;
	}
}

_Bool scanBoolean(const _Bool defaultVal, const char* question) {
	while(1) {
		printf("%s (%s)\n: ",question,defaultVal?"Y/n":"y/N");
		char c;
		if(scanf("%c",&c)!=1) return defaultVal;
		switch(c) {
		case 'y':
		case 'Y':
			scanf("%c",&char_sink); //catch newline
			return 1;
		case 'n':
		case 'N':
			scanf("%c",&char_sink); //catch newline
			return 0;
		case '\n':
			return defaultVal;
		}
	}
	return defaultVal;
}

int main(void) {
	debug=scanBoolean(0,"Debug output?");
	showFormula=scanBoolean(1,"Show formula after solve/defeat?");
	double highscore[10];
	char names[10][32],*hnames[10];
	for(char i=0;i<10;i++) {
		highscore[i]=0;
		hnames[i]=&(names[i]);
		names[i][0]='\0';
	}
	do{
		double score=0;
		
		for(unsigned char lives=20;lives>0;) {
			char* formula=generateValid(clock());
			int *var=malloc(3*sizeof(int)),current=0;
			if(!var) return 1;
			for(unsigned short i=0;i<3;i++) {
				current=eval(formula,current,i);
				var[i]=current;
			}
			size_t varlen=3;
			_Bool success=0;
			do {
				lives--;
				printf("\033[2J\033[1;1H"); //clear commandline
				printf("Score: %7.3f\nRemaining Lives: %d\nTry: %d\n\nEnter the next number in the sequence\n",score,lives,(unsigned int)(varlen-2));
				for(size_t i=0;i<varlen;i++) {
					printf("%d -> ",var[i]);
				}
				int input;
				if(scanf("%d",&input)==1) {
					scanf("%c",&char_sink); //catch newline
					current=eval(formula,current,varlen);
					if(input==current) {
						success=1;
						break;
					}
					varlen++;
					var=realloc(var,sizeof(int)*varlen);
					if(!var) return 1;
					var[varlen-1]=current;
					printf("Incorrect; Next element: %d\n",current);
				} else {
					printf("\nIllegal Input\n");
				}
			} while(scanBoolean(1,"Retry?"));
			if(success) {
				lives++;
				score+=getLength(formula)/sqrt(varlen-2);
				printf("Correct\nNew Score: %7.3f\n",score);
			}
			if(showFormula) {
				print(formula);
				printf("\n");
				scanf("%c",&char_sink); //wait for input
			}
			free(formula);
			free(var);
		}
		if(highscore[9]<score) {
			highscore[9]=score;
			unsigned char location=9;
			for(;location>0;location--) {
				if(highscore[location]>highscore[location-1]) {
					double swap=highscore[location-1];
					char *nswap;
					nswap=hnames[location-1];
					
					highscore[location-1]=highscore[location];
					hnames[location-1]=hnames[location];
					highscore[location]=swap;
					hnames[location]=nswap;
				} else break;
			}
			printf("New Highscore, enter your name(up to 31 characters): ");
			scanf("%31s",hnames[location]);
			scanf("%c",&char_sink); //catch newline
			if(scanBoolean(1,"Show highscores?")) {
				printf("\033[2J\033[1;1H"); //clear commandline
				for(location=0;location<10;location++) {
					printf("%2d: %7.3f %s\n",location+1,highscore[location],hnames[location]);
				}
			}
		}
	} while(scanBoolean(1,"Continue?"));
	return 0;
}
