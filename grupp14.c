
#include "grupp14.h"

nrfx_uarte_t instance = NRFX_UARTE_INSTANCE(0);

// RTC instance to measure random seed
nrfx_rtc_t rtc_instance = NRFX_RTC_INSTANCE(0);


char uarte_buffer; // Temporary char buffer for low-level reads
char input_buffer[MAX_LENGTH]; // Full string buffer for integer input

































/*************** Linked List functions ****************/


//Returnera en tom lista - funktionen är färdig
List create_empty_list(void)
{
    return NULL;
}

//Här skapas nya noder. Denna funktion är den enda som ska använda malloc.
//Denna funktion ska i sin tur bara kallas av add_first och add_last.
//Notera att den är static och inte finns med i interfacet
static Node * create_list_node(const Data data)
{
	Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL; // Kan också skriva ut ett felmeddelande
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
    
    //glöm inte att kolla så att malloc inte returnerade NULL
}

//Är listan tom?
//Returnerar 1 om listan är tom, annars 0
int is_empty(const List list)
{
if (list == NULL)
    return 1;
else 
    return 0;
}

//Lägg till en nod först i listan
void add_first(List *list, const Data data)
{
Node* new_node = create_list_node(data);
    if (new_node == NULL) 
        return;

    new_node->next = *list; // Point the new node's next to what the current head is pointing to

    *list = new_node; // Update the head to point to the new node
}

//lägg till nod sist i listan
void add_last(List *list, const Data data)
{
if (*list == NULL) { // Check if this is the last node
        *list = create_list_node(data);
        return;
    }
    add_last(&(*list)->next, data); // go the the next node and check if that is the last node.
}

//Ta bort första noden i listan
//precondition: listan är inte tom (testa med assert)
void remove_first(List *list)
{
 assert(!is_empty(*list)); //Make sure the list is not empty

    Node *temp = *list; // Save the current first node (head)
    *list = temp->next;// Move head to the next node
    free(temp);
}

//ta bort sista noden i listan
//precondition: listan är inte tom (testa med assert)
void remove_last(List *list)
{
assert(!is_empty(*list));

    if ((*list)->next == NULL) { // If the next node is NULL remove it.
        free(*list);
        *list = NULL;
    } else {
        remove_last(&(*list)->next); // Go next node to find the last node
    }
}

//töm listan (ta bort alla noder ur listan)
//postcondition: Listan är tom, *list är NULL
void clear_list(List *list)
{
	while (!is_empty(*list)) { // If not empty 
        remove_first(list);    // Then remove the first element
    }
    //alla noder ska frigöras
}

//Skriv ut listan genom UART
void print_list(const List list)
{
const Node* temp = list;
    while (temp != NULL) {
        char buffer[32];
        sprintf(buffer, "%d  ", temp->data);
        uarte_write(buffer, strlen(buffer));
        temp = temp->next;
}
}
//returnera första datat i listan
//precondition: listan är inte tom (testa med assert)

Data get_first_element(const List list)
{
 assert(!is_empty(list));
    return list->data;
}

//returnera sista datat i listan. 
//precondition: listan är inte tom (testa med assert)
Data get_last_element(const List list)
{
assert(!is_empty(list));
    const Node *temp = list;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp->data;
}

//Returnera hur många noder som finns i listan
int number_of_nodes(const List list)
{
    if (list == NULL) 
        return 0;

    return 1 + number_of_nodes(list->next);
}

//Sök efter data i listan, returnera 1 om datat finns, annars 0.
int search(const List list, const Data data)
{
	if (list == NULL) return 0;
    if (list->data == data) return 1;
    return search(list->next, data);
}

//Ta bort data ur listan (första förekomsten), returnera 0 om datat inte finns, annars 1
int remove_element(List *list, const Data data)
{
 if (*list == NULL) return 0;

    if ((*list)->data == data) {
        Node *temp = *list; // Save the node to delete
        *list = temp->next; // Move head/next pointer past this node
        free(temp);
        return 1;
    }
    return remove_element(&(*list)->next, data);
}

