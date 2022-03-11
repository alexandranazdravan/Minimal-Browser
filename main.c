/*Nazdravan Georgiana Alexandra 314CB*/
#include "utils.h"

int main(int argc, char** argv)
{	
	/*se citeste din fisiere*/
	FILE* fin = fopen(argv[1],"r");
	FILE* fout = fopen(argv[2],"w+");

	Tab* TabCurent;

	ListaTaburi *taburi = NULL;

	/*se creeaza coada de istoric global*/
	Coada* queue = createQ();

	/*se creeaza acel tab cu campurile pe NULL*/
	newtab(&TabCurent, &taburi);

	CoadaRes* priorityQueue = NULL;

	/*se utilizeaza un buffer pentru citirea comenzilor din fisier*/
	char* buffer = (char*)malloc(sizeof(char)*35);
	
	if (buffer == NULL) {
        	printf("Alocare de memorie esuata!\n");
        	exit(0);
    	}

	unsigned long band = 1024;

	while(!feof(fin)) {
		/*se coteste buffer-ul si se apeleaza functiile*/
		fscanf(fin,"%s",buffer);

		/*daca se ajunge la finalul fisierului, se opreste citirea*/
		if(feof(fin)) 
			break;

		if(strcmp(buffer,"newtab") == 0)
			newtab(&TabCurent, &taburi);
		else if(strcmp(buffer,"goto") == 0) {
			open_page(&TabCurent, queue, fin);
			waitOne(&priorityQueue,&TabCurent,band);
		}
		else if(strcmp(buffer,"back") == 0)
			back(&TabCurent,fout);
		else if(strcmp(buffer,"forward") == 0)
			forward(&TabCurent,fout);
		else if(strcmp(buffer,"print_open_tabs") == 0)
			print_open_tabs(taburi,fout);
		else if(strcmp(buffer,"change_tab") == 0)
			change_tabs(&taburi,&TabCurent,fin);
		else if(strcmp(buffer,"deltab") == 0)
			deltab(&taburi,&TabCurent);
		else if(strcmp(buffer,"history") == 0)
			history(queue, fout);
		else if(strcmp(buffer,"del_history") == 0)
			del_history(queue,fin);
		else if(strcmp(buffer,"list_dl") == 0)
			list_dl(TabCurent,fout);
		else if(strcmp(buffer,"downloads") == 0)
			downloads(&priorityQueue,fout);
		else if(strcmp(buffer,"download") == 0)
			download(TabCurent, &priorityQueue,fin);
		else if(strcmp(buffer,"wait") == 0)
			TabCurent = wait(&priorityQueue,&TabCurent,band,fin);
		else if(strcmp(buffer,"set_band") == 0)
			set_band(&band,fin);
	}
	/*se inchid fisierele si se elibereaza buffer-ul*/
	fclose(fin);
	fclose(fout);
	free(buffer);

	return 0;
}
