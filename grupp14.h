#ifndef grupp14
#define grupp14

#include <nrfx.h>
#include <nrf5340_application.h>
#include <nrfx_config.h>
#include <nrf.h>
#include <nrf_gpio.h>
#include <nrfx_uarte.h>
#include <nrfx_systick.h>
#include <stdlib.h>     // For atoi
#include <string.h>     // For strlen
#include <stdio.h>      // For sprintf
#include <assert.h>
#include <nrfx_rtc.h>
#include <stdbool.h>
#include <math.h>



// UART TX and RX pins for NRF5340DK
#define PIN_TX (20)
#define PIN_RX (22)

// Define Button pin for button press (using Button 1 as an example)

#define BUTTON_1 23  
#define BUTTON_2 24  
#define BUTTON_3 8
#define BUTTON_4 9  



// LED pin mappings
#define LED1 (28)
#define LED2 (29)
#define LED3 (30)
#define LED4 (31)

// Define the number of "ticks" before switching on/off LEDs
#define LED_ON_TIME 200

#define MAX_LENGTH 100
#define CLEAR_SCREEN "\033c"
 
typedef int Data;






/*************** Linked List functions ****************/


/*
//struktdefinition för enkellänkad lista
typedef struct node
{
	Data data;
	struct node* next;
}Node;


typedef Node* List; //Listan representeras av en nodpekare. 
*/

//struktdefinition för enkellänkad lista
typedef struct node
{
	Data data;
	struct node* next;
}Node;


typedef Node* List; //Listan representeras av en nodpekare. 



//Returnera en tom lista
List create_empty_list(void);

//Är listan tom?
//Returnerar 1 om listan är tom, annars 0
int is_empty(const List list);

//Lägg till en nod först i listan
void add_first(List *list, const Data data);

//Lägg till nod sist i listan
void add_last(List *list, const Data data);

//Ta bort första noden i listan
void remove_first(List *list);

//Ta bort sista noden i listan
void remove_last(List *list);

//Töm listan (ta bort alla noder ur listan)
void clear_list(List *list);

//Skriv ut listan genom UART
void print_list(const List list);

//Returnera första datat i listan
Data get_first_element(const List list);

//Returnera sista datat i listan
Data get_last_element(const List list);

//Returnera hur många noder som finns i listan
int number_of_nodes(const List list);

//Sök efter data i listan, returnera 1 om datat finns, annars 0.
int search(const List list, const Data data);

//Ta bort data ur listan (första förekomsten), returnera 0 om datat inte finns, annars 1
int remove_element(List *list, const Data data);

void run_button_logic(List queue, List stack);






/*************** Binary Tree functions ****************/

struct treeNode
{
int data;
struct treeNode* left;
struct treeNode* right;
};
typedef struct treeNode* BSTree;



BSTree create_tree_node(int data);

/* Skapar ett tomt trad */
BSTree create_empty_tree(void);
/* Returnerar 1 ifall trädet är tomt, 0 annars */
int is_empty_tree(const BSTree tree);
/* Sätter in 'data' sorterat i *tree*/
void insert_sorted(BSTree* tree, int data);

/* Utskriftsfunktioner*/
void preorder_traversal_recursive(const BSTree tree);
void print_preorder(const BSTree tree);


void inorder_traversal_recursive(BSTree root);
void print_inorder(const BSTree tree);

void print_postorder(const BSTree tree);
void postorder_traversal_recursive(const BSTree tree);



/* Returnerar 1 om 'data' finns i tree, 0 annars */
int find(const BSTree tree, int data);
/* Tar bort 'data' från trädet om det finns */
void remove_element_tree(BSTree* tree, int data);
/* Returnerar hur många noder som totalt finns i trädet */
int number_of_nodes_tree(const BSTree tree);
/* Returnerar hur djupt trädet är */
int depth(const BSTree tree);
/* Returnerar minimidjupet för trädet */
int min_depth(const BSTree tree);

void write_to_array(const BSTree tree, int* array, int* index);
int* write_sorted_to_array(const BSTree tree);
void build_tree_sorted_from_array(BSTree* tree, const int arr[], int start, int end);

/* Balansera trädet sa att depth(tree) == min_depth(tree) */
void balance_tree(BSTree* tree);
/* Töm trädet och frigör minnet för de olika noderna */
void free_tree(BSTree* tree);

int count_nodes(const BSTree tree);


int is_sorted_array(int arr[], int size);
int is_sorted_list(const List list);
void fill_random_array(int arr[], int size);
void fill_random_list(List *list, int size);

void bubble_sort_list(List *head);
void merge_sort(int arr[], int left, int right);
void merge(int arr[], int left, int mid, int right);
void insertion_sort(int arr[], int size);




/*************** Other Helper functions ****************/
int getRand();
void initialize();
void uarte_write(char* data, int length);
void uarte_write_int(int value);
void print_array(int arr[], int size);
void readString(char* str);
int readInt();
void toggleLED();





// Globala variabler (tillgängliga i andra filer)
extern nrfx_uarte_t instance;
extern nrfx_rtc_t rtc_instance;
extern char uarte_buffer;
extern char input_buffer[MAX_LENGTH];

#endif 