void run_button_logic(List queue, List stack) {
    char buffer[100];
    char msg1[] = "Even => pushed to stack\n\r";
    char msg2[] = "Odd => enqueued to queue\n\r";
    char msg3[] = "Queue is empty\n\r";


    // Button states (previous and current) 1 = off
    int button1_prev = 1; 
    int button2_prev = 1;
    int button3_prev = 1;

    // track if any action happened
    int action_taken = 0;

    while (1) {
        // Read button states (current)
        int button1_now = nrf_gpio_pin_read(BUTTON_1);
        int button2_now = nrf_gpio_pin_read(BUTTON_2);
        int button3_now = nrf_gpio_pin_read(BUTTON_3);

        // Check for button 1 press (button 1 was just pressed)
        if (button1_prev == 1 && button1_now == 0) { 
            // Button 1 was just pressed
            int n = getRand() % 100;
            sprintf(buffer, "Generated number: %d\n\r", n);
            uarte_write(buffer, strlen(buffer));

            if (n % 2 == 0) {
                add_first(&stack, n);
                uarte_write(msg1, strlen(msg1));
            } else {
                add_last(&queue, n);
                uarte_write(msg2, strlen(msg2));
            }

            action_taken = 1; // Set action_taken to 1, because a change happened
        }

        // Check for button 2 press (button 2 was just pressed)
        if (button2_prev == 1 && button2_now == 0) { // Rising edge detection
            // Button 2 was just pressed
            if (!is_empty(stack)) {
                int popped = get_first_element(stack);
                remove_first(&stack);
                sprintf(buffer, "Popped from stack: %d\n\r", popped);
                uarte_write(buffer, strlen(buffer));
            } else {
                uarte_write("Stack is empty\n\r", 18);
            }

            action_taken = 1; // Set action_taken to 1, because a change happened
        }

        // Check for button 3 press (button 3 was just pressed)
        if (button3_prev == 1 && button3_now == 0) {
            // Button 3 was just pressed
            if (!is_empty(queue)) {
                int dequeued = get_first_element(queue);
                remove_first(&queue);
                sprintf(buffer, "Dequeued from queue: %d\n\r", dequeued);
                uarte_write(buffer, strlen(buffer));
            } else {
                uarte_write("Queue is empty\n\r", 18);
            }

            action_taken = 1; // Set action_taken to 1, because a change happened
        }

        // Update previous button states for the next loop
        button1_prev = button1_now;
        button2_prev = button2_now;
        button3_prev = button3_now;

        // Only print queue/stack info if any action (button press) occurred
        if (action_taken) 
        {
            if (!is_empty(queue)) {
                sprintf(buffer, "Front of queue: %d\n\r", get_first_element(queue));
                uarte_write(buffer, strlen(buffer));
            } else {
                uarte_write(msg3, strlen(msg3));
            }

            if (!is_empty(stack)) {
                sprintf(buffer, "Top of stack: %d\n\r", get_first_element(stack));
                uarte_write(buffer, strlen(buffer));
            } else {
                uarte_write(msg3, strlen(msg3));
            }

            // Reset action taken
            action_taken = 0;
            nrfx_systick_delay_ms(1000);
        }
    }
}















/*************** Binary Tree functions ****************/
/*

*/






int count_nodes(const BSTree tree) {
    if (tree == NULL)
        return 0;
    return 1 + count_nodes(tree->left) + count_nodes(tree->right);
}


BSTree create_tree_node(int data) {
    BSTree newNode = (BSTree)malloc(sizeof(struct treeNode));
    if (newNode != NULL) {
        newNode->data = data;
        newNode->left = NULL;
        newNode->right = NULL;
    }
    return newNode;
}

BSTree create_empty_tree(void) {
    return NULL;
}

int is_empty_tree(const BSTree tree) {
    return tree == NULL;
}

void insert_sorted(BSTree* tree, const int data) {
    if (*tree == NULL) {
        *tree = create_tree_node(data);
        return;
    }
    if (data < (*tree)->data) {
        insert_sorted(&((*tree)->left), data);
    } else if (data > (*tree)->data) {
        insert_sorted(&((*tree)->right), data);
    }
}







void inorder_traversal_recursive(BSTree root) {
if (root == NULL) return;

inorder_traversal_recursive(root->left);
uarte_write_int(root->data);
inorder_traversal_recursive(root->right);
}

void print_inorder(BSTree root) {
if (root == NULL) {
    char buff[] = "The tree is empty\n\r";
    uarte_write(buff, strlen(buff));
    return;
}
inorder_traversal_recursive(root);
}



void preorder_traversal_recursive(const BSTree tree) {
    if (tree == NULL)
        return;

    uarte_write_int(tree->data);
    preorder_traversal_recursive(tree->left);
    preorder_traversal_recursive(tree->right);
}

void print_preorder(const BSTree tree) {
    if (tree == NULL) {
    char buff[] = "The tree is empty\n\r";
    uarte_write(buff, strlen(buff));
    return;
    }
    preorder_traversal_recursive(tree);
}




void postorder_traversal_recursive(const BSTree tree) {
    if (tree == NULL)
        return;

    postorder_traversal_recursive(tree->left);
    postorder_traversal_recursive(tree->right);

    uarte_write_int(tree->data);
}

