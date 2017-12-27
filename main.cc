
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <bitset>

//using namespace std;
/*
#include <cstudio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <bitsream>
*/

unsigned int bintohex(char *digits){
  unsigned int res=0;
  while(*digits)
    res = (res<<1)|(*digits++ -'0');
  return res;
}


bool char_equal(char* a, char* b, int N){
  int count=0;
  for(int i=0;i<N;++i)
  {
    if(a[i]==b[i])count++;
  }
  return count==N;
}

int get_setindex(char* addrBin, int tag_size, int index_size){
  int index_value=0;
 // printf("%s\n",addrBin);
  for(int i=0;i<index_size;++i){
    if(addrBin[tag_size+i]=='1')
    {
    index_value+= pow(2,index_size-i-1);
    }
  }
 // printf("%d\n", index_value);
  return index_value;
}



struct cache_status
{
  int access_count;  
  int read_count; 
  int write_count; 
  int replacement_count;
  int miss_count;
  int hit_count;
  int read_miss_count;
  int write_miss_count; 
  int read_hit_count;
  int write_hit_count;
  int write_back_count; 
  double hit_rate;
  double miss_rate;
  int total_memory_traffic;
} status ={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void printstatus(int write_policy)
{
  
  status.hit_count=status.read_hit_count + status.write_hit_count;
  status.hit_rate=(double) status.hit_count/status.access_count;
  status.miss_rate=(double) status.miss_count/status.access_count;
  if(!write_policy)
  status.total_memory_traffic=status.read_miss_count+status.write_miss_count+status.write_back_count;
  else
  status.total_memory_traffic=status.read_miss_count+status.write_count+status.write_back_count;
 
  printf("======  Simulation results (raw) ======\n");
  printf("a. number of L1 reads:%d\n",status.read_count);
  printf("b. number of L1 read misses:%d\n",status.read_miss_count);
  printf("c. number of L1 writes:%d\n", status.write_count);
  printf("d. number of L1 write misses:%d\n", status.write_miss_count);
  printf("e. L1 miss rate: %.4f \n", status.miss_rate);
  printf("f. number of writebacks from L1:%d\n",status.write_back_count);
  printf("g. total memory traffic: %d\n",status.total_memory_traffic);


}

class block 
{
public:
      char tag[32];
      unsigned long timestamp;
      bool valid;
      bool dirty;
      int block_recency_count;
      int block_freq_count;
      int block_aging_count;

block(int tag_size)
{
    
 for(int i=0;i<tag_size;++i)
   tag[i]='0';//initialize tag to random chars

 valid=false;
 dirty=false;
 block_recency_count=0;
 block_freq_count=0;
 block_aging_count=0;
}

};


class set {
public:
      block* blocks[256];
      int set_associativity;

set(int set_associativity,int tag_size)
{
  for(int i=0;i<set_associativity;++i)
  {
 //   printf("going to create block no. %d\n",i);
    blocks[i]=new block(tag_size); 
   // printf("block tag is %s\n", blocks[i]->tag);
   // printf("The block valid is %d\n",blocks[i]->valid);
  }
}  

void set_valid(int location)
{
  blocks[location]->valid=true;
}

void set_age_count(int location)
{
  blocks[location]->block_aging_count=blocks[location]->block_freq_count;
}
/*
char get_tag(int location)
{
return blocks[location]->tag;
}
*/
void set_dirty(int location)
{
  blocks[location]->dirty = true;
}

void clear_dirty(int location)
{
blocks[location]->dirty = false;
}

int ishit(char* addrBin, int tag_size)
{
  for(int j=0;j<set_associativity;++j)
  {
    if(char_equal(blocks[j]->tag, addrBin, tag_size) && blocks[j]->valid)
    {
//	  printf("hit on set %d line %d\n",setIndex, j);
	  return j;
    }
  }
    //Give signal to processor or lower level cache that it is a miss
    return -1;
}

int emptyLineAvailable()
{
//printf("Entering %d\n",set_associativity);
  for(int j=0;j<set_associativity;++j)
  {
//  printf("The block valid is %d\n",blocks[j]->valid);
     if (blocks[j]->valid==false)
{ 
    // printf("Yes");
     return j;
} 
 }
    return -1;
}


bool check_dirty(int location)
{
    if (blocks[location]->dirty==true) 
      return true;
    else
      return false;
}


void update_recency_count(int location)
{
  if(blocks[location]->valid)
  {
    for(int i=0;i<set_associativity;++i)
    {
      if(blocks[i]->valid && (blocks[i]->block_recency_count < blocks[location]->block_recency_count))
      {
	blocks[i]->block_recency_count++;
//	blocks[i]->block_recency_count = blocks[i]->block_recency_count% set_associativity;
      }
    }
  }
  else
  {
    for(int i=0;i<set_associativity;++i)
    {
     if(blocks[i]->valid)
      {
        blocks[i]->block_recency_count++;
      }
    }
  }
blocks[location]->block_recency_count=0;//clear the age of hitted line
}  


void update_freq_count(int location)
{
blocks[location]->block_freq_count++;
}

void set_tag(char* addrBin, int location, int tag_size)
{ 
    
    for(int i=0;i<tag_size;++i) 
      blocks[location]->tag[i]=addrBin[i];
    //printf("tag is %s \n", block[location]->tag);
//    status.stream_in_count++;
}


int get_lru_eviction_spot()
{ 
  int location=0;
  for(int i=0;i<set_associativity;++i)
  { 
    if((blocks[i]->block_recency_count >= blocks[location]->block_recency_count))
    {
      location=i;
    }
  }  
return location;
}

int get_lfu_eviction_spot()
{ 
  int location=0;

  for(int i=set_associativity-1;i>=0;--i)
  { 
    if((blocks[i]->block_freq_count <= blocks[location]->block_freq_count))
    {
      location=i;
    }
  }  
return location;
}

};
	
class cache {
public:
      int cache_size;
      int block_size;
      int set_associativity;
      int tag_size;
      int index_size;
      int num_sets;
      set* sets[65536];
      char replacement_policy;
      char write_policy;

void init_cache()
{
  num_sets=cache_size/(block_size*set_associativity);
  tag_size=(32-(int)log2(num_sets)-(int)log2(block_size)); 
  index_size=(int)log2(num_sets);  
//  printf("num_sets %d\n", num_sets);
//  printf("tag_size %d\n", tag_size);
//  printf("index_size %d\n", index_size);
  for(int i=0;i<num_sets;++i) 
{
  // printf("going to create set no. %d\n",i);
    sets[i]= new set(set_associativity,tag_size);
    sets[i]->set_associativity=set_associativity;
}
}
  


int read(char* addrBin)
{
    //printf("#1: Read %p\n",addrBin);
    int setindex = get_setindex(addrBin,tag_size,index_size);
    //printf("Current set    %d:\n", setindex);
    //printf("reading set %d...\n ", setindex);
    status.read_count++;
    status.access_count++;
    int hitornot=0;
    int hit_location=sets[setindex]->ishit(addrBin,tag_size);
    if(hit_location!=-1)
    {
      hitornot=1;
      status.read_hit_count++;
      //printf("L1 HIT\n");
      if(!replacement_policy)
      {
        // printf("L1 UPDATE LRU\n");
        //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
	sets[setindex]->update_recency_count(hit_location);
      }
      else
       {
     //printf("L1 UPDATE LFU\n");
       //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
	sets[setindex]->update_freq_count(hit_location);
        //printf("%d",sets[setindex]->blocks[hit_locaiton]->freq_count);
       }
    } //if end
    else 
    { 
      status.read_miss_count++;
      status.miss_count++;
      //printf("L1 MISS\n");
      int empty_location=sets[setindex]->emptyLineAvailable();
     // printf("I am read\n");
     // printf("The empty location is %d\n",empty_location);
      if(empty_location!=-1)
      {
	//printf("R missed, space available, stream in \n");
	if(!replacement_policy)
	{
         //printf("L1 UPDATE LRU\n");
//         printf("Changed set     %d:    D\n",setindex);
	  sets[setindex]->set_tag(addrBin,empty_location,tag_size);
          //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
          sets[setindex]->update_recency_count(empty_location); 
	  sets[setindex]->set_valid(empty_location);
        }
	else
	{
        //printf("L1 UPDATE LFU\n");
        //printf("Changed set     %d:    D\n",setindex);
	  sets[setindex]->set_tag(addrBin,empty_location,tag_size);
          //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
          sets[setindex]->update_freq_count(empty_location);
          sets[setindex]->set_valid(empty_location);
        }
      } //pending top else
      else 
      {
	//printf("R missed and full, do eviction\n");
	if(!replacement_policy)
	{
          //printf("L1 UPDATE LRU\n");
          int location= sets[setindex]->get_lru_eviction_spot();
	  if (sets[setindex]->check_dirty(location))//this is done only for wbwa
	  {
	    status.write_back_count++;
            sets[setindex]->clear_dirty(location);
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	  }
          //for wtna just remove the block and proceed with replacement of tag no updation is required
	  //update the cache data contents
	 // printf("Changed set     %d:     D\n",setindex);
	  sets[setindex]->set_tag(addrBin,location,tag_size);
          //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	  sets[setindex]->update_recency_count(location);      
        }
	else
  	{
          //printf("L1 UPDATE LFU\n");
          int location= sets[setindex]->get_lfu_eviction_spot();
	  if (sets[setindex]->check_dirty(location))
	  {
	    status.write_back_count++;
            sets[setindex]->clear_dirty(location);
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	  }
//	printf("Changed set     %d:    D\n",setindex);
        sets[setindex]->set_tag(addrBin,location,tag_size);
        //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	sets[setindex]->set_age_count(location);
	sets[setindex]->update_freq_count(location);	
        }
      }
   }
return hitornot;
}

void write(char* addrBin)
{
    //printf("#1: Write %p\n",addrBin);
    int setindex = get_setindex(addrBin,tag_size,index_size);
//   printf("%X\n",bintohex(addrBin));
    //printf("Current set    %d:\n", setindex);
    status.write_count++;
    status.access_count++;
   //printf("I am write\n");
    int hit_location=sets[setindex]->ishit(addrBin,tag_size);
    //printf("L1 Write:%s(tag %p, index %d\n)",addrBin,sets[setindex]->blocks[hit_location]->tag,setindex);
    //printf("Current set    %d:\n", setindex);
    if(hit_location!=-1)
    {
      status.write_hit_count++;
      //printf("L1 HIT\n");
      if(!replacement_policy)
      {
        //printf("L1 UPDATE LRU\n");
	sets[setindex]->update_recency_count(hit_location);
        if(!write_policy)
        {//update the cache data contents for wbwa
	  //printf("L1 SET DIRTY\n");
          //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
       //  sets[setindex]->update_recency_count(hit_location);
	  sets[setindex]->set_dirty(hit_location);
        }
        else
        {
         ;//update the cache data contents
          //call the function which provide addrbin to higher level memory for updation for wtna
	}
      }
      else
      {
      //  printf("L1 UPDATE LFU\n");
	sets[setindex]->update_freq_count(hit_location);
        if(!write_policy)
        {//update the cache data contents
	//  printf("L1 SET DIRTY\n");
         // printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[hit_location]->tag));
         // sets[setindex]->update_freq_count(hit_location);
	  sets[setindex]->set_dirty(hit_location);
        }
        else
        {
         ; //update the cache data contents
           //call the function which provide addrbin to higher level memory for updation for wtna
	}
      }
    } 
    else 
    { 
      status.write_miss_count++;
      status.miss_count++;
     // printf("L1 MISS\n");
      int empty_location=sets[setindex]->emptyLineAvailable();
      //printf("Changed set     %d:    D\n",setindex);
//      printf("location is %d", empty_location);
      if(!write_policy)//for wbwa
      {
        if(empty_location!=-1)
        {
//	  printf("W missed, space available, stream in \n");
	  if(!replacement_policy)
	  {
            //printf("L1 UPDATE LRU\n");
	    sets[setindex]->set_tag(addrBin,empty_location,tag_size);
	    //update the cache data contents
            sets[setindex]->update_recency_count(empty_location); 
	    sets[setindex]->set_valid(empty_location);
	    //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
	    sets[setindex]->set_dirty(empty_location);
          }
	  else
	  {
        //    printf("L1 UPDATE LFU\n");
	    sets[setindex]->set_tag(addrBin,empty_location,tag_size);
	    //update the cache contents
            sets[setindex]->update_freq_count(empty_location);
            sets[setindex]->set_valid(empty_location);
	  //  printf("L1 SET DIRTY\n");
           // printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[empty_location]->tag));
            sets[setindex]->set_dirty(empty_location);
          }
        } 
        else 
        {
//	  printf("W missed and full, do eviction\n");
	  if(!replacement_policy)
	  {
	    //printf("L1 UPDATE LRU\n");
            int location= sets[setindex]->get_lru_eviction_spot();
	    if (sets[setindex]->check_dirty(location))
	    {
	      status.write_back_count++;
	    //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	    }
	    sets[setindex]->set_tag(addrBin,location,tag_size);
            //update the cache data contents
            //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	    sets[setindex]->update_recency_count(location);
            sets[setindex]->set_dirty(location);
          }
	  else
  	  {
            //printf("L1 UPDATE LFU\n");
            int location= sets[setindex]->get_lfu_eviction_spot();
	    if (sets[setindex]->check_dirty(location))
	    {
	      status.write_back_count++;
	      //call the function which obtains the tag and provide higher level memory tag, set index and location to write
	    }
	    sets[setindex]->set_tag(addrBin,location,tag_size);
            //update the cache data contents
            //printf("L1 SET DIRTY\n");
            //printf("Changed set     %d:    %X   D\n",setindex,bintohex(sets[setindex]->blocks[location]->tag));
	    sets[setindex]->set_age_count(location);
	    sets[setindex]->update_freq_count(location);
            sets[setindex]->set_dirty(location);	
          }
        }
      }
      else
      {
      ;//call the function which provide addrbin to higher level memory for updation
      }
   }
}      
    
void set_cache_param(int param, int value)
{
  switch (param) 
   {
     case 1:
      block_size = value;
//      printf("did");
     break;
     case 2:
      cache_size = value;
//      printf("did");
     break;
     case 3:
      set_associativity = value;
//      printf("did");
     break;
     case 4:
      replacement_policy = value;
//      printf(value);
     break;
     case 5:
       write_policy = value;
     break;
     default:
       printf("error set_cache_param: bad parameter value\n");
       exit(-1);
  }
}

};

int is_power_of_2(int i) {
    if ( i <= 0 ) 
    {
        return 0;
    }
    return ! (i & (i-1));
}

bool parameter_sanity_check( char* argv[])
{
int arg=0;
int value=0;
for(arg=1; arg<7; arg++)
{
     if(arg==1) 
     {
        printf("  L1_BLOCKSIZE:    %s\n",argv[arg]);
	value = atoi(argv[arg]);
        if(is_power_of_2(value)==0)
        {
	  printf("The cache block size is not power of two\n");
	  return false;
        }
        continue;
     }
     if(arg==2)
     {
	printf("  L1_SIZE:     %s\n",argv[arg]);
	value = atoi(argv[arg]);
        if(is_power_of_2(value)==0)
        {
	  printf("The cache size is not power of two\n");
          return false;
        } 
        continue;
     }
  
    if(arg==3) 
    {
	printf("  L1_ASSOC:     %s\n",argv[arg]);
	value = atoi(argv[arg]);
        continue;
    }

    if(arg==4) 
    {
	if(*argv[arg]==0)
        {
	  printf("  L1_REPLACEMENT_POLICY:   %s\n",argv[arg]);
        }
	else
        {
	  value = atoi(argv[arg]);
	  printf("  L1_REPLACEMENT_POLICY:   %s\n",argv[arg]);
        }
        continue;
     }

    if(arg==5) 
    {
	if(*argv[arg]==0)
        {
	  printf("  L1_WRITE_POLICY:    %s\n",argv[arg]);
        }
	else
        {
	  printf("  L1_WRITE_POLICY:    %s\n",argv[arg]);
        }
      continue;
    }

    if(arg==6) 
    {
      printf("    trace_file:    %s\n",argv[arg]);
      continue;
    }  
}
return true;
}

char* hexToBin(char* in){ 
    int x = 4;
    int size;
    size = strlen(in);
   //printf("size is %d\n",size);
   char input[]="00000000";
    int i;
    for (i = 0; i < size + 1; i++) 
    {
        input[8-size+i] = in[i];
    }

    //printf("buffer : %s\n", input);

    char* output = new char[8*4+1];
   
    for (int i = 0; i < 8; i++)
    {
        if (input[i] =='0') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='1') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='2') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='3') {
            output[i*x +0] = '0';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='4') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='5') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='6') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }    
        else if (input[i] =='7') {
            output[i*x +0] = '0';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }    
        else if (input[i] =='8') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='9') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }
        else if (input[i] =='a') {    
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }
        else if (input[i] =='b') {
            output[i*x +0] = '1';
            output[i*x +1] = '0';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }
        else if (input[i] =='c') {
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '0';
        }
        else if (input[i] =='d') {    
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '0';
            output[i*x +3] = '1';
        }
        else if (input[i] =='e'){    
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '0';
        }
        else if (input[i] =='f') {
            output[i*x +0] = '1';
            output[i*x +1] = '1';
            output[i*x +2] = '1';
            output[i*x +3] = '1';
        }
    }

    output[32] = '\0';
    //printf("strlen of output is %d\n",strlen(output));
    return output;
}

