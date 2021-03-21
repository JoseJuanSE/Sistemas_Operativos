#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#define MAX 80
key_t key; 
int msgid; 
struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 
void leer(){
	printf("Tu dices : "); 
    fgets(message.mesg_text,MAX,stdin); 
    msgsnd(msgid, &message, sizeof(message), 0);
}
void imprimir(){
    msgrcv(msgid, &message, sizeof(message), 1, 0); 
    printf("Otro Dice: %s \n",message.mesg_text); 
}
int main(){ 
    key = ftok("progfile", 65);  
    msgid = msgget(key, 0666 | IPC_CREAT); 
    message.mesg_type = 1; 
    for(int i=0;i>-1;i++){
        if(i%2==1)imprimir();
        else leer();
    }  	
    return 0; 
} 