void print_postorder(const BSTree tree) {
    if (tree == NULL) {
    char buff[] = "The tree is empty\n\r";
    uarte_write(buff, strlen(buff));
    return;
    }
    postorder_traversal_recursive(tree);
}






int find(const BSTree tree, int data) {
    if (tree == NULL) return 0;
    if (data == tree->data) return 1;
    if (data < tree->data) return find(tree->left, data);
    else return find(tree->right, data);
}

void remove_element_tree(BSTree* tree, int data) {
    if (*tree == NULL) return;
    if (data < (*tree)->data) {
        remove_element_tree(&(*tree)->left, data);
    } else if (data > (*tree)->data) {
        remove_element_tree(&(*tree)->right, data);
    } else {
        if ((*tree)->left == NULL && (*tree)->right == NULL) {
            free(*tree);
            *tree = NULL;
        } else if ((*tree)->left == NULL) {
            struct treeNode* temp = *tree;
            *tree = (*tree)->right;
            free(temp);
        } else if ((*tree)->right == NULL) {
            struct treeNode* temp = *tree;
            *tree = (*tree)->left;
            free(temp);
        } else {
            struct treeNode* successor = (*tree)->right;
            while (successor->left)
                successor = successor->left;
            (*tree)->data = successor->data;
            remove_element_tree(&(*tree)->right, successor->data);
        }
    }
}

int depth(const BSTree tree) {
    if (tree == NULL) return 0;
    int left = depth(tree->left);
    int right = depth(tree->right);
    return 1 + (left > right ? left : right);
}

int min_depth(const BSTree tree) {
    int n = count_nodes(tree);
    if (n == 0) return 0;
    return (int)ceil(log2(n + 1));
}



void write_to_array(const BSTree tree, int* array, int* index) {
    if (tree == NULL) return;
    write_to_array(tree->left, array, index);
    array[(*index)++] = tree->data;
    write_to_array(tree->right, array, index);
}

int* write_sorted_to_array(const BSTree tree) {
    int count = count_nodes(tree);
    int* array = (int*)malloc(sizeof(int) * count);
    int index = 0;
    write_to_array(tree, array, &index);
    return array;
}

void build_tree_sorted_from_array(BSTree* tree, const int arr[], int start, int end) {
    if (start > end) return;
    int mid = (start + end) / 2;
    insert_sorted(tree, arr[mid]);
    build_tree_sorted_from_array(tree, arr, start, mid - 1);
    build_tree_sorted_from_array(tree, arr, mid + 1, end);
}

void balance_tree(BSTree* tree) {
    int* sorted_data = write_sorted_to_array(*tree);
    int size = count_nodes(*tree);
    free_tree(tree);
    build_tree_sorted_from_array(tree, sorted_data, 0, size - 1);
    free(sorted_data);
}

void free_tree(BSTree* tree) {
    if (*tree == NULL) return;
    free_tree(&(*tree)->left);
    free_tree(&(*tree)->right);
    free(*tree);
    *tree = NULL;
}













int is_sorted_array(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0; // Not sorted
        }
    }
    return 1; // Sorted
}




int is_sorted_list(const List list) {
    if (list == NULL || list->next == NULL)
        return 1; // Empty or one node is sorted

    const Node* current = list;
    while (current->next != NULL) {
        if (current->data > current->next->data)
            return 0; // Not sorted
        current = current->next;
    }
    return 1; // Sorted
}


void fill_random_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = getRand();
    }
}


void fill_random_list(List *list, int size) {
    for (int i = 0; i < size; i++) {
        int r = getRand();
        add_last(list, r);
    }
}

void bubble_sort_list(List *head) {
    if (head == NULL || *head == NULL || (*head)->next == NULL) return;

    // -------- Skip first N nodes --------
    //head = &((*head)->next->next); // Add the amount ->next to skip that amount 
 

    int swapped;
    Node** ptr;
    do {
        swapped = 0;
        ptr = head;

        while ((*ptr)->next != NULL) {
            Node* first = *ptr;
            Node* second = first->next;

            // -------- Skip last N nodes --------
            /*
             int remaining = 0;
             Node* temp = second;
             while (temp != NULL) {
                 temp = temp->next;
                 remaining++;
             }
             if (remaining < 4) break;  // Skip last 3 nodes
            */
            
            if (first->data > second->data) { // Small to big >, big to small <
                // Swap nodes
                first->next = second->next;
                second->next = first;
                *ptr = second;
                swapped = 1;
            }
            ptr = &((*ptr)->next);
        }
    } while (swapped);
}






void merge(int arr[], int left, int mid, int right) {
    // How big is the array
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Skapa temporära arrayer
    int* L = (int*)malloc(n1 * sizeof(int));
    int* R = (int*)malloc(n2 * sizeof(int));

    // Kopiera data till temporära arrayer
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Slå ihop de temporära arrayerna
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) //small to big <=, big to small >=
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    // Kopiera kvarvarande element
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];

    // Frigör minne
    free(L);
    free(R);
}