class bimodal
{
public:
unsigned int prediction_table[20000];

bimodal(int size)
{
for(int i=0;i<size;i++)
prediction_table[i]=2;
}

void update_table(int index, int a)
{
if(a==0)
{
if(prediction_table[index]!=0)
prediction_table[index]--;
}
else
{
if(prediction_table[index]!=3)
prediction_table[index]++;
}
//printf(" new value %d\n",prediction_table[index]);
}

int get_prediction(int index)
{
return prediction_table[index];
}

void print_table(int size)
{
for(int i=0;i<size;i++)
printf("%d	%d\n",i,prediction_table[i]);
}
};

class gshare
{
public:
char bhr[32];
int size;
int bhr_size;
unsigned int prediction_table[20000];

gshare(int t_size,int b_size)
{
size=t_size;
bhr_size=b_size;
for(int j=0; j<bhr_size;j++)
{
bhr[j]='0';
}
//exit(-1);
bhr[bhr_size+1]='\0';
for(int i=0;i<size;i++)
prediction_table[i]=2;
}

int make_index(char* addrBin, int value)
{
int tag_size=32-(value+2);
int index_value=0;
//printf("%d %d %d\n",value,bhr_size,tag_size);
for(int i=0;i<bhr_size;i++){
//printf("%c %c\n",addrBin[tag_size+i],bhr[i]);
if((addrBin[tag_size+i]=='1' && bhr[i]=='0')||(addrBin[tag_size+i]=='0' && bhr[i]=='1')){
index_value+=pow(2,value-i-1);
//printf("%d\n",index_value);
}
}
//printf("%d\n",index_value);
for(int i=bhr_size;i<value;++i){
    if(addrBin[tag_size+i]=='1')
    {
    index_value+= pow(2,value-i-1);
    }
}
return index_value;
}

void update_bhr(int a)
{
for(int i=bhr_size-1;i>0;i--)
bhr[i]=bhr[i-1];
if(a==0)
bhr[0]='0';
else
bhr[0]='1';
/*
for(int j=0; j<bhr_size;j++)
{
printf("%c",bhr[j]);
}
printf("\n");
*/
}

void update_table(int index, int a)
{
if(a==0)
{
if(prediction_table[index]!=0)
{
//printf("index %d old value %d",index,prediction_table[index]);
prediction_table[index]--;
//printf("new value 1 %d\n", prediction_table[index]);
}
}
else
{
if(prediction_table[index]!=3)
prediction_table[index]++;
}
//printf(" new value %d\n",prediction_table[index]);
}

int get_prediction(int index)
{
return prediction_table[index];
}

void print_table(int size)
{
for(int i=0;i<size;i++)
printf("%d	%d\n",i,prediction_table[i]);
}
};

