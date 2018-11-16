/*
 * CPE-555 Real-Time and Embedded Systems
 * Stevens Institute of Technology
 * Spring 2017
 *
 * FreeRTOS Template
 *
 * Name: Daniel Kim
 */

/* Standard includes */
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"

/*Include task and queue */
#include "task.h"
#include "queue.h"
#include "semphr.h" //Needed to stop repeating print statements while display is running

/* Global Variable from Slides*/
SemaphoreHandle_t xSemaphore = NULL;
QueueHandle_t xQueue;

static TaskHandle_t DisplayHandle;
static TaskHandle_t TimeHandle;

/* Print Statements */
void PrintTime(int temp){
	int minutes = temp / 60;
	int seconds = temp - (minutes*60);
	printf("%d: ", minutes);
	printf(" %d\n", seconds);
}

void TimeTask(int parameter){

	//Initially send the initial parameter to the queue
	int temp =  parameter;
	xQueueSend(xQueue, &temp, 0);

	//Create a Semaphore here and give allowing to go to other task
	xSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphore);

	for (;;){

		/* When count is done */
		if (temp == 0){
			vTaskDelete(TimeHandle);
		}


		temp = temp - 1;
		xQueueSend(xQueue, &temp, 0);
		vTaskDelay(1000/portTICK_RATE_MS);
		xSemaphoreGive(xSemaphore); //Allow other task to access.
	}
}
void DisplayTask(){
	int temp = 0;
	for(;;){
		if (xSemaphore != NULL){
			if (xSemaphoreTake(xSemaphore, (TickType_t) 10) == pdTRUE){
				if(xQueue != 0){
					xQueueReceive(xQueue, &temp, 10);
				}
				if(temp == 0){
					PrintTime(temp);
					printf("bYe");
					vTaskDelete(DisplayHandle);
				}
				else{
					PrintTime(temp);
				}
			}
		}
	}
}


int main( void )
{
	/* set STDOUT to be non-buffered to that printf() messages display immediately */
	setbuf(stdout, NULL);
	int seconds, minutes = 0;
	printf("Enter Minutes\n");
	scanf("%d", &minutes);
	printf("Enter Seconds\n");
	scanf("%d", &seconds);
	int total_sec = (minutes * 60) + seconds;
//	printf("Total seconds is %d \n", total_sec);

	xQueue = xQueueCreate(10, sizeof(int));
	xTaskCreate( DisplayTask, "DisplayTask", 1000, NULL, 1, &DisplayHandle);
	//Time Task higher priority as Display needs to read from queue first
	xTaskCreate( TimeTask, "TimingTask", 1000, (int) total_sec, 2, &TimeHandle);

	vTaskStartScheduler();

	return 0;
}
