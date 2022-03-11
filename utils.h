/*Nazdravan Georgiana Alexandra 314CB*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifndef _DOWNLOAD_RESOURCE_
#define _DOWNLOAD_RESOURCE_

/*
	downloaded_size can be used to save the currently downloaded size;
	by default downloaded_size is set to 0
*/
typedef struct{
	char name[100];
	unsigned long dimension;
	unsigned long currently_downloaded;
} Resource;

// coada folosita la history
typedef struct history{
     char info[41]; 
     struct history* next;
} CoadaHistory;

typedef struct coada{
	CoadaHistory* front, *end;
} Coada;

typedef struct res{
     void* info;  // Resource
     struct res* next;
} CoadaRes;

typedef struct page
{
	char url[21];
	int num_res;
	Resource* resources;
} Pagina;

typedef struct stack{
	Pagina info; 
	struct stack* next;
} StackPagini;

typedef struct tab
{
	Pagina* current_page;
	StackPagini* back_stack;
	StackPagini* forward_stack;
}Tab;

typedef struct lista
{
  Tab* info;
  struct lista *next;

} ListaTaburi;

Resource* get_page_resources(const char *URL_Name, int *n);
void adaugareTab(ListaTaburi** taburi, Tab** TabCurent);
void newtab(Tab** TabCurent, ListaTaburi** taburi);
void deltab(ListaTaburi** taburi, Tab** TabCurent);
void change_tabs(ListaTaburi** taburi, Tab** TabCurent, FILE* fin);
void print_open_tabs(ListaTaburi* taburi, FILE* fout);
void open_page(Tab** TabCurent,Coada* queue, FILE* fin);
void back(Tab** TabCurent, FILE* fout);
void forward(Tab** TabCurent, FILE* fout);
void push(StackPagini**top, Pagina page);
int isEpmty(StackPagini* top);
Pagina pop(StackPagini** top);
Pagina top(StackPagini* top);
void move(Tab** TabCurent, Pagina page);
void* createQ();
int Enqueue(void* queue, void* url);
void history(void* queue, FILE* fout);
int ExtrQ(void* queue);
void DelQ(void* queue);
void del_history(void* queue, FILE* fin);
Coada* CopieQ(void* queue);
void list_dl(Tab* TabCurent, FILE* fout);
void downloads(CoadaRes** priorityQueue,FILE* fout);
void download(Tab* TabCurent, CoadaRes** priorityQueue, FILE* fin);
Resource* copiereResursa(Resource resursa);
void* createQPriority();
void InsSortata(CoadaRes** priorityQueue, Resource* copie);
unsigned long diferenta(Resource nod);
Tab* wait(CoadaRes** priorityQueue, Tab** TabCurent,unsigned long band, FILE* fin);
void ExtrQPriority(CoadaRes** priorityQueue);
void actualizareDownload(Tab** TabCurent, unsigned long band, char* nume);
void set_band(unsigned long* band, FILE* fin);
int nrElem(void* queue);
Tab* waitOne(CoadaRes** priorityQueue, Tab** TabCurent, unsigned long band);

#endif /* _DOWNLOAD_RESOURCE_ */