class hybrid
{
public:
unsigned int predictor_chooser[20000];
bimodal* x;
gshare* y;
int bimodal_size;
int gshare_size;
int bhr_size;
int x_predict;
int y_predict;
int bi_index;
int gs_index;

hybrid(int size,int b_size, int g_size, int bh_size)
{
gshare_size=g_size;
bimodal_size=b_size;
bhr_size=bh_size;
x = new bimodal(pow(2,bimodal_size));
y = new gshare(pow(2,gshare_size), bhr_size);
//printf("%d %d %d\n",bimodal_size, gshare_size, bhr_size);
for(int i=0;i<size;i++)
{
predictor_chooser[i]=1;
//printf("%d %d %d\n",bimodal_size, gshare_size, bhr_size);
//printf("%d",predictor_chooser[i]);
}
//printf("\n");
//printf("%d %d %d",bimodal_size, gshare_size, bhr_size);
//x = new bimodal(pow(2,bimodal_size));
//y = new gshare(pow(2,gshare_size), bhr_size);
}

int get_prediction(char* addrBin,int index)
{
int x_prediction,y_prediction;
int b_index,g_index;
int tag_size;
tag_size=32-(bimodal_size+2);
b_index = get_setindex(addrBin,tag_size,bimodal_size);
bi_index=b_index;
x_prediction=x->get_prediction(b_index);
if(x_prediction>=2)
x_predict=1;
else
x_predict=0;
g_index = y->make_index(addrBin,gshare_size);
gs_index=g_index;
y_prediction=y->get_prediction(g_index);
if(y_prediction>=2)
y_predict=1;
else
y_predict=0;
if(predictor_chooser[index]<=1)
{
//printf("BIMODAL index: %d",b_index);
//printf(" old value: %d",x_prediction);
return x_prediction;
}
else
{
//printf("GSHARE index: %d",g_index);
//printf(" old value: %d",y_prediction);
return y_prediction;
}
}

void update_tables(char* addrBin, int index, int a)
{
y->update_bhr(a);
if(predictor_chooser[index]<=1)
{
//p_index = get_setindex(addrBin,tag_size,bimodal_size);
//printf("pindex\n %d",p_index);
x->update_table(bi_index,a);
}
else
{
//p_index = y->make_index(addrBin,gshare_size);
//printf("pindex\n %d",p_index);
y->update_table(gs_index,a);
}
}

void update_chooser(int index, int a)
{
//printf("predictor chooser%d\n",predictor_chooser[index]);
if((a==0 && x_predict==0 && y_predict==1)||(a==1 && x_predict==1 && y_predict==0))
{
if(predictor_chooser[index]!=0)
predictor_chooser[index]--;
}
else if((a==0 && x_predict==1 && y_predict==0)||(a==1 && x_predict==0 && y_predict==1))
{
if(predictor_chooser[index]!=3)
predictor_chooser[index]++;
}
//printf("Updated chooser value%d\n",predictor_chooser[index]);
}

void print_table(int size)
{
for(int i=0;i<size;i++)
printf("%d      %d\n",i,predictor_chooser[i]);
}
};

