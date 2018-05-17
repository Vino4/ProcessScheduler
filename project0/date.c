#include "date.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct date
{
    int day;
    int month;
    int year;

};

/*
 * date_create creates a Date structure from `datestr`
 * `datestr' is expected to be of the form "dd/mm/yyyy"
 * returns pointer to Date structure if successful,
 *         NULL if not (syntax error)
 */
Date *date_create(char *datestr){
	char *token;
	int day, month, year;

	token = strtok(datestr, "/");

   /* walk through other tokens */
	day = atoi(token);
	token = strtok(NULL, "/");
	month = atoi(token);
	token = strtok(NULL, "/");
	year = atoi(token);
	token = strtok(NULL, "/");

	if (day < 1 || day > 31 || month < 1 || month > 12 || year <1){
		return NULL;
	}

	Date * newDate = (Date*) malloc (sizeof(Date));
	newDate->day = day;
	newDate->month = month;
	newDate->year = year;
	printf( "DATE_CREATE: Day: %d Month: %d Year: %d\n", newDate->day,  newDate->month, newDate->year);

	return newDate;
}

/*
 * date_duplicate creates a duplicate of `d'
 * returns pointer to new Date structure if successful,
 *         NULL if not (memory allocation failure)
 */
Date *date_duplicate(Date *d){
	Date * newDate = (Date*) malloc (sizeof(Date));
	newDate->day = d->day;
	newDate->month = d->month;
	newDate->year = d->year;
	return newDate;
}

/*
 * date_compare compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */
int date_compare(Date *date, Date *date2){
	if (date->year > date2->year){
		return 1;
	} else if (date->year < date2->year) {
		return -1;
	} else {
		if (date->month > date2->month){
			return 1;
		} else if (date->month < date2->month) {
			return -1;
		} else {
			if (date->day > date2->day){
				return 1;
			} else if (date->day < date2->day) {
				return -1;
			} else {
				return 0;
			}
		}
	}
}

/*
 * date_destroy returns any storage associated with `d' to the system
 */
void date_destroy(Date *d){
	free(d);
}
/*
int main(void){

	char * test = (char*) malloc(sizeof(char) * 20);
	strcpy(test, "14/03/1969");
	Date * test2 = date_create(test);
	printf( "MAIN: Day: %d Month: %d Year: %d\n", test2->day,  test2->month, test2->year);
	strcpy(test, "14/03/1969");
	Date * test3 = date_create(test);
	printf( "MAIN: Day: %d Month: %d Year: %d\n", test3->day,  test3->month, test3->year);
	int comp = date_compare(test2, test3);
	printf("smaller is %d\n", comp);
	comp = date_compare(test3, test2);
	printf("bigger is %d\n", comp);
	date_destroy(test2);
	date_destroy(test3);
	free(test);


	return 0;
}
*/