void merge_sort(int arr[], int left, int right) {

    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sortera vänstra och högra halvan
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);

        // Slå ihop sorterade halvor
        merge(arr, left, mid, right);
    }
}

void insertion_sort(int arr[], int size) {


    for (int i = 1 + 0; i < size - 0; i++) {// to skip first elements i = 1 + amount to skip. to skip last element i < size - amount to skip
        int key = arr[i]; // The element to be inserted
        int j = i - 1;

        while (j >= 0 && arr[j] < key) { // small to big >, big to small <.     j >= amount to skip first.
            arr[j + 1] = arr[j];  // Shift element to the right
            j = j - 1;
        }

        // Insert the key at its correct position
        arr[j + 1] = key;
    }
}

/*************** Other Helper functions ****************/
void initialize()
{
    // Initiera GPIO för knapp
    nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_input(BUTTON_3, NRF_GPIO_PIN_PULLUP);

    //Initierar UARTE:
    const nrfx_uarte_config_t configg = NRFX_UARTE_DEFAULT_CONFIG(PIN_TX, PIN_RX);
    nrfx_err_t errrr = nrfx_uarte_init(&instance, &configg, NULL);
    //Initiera RTC
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
    nrfx_err_t errr = nrfx_rtc_init(&rtc_instance, &config, NULL);
    nrfx_rtc_enable(&rtc_instance);
}

void initializeRTC(void){
while (nrf_gpio_pin_read(BUTTON_1) == 1)
{
    // Gör inget, bara vänta
}

// Läs av RTC-counter
int rtc_value = nrfx_rtc_counter_get(&rtc_instance)/32.768f; //Divide to get milli sekunder

// Initiera slumptalsgeneratorn med RTC-värdet
srand(rtc_value);
}

int getRand()
{
int r = rand();
return r % 100;// 0-99
}

// Send data over UART
void uarte_write(char* data, int length)
{
    nrfx_uarte_tx(&instance, data, length, 0);
}

void uarte_write_int(int value)
{
    char buffer[100];
    sprintf(buffer, "%d  ", value);
    uarte_write(buffer, strlen(buffer));
}

void print_array(int arr[], int size) {
    char buffer[50];
    for (int i = 0; i < size; i++) {
        sprintf(buffer, "%d ", arr[i]);
        uarte_write(buffer, strlen(buffer));
    }
}

// Read a full string from UART until Enter (or carriage return)
void readString(char* str)
{
    char tempChar;
    int i = 0;

    while(i < MAX_LENGTH - 1)
    { 
        nrfx_uarte_rx(&instance, &tempChar, sizeof(tempChar));

        if(tempChar == 13 || tempChar == '\r') // Enter key
        {
            break;
        }

        str[i++] = tempChar;
    }
    str[i] = '\0'; // Null-terminate the string
}

// Convert user input to integer
int readInt()
{
    readString(input_buffer);
    return atoi(input_buffer);
}

// Main LED toggle logic
void toggleLED() 
{
    // Declare the LEDpin variable here
    char Clear_screen[] = CLEAR_SCREEN;
    int LEDstates[4] = {1, 1, 1, 1}; // Track each LED state (1 = off, 0 = on)
    int tracker = 0;
    int breakLoop = 1;
    char done[] = "It worked, the loop broke";
    char message[] = "Do you want to break from toggling? (5 for yes, 6 for NO)\n\r";

    while(breakLoop == 1) 
    {
        // Continuously check for UART input
        nrfx_err_t errr = nrfx_uarte_rx(&instance, &uarte_buffer, sizeof(uarte_buffer));

        
        // Handle the UART input based on the received character
          char LEDpin = 0;
           
         switch (uarte_buffer) 
         {
            case '1':
                    LEDpin = LED1;
                    tracker = 0;
                    break;

            case '2':
                    LEDpin = LED2;
                    tracker = 1;
                    break;

            case '3':
                    LEDpin = LED3;
                    tracker = 2;
                    break;

            case '4':
                    LEDpin = LED4;
                    tracker = 3;
                    break;
                    
            default:
                    break;

         }
            // Toggle the LED based on its current state
            if (LEDstates[tracker] == 0) // if on then turn it off
            { 
                nrf_gpio_pin_write(LEDpin, 1);
                LEDstates[tracker] = 1; // Update the states of the LED
            }
            else 
            {
                nrf_gpio_pin_write(LEDpin, 0); // Turn it on
                LEDstates[tracker] = 0;
            }
        
    }
}