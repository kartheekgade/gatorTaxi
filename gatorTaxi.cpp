#include <limits>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <sstream>
#include <vector>
using namespace std;

// Create a structure for data with {id,cost,duration} of ride
struct ride
{
    int id;
    int cost;
    int duration;

};
//Create a heap structure  
struct rideHeapNode
{
    rideHeapNode* left;
    rideHeapNode* right;
    rideHeapNode* parent;
    ride data;
};

//Create a Red-Black tree structure
struct rideRBNode
{
    rideRBNode* left;
    rideRBNode* right;
    rideRBNode* parent;
    ride data;
    int color; // '0-Red' '1-Black'
};
typedef rideHeapNode* rideHeapNodePtr;
typedef rideRBNode* rideRBNodePtr;
rideHeapNodePtr hashIndex[2000];
int heapSize;
class Ride {
    rideHeapNode* heapHead;
    rideRBNode* rbHead;
public:
    Ride();
    void insertHeap(int id, int cost, int duration);
    ride topRide();
    void removeHeap(int id);
    void insertRB(int id, int cost, int duration);
    void insertRBfix(rideRBNodePtr t);
    void leftrotateRB(rideRBNodePtr t);
    void rightrotateRB(rideRBNodePtr t);
    ride deleteRB(int id);
    void deleteRB_nodefix(rideRBNodePtr t);
    ride searchRB(int id);
    void updateCostRB(int id, int new_duration);
    rideRBNodePtr successor(rideRBNodePtr temp_node);
    void heapifyDown(rideHeapNodePtr head);
    string rangeQueryUntil(int id1, int id2);
    void updateHeap(int id, int new_duration);
    void processInputFile(const string& file_name);
};
Ride::Ride() {
    heapHead = NULL;
    rbHead = NULL;
}
// -------- Heap operations --------  

void Ride::heapifyDown(rideHeapNodePtr head) {
    if (head == nullptr) {
        return;
    }
    rideHeapNodePtr itr = head;
    while (itr != nullptr)
    {
        if (itr->left != nullptr && itr->data.cost > itr->left->data.cost) {
            swap(hashIndex[itr->data.id], hashIndex[itr->left->data.id]);
            swap(itr->data, itr->left->data);
        }

        if (itr->right != nullptr && itr->data.cost > itr->right->data.cost)
        {
            swap(hashIndex[itr->data.id], hashIndex[itr->right->data.id]);
            swap(itr->data, itr->right->data);
        }
        itr = itr->left;
    }
}

