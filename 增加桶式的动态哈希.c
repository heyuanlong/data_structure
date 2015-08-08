/*
当重新分配还是分配到一个桶层时候，会出现哈希key不够用，就会出现引用了无效内存的bug，所以使用more_ceng
arr[j]=strtol( bi, NULL, 2);//把二进制字符串bi转换为十进制


测试数据
set 电风 111
set 风 
set 赋给 22
set 4风 33
set 4 44
set 5 55
set a 66
set c 77
set x 88
set ww 放了
set qq 撒解放了
set e 解放了
set yy 旦解放了
set ii 撒


get 电风
get 风 
get 4风
get 4 
get 5 
get a 
get c 
get x 
get ww
get qq
get e 
get yy
get ii


*/
#include<conio.h> 
#include<math.h> 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define tongsize 8    //桶大小是2的n次方
#define VALUESIZE 3  //值链表的长度

//---------------------------------------------------------------------------//
typedef struct value *value_pointer;  //值的结构体
typedef struct value 
{  
	char key[10];
    char zhi[20];  
    value_pointer link;  
}value;  

typedef struct zitong *zitong_pointer;//桶的结构体
typedef struct zitong
{ 
       int biao_tong;  /*0 is 指向值 , 非0 则是指向一个子桶*/ 
       int geshu;//值的个数

       value_pointer value_link;
       zitong_pointer zitong_link;
}zitong; 

zitong tong[tongsize];  
int ceng=1;//层次深度,分配桶的时候，可能ceng加+1


//---------------------------------------------------------------------------//
zitong_pointer init_tong(int biaoceng);//分配和初始化桶，可以改为内存直接付0
value_pointer init_value();//分配值的空间

int binarystring(char c,char*bite,int j,int bite_len);//返回二进制
int* bitetoarr(char* bite,int bishu,int more_ceng) ;//比特转换为数字
int* hash_key(char *key);//返回哈希key，key转为比特，比特再转为数字数组

void ret_fenpei(value_pointer a,zitong_pointer b,int now_ceng);//a值，b是桶，c是层次
value_pointer set_find(zitong_pointer root_arr,int pre_biao_tong, int*hk,int now_ceng);//hk是key的哈希值
void set(zitong_pointer root_arr,int pre_biao_tong);

value_pointer get_find(zitong_pointer root_arr, int*hk,int now_ceng,char* key);//hk是key的哈希值
void getvalue(zitong_pointer root_arr);

//---------------------------------------------------------------------------//

void main()
{
    zitong root;//根小桶
    root.biao_tong=1;//表示指向的子桶
    root.zitong_link = init_tong(root.biao_tong);
      printf("请输入  set如set key value.   get如get key;quit退出\n");

	char caozuo[10];
	int sel;
     do{
         scanf("%s",&caozuo);
         if (strcmp(caozuo,"set")==0)
            sel=1;
         else if (strcmp(caozuo,"get")==0)
            sel=2;
        else if (strcmp(caozuo,"look")==0)
            sel=3;
        else if (strcmp(caozuo,"quit")==0)
            sel=0;
        else sel=-1;


         switch(sel)
        {
          case 0:printf("\t\t\t^-^再见！^-^ \t\t\t\n");system("pause");break;
          case 1:     set(root.zitong_link,root.biao_tong);break;
          case 2:getvalue(root.zitong_link);printf("\n\n");break;
          //case 3:look();printf("\n");break;
          default: printf("请输入正确的选项号！");printf("\n\n");break;
        }
    }while(sel!=0);
}


zitong_pointer init_tong(int biaoceng) //分配和初始化桶，可以改为内存直接付0
{ 
   zitong_pointer t;
   t=(zitong_pointer)malloc(sizeof(zitong)*tongsize);
       int i; 
       for(i=0;i<tongsize;i++) {
              t[i].biao_tong=0;
              t[i].geshu=0;
              t[i].value_link=NULL;
              t[i].zitong_link=NULL; 
            }

  if (biaoceng > ceng)//当这个桶的深度大于当前最大深度时，ceng才+1
 	  ceng++;//

   return t;         
} 
value_pointer init_value()//分配值的空间
{
	value_pointer v;
	v=(value_pointer)malloc(sizeof(value));
	v->link=NULL;
	return v;
}

int binarystring(char c,char*bite,int j,int bite_len)//获取一个字符的二进制
{
  int i;
  for(i=0;i<8;i++)
  { 
  	if(j*8+i >=bite_len)return 1;//二进制足够了

  	if (c & 0x80)
  	 bite[j*8+i]='1';
    else 
    	bite[j*8+i]='0'; 

    c <<= 1;
  }
  return 0;
}

