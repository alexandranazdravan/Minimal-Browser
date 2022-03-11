/*Nazdravan Georgiana Alexandra 314CB*/

#include "utils.h"

//returns a dynamically allocated vector of Resources
Resource* get_page_resources(const char *URL_Name, int *n){
    int i, j, name_len, url_name_len = strlen(URL_Name);

    unsigned int resource_nr = 0;
    uint32_t size = 0;
    if(URL_Name == NULL){
        return NULL;
    }

    for(i = 0; i < url_name_len; i++){
        resource_nr += URL_Name[i];
    }

    resource_nr %= 13;
    Resource *result = (Resource *)calloc(resource_nr, sizeof(Resource));
    *n = resource_nr;
    for(i = 0; i < resource_nr; i++){
        sprintf(result[i].name, "%s-(%d)", URL_Name, i);
        size = 0;
    name_len = strlen(result[i].name);

        for(j = 0; j < name_len; j++){
            size += result[i].name[j];
        }
        /* Some randomness */
        size ^= size << 3;
        size += size >> 5;
        size ^= size << 4;
        size += size >> 17;
        size ^= size << 25;
        size += size >> 6;

        /* 100MB Maximum size */
        result[i].dimension = size % 104857601;
        result[i].currently_downloaded = 0;
    }

    return result;
}

