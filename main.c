
#include "grupp14.h"
#define TREE_SIZE 10
#define SIZE 10
#include "nrfx_systick.h"






int main(void) {
initialize();
initializeRTC();
nrfx_systick_init();

char msg[] = "is sorted\n\r";
char msg1[] = "is NOT sorted!\n\r";
char nyrad[] = "\n\r";

List list = create_empty_list();

fill_random_list(&list, SIZE);

int arr[SIZE];
fill_random_array(arr, SIZE);

//print_array(arr, SIZE);


print_list(list);
//uarte_write_int(list->next->next->next->next->next->next->next->next->next->data);

uarte_write(nyrad, strlen(nyrad));



if (is_sorted_array(arr, SIZE)) {
    uarte_write(msg, strlen(msg));
} else {
    uarte_write(msg1, strlen(msg1));
}
/*
if (is_sorted_list(list)) {
    uarte_write(msg, strlen(msg));
} else {
    uarte_write(msg1, strlen(msg1));
}

*/



// balance_list(&list);

nrf_systick_val_clear();                // Starta tidtagning
int pretime = nrf_systick_val_get();

bubble_sort_list(&list);
//merge_sort(arr, 0, SIZE - 1 - 2); // (arr, how many to skip first, SIZE - 1 - how many to skip last)
//insertion_sort(arr, SIZE);

int posttime = nrf_systick_val_get();   // Stoppa tidtagning

int ticks = pretime - posttime;


char buf[100];
sprintf(buf, "%d ticks\n\r", ticks);
uarte_write(buf, strlen(buf));

//print_array(arr, SIZE);

print_list(list);

if (is_sorted_array(arr, SIZE)) {
    uarte_write(msg, strlen(msg));
} else {
    uarte_write(msg1, strlen(msg1));
}



/*
if (is_sorted_list(list)) {
    uarte_write(msg, strlen(msg));
} else {
    uarte_write(msg1, strlen(msg1));
}
*/

    while (1) {}


    return 0;
}



/*
    int arr[SIZE];
    fill_random_array(arr, SIZE);
    print_array(arr, SIZE);
    uarte_write(msg4, strlen(msg4));
    int sorted_arr = is_sorted_array(arr, SIZE);


    if (sorted_arr)
        uarte_write(msg, strlen(msg));
    else
        uarte_write(msg1, strlen(msg1));

*/