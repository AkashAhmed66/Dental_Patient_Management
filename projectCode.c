#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


pthread_mutex_t patient_mutex, waiting_mutex;
sem_t dentist_sem, patient_sem;

int total_patient, total_chair, patient_waiting = 0, patient_arrived = 0;

void *dentist(void *arg) {
    while(1){
        printf("-------- Dentist: Sleeping --------\n\n");

        sem_wait(&dentist_sem);

        printf("-------- Dentist: Awake, Operating on Patient --------\n\n");

        sleep(rand() % 5 + 1);

        printf("-------- Dentist: Procedure Complete, Patient Leaving --------\n\n");

        sem_post(&patient_sem);

        patient_arrived++;

        if(patient_arrived >= total_patient) break;
    }
}

void *patient(void *arg) {

    int t = *(int *)arg;

    sleep(rand() % 10 + t);

    pthread_mutex_lock(&waiting_mutex);

    if(patient_waiting >= total_chair){
        printf("Patient arrived, no empty chair available. Leaving.\n\n");
        patient_arrived++;

        pthread_mutex_unlock(&waiting_mutex);

        pthread_exit(NULL);
    }

    patient_waiting++;

    printf("Patient arrived and waiting in the waiting room.\n\n");

    pthread_mutex_unlock(&waiting_mutex);

    pthread_mutex_lock(&patient_mutex);

    printf("Patient is in the dentist chamber.\n\n");
    
    patient_waiting--;

    sem_post(&dentist_sem);
    sem_wait(&patient_sem);


    printf("Patient leaving after treatment.\n\n");

    pthread_mutex_unlock(&patient_mutex);

}

int main() {
    printf("Enter the number of total_patient: ");
    scanf("%d", &total_patient);

    printf("Enter the number of total_chair: ");
    scanf("%d", &total_chair);

    pthread_t patient_threads[total_patient], dentist_thread;

    sem_init(&dentist_sem, 0, 0);
    sem_init(&patient_sem, 0, 0);

    pthread_mutex_init(&patient_mutex, NULL);
    pthread_mutex_init(&waiting_mutex, NULL);

    int t[total_patient];

    for(int i = 0; i < total_patient; i++){
        t[i] = i + 1;
        pthread_create(&patient_threads[i], NULL, patient, &t[i]);
    }

    pthread_create(&dentist_thread, NULL, dentist, NULL);
    pthread_join(dentist_thread, NULL);

    for(int i = 0; i < total_patient; i++){
        pthread_join(patient_threads[i], NULL);
    }

    sem_destroy(&dentist_sem);
    sem_destroy(&patient_sem);

    pthread_mutex_destroy(&waiting_mutex);
    pthread_mutex_destroy(&patient_mutex);

    return 0;
}