//To get to ride with minimum cost and pop the ride
ride Ride::topRide() {
    struct ride topRide;
    if (heapHead == NULL) {
        topRide = { 0,0,0 };
    }
    else if (heapHead->left == nullptr && heapHead->right == nullptr)
    {
        topRide = heapHead->data;
        delete heapHead;
        heapHead = nullptr;
        deleteRB(topRide.id);
        heapSize = 0;
    }
    else if (heapHead->right == nullptr)
    {
        topRide = heapHead->data;
        rideHeapNodePtr t = heapHead;
        heapHead = heapHead->left;
        delete t;
        heapifyDown(heapHead);
        deleteRB(topRide.id);
        heapSize--;
    }
    else {
        topRide = heapHead->data;
        rideHeapNodePtr itr = heapHead;
        while (itr->right != nullptr)
            itr = itr->right;
        swap(itr->data, heapHead->data); //swap the pointer
        itr = itr->parent;
        rideHeapNodePtr t = itr->right;
        itr->right = nullptr;
        delete t;
        heapifyDown(heapHead);
        deleteRB(topRide.id);
        heapSize--;
    }
    hashIndex[topRide.id] = NULL;
    return topRide;
    //std::cout << "(" << topRide.id << ", " << topRide.cost << ", " << topRide.duration << ")";
}
//To insert the ride into the min heap and update the node pointer in heapIndex
void Ride::insertHeap(int rideNumber, int rideCost, int rideDuration) {
    if (heapHead == nullptr) {
        heapHead = new rideHeapNode;
        heapHead->data.id = rideNumber;
        heapHead->data.cost = rideCost;
        heapHead->data.duration = rideDuration;
        heapHead->parent = nullptr;
        heapHead->left = nullptr;
        heapHead->right = nullptr;
        hashIndex[rideNumber] = heapHead; //Update the node pointer in heapIndex
        heapSize++;
    }
    else {
        rideHeapNodePtr temp_parent = heapHead; // free postion for parent node
        rideHeapNodePtr temp = new rideHeapNode;
        ride rec;
        //find the parent of a free position and then insert value d to this positon(a node that that has less than 2 childrend.)
        while (temp_parent->left != NULL && temp_parent->right != NULL) {
            if (rand() % 2 == 0)
            {
                temp_parent = temp_parent->left;
            }
            else
            {
                temp_parent = temp_parent->right;
            }
        }
        //if left child is NULL then insert to left child, else insert to right child.
        if (temp_parent->left == NULL) {
            temp_parent->left = temp;
            hashIndex[rideNumber] = temp;
            heapSize++;
        }
        else {
            temp_parent->right = temp;
            hashIndex[rideNumber] = temp;
            heapSize++;
        }
        //insert the new value to the position temp.
        temp->parent = temp_parent;
        temp->data.id = rideNumber;
        temp->data.cost = rideCost;
        temp->data.duration = rideDuration;
        temp->left = NULL;
        temp->right = NULL;

        while (temp_parent != NULL && temp_parent->data.cost >= temp->data.cost) {
            if (temp_parent->data.cost == temp->data.cost && temp_parent->data.duration > temp->data.duration) {
                //swap the data
                swap(hashIndex[temp->data.id], hashIndex[temp_parent->data.id]);
                rec = temp_parent->data;
                temp_parent->data = temp->data;
                temp->data = rec;
                //set to one level higher
                temp = temp_parent;
                temp_parent = temp->parent;

            }

            else if (temp_parent->data.cost == temp->data.cost && temp_parent->data.duration < temp->data.duration)
            {
                break;
            }

            else {
                //swap the data
                swap(hashIndex[temp->data.id], hashIndex[temp_parent->data.id]);
                rec = temp_parent->data;
                temp_parent->data = temp->data;
                temp->data = rec;
                //set to one level higher
                temp = temp_parent;
                temp_parent = temp->parent;
            }
        }

    }

}
void Ride::updateHeap(int id, int new_duration)
{
    // Get the rideHeapNode pointer from hashIndex using the given id
    rideHeapNodePtr node = hashIndex[id];

    // Update the cost of the ride if between duration and 2* duration
    if (new_duration > node->data.cost && new_duration <= node->data.cost) {
        node->data.cost = node->data.cost + 10;
        node->data.duration = new_duration;
        // Maintain the min heap property by swapping the ride with its parent
        while (node->parent != nullptr && node->data.cost < node->parent->data.cost)
        {
            // Swap the ride and its parent
            rideHeapNodePtr parent = node->parent;
            rideHeapNodePtr grandparent = parent->parent;
            if (grandparent != nullptr)
            {
                if (grandparent->left == parent)
                    grandparent->left = node;
                else
                    grandparent->right = node;
            }
            else
            {
                // If the node is the root, update the root of the heap in hashIndex
                hashIndex[node->data.id] = node;
            }
            node->parent = grandparent;
            if (parent->left == node)
            {
                parent->left = node->left;
                if (node->left != nullptr)
                    node->left->parent = parent;
                node->left = parent;
            }
            else
            {
                parent->right = node->right;
                if (node->right != nullptr)
                    node->right->parent = parent;
                node->right = parent;
            }
            parent->parent = node;
            node = parent;
        }
    }
    //If greater than 2*duration remove the node from heap
    else if (new_duration > 2 * node->data.duration) {
        removeHeap(id);
    }
    else {
        node->data.duration = new_duration;
    }

}
//Remove a node from the min heap using heapIndex
void Ride::removeHeap(int id) {
    if (heapHead->data.id == id) {
        hashIndex[id] = NULL;
        if (heapHead->left == nullptr && heapHead->right == nullptr)
        {
            delete heapHead; //Delete node
            heapHead = nullptr;
            heapSize = 0;
            return;
        }
        //If min node right child is null than assign head to left child and delete
        if (heapHead->right == nullptr)
        {
            rideHeapNodePtr t = heapHead;
            heapHead = heapHead->left;
            delete t;
            heapifyDown(heapHead); // heapify the head node
            heapSize--;
            return;
        }
        // else for any chase take a right most child and swap
        else {
            rideHeapNodePtr itr = heapHead;
            while (itr->right != nullptr)
                itr = itr->right;
            swap(itr->data, heapHead->data);
            itr = itr->parent;
            rideHeapNodePtr t = itr->right;
            itr->right = nullptr;
            delete t;                 //After swaping with rightmost child delete node
            heapifyDown(heapHead);
            heapSize--;
            return;
        }
    }
    else
    {

        rideHeapNodePtr nodeToRemove = nullptr;
        nodeToRemove = hashIndex[id];

        if (nodeToRemove == nullptr)
        {
            return;
        }

        // Remove the node from the hash index
        int hash = id;

        hashIndex[hash] = nullptr;

        // Swap the node to be removed with the last node in the min heap
        swap(nodeToRemove, hashIndex[heapSize - 1]);

        // Delete the node
        delete nodeToRemove;

        // Restore the heap property after deletion
        heapSize--;
        heapifyDown(hashIndex[0]);
        return;
    }
}

void Ride::insertRB(int id, int cost, int duration) {
    rideRBNodePtr temp_node, temp_node1; // Declaring two temporary node pointers
    rideRBNodePtr t = new rideRBNode; // Creating a new node with dynamic memory allocation
    t->data.id = id; // Assigning the id value to the data member of the new node
    t->data.cost = cost; // Assigning the cost value to the data member of the new node
    t->data.duration = duration; // Assigning the duration value to the data member of the new node
    t->left = NULL; // Initializing the left child of the new node to NULL
    t->right = NULL; // Initializing the right child of the new node to NULL
    t->color = 0; // Initializing the color of the new node to 0 (assuming black)

    temp_node = rbHead; // Assigning the head of the RB tree to a temporary node pointer
    temp_node1 = NULL; // Initializing another temporary node pointer to NULL

    if (rbHead == NULL) // If the RB tree is empty
    {
        rbHead = t; // Make the new node as the head of the RB tree
        t->parent = NULL; // Set the parent of the new node to NULL as it is the root node
    }
    else
    {
        while (temp_node != NULL) // Traverse the RB tree until we find the appropriate position for the new node
        {
            temp_node1 = temp_node; // Store the current node as previous node
            if (temp_node->data.id < t->data.id) // If the id of current node is less than the id of new node
                temp_node = temp_node->right; // Move to the right child
            else
                temp_node = temp_node->left; // Otherwise, move to the left child
        }
        t->parent = temp_node1; // Set the parent of the new node to the previous node
        if (temp_node1->data.id < t->data.id) // If the id of previous node is less than the id of new node
            temp_node1->right = t; // Make the new node as the right child of previous node
        else
            temp_node1->left = t; // Otherwise, make the new node as the left child of previous node
    }
    insertRBfix(t); // Call a function to fix the Red-Black tree properties after inserting the new node
}
void Ride::insertRBfix(rideRBNodePtr t)
{
    rideRBNodePtr u;
    if (rbHead == t)
    {
        t->color = 1; // Set the color of the root node to black
        return;
    }
    while (t->parent != NULL && t->parent->color == 0)
    {
        rideRBNodePtr gp = t->parent->parent; // Get the grandparent of the current node
        if (gp->left == t->parent) // If the parent of the current node is the left child of the grandparent
        {
            if (gp->right != NULL) // If the right child of the grandparent exists
            {
                u = gp->right; // Set u to be the right child of the grandparent
                if (u->color == 0) // If u is red
                {
                    t->parent->color = 1; // Set the color of the parent to black
                    u->color = 1; // Set the color of u to black
                    gp->color = 0; // Set the color of the grandparent to red
                    t = gp; // Move t to the grandparent for further fixing
                }
            }
            else
            {
                if (t->parent->right == t)
                {
                    t = t->parent;
                    leftrotateRB(t); // Perform a left rotation on the parent of the current node
                }
                t->parent->color = 1; // Set the color of the parent to black
                gp->color = 0; // Set the color of the grandparent to red
                rightrotateRB(gp); // Perform a right rotation on the grandparent
            }
        }
        else
        {
            if (gp->left != NULL)
            {
                u = gp->left; // Set u to be the left child of the grandparent
                if (u->color == 0) // If u is red
                {
                    t->parent->color = 1; // Set the color of the parent to black
                    u->color = 1; // Set the color of u to black
                    gp->color = 0; // Set the color of the grandparent to red
                    t = gp; // Move t to the grandparent for further fixing
                }
            }
            else
            {
                if (t->parent->left == t)
                {
                    t = t->parent;
                    rightrotateRB(t); // Perform a right rotation on the parent of the current node
                }
                t->parent->color = 1; // Set the color of the parent to black
                gp->color = 0; // Set the color of the grandparent to red
                leftrotateRB(gp); // Perform a left rotation on the grandparent
            }
        }
        rbHead->color = 1; // Set the color of the root node to black
    }
}


ride Ride::deleteRB(int id)
{
    rideRBNodePtr temp_node; // Pointer to traverse the red-black tree
    temp_node = rbHead; // Start from the root of the tree
    rideRBNodePtr y = NULL; // Node to be deleted
    rideRBNodePtr temp_node1 = NULL; // Child of the node to be deleted
    int found = 0; // Flag to indicate if the node with the given id was found
    struct ride rideDel; // Struct to store the ride data to be deleted
    rideDel.id = 0; // Initialize rideDel with default values of 0 for id, cost, and duration
    rideDel.cost = 0;
    rideDel.duration = 0;

    if (rbHead == NULL)
    {
        return rideDel; // If the red-black tree is empty, return the default rideDel indicating ride not found
    }

    // Search for the node with the given id in the red-black tree
    while (temp_node != NULL && found == 0)
    {
        if (temp_node->data.id == id)
            found = 1;  // If node with given id is found, set found flag to 1
        if (found == 0)
        {
            if (temp_node->data.id < id) temp_node = temp_node->right;  // If id is greater, move to right child
            else
                temp_node = temp_node->left;  // If id is smaller, move to left child
        }
    }
    if (found == 0)
    {
        return rideDel; // If node with given id was not found, return the default rideDel indicating ride not found
    }
    else
    {
        rideDel = temp_node->data; // Store the ride data of the node to be deleted in rideDel
        if (temp_node->left == NULL || temp_node->right == NULL)
            y = temp_node; // If node to be deleted has at most one child, set y to the node to be deleted
        else
            y = successor(temp_node); // If node to be deleted has two children, set y to its successor node
        if (y->left != NULL)
            temp_node1 = y->left; // Set temp_node1 to the child of the node to be deleted (y)
        else
        {
            if (y->right != NULL)
                temp_node1 = y->right; // Set temp_node1 to the child of the node to be deleted (y)
            else
                temp_node1 = NULL;  // If y has no children, set temp_node1 to NULL
        }
        if (temp_node1 != NULL)
            temp_node1->parent = y->parent;  // Update the parent of temp_node1 to be the parent of y
        if (y->parent == NULL)
            rbHead = temp_node1; // If y is the root, update the root of the tree to temp_node1
        else
        {
            if (y == y->parent->left)
                y->parent->left = temp_node1;  // If y is a left child, update the left child of y's parent to temp_node1
            else
                y->parent->right = temp_node1;  // If y is a right child, update the right child of y's parent to temp_node1
        }
        if (y != temp_node)
        {
            temp_node->color = y->color;  // If y is not the node

            temp_node->data = y->data;  // If y is not the node to be deleted, update the data of temp_node with y's data

        }
        if (y->color == 1 && temp_node1 != nullptr)
            deleteRB_nodefix(temp_node1);  // If y was a black node, perform fixup to maintain red-black tree properties
    }
    return rideDel;
}

// To fix the Red-Black tree after deletion of node
void Ride::deleteRB_nodefix(rideRBNodePtr temp_node)
{
    rideRBNodePtr s;
    while (temp_node != rbHead && temp_node->color == 1)
    {
        if (temp_node->parent->left == temp_node)
        {
            s = temp_node->parent->right;

            // Case 1: Right sibling is red
            if (s->color == 0)
            {
                s->color = 1; // Set sibling's color to black
                temp_node->parent->color = 0; // Set parent's color to red
                leftrotateRB(temp_node->parent); // Perform left rotation on parent
                s = temp_node->parent->right; // Update sibling to right child of parent
            }

            // Case 2: Both children of sibling are black
            if (s->right->color == 1 && s->left->color == 1)
            {
                s->color = 0; // Set sibling's color to red
                temp_node = temp_node->parent; // Move up to parent
            }
            else
            {
                // Case 3: Right child of sibling is black and left child is red
                if (s->right->color == 1)
                {
                    s->left->color = 1; // Set sibling's left child's color to black
                    s->color = 0; // Set sibling's color to red
                    rightrotateRB(s); // Perform right rotation on sibling
                    s = temp_node->parent->right; // Update sibling to right child of parent
                }

                s->color = temp_node->parent->color; // Set sibling's color to parent's color
                temp_node->parent->color = 1; // Set parent's color to black
                s->right->color = 1; // Set sibling's right child's color to black
                leftrotateRB(temp_node->parent); // Perform left rotation on parent
                temp_node = rbHead; // Set temp_node to rbHead to exit the loop
            }
        }
        else
        {
            // Case 4: Mirror image of Case 1,2,3 for left child
            s = temp_node->parent->left;
            if (s->color == 0)
            {
                s->color = 1;
                temp_node->parent->color = 0;
                rightrotateRB(temp_node->parent);
                s = temp_node->parent->left;
            }
            if (s->left->color == 1 && s->right->color == 1)
            {
                s->color = 0;
                temp_node = temp_node->parent;
            }
            else
            {
                if (s->left->color == 1)
                {
                    s->right->color = 1;
                    s->color = 0;
                    leftrotateRB(s);
                    s = temp_node->parent->left;
                }
                s->color = temp_node->parent->color;
                temp_node->parent->color = 1;
                s->left->color = 0;
                rightrotateRB(temp_node->parent);
                temp_node = rbHead;
            }
        }
        temp_node->color = 1; // Set temp_node's color to black
        rbHead->color = 1; // Set rbHead's color to black
    }
}


void Ride::leftrotateRB(rideRBNodePtr temp_node) {
    if (temp_node->right == NULL)  // Check if the right child of temp_node is NULL
        return;  // If so, return and exit the function
    else
    {
        rideRBNodePtr y = temp_node->right;  // Store the right child of temp_node in y
        if (y->left != NULL)
        {
            temp_node->right = y->left;  // Set the right child of temp_node as the left child of y
            y->left->parent = temp_node;  // Set the parent of the left child of y as temp_node
        }
        else
            temp_node->right = NULL;  // Set the right child of temp_node as NULL
        if (temp_node->parent != NULL)
            y->parent = temp_node->parent;  // Set the parent of y as the parent of temp_node
        if (temp_node->parent == NULL)
            rbHead = y;  // If temp_node is the root, set rbHead as y
        else
        {
            if (temp_node == temp_node->parent->left)
                temp_node->parent->left = y;  // Set y as the left child of temp_node's parent
            else
                temp_node->parent->right = y;  // Set y as the right child of temp_node's parent
        }
        y->left = temp_node;  // Set temp_node as the left child of y
        temp_node->parent = y;  // Set y as the parent of temp_node
    }
}


void Ride::rightrotateRB(rideRBNodePtr temp_node) {
    if (temp_node->left == NULL)  // Check if the left child of temp_node is NULL
        return;  // If so, return and exit the function
    else
    {
        rideRBNodePtr y = temp_node->left;  // Store the left child of temp_node in y
        if (y->right != NULL)
        {
            temp_node->left = y->right;  // Set the left child of temp_node as the right child of y
            y->right->parent = temp_node;  // Set the parent of the right child of y as temp_node
        }
        else
            temp_node->left = NULL;  // Set the left child of temp_node as NULL
        if (temp_node->parent != NULL)
            y->parent = temp_node->parent;  // Set the parent of y as the parent of temp_node
        if (temp_node->parent == NULL)
            rbHead = y;  // If temp_node is the root, set rbHead as y
        else
        {
            if (temp_node == temp_node->parent->left)
                temp_node->parent->left = y;  // Set y as the left child of temp_node's parent
            else
                temp_node->parent->right = y;  // Set y as the right child of temp_node's parent
        }
        y->right = temp_node;  // Set temp_node as the right child of y
        temp_node->parent = y;  // Set y as the parent of temp_node
    }
}

rideRBNodePtr Ride::successor(rideRBNodePtr temp_node)
{
    rideRBNodePtr y = NULL;
    if (temp_node->left != NULL)
    {
        y = temp_node->left;
        while (y->right != NULL)
            y = y->right;
    }
    else
    {
        y = temp_node->right;
        while (y->left != NULL)
            y = y->left;
    }
    return y;
}

ride Ride::searchRB(int id)
{
    struct ride rideDetails;
    rideDetails.id = 0;
    rideDetails.cost = 0;
    rideDetails.duration = 0;
    if (rbHead == NULL)
    {
        return  rideDetails;
    }
    rideRBNodePtr temp_node = rbHead;
    int found = 0;
    while (temp_node != NULL && found == 0)
    {
        if (temp_node->data.id == id)
            found = 1;
        if (found == 0)
        {
            if (temp_node->data.id < id) temp_node = temp_node->right;
            else
                temp_node = temp_node->left;
        }
    }
    if (found == 0)
        return rideDetails;
    else
    {
        return rideDetails = temp_node->data;
    }
}

void Ride::updateCostRB(int id, int new_duration)
{
    struct ride rideDetails;  // Create a local struct ride variable to store ride details
    rideDetails.id = 0;  // Initialize id in rideDetails to 0
    rideDetails.cost = 0;  // Initialize cost in rideDetails to 0
    rideDetails.duration = 0;  // Initialize duration in rideDetails to 0

    if (rbHead == NULL)  // Check if the red-black tree is empty
    {
        return;  // If so, return and exit the function
    }

    rideRBNodePtr temp_node = rbHead;  // Initialize temp_node as the root of the red-black tree
    int found = 0;  // Initialize a flag variable found as 0

    while (temp_node != NULL && found == 0)  // Traverse the red-black tree until temp_node becomes NULL or found is set to 1
    {
        if (temp_node->data.id == id)  // Check if the id in the current node matches the given id
            found = 1;  // If so, set found flag as 1 to indicate id was found
        if (found == 0)
        {
            if (temp_node->data.id < id)
                temp_node = temp_node->right;  // If given id is greater, move to the right child
            else
                temp_node = temp_node->left;  // If given id is smaller, move to the left child
        }
    }

    if (found == 0)
        return;  // If id was not found, return and exit the function
    else
    {
        rideDetails = temp_node->data;  // Store the data of temp_node in rideDetails

        if (rideDetails.duration < new_duration && new_duration <= 2 * (rideDetails.duration)) {
            temp_node->data.cost = temp_node->data.cost + 10;  // Update the cost in temp_node by adding 10
            temp_node->data.duration = new_duration;  // Update the duration in temp_node with the given new_duration
        }
        else if (new_duration > 2 * (rideDetails.duration))
        {
            deleteRB(rideDetails.id);  // If new_duration is more than twice the current duration, delete the node with id
        }
        else {
            temp_node->data.duration = new_duration;  // Update the duration in temp_node with the given new_duration
        }
    }
}

string Ride::rangeQueryUntil(int id1, int id2) {
    rideRBNodePtr temp = rbHead; // Set the temp pointer to the root of the red-black tree
    bool firstRidePrinted = false; // Boolean flag to keep track if the first ride in the range has been printed to avoid printing extra commas
    bool noRidesInRange = true; // Boolean flag to keep track if there are no rides in the specified range

    string rideDetailsList = "";
    if (temp == nullptr) { // Check if the red-black tree is empty
        rideDetailsList = rideDetailsList + "{0, 0, 0}";
        return rideDetailsList;
    }

    std::stack<rideRBNodePtr> stack; // Create a stack to perform an iterative in-order traversal of the red-black tree
    rideRBNodePtr curr = temp; // Initialize curr pointer to temp (root of the red-black tree)

    while (curr != nullptr || !stack.empty()) { // Traverse the red-black tree until curr becomes nullptr and the stack is empty
        while (curr != nullptr) { // Traverse to the leftmost node of the current subtree and push nodes onto the stack
            stack.push(curr);
            curr = curr->left;
        }

        curr = stack.top(); // Pop the top node from the stack and set curr pointer to it
        stack.pop();

        if (curr->data.id >= id1 && curr->data.id <= id2) { // Check if the id of the current node is within the specified range [id1, id2]
            if (firstRidePrinted) {
                rideDetailsList = rideDetailsList + ","; // If not the first ride in the range, print a comma before printing the ride details
            }
            else {
                firstRidePrinted = true; // If it's the first ride in the range, set firstRidePrinted flag as true
            }
            rideDetailsList = rideDetailsList + "(" + to_string(curr->data.id) + ", " + to_string(curr->data.cost) + ", " + to_string(curr->data.duration) + ")"; // Print the id, cost, and duration of the current node
            noRidesInRange = false; // Set noRidesInRange flag as false to indicate that at least one ride is in the specified range
        }

        curr = curr->right; // Move to the right child of the current node
    }

    if (noRidesInRange) { // If no rides were found in the specified range
        rideDetailsList = rideDetailsList + "{0, 0, 0}"; // Print a default ride with id, cost, and duration as 0
    }
    return rideDetailsList;
}