int main(int argc, char *argv[]) 
{
if(strcmp(argv[1],"bimodal")==0)
{
int value=atoi(argv[3]);

if(value==0)
{
//printf("This is bimodal");
 value = atoi(argv[2]);
bimodal* mybimodal=new bimodal(pow(2,value));
//mybimodal->print_table(pow(2,value));
char* fileString;
int tag_size;
int index;
int taken_or_not;
int count=0;
int mis_count=0;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
float h=0;
if(inFile==NULL)
{
    printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
//        printf("%d. PC: %s %s\n",(count+1),temp2,temp);
	if(feof(stdin)) break;
        tag_size=32-(value+2);
        index = get_setindex(hexToBin(&temp2[0]),tag_size,value);         
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
//        mybimodal->update_table(index,taken_or_not);
//	printf("BIMODAL index: %d old value: %d",index,mybimodal->get_prediction(index));
        if(((mybimodal->get_prediction(index)<2 && taken_or_not==1) || (mybimodal->get_prediction(index)>=2 && taken_or_not==0)))
        mis_count++;       
	mybimodal->update_table(index,taken_or_not); 
//        printf(" new value %d\n",mybimodal->get_prediction(index));
        count++;
    }
printf("COMMAND\n");
printf("./sim bimodal %d 0 0 %s\n",value,argv[argc-1]);
printf("OUTPUT\n");
printf("number of predictions:	%d\n",count);
printf("number of mispredictions:	%d\n",mis_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:	%.2f%%\n",h);
printf("FINAL BIMODAL CONTENTS\n");
mybimodal->print_table(pow(2,value));
}
}

else
{
int value = atoi(argv[2]);
int value1,value2;
bimodal* mybimodal=new bimodal(pow(2,value));
//mybimodal->print_table(pow(2,value));
char* fileString;
int tag_size;
int index;
int taken_or_not;
int count=0;
int countb=0;
int misb_count=0;
int misnb_count=0;
int mis_count=0;
int biprediction;
int prediction;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
float h=0;
 cache* mycache=new cache();
    mycache->set_cache_param(1, 4);
    value1 = atoi(argv[3]);
    mycache->set_cache_param(2, value1);
    value2 = atoi(argv[4]);
    mycache->set_cache_param(3, value2);
    mycache->set_cache_param(4, 0);
    mycache->set_cache_param(5, 0);
    mycache->init_cache();
    int hitornot;
if(inFile==NULL)
{
    printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
//        printf("%d. PC: %s %s\n",(count+1),temp2,temp);
	if(feof(stdin)) break;
       hitornot = mycache->read(hexToBin(&temp2[0]));  
        tag_size=32-(value+2);
        index = get_setindex(hexToBin(&temp2[0]),tag_size,value);         
        biprediction = mybimodal->get_prediction(index);
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
        if(hitornot==1)
        {
          countb++;
          if(biprediction<2)
           prediction=0;
          else
           prediction=1;
          if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
          misb_count++;
          mybimodal->update_table(index,taken_or_not);
        }
        else
        {
        prediction=0;
        if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
        misnb_count++;
}
if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
 mis_count++;
  count++;
}
printf("COMMAND\n");
printf("./sim bimodal %d %d %d %s\n",value, value1, value2 ,argv[argc-1]);
printf("OUTPUT\n");
printf("size of BTB:	 %d\n",value1);
printf("number of branches:	 %d\n",count);
printf("number of predictions from branch predictor:	%d\n",countb);
printf("number of mispredictions from branch predictor:	%d\n",misb_count);
printf("number of branches miss in BTB and taken:	 %d\n",misnb_count);
printf("total mispredictions:	%d\n",misb_count+misnb_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:	%.2f%%\n\n",h);
printf("FINAL BTB CONTENTS\n"); 
for(int i=0;i<mycache->num_sets;i++)
{
printf("set %d:",i);
for(int j=0;j<mycache->set_associativity;j++)
{
if(mycache->sets[i]->blocks[j]->dirty)
printf("  %x   D   ",bintohex(mycache->sets[i]->blocks[j]->tag));
else
printf("  %x       ",bintohex(mycache->sets[i]->blocks[j]->tag));
}
printf("\n");
}
printf("\n");
printf("FINAL BIMODAL CONTENTS\n");
mybimodal->print_table(pow(2,value));
}
}
}
      


else if(strcmp(argv[1],"gshare")==0)
{
int value=atoi(argv[4]);

if(value==0)
{
int value = atoi(argv[2]);
int value1 = atoi(argv[3]);
gshare* mygshare=new gshare(pow(2,value),value1);
//mygshare->bhr=new char[value1];
char* fileString;
int index;
int taken_or_not;
int count=0;
int mis_count=0;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
float h=0;
if(inFile==NULL)
{
    printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
	if(feof(stdin)) break;
        index = mygshare->make_index(hexToBin(&temp2[0]),value);    
//printf("%d. PC: %s %s\n",(count+1),temp2,temp);
//        printf("%d",index); 
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
//        mybimodal->update_table(index,taken_or_not);
//	printf("BIMODAL index: %d old value: %d",index,mybimodal->get_prediction(index));
        if(((mygshare->get_prediction(index)<2 && taken_or_not==1) || (mygshare->get_prediction(index)>=2 && taken_or_not==0)))
        mis_count++;       
 //       exit(-1);
	mygshare->update_table(index,taken_or_not);
        mygshare->update_bhr(taken_or_not); 
//        printf(" new value %d\n",mybimodal->get_prediction(index));
        count++;
    }
printf("COMMAND\n");
printf("./sim gshare %d %d 0 0 %s\n",value,value1,argv[argc-1]);
printf("OUTPUT\n");
printf("number of predictions:	%d\n",count);
printf("number of mispredictions:	%d\n",mis_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:	%.2f%%\n",h);
printf("FINAL GSHARE CONTENTS\n");
mygshare->print_table(pow(2,value));
}
}

else
{
int value = atoi(argv[2]);
int value1,value2,value3;
value1 = atoi(argv[3]);
gshare* mygshare=new gshare(pow(2,value),value1);
//mybimodal->print_table(pow(2,value));
char* fileString;
int index;
int taken_or_not;
int count=0;
int countb=0;
int misb_count=0;
int misnb_count=0;
int mis_count=0;
int biprediction;
int prediction;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
float h=0;
 cache* mycache=new cache();
    mycache->set_cache_param(1, 4);
    value2 = atoi(argv[4]);
    mycache->set_cache_param(2, value2);
    value3 = atoi(argv[5]);
    mycache->set_cache_param(3, value3);
    mycache->set_cache_param(4, 0);
    mycache->set_cache_param(5, 0);
    mycache->init_cache();
    int hitornot;
if(inFile==NULL)
{
   printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
//        printf("%d. PC: %s %s\n",(count+1),temp2,temp);
	if(feof(stdin)) break;
       hitornot = mycache->read(hexToBin(&temp2[0])); 
index = mygshare->make_index(hexToBin(&temp2[0]),value);          
        biprediction = mygshare->get_prediction(index);
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
        if(hitornot==1)
        {
          countb++;
          if(biprediction<2)
           prediction=0;
          else
           prediction=1;
          if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
          misb_count++;
          mygshare->update_table(index,taken_or_not);
mygshare->update_bhr(taken_or_not);
        }
        else
        {
        prediction=0;
        if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
        misnb_count++;
}
if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
 mis_count++;
  count++;
}
printf("COMMAND\n");
printf("./sim gshare %d %d %d %d %s\n",value, value1, value2, value3, argv[argc-1]);
printf("OUTPUT\n");
printf("size of BTB:	 %d\n",value2);
printf("number of branches:	 %d\n",count);
printf("number of predictions from branch predictor:	%d\n",countb);
printf("number of mispredictions from branch predictor:	%d\n",misb_count);
printf("number of branches miss in BTB and taken:	 %d\n",misnb_count);
printf("total mispredictions:	%d\n",misb_count+misnb_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:	%.2f%%\n\n",h);
printf("FINAL BTB CONTENTS\n"); 
for(int i=0;i<mycache->num_sets;i++)
{
printf("set %d:",i);
for(int j=0;j<mycache->set_associativity;j++)
{
if(mycache->sets[i]->blocks[j]->dirty)
printf("  %x   D   ",bintohex(mycache->sets[i]->blocks[j]->tag));
else
printf("  %x       ",bintohex(mycache->sets[i]->blocks[j]->tag));
}
printf("\n");
}
printf("\n");
printf("FINAL GSHARE CONTENTS\n");
mygshare->print_table(pow(2,value));
}
}
}



else if(strcmp(argv[1],"hybrid")==0)
{
int value = atoi(argv[2]);
int value1 = atoi(argv[3]);
int value2 = atoi(argv[4]);
int value3 = atoi(argv[5]);
int value4 = atoi(argv[6]);
int value5 = atoi(argv[7]);

if(value4==0)
{
//printf("%d %d %d %d\n",value,value1,value2,value3);
hybrid* myhybrid=new hybrid(pow(2,value),value3,value1,value2);
//myhybrid->print_table(pow(2,value));
//exit(-1);
//mygshare->bhr=new char[value1];
char* fileString;
int index;
int taken_or_not;
int count=0;
int mis_count=0;
int tag_size;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
float h=0;
int prediction;
char* addrBin;
if(inFile==NULL)
{
    printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
	if(feof(stdin)) break;
        tag_size=32-(value+2);
addrBin=hexToBin(&temp2[0]);
        index = get_setindex(addrBin,tag_size,value);
//        printf("%d\n",index);
//        exit(-1);    
//        printf("%d",index);
//printf("%d. PC: %s %s\n",(count+1),temp2,temp);
//printf("Chooser index %d\n", index);
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
//        mybimodal->update_table(index,taken_or_not);
//	printf("BIMODAL index: %d old value: %d",index,mybimodal->get_prediction(index));
prediction=myhybrid->get_prediction(addrBin,index);
        if(((prediction<2 && taken_or_not==1) || (prediction>=2 && taken_or_not==0)))
        mis_count++;       
//        exit(-1);
	myhybrid->update_tables(addrBin,index,taken_or_not);
//        exit(-1);            
        myhybrid->update_chooser(index,taken_or_not);
//        exit(-1);     
//        printf(" new value %d\n",mybimodal->get_prediction(index));
        count++;
    }
//exit(-1);

printf("COMMAND\n");
printf("./sim hybrid %d %d %d %d 0 0 %s\n",value,value1,value2,value3,argv[argc-1]);
printf("OUTPUT\n");
printf("number of predictions:  %d\n",count);
printf("number of mispredictions:       %d\n",mis_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:     %.2f%%\n",h);
printf("FINAL CHOOSER CONTENTS\n");
myhybrid->print_table(pow(2,value));
printf("FINAL GSHARE CONTENTS\n");
myhybrid->y->print_table(pow(2,value1));
printf("FINAL BIMODAL CONTENTS\n");
myhybrid->x->print_table(pow(2,value3));

}
}

else
{
hybrid* myhybrid=new hybrid(pow(2,value),value3,value1,value2);
//mybimodal->print_table(pow(2,value));
char* fileString;
int index;
int taken_or_not;
int count=0;
int countb=0;
int misb_count=0;
int misnb_count=0;
int mis_count=0;
int biprediction;
int prediction;
char* addrBin;
float h=0;
int tag_size;
fileString = argv[argc-1];
FILE* inFile = freopen(fileString,"r",stdin);
 cache* mycache=new cache();
    mycache->set_cache_param(1, 4);
    mycache->set_cache_param(2, value4);
    mycache->set_cache_param(3, value5);
    mycache->set_cache_param(4, 0);
    mycache->set_cache_param(5, 0);
    mycache->init_cache();
    int hitornot;
if(inFile==NULL)
{
   printf("PLEASE CHECK YOUR FILE \n \n");
}
   else
{
   //printf("FILE SUCCESSFULLY LOADED \n");
   while(true)
   { 
        char temp2[8]; //to store r/w indicator
        fscanf(stdin,"%s",temp2);//scan them into temp[]
	char temp[2];//to store address starting with 0x..
	fscanf(stdin,"%s",temp);
//        printf("%d. PC: %s %s\n",(count+1),temp2,temp);
	if(feof(stdin)) break;
       tag_size=32-(value+2);
addrBin=hexToBin(&temp2[0]);
index = get_setindex(addrBin,tag_size,value);
       hitornot = mycache->read(hexToBin(&temp2[0]));           
        biprediction = myhybrid->get_prediction(addrBin,index);
        if(strcmp(temp,"t")==0)
	taken_or_not=1;
	else
	taken_or_not=0;
        if(hitornot==1)
        {
          countb++;
          if(biprediction<2)
           prediction=0;
          else
           prediction=1;
          if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
          misb_count++;
          myhybrid->update_tables(addrBin,index,taken_or_not);
myhybrid->update_chooser(index,taken_or_not);
        }
        else
        {
        prediction=0;
        if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
        misnb_count++;
}
if((prediction==0 && taken_or_not==1)||(prediction==1 && taken_or_not==0))
 mis_count++;
  count++;
}
printf("COMMAND\n");
printf("./sim hybrid %d %d %d %d %d %d %s\n",value, value1, value2, value3, value4, value5, argv[argc-1]);
printf("OUTPUT\n");
printf("size of BTB:	 %d\n",value4);
printf("number of branches:	 %d\n",count);
printf("number of predictions from branch predictor:	%d\n",countb);
printf("number of mispredictions from branch predictor:	%d\n",misb_count);
printf("number of branches miss in BTB and taken:	 %d\n",misnb_count);
printf("total mispredictions:	%d\n",misb_count+misnb_count);
h=((mis_count*1.00)/(count*1.00))*100.00;
printf("misprediction rate:	%.2f%%\n\n",h);
printf("FINAL BTB CONTENTS\n"); 
for(int i=0;i<mycache->num_sets;i++)
{
printf("set %d:",i);
for(int j=0;j<mycache->set_associativity;j++)
{
if(mycache->sets[i]->blocks[j]->dirty)
printf("  %x   D   ",bintohex(mycache->sets[i]->blocks[j]->tag));
else
printf("  %x       ",bintohex(mycache->sets[i]->blocks[j]->tag));
}
printf("\n");
}
printf("\n");
printf("FINAL HYBRID CONTENTS\n");
myhybrid->print_table(pow(2,value));
}
}
}


else 
{
printf("Wrong input");
}

}