void adaugareTab(ListaTaburi** taburi, Tab** TabCurent){
    /*functie care adauga un tab in lista de taburi*/

    /*se aloca o celula de lista de taburi, care este strcuturata ca o 
    lista simplu inlantuita*/
    ListaTaburi* aux = (ListaTaburi*)malloc(sizeof(ListaTaburi));

    if (aux == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    /* se "seteaza" campurile unei celule de lista*/
    aux->info = *TabCurent;
    aux->next = NULL;

    /*daca lista de taburi nu are elemente, atunci celula aux creata
    devine primul element altfel,
    se parcurge lista cu un pointer p si se insereaza ux la final*/
    /*practic, cand intra pe if, si nu pe else este cazul de inceput
    cand avem doar un tab gol deschis*/
    if((*taburi) == NULL)
            (*taburi) = aux;
    else
        {
            ListaTaburi* p;
            for(p = *taburi; p->next != NULL; p = p->next);
            p->next = aux;
        }

}

void newtab(Tab** TabCurent, ListaTaburi** taburi){
    /*functie care initializeaza un tab*/

    /*se aloca elementul de tip tab*/
    *TabCurent = (Tab*)malloc(sizeof(Tab));

    if (*TabCurent == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    /*se pun toate campurile unui tab pe NULL, cum se cere in cerinta*/
	(*TabCurent)->current_page = NULL;
	(*TabCurent)->back_stack = NULL;
	(*TabCurent)->forward_stack = NULL;

    /*se apeleaza functia pentru a adauga prorpiu zis tab-ul
    in lista*/
    adaugareTab(taburi, TabCurent);
}

void deltab(ListaTaburi** taburi, Tab** TabCurent){
    /*functie care sterge ultimul tab din lista*/

    /*se foloseste un pointer care sa arate spre zona de tab uri si 
    un altul, de acelasi tip, care este, practic, folosit pentru a retine
    elementul anterior, si la inceput este initializat cu NULL*/
	ListaTaburi* aux = *taburi;
	ListaTaburi* PozitieCurentaNoua = NULL;

    /*se verifica daca exista un singur tab;
    in acest caz se elibereaza zona de memorie si se seteaza tab-ul pe NULL*/
	if((*taburi)->next == NULL)
	{
		free(*taburi);
		*TabCurent = NULL;
	}

    /*parcurgem un for doar pentru a ajunge cu pointer-ul aux pe tabul curent*/
	for(;aux->info != *TabCurent; PozitieCurentaNoua = aux, aux = aux->next);

    /*daca aux->next este NULL, adica daca este ultimul element, atunci se muta
    pozitia tabului pe PozitiaCurentaNoua->info pentru a nu se pierde altfel, 
    daca tab-ul curent este oricare alt element, se continua for-ul pana la NULL
    si se elibereaza zona de memorie a celulei de lista*/
	if(aux->next == NULL)
		{
			*TabCurent = PozitieCurentaNoua->info;

            free(aux);
		}
	else
		{
			for(;aux->next != NULL; PozitieCurentaNoua = aux, aux = aux->next);

            free(aux);
		}

    /*la final, se seteaza pointer-ul de next pe NULL*/
	PozitieCurentaNoua->next = NULL;
}

void change_tabs(ListaTaburi** taburi, Tab** TabCurent, FILE *fin){
    /*functie prin care se schimba tab-uri*/

    /*se citeste numarul tab-ului din fisier*/
	int nrTab;
	fscanf(fin,"%d", &nrTab);

    /*se foloseste, din nou, ca mai sus, un pointer aux*/
	ListaTaburi* aux = *taburi;

    /*daca este vorba de tab-ul 0, adica prima, inseamna ca tab-ul curent
    trebuie sa pointeze acolo unde pointeaza taburi->info altfel,
    se foloseste un for pentru a ajunge cu aux pe pozitia respectiva, iar
    tab-ul curent va fi schimbat*/
    if(nrTab == 0)
        (*TabCurent) = (*taburi)->info;
    else{
        int i;

	    for(i = nrTab; aux != NULL && i > 0; i--){
            aux = aux->next;
        }

    (*TabCurent) = aux->info;

    }

}

void print_open_tabs(ListaTaburi* taburi,FILE* fout) {
    /*functie care afiseaza tab-urile deschise*/

    ListaTaburi *temp = taburi;
    int i;

    /*parcurgem un for, atat cu i cat si cu temp, adica cu lista de tab-uri
    si afisam i-ul, adica nr. paginii deschise(curente) si url-ul ei*/
    for (i = 0; temp != NULL; temp = temp->next, i++) {
        if (temp->info->current_page != NULL)
          fprintf(fout,"(%d: %s)\n", i, temp->info->current_page->url);
        else
          fprintf(fout,"(%d: empty)\n", i);
    }
}

void open_page(Tab** TabCurent, Coada* queue, FILE* fin) {
    /*functie care deschide o pagina*/

    /*se aloca campul de pagina curenta a tab-ului*/
    (*TabCurent)->current_page = (Pagina*)malloc(sizeof(Pagina)); 

    if ((*TabCurent)->current_page == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    /*se citeste ur;-ul paginii din fisiser*/
    fscanf(fin,"%s", (*TabCurent)->current_page->url);

    /*se apeleaza functia pentru resurse pentru a se "completa" campul
    de Resources dintr-o pagina curenta*/
    (*TabCurent)->current_page->resources = 
        get_page_resources((*TabCurent)->current_page->url, 
        &(*TabCurent)->current_page->num_res);

    /*se adauga in stiva de back pagina curenta*/
    push(&(*TabCurent)->back_stack, *(*TabCurent)->current_page);

    /*se adauga in coada globala a istoricului pagina curenta*/
    Enqueue(queue, (*TabCurent)->current_page->url);

    /*se verifica daca stiva de forward este goala;
    daca nu este goala, se goleste*/
    if ((*TabCurent)->forward_stack != NULL){
      free((*TabCurent)->forward_stack);
      (*TabCurent)->forward_stack = NULL;
    }
   
}

void back(Tab** TabCurent, FILE* fout) {
    /*functie care ne duce cu o pagina in urma*/

    /*se verifica daca e goala stiva; daca este, se afiseaza mesajul si
    se da return pentru a iesi din functie*/
    if ((*TabCurent)->back_stack == NULL) {
      fprintf(fout,"can't go back, no pages in stack\n");
      return;
    }

    /*atunci cand se da back, pagina curenta se adauga in stiva de forward(push)
    si se scoate din cea de back(pop)*/
    push(&(*TabCurent)->forward_stack, *(*TabCurent)->current_page);
    pop(&(*TabCurent)->back_stack);

    /*daca stiva de back mai are pagini, se apeleaza top si se muta ce e
    in aux, adica cel mai de sus element din stiva, in pagina curenta a 
    tab-ul curent; daca nu mai are pagini in back, pagina curenta devine NULL*/
    if((*TabCurent)->back_stack != NULL){
        Pagina aux = top((*TabCurent)->back_stack);
        move(TabCurent, aux);
    }
    else
        (*TabCurent)->current_page = NULL;

}

void forward(Tab** TabCurent, FILE* fout) {
    /*functie care ne duce cu o pagina in fata*/

    /*daca nu exista elemente in stiva, adica daca nu s-a apelat back,
    se afiseaza mesajul si se da return pentru a iesi din functie*/
    if ((*TabCurent)->forward_stack == NULL) {
      fprintf(fout,"can't go forward, no pages in stack\n");
      return;
    }

    /*se adauga in stiva de back pagina din forward*/
    push(&(*TabCurent)->back_stack, ((*TabCurent)->forward_stack)->info);

    /*cel mai de sus element din stiva se scoate din stiva si se salveaza
    in aux*/
    Pagina aux =  pop(&(*TabCurent)->forward_stack);

    /*se apeleaza functia move pentru a muta url-ul paginii, numarul de resurse
    si vectorul de resurse in noua pagina curenta*/
    move(TabCurent, aux);

}

void push(StackPagini**top, Pagina page) {
    /*functie de push*/

    /*se adauga un element in stiva*/
    StackPagini* temp = (StackPagini*)malloc(sizeof(StackPagini));

    if (temp == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    temp->info = page;
    temp->next = *top;
    *top = temp;
}

int isEpmty(StackPagini* top) {
    /*functie care verifica daca o stiva e goala*/
    return top == NULL;
}

Pagina pop(StackPagini** top) {
    /*functie care scoate elementul cel mai de sus din stiva*/
    Pagina aux;
      if (isEpmty(*top))
      return aux;
    StackPagini *temp = *top;
    aux = temp->info;
    *top = (*top)->next;
    free(temp);
    return aux;
}
 
 Pagina top(StackPagini* top) {
    /*functie care doar afiseaza ce este in ultimul element din stiva,
    adica o pagina*/
    if(!isEpmty(top))
        return top->info;
}

void move(Tab** TabCurent, Pagina page){
    /*functie care muta campurile dintr-o pagina
    in pagina curenta a tab-ului*/

    strcpy((*TabCurent)->current_page->url,page.url);
    (*TabCurent)->current_page->num_res = page.num_res;
    (*TabCurent)->current_page->resources = 
        get_page_resources((*TabCurent)->current_page->url, 
        &(*TabCurent)->current_page->num_res);
}

Coada* CopieQ(void* queue){
    /*functie care face o copie a unei cozi*/
    /*este necesara deoarece daca as itera direct prin coada,
    as pierde-o*/

    void* queueaux = createQ();

    ((Coada*)queueaux)->front = ((Coada*)queue)->front;
    ((Coada*)queueaux)->end = ((Coada*)queue)->end;

    return ((Coada*)queueaux);
}

void history(void* queue,FILE* fout) {
    /*functie care afiseaza coada istoricului global*/

    /*se creeaza o copie aux a cozii*/
    Coada* aux = CopieQ(queue);

    /*se parcurge coada si se afiseaza elementele din ea*/
    for(; ((Coada*)aux)->front != ((Coada*)aux)->end; 
        ((Coada*)aux)->front = (((Coada*)aux)->front)->next){  

        fprintf(fout,"%s\n", (char*)(((Coada*)aux)->front->info));
    }

    /*se afiseaza ultimul element din coada*/
    if(((Coada*)aux)->front != NULL)
        fprintf(fout,"%s\n",(char*)(((Coada*)aux)->front->info));
}

void* createQ(){
    /*functie care creeaza o coada*/

    Coada* queue = (Coada*)malloc(sizeof(Coada));

       if (queue == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    queue->front = queue->end = NULL;
    return queue;
}

int Enqueue(void* queue, void* url){
    /*functie care adauga un element la finalul cozii*/
    CoadaHistory* temp = (CoadaHistory*)malloc(sizeof(CoadaHistory));

    if (temp == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    temp->next = NULL;

    strcpy(temp->info,url);

    if(((Coada*)queue)->end == NULL)
        ((Coada*)queue)->end = temp;
    else
      {((Coada*)queue)->end->next = temp;
        ((Coada*)queue)->end = temp;}
    if(((Coada*)queue)->front == NULL)
        ((Coada*)queue)->front = ((Coada*)queue)->end;
    return 1;
}

int ExtrQ(void* queue) {
    /*functie care extrage un element de la inceputul cozii*/
    CoadaHistory* temp = (CoadaHistory*)malloc(sizeof(CoadaHistory));

    if (temp == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

     temp = ((Coada*)queue)->front;

     ((Coada*)queue)->front = temp->next;
     free(temp);
     return 1;
}

void DelQ(void* queue){
    /*functie care sterge o coada intreaga*/

    CoadaHistory* temp;
    while(((Coada*)queue)->front != NULL){
        temp = ((Coada*)queue)->front;

        ((Coada*)queue)->front = ((Coada*)queue)->front->next;

        free(temp);

    }

    free(queue);
}

void del_history(void* queue, FILE* fin){
    /*functie care sterge un anumit numar de elemente din coada*/

    int nr;
    int i;

    /*se citeste numarul din fisier*/
    fscanf(fin,"%d",&nr);

    /*se numara cate elemente sunt in coada*/
    int nrelem = nrElem(queue);

    /*daca numarul citit din fiser e 0 sau daca e mai mare decat numarul de
    elemente din coada, atunci se sterge coada complet
    (si se apeleaza din nou functia de createQ ca altfel da seg fault)*/
    /*daca nu e niciuna din variantele de mai sus, se apeleaza functia
    de extragere de nr ori*/
    if(nr > nrelem){
        DelQ(queue);
        queue = createQ();
    }
    else if(nr == 0){
        DelQ(queue);
        queue = createQ();
    }
    else{
        for(i = 0; i < nr; i++) {
            ExtrQ(queue);
        }
    } 
}

int nrElem(void* queue){
    /*functie care numara numarul de elemente dintr-o coada*/

    /*din nou se creeaza coada auxiloara ca sa nu se piarda coada*/
     Coada* aux = CopieQ(queue);

     int nrelem = 0;

     for(; ((Coada*)aux)->front != ((Coada*)aux)->end; 
        ((Coada*)aux)->front = (((Coada*)aux)->front)->next)
        nrelem++;

    return nrelem;
}

void list_dl(Tab* TabCurent, FILE* fout){
    /*functie care afiseaza lista de resurse ce pot fi descarcate*/
    Tab *temp = TabCurent;
    int i;
    for (i = 0; i < temp->current_page->num_res; i++) {
        fprintf(fout,"[%d - \"%s-(%d)\" : %lu]\n", i, 
            temp->current_page->url, i, 
            temp->current_page->resources[i].dimension);
    }
}

void set_band(unsigned long* band, FILE* fin){
    /*functie care citeste viteza de transfer din fisier*/
    fscanf(fin,"%lu", band);
}

void downloads(CoadaRes** priorityQueue,FILE* fout){
    /*functie care afiseaza descarcarile*/

    /*priorityQueue este, de fapt, o lista generica caci este mai usor
    de inserat in ea si de lucrat cu ea*/
    CoadaRes* temp = *priorityQueue;

    /*cu un for se afiseaza in fisier resursele din care se mai descarca*/
    /*cu celalalt for se afiseaza ce este completed*/
    for(; temp != NULL; temp = temp->next)
         if(((*((Resource*)(temp->info))).dimension) - 
            ((*((Resource*)(temp->info))).currently_downloaded) != 0){

            unsigned long diff = ((*((Resource*)(temp->info))).dimension) - 
                ((*((Resource*)(temp->info))).currently_downloaded);

            fprintf(fout,"[\"%s\" : %lu/%lu]\n", 
                ((*((Resource*)(temp->info))).name), 
                diff, ((*((Resource*)(temp->info))).dimension));
        }

    for(; temp != NULL; temp = temp->next)
         if(((*((Resource*)(temp->info))).dimension) - 
            ((*((Resource*)(temp->info))).currently_downloaded) == 0){

            fprintf(fout,"[\"%s\" : completed]\n", 
                ((*((Resource*)(temp->info))).name));

        }
}

void download(Tab* TabCurent, CoadaRes** priorityQueue, FILE* fin){
    /*functie care adauga resursa in coada de prioritati*/

    int index;

    /*se citeste index-ul din fisier*/
    fscanf(fin,"%d", &index);

    /*se face o copie e resursei care se afla pe pozitia index in vector*/
    Resource* copie = copiereResursa(TabCurent->current_page->resources[index]);

    /*se insereaza in coada de prioritati copia creata*/
    InsSortata(priorityQueue, copie);
}

void InsSortata(CoadaRes** priorityQueue, Resource* copie){
    /*functie care insereaza sortat in coada de prioriati*/
    CoadaRes* head = *priorityQueue;

    /*se aloca celula ce trebuie inserata*/
    CoadaRes* aux = (CoadaRes*)malloc(sizeof(CoadaRes));
    aux->next = NULL;
    (aux->info) = (void*)copie;

    /*daca nu exista eleemnte in coada, atunci primul element devine aux*/
    if(*priorityQueue == NULL){
        (*priorityQueue) = aux;
        return;
    }

    /*se face diferenta si se memoreaza in variabile*/
    unsigned long remainingH = diferenta(*(Resource*)(head->info));
    unsigned long remainingC = diferenta(*copie);

    /*se verifica care e mai mare pentru a vedea cum trebuie adaugata in lista:
    dupa un element sau inaintea lui*/
    /*in cazul in care sunt mai multe elemente, se verifica si diferenta resurselor urmatoare*/
   if(remainingH > remainingC){
        aux->next = (*priorityQueue);
        (*priorityQueue) = aux;
    }
    else{
        while(head->next != NULL && diferenta(*(Resource*)(head->next->info))
                                             < remainingC){
            head = head->next;
        }
        aux->next = head->next;
        head->next = aux;
    }

}

unsigned long diferenta(Resource nod){
    /*functie care face calculeaza, practic, cat a mai ramas de descarcat*/
    return (nod.dimension - nod.currently_downloaded);

}

void* createQPriority(){
    /*functie care creeaza o coada de prioritati*/
    CoadaRes* queue = (CoadaRes*)malloc(sizeof(CoadaRes));

       if (queue == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }
    queue->next = NULL;
    return queue;
}


Resource* copiereResursa(Resource resursa){
    /*functie care copiaza o resursa si intoarce resursa copiata*/

    Resource* aux = (Resource*)malloc(sizeof(Resource));

    if (aux == NULL) {
        printf("Alocare de memorie esuata!\n");
        exit(0);
    }

    strcpy(aux->name, resursa.name);

    aux->dimension = resursa.dimension;

    aux->currently_downloaded = resursa.currently_downloaded;

    return aux;
}

void ExtrQPriority(CoadaRes** priorityQueue){
    /*functie care extrage elementul din coada*/
    CoadaRes* temp = *priorityQueue;
    *priorityQueue = (*priorityQueue)->next;
    free(temp);
}

void actualizareDownload(Tab** TabCurent, unsigned long band, char* nume){
    /*functie care actualizeaza ce este in currently downloaded*/
    int i;
     Tab* temp = *TabCurent;

     /*se parcurge vectorul de resurse si atunci cand se gaseste resursa care
     are acelasi nume cu cel din coada de prioritati, se actualizeaza download-ul*/
    for(i = 0; i < (*TabCurent)->current_page->num_res; i++){
        if(strcmp((*TabCurent)->current_page->resources[i].name , nume) == 0){
            (*TabCurent)->current_page->resources[i].currently_downloaded += band;
        }
    }

}

Tab* wait(CoadaRes** priorityQueue, Tab** TabCurent, unsigned long band, FILE* fin){
    /*functie prin care se descarca resurse in functie de nr de secunde*/

    int sec;
    int i;

   fscanf(fin,"%d", &sec);

   /*se parcurge numarul de secunde si se actualizeaza in tab-ul curent*/
    for(i = 0; i < sec; i++)
        (*TabCurent) = waitOne(priorityQueue, TabCurent,band);
   
   return *TabCurent;
}

Tab* waitOne(CoadaRes** priorityQueue, Tab** TabCurent, unsigned long band){
    /*functie la care se sta o secunda, apelata impreuna cu open_page*/

    /*se verifica daca coada de prioriati e nula*/
    if((*priorityQueue) == NULL) 
        return (*TabCurent);

    Tab* temp = *TabCurent;
    /*se descarca un anumit numar de Bps in functie de cat este viteza
    de transfer*/
    (*(Resource*)((*priorityQueue)->info)).currently_downloaded += band;

    /*se actualizeaza currentlu_downloaded in tab-ul curent*/
    actualizareDownload(TabCurent, band,(*(Resource*)((*priorityQueue)->info)).name);

   return *TabCurent;
}