int* hash_key(char *key)//返回哈希key，key转为比特，比特再转为数字数组
{
  //字符串key的长度乘以8就得到比特长度

  int more_ceng=ceng+3;//加多越多越安全
  int len =strlen(key);//字符串长度
  int bishu=(int)(log10(tongsize)/log10(2)) ;//使用bishu个比特来确定一个数字
  int bite_len=bishu*more_ceng;//刚开始是3*1=3

  char *bite=(char*)malloc(bite_len);//申请存放01110011的字符串

  for (int i = 0; i < len; ++i)//把字符转换为比特
  		if(binarystring(key[i],bite,i,bite_len) == 1)break;


  	int bite_len_now=strlen(bite);//0011010字符串的长度
	  while(1)//使得二进制足够长
	  {
	  	int k=0;
	  	if (bite_len_now < bite_len){
	  		bite[bite_len_now+k]=bite[k];
	  		++k;
	  	}
	  	else 
	  		break;
	  }

  int* arr= bitetoarr(bite,bishu,more_ceng);//比特转换为数字
  free(bite);
  return arr;

}
int* bitetoarr(char* bite,int bishu,int more_ceng) //比特转换为数字
{
  int *arr;
  char bi[10]={0};
  arr=(int *)malloc(sizeof(int)*more_ceng);//刚开始是1
  for (int j = 0; j < more_ceng; ++j)
  {
  	bi[0]=bite[j*bishu+0];//bite数组的中10010110
  	bi[1]=bite[j*bishu+1];
  	bi[2]=bite[j*bishu+2];
  	
  	arr[j]=strtol( bi, NULL, 2);
  }
 return arr;
}


void ret_fenpei(value_pointer a,zitong_pointer b,int now_ceng)//a值，b是桶，c是层次
{
   int* fen_hk=hash_key(a->key);
   a->link=b[ fen_hk[now_ceng] ].value_link;//连接后面的
   b[ fen_hk[now_ceng] ].value_link=a;//连接到头部
   b[ fen_hk[now_ceng] ].geshu++;//个数加+1
   free(fen_hk);
}



value_pointer set_find(zitong_pointer root_arr,int pre_biao_tong, int*hk,int now_ceng)//hk是key的哈希值
{
	//key是否存在
  //在哪个子桶
  //得先判断key是否存在了 分1对应的子桶无值  2.扫描链表
  //不存在   分1添加到链表的头部  2.值满了，申请新的桶再分配
  if (now_ceng>=ceng)
     printf("error now_ceng is over");
  zitong_pointer here=&root_arr[ hk[now_ceng] ];//达到桶的层次节点

  int biaoji;//桶的标记，子桶或是值，非0表示桶的层次
  biaoji=here->biao_tong;
  value_pointer vp;

    if (0==biaoji)//子桶指向链表值
    {
	   if(VALUESIZE > here->geshu)//子桶无值或者子桶有值,并且小于链的长度
	      {
	      	value_pointer vp=init_value();//申请空间
	      	vp->link=here->value_link;//连接后面的
	      	here->value_link=vp;//连接到头部啊

	        here->geshu++;//值的个数加+1
	        return vp;
	      }
	      else//子桶的值达到VALUESIZE
	      {
	      		value_pointer temp2;
	      		value_pointer temp=here->value_link;//把链表拿出来
	      		here->biao_tong=pre_biao_tong+1;//子桶标记

	      		here->zitong_link=init_tong(here->biao_tong);//========分配新的子桶	      		
	      		while(temp != NULL)//循环递归重新在新子桶上分配
	      		{
	      			temp2=temp->link;
	      			ret_fenpei(temp,here->zitong_link,now_ceng+1);//递归重新分配
	      			temp=temp2;
	      		}
	      		vp=set_find(here->zitong_link,here->biao_tong,hk,now_ceng+1);//就在新的子桶中寻找了，由hk[now_ceng+1]子哈希确定新子桶的桶层次
	      		return vp;
	      }
    }
  else//说明子桶指向子桶，//进入子桶中寻找了，由hk[now_ceng+1]子哈希确定子桶的桶层次
    {
		vp=set_find(here->zitong_link,here->biao_tong,hk,now_ceng+1);
		return vp;
    }

}


value_pointer get_find(zitong_pointer root_arr, int*hk,int now_ceng,char* key)//hk是key的哈希值
{
  //在哪个子桶层
  //得先判断key是否存在了 分1对应的子桶无值  2.扫描链表
  //不存在   分1添加到链表的头部  2.值满了，申请新的桶再分配
  if (now_ceng>=ceng)
     printf("error now_ceng is over");
  zitong_pointer here=&root_arr[ hk[now_ceng] ];//达到桶的层次节点

  int biaoji;//桶的标记，子桶或是值，非0表示桶的层次
  biaoji=here->biao_tong;
  value_pointer vp;
  
  vp=here->value_link;
    if (0==biaoji)//子桶指向链表值
    {
	   while(vp != NULL)
	   {
	   	 if (0 == strcmp(vp->key,key) )
	   		return vp;
	     vp=vp->link;
	   }
	   return NULL;
    }
  else//说明子桶指向子桶，
    {
		vp=get_find(here->zitong_link,hk,now_ceng+1,key);//进入子桶中寻找了，由hk[now_ceng+1]子哈希确定子桶的桶层次
		return vp;
    }

}

void set(zitong_pointer root_arr,int pre_biao_tong)
{ 
  char key[10],zhi[20];
  int* hk;
  value_pointer p;

  scanf("%s %s",&key,&zhi);
   hk=hash_key(key);

   p=get_find(root_arr,hk,0,key);//是否已经存在相同的key了

	if (p ==NULL)//说明不存在节点
	  p=set_find(root_arr,pre_biao_tong,hk,0);//发现点

  strcpy(p->key,key);  
  strcpy(p->zhi,zhi);
  free(hk);
}


void getvalue(zitong_pointer root_arr)
{
  char key[10];
    int* hk;
  value_pointer p;
  scanf("%s",&key);
  
   hk=hash_key(key);
   p=get_find(root_arr,hk,0,key);//发现点
   
	if (p==NULL)
		 printf("no key to value");
	else
		printf("%s",p->zhi);
free(hk);
}

