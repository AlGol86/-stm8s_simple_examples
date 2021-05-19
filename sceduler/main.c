#include "main.h"
#include "iostm8s103f3.h"

sys_time_t sys_time;

int main( void )
{
  init_tim1();
  
  while(1){
  }
  
}

void init_tim1(void){
  TIM1_PSCRH=10;
  TIM1_PSCRL=0;
  TIM1_ARRH=10;
  TIM1_ARRL=100;
  TIM1_IER_UIE=1;
  asm("rim");
  TIM1_CR1_CEN=1;
}

__interrupt void tim1(void){
  extern sys_time_t sys_time;
  TIM1_SR1_UIF=0;
  if(sys_time.time==0){sys_time.time++;}
  char i_task;
  for(i_task=0;i_task<bottom_task;i_task++){
   if(sys_time.time==sys_time.task_time[i_task] ){
    invoke_task((task_t)i_task,sys_time.task_arg[i_task]);
    if(sys_time.task_period[i_task]!=0)
      sys_time.task_time[i_task]=sys_time.time+sys_time.task_period[i_task];
    else 
     sys_time.task_time[i_task]=0;
   }
  }
  sys_time.time++;
}

void scedule(task_t task, int arg, int postponement, int period){
 extern sys_time_t sys_time;
 if(sys_time.time==0){sys_time.time++;}
 if(postponement==0){postponement++;}
 sys_time.task_arg[task]=arg;
 sys_time.task_period[task]=period;
 sys_time.task_time[task]=sys_time.time+postponement;
}

void cut_from_scedule(task_t task){
 extern sys_time_t sys_time;
 sys_time.task_time[task]=0;
}

void invoke_task(task_t task, int arg){
  switch(task){
    
  case some_task:
    some(arg);
    break;
    
  case another_task:
    another(arg);
    break;
    
  default:
    break;
  }
}


void some(int arg){
  int i=0,j=0;
  for(i=0;i<arg*2;i++){
  PB_DDR^=32;
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
    }
}

void another(int arg){
  int i=0,j=0;
  for(i=0;i<arg*2;i++){
  PB_DDR^=32;
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
  for(j=0;j<30000;j++){}
  }
}

