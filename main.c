#include <reg52.h>
#include <stdlib.h>
#include <string.h>

sbit SRCLK=P3^6;
sbit RCLK_1=P3^5;	
sbit SER=P3^4;

void delay(unsigned int x);
unsigned char get_ma_keyb(void);

void delay_10us(unsigned int ten_us);
void hc595_write_data(unsigned char dat);

unsigned char col[8]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
void ledprint(unsigned char row[8]);

struct snakebody
{
	//unsigned char x,y;
	unsigned char xy;
	struct snakebody* next;
}body;
 
static unsigned xdata mempool[129];
void main()
{										  
	unsigned char screen[8],i,j,k,dere,foodx,foody,kv,step,estep;
	
	struct snakebody* sp=&body;			
	struct snakebody* mid;	
										
	memset(mempool,0,sizeof(mempool));
	init_mempool(mempool,sizeof(mempool));	
	sp->next=malloc(sizeof(struct snakebody));
	sp->next->next=(struct snakebody*)malloc(sizeof(struct snakebody));			
	(sp->next->next)->xy=0x02;
	(sp->next)->xy=0x12;
	sp->xy=0x22;

	dere=0;
			
	step=0;
	foodx=42;
	
	while(1)
	{							   
		for(i=0;i<8;i++)
		{
			screen[i]=0x0;
		}				   	   
		mid=sp;
		while(mid->next->next!=NULL)	mid=mid->next;
		mid->next->next=sp->next;
		sp->next=mid->next;
		mid->next=NULL;
		mid=sp;
		while(mid->next!=NULL)	mid=mid->next;
		mid->next=NULL;

		sp->next->xy=sp->xy;
		switch(dere)
		{
			case 0:
			{
				if((sp->xy)>>4==7)
				{
					i=(sp->xy<<4);
					sp->xy=i>>4;
					break;
				}
				i=(sp->xy<<4);
				sp->xy=(((sp->xy>>4)+0x01)<<4)+(i>>4);
				break;
			}break;
			case 1:
			{
				i=(sp->xy<<4);
				if(i>>4==7)
				{
					i=sp->xy>>4;
					sp->xy=i<<4;
					break;
				}
				sp->xy=sp->xy+1;
				break;
			}break;
			case 2:
			{
				if(sp->xy>>4==0)
				{
					i=sp->xy<<4;
					sp->xy=0x70+(i>>4);
					break;
				}
				i=sp->xy<<4;
				sp->xy=(((sp->xy>>4)-1)<<4)+(i>>4);
				break;
			}break;
			case 3:
			{
				i=sp->xy<<4;
				if(i>>4==0)
				{	
					i=sp->xy>>4;
					sp->xy=(i<<4)+0x07;
					break;
				}
				sp->xy=sp->xy-1;
				break;
			}break;
		}
  		
		//foodgenerate
		if(foodx==42&&step>6&&step-estep>4)
		{
			srand(sp->xy);

REGENE:
			foodx=rand()%8;
			foody=rand()%8;
			mid=sp;
			while(mid->next!=NULL)
			{
				if(mid->xy>>4==foodx)	goto REGENE;
				i=mid->xy<<4;
				if(i>>4==foody)	goto REGENE;
				mid=mid->next;
			}
		}

		//enlong
		if((sp->xy==(foodx<<4)+foody)&&foodx!=42)
		{
			foodx=42;
			estep=step;
			mid=malloc(sizeof(struct snakebody));
			mid->next=sp->next;
			sp->next=mid;
			mid->xy=sp->xy;
			switch(dere)
			{
				case 0:
				{
					if((sp->xy)>>4==7)
					{
						i=(sp->xy<<4);
						sp->xy=i>>4;
						break;
					}
					i=(sp->xy<<4);
					sp->xy=(((sp->xy>>4)+0x01)<<4)+(i>>4);
					break;
				}break;
				case 1:
				{
					i=(sp->xy<<4);
					if(i>>4==7)
					{
					i=sp->xy>>4;
					sp->xy=i<<4;
					break;
				}
				sp->xy=sp->xy+1;
				break;
				}break;
				case 2:
				{
					if(sp->xy>>4==0)
					{
						i=sp->xy<<4;
						sp->xy=0x70+(i>>4);
						break;
					}
					i=sp->xy<<4;
					sp->xy=(((sp->xy>>4)-1)<<4)+(i>>4);
					break;
				}break;
				case 3:
				{
					i=sp->xy<<4;
					if(i>>4==0)
					{	
						i=sp->xy>>4;
						sp->xy=(i<<4)+0x07;
						break;
				}
				sp->xy=sp->xy-1;
				break;
				}break;
			}
		}



		//set screen
		mid=sp;		 	 	  	 
		while(mid!=NULL)
		{
			i=mid->xy<<4;
			screen[(mid->xy>>4)]|=(1<<(7-(i>>4)));
			mid=mid->next;
		}
		if(foodx!=42)
		{
			i=foody<<4;
			screen[foodx]|=(1<<(7-(i>>4)));
		}

		//crash test
		mid=sp;
		i=0;	//body count
		while(mid!=NULL)
		{
			i+=1;
			mid=mid->next;
		}
		k=0;	//number of print
		for(j=0;j<8;j++)
		{
			k+=screen[j]%2+(screen[j]>>1)%2+(screen[j]>>2)%2+(screen[j]>>3)%2
			+(screen[j]>>4)%2+(screen[j]>>5)%2+(screen[j]>>6)%2+(screen[j]>>7)%2;
		}
		if((foodx==42&&k!=i)||(foodx!=42&&k!=i+1))	while(1);

  		//print							
		for(i=0;i<35;i++) ledprint(screen);

		//input
		kv=get_ma_keyb();
		switch(kv)
		{
			case 8:
			{
				if(dere==2)	break;
				dere=0;break;
			}
			case 11:
			{
				if(dere==3)	break;
				dere=1;break;
			}
			case 6:
			{
				if(dere==0)	break;
				dere=2;break;
			}
			case 3:
			{
				if(dere==1)	break;
				dere=3;break;
			}
			default:break;
		}
	step+=1;

	}
}

