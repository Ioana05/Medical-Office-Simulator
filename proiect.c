// 21. SimulareCabinet
// Sa se implementeze un program care simuleaza activitatea dintr-un cabinet medical:
// vor fi create un numar dat de thread-uri pacienti care vor astepta pentru eliberarea
// unor resurse reprezentand doctorii (pot fi niste structuri iar consultatia sa consiste
// blocarea acelei structuri si si afisarea id-ilui doctorului). Clientii vor ocupa resursa 
// doctor pentru o perioada random care sa nu depaseasca o limita data. Fiecare pacient va 
// fi generat la un interval aleator pentru o perioada de timp. Dupa consultatie, pacientul 
// isi va afisa timpul de asteptare si timpul consultatiei

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// consideram limita de timp pentru o consultatie doua secunde
# define MAX_TIME 2
# define DOCTORI 8


// structura va contine un mutex si o variabila prin care vom verifica
// daca doctorul este ocupat sau nu

struct Doctor {
    bool disponibil;
    pthread_mutex_t mutex;
    int pacient;
};

// consideram ca exista 8 doctori
struct Doctor doctori[DOCTORI+1];

// vom folosi un vector pentru a retine timpul la care 
// a inceput fiecare proces si un vector pt a retine timpul 
// unei consultatii
int *timpClient;
int *timpConsultatie;



// in interiorul functiei "Sala de asteptare" vom cauta un doctor disponibil
// pentru pacientul curent

void* SalaDeAsteptare(void* arg)
{
    int ok = 0;
    int timpFinal = 0;

    // facem conversia de la void la int
    int* NrOrdine = malloc(sizeof(int));
    *NrOrdine = *(int*)arg;

    // timpul la care un pacient a intrat in sala
    timpClient[*NrOrdine] =  clock();

    // atribuim o valoare corespunzatoare duratei consultatiilor
    //  care sa se afle in intervalul (0, durataMaxima)
    timpConsultatie[*NrOrdine] = rand() % MAX_TIME+1;

    // parcurgem cu un for doctorii si verificam care este disponibil
    for (int doctor = 1; doctor <= DOCTORI; doctor++)
    {
        if (ok == 1)
            continue;
        if (doctori[doctor].disponibil == 1)
        {
            ok = 1;
            doctori[doctor].disponibil = 0;
            doctori[doctor].pacient = *NrOrdine;

            // blocam doctorul pentru perioada de timp calculata mai sus
            pthread_mutex_lock(&(doctori[doctor].mutex));
            printf("Doctorul %d este ocupat cu pacientul %d\n", doctor, doctori[doctor].pacient);
            sleep(timpConsultatie[doctori[doctor].pacient]);
            pthread_mutex_unlock(&(doctori[doctor].mutex));
            
            // cand elibereaza mutex ul
            timpFinal = clock();
            float timpTotal = ((double)(timpFinal - timpClient[doctori[doctor].pacient])) / CLOCKS_PER_SEC;
            printf("Durata consultatiei pentru pacientul %d este %d, iar timpul de asteptare este %f\n", doctori[doctor].pacient, timpConsultatie[doctori[doctor].pacient], timpTotal );
            fflush(stdout);
            doctori[doctor].disponibil = 1;
            free(NrOrdine);

        }
    }
    // pacientul nu gaseste niciun doctor disponibil
    if (ok == 0 )
    { 
        int  i = 0;
        printf("pacientul %d asteapta\n",*NrOrdine );
        // loopez prin vectorul de doctori pana un doctor redevine disponibil
        // pthread_mutex_trylock returneaza 0 cand reuseste sa blocheze mutexul(operatia s a rezolvat cu succes)
        while(pthread_mutex_trylock(&(doctori[i%DOCTORI + 1].mutex))!=0) 
        {
            i++;
            if (i > DOCTORI)
                i = 0;
        }
        ok = 1;

        doctori[i%DOCTORI+1].disponibil = 0;
        doctori[i%DOCTORI+1].pacient = *NrOrdine;

        // blocam doctorul pentru perioada de timp calculata mai sus
        printf("Doctorul %d este ocupat cu pacientul %d.\n", i%DOCTORI+1, doctori[i%DOCTORI+1].pacient);
        fflush(stdout);
        sleep(timpConsultatie[doctori[i%DOCTORI+1].pacient]);
        pthread_mutex_unlock(&(doctori[i%DOCTORI+1].mutex));
        
        timpFinal = clock();
        float timpTotal = ((double)(timpFinal - timpClient[doctori[i%DOCTORI+1].pacient])) / CLOCKS_PER_SEC;
        printf("Durata consultatiei pentru pacientul %d este %d, iar timpul de asteptare este %f\n", doctori[i%DOCTORI+1].pacient, timpConsultatie[doctori[i%DOCTORI+1].pacient], timpTotal );
        fflush(stdout);
        doctori[i%DOCTORI+1].disponibil = 1;
        free(NrOrdine);

    }


    //  // daca nu am gasit niciun doctor disponibil,
    //  // threadul va astepta pana va termina un doctor


 }

int main() 
{   
    timpClient = (int *)malloc(62 * sizeof(int));
    timpConsultatie = (int *)malloc(62 * sizeof(int));
    

    // initializam doctorii cu valorile corespunzatoare
    for (int  i = 1; i <= DOCTORI; i++ )
    {
        doctori[i].disponibil = true;
        if (pthread_mutex_init(&doctori[i].mutex, NULL)) {
            perror(NULL);
            return errno;
        } 
        doctori[i].pacient = 0;
    }

    // pp ca exista 30 de pacienti
    pthread_t clienti[62];
    
    for (int i = 1; i <= 61; i++)
    {
        int* NrOrdine = (int*)malloc(sizeof(int));
        *NrOrdine = i;
        pthread_create(&clienti[i], NULL, SalaDeAsteptare, NrOrdine);
    } 
    for (int i = 1; i <= 61; i++)
        {
            void *retval;
            pthread_join(clienti[i], &retval);
        }

    // Eliberez resursele

    for (int i = 1; i <= DOCTORI; i++ )
    {
        pthread_mutex_destroy(&doctori[i].mutex);
    }

    free(timpClient);
    free(timpConsultatie);

    return 0;

}