// Input outup file processing
void Ride::processInputFile(const string& file_name) {
    ifstream file(file_name); // Open input file
    string line;
    ofstream output_file("output.txt");

    while (getline(file, line)) {
        // Split the line into words


        // Process the command based on the input format
        if (line.find("Insert") != string::npos) {
            vector<int> input_arg;
            // To find the parameters

            int leftIndex = line.find_first_of("(") + 1;
            int rightIndex = line.find_last_of(")");
            std::string paramterString(line, leftIndex, rightIndex - leftIndex);
            int start = 0; int end;

            for (size_t i = 0; i < paramterString.size(); i++) {
                if (paramterString.at(i) == ',') {
                    end = i;
                    input_arg.push_back(std::stoi(paramterString.substr(start, end)));
                    start = i + 1;
                }
                if (i == paramterString.size() - 1)
                    input_arg.push_back(std::stoi(paramterString.substr(start, i + 1)));
            }
            int id = input_arg[0];
            int cost = input_arg[1];
            int duration = input_arg[2];
            if (hashIndex[id] != nullptr) {
                output_file << "Duplicate RideNumber " << endl;
            }
            else {
                insertHeap(id, cost, duration);
                insertRB(id, cost, duration);
            }
        }
        else if (line.find("GetNextRide") != string::npos) {
            struct ride topRide = Ride::topRide();
            if (topRide.id == 0)
                output_file << "No active ride requests" << endl;
            else
                output_file << "(" << topRide.id << ", " << topRide.cost << ", " << topRide.duration << ")" << endl;

        }
        // Process the command based on the input format
        else if (line.find("Print") != string::npos) {
            vector<int> input_arg;
            // To find the parameters

            int leftIndex = line.find_first_of("(") + 1;
            int rightIndex = line.find_last_of(")");
            std::string paramterString(line, leftIndex, rightIndex - leftIndex);
            int start = 0; int end;

            for (size_t i = 0; i < paramterString.size(); i++) {
                if (paramterString.at(i) == ',') {
                    end = i;
                    input_arg.push_back(std::stoi(paramterString.substr(start, end)));
                    start = i + 1;
                }
                if (i == paramterString.size() - 1)
                    input_arg.push_back(std::stoi(paramterString.substr(start, i + 1)));
            }
            if (input_arg.size() == 2) {
                string rideList = Ride::rangeQueryUntil(input_arg[0], input_arg[1]);
                output_file << rideList << endl;
            }
            else if (input_arg.size() == 1) {
                struct ride rideDetails = Ride::searchRB(input_arg[0]);
                output_file << "(" << rideDetails.id << ", " << rideDetails.cost << ", " << rideDetails.duration << ")" << endl;
            }

        }
        else if (line.find("UpdateTrip") != string::npos) {
            vector<int> input_arg;
            // To find the parameters

            int leftIndex = line.find_first_of("(") + 1;
            int rightIndex = line.find_last_of(")");
            std::string paramterString(line, leftIndex, rightIndex - leftIndex);
            int start = 0; int end;

            for (size_t i = 0; i < paramterString.size(); i++) {
                if (paramterString.at(i) == ',') {
                    end = i;
                    input_arg.push_back(std::stoi(paramterString.substr(start, end)));
                    start = i + 1;
                }
                if (i == paramterString.size() - 1)
                    input_arg.push_back(std::stoi(paramterString.substr(start, i + 1)));
            }
            updateCostRB(input_arg[0], input_arg[1]);
            updateHeap(input_arg[0], input_arg[1]);
        }
        else if (line.find("CancelRide") != string::npos) {
            vector<int> input_arg;
            // To find the parameters

            int leftIndex = line.find_first_of("(") + 1;
            int rightIndex = line.find_last_of(")");
            std::string paramterString(line, leftIndex, rightIndex - leftIndex);
            int start = 0; int end;

            for (size_t i = 0; i < paramterString.size(); i++) {
                if (paramterString.at(i) == ',') {
                    end = i;
                    input_arg.push_back(std::stoi(paramterString.substr(start, end)));
                    start = i + 1;
                }
                if (i == paramterString.size() - 1)
                    input_arg.push_back(std::stoi(paramterString.substr(start, i + 1)));
            }
            deleteRB(input_arg[0]);
            removeHeap(input_arg[0]);
        }

    }
    file.close(); // Close input file
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " file_name" << endl;
        return 1;
    }
    Ride ride;
    ride.processInputFile(argv[1]);

    return 0;
}