void ledprint(unsigned char row[8])
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		P0=col[i];
		hc595_write_data(row[i]);
		delay_10us(100);
		hc595_write_data(0x00);
	}
}

unsigned char get_ma_keyb(void)
{
	unsigned char key_value=0;

	P1=0xF7;
	if(P1!=0xF7)
	{
		//delay(30);
		switch(P1)
		{
			case 0x77:key_value=1;break;
			case 0xB7:key_value=5;break;
			case 0xD7:key_value=9;break;
			case 0xE7:key_value=13;break;
		}
	}
	//while(P1!=0xF7);

	P1=0xFB;
	if(P1!=0xFB)
	{
		//delay(30);
		switch(P1)
		{
			case 0x7B:key_value=2;break;
			case 0xBB:key_value=6;break;
			case 0xDB:key_value=10;break;
			case 0xEB:key_value=14;break;
		}
	}
	//while(P1!=0xFB);

	P1=0xFD;
	if(P1!=0xFD)
	{
		//delay(30);
		switch(P1)
		{
			case 0x7D:key_value=3;break;
			case 0xBD:key_value=7;break;
			case 0xDD:key_value=11;break;
			case 0xED:key_value=15;break;
		}
	}
	//while(P1!=0xFD);

	P1=0xFE;
	if(P1!=0xFE)
	{
		//delay(30);
		switch(P1)
		{
			case 0x7E:key_value=4;break;
			case 0xBE:key_value=8;break;
			case 0xDE:key_value=12;break;
			case 0xEE:key_value=16;break;
		}
	}
	//while(P1!=0xFE);
	return key_value;
}

void delay(unsigned int x)
{
        unsigned int i,j;
        for(i=0;i<x;i++)
        for(j=0;j<120;j++);
}


void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

void hc595_write_data(unsigned char dat)
{
	unsigned char i=0;
	
	for(i=0;i<8;i++)
	{
		SER=dat>>7;
		dat<<=1;
		SRCLK=0;
		delay_10us(1);
		SRCLK=1;
		delay_10us(1);	
	}
	RCLK_1=0;
	delay_10us(1);
	RCLK_1=1;	
}
