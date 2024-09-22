#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

const int MAXIMUM = 4;
const int MINIMUM = 2;

class BTree {
    struct Node {
        string artist;
        string track;
        int year;
        string genre;
        struct Node* parent;
        string element[MAXIMUM];
        struct Node* child[MAXIMUM];
    };
    using NODE = Node*;
    NODE root;

public:
    // BTree code unfinished - is not implemented in the final version of the code, but kept anyways for documentation.
    BTree() {
        root = createNode();
    }
    void readFile() {
        ifstream myFile;
        myFile.open("musicdata.txt");

        string firstLine = "";
        if (myFile.is_open()) {
            getline(myFile, firstLine);
            while (myFile.good()) {
                // Artist name
                string artistName = "";
                getline(myFile, artistName, '\t');
                // Track name
                string trackName = "";
                getline(myFile, trackName, '\t');
                // Year
                string releaseYear = "";
                getline(myFile, releaseYear, '\t');
                // Genre
                string genreName = "";
                getline(myFile, genreName);
                if (isdigit(releaseYear[0])) {
                    insertElement(artistName, trackName, stoi(releaseYear), genreName);
                }
            }
        }
    }
    // finds if key is present
    void search(string track) {
        NODE current = findLeaf(root, track);
        if (current->track == "") {
            cout << "No match to the inputted song found!" << endl;
        }
        else {
            cout << "Artist: " << current->artist << ", Track: " << current->track << ", Release Year: "
                 << current->year << ", Genre: " << current->genre << endl;
        }
        return;
    }
    // creates new node where elements are set to -1 and child to null
    static NODE createNode() {
        NODE current = new struct Node;
        for (int i = 0; i < MAXIMUM; i++) {
            current->element[i] = "";
            current->child[i] = NULL;
        }
        current->parent = NULL;
        return current;
    }
    // sorts elements in ascending order
    static void sortNode(NODE n) {
        string currTrack;
        NODE currChild;
        for (int i = 0; i < MAXIMUM && n->element[i] != ""; i++) {
            for (int j = i + 1; j< MAXIMUM && n->element[j] != ""; j++) {
                if (n->element[i] > n->element[j]) {
                    currTrack = n->element[i];
                    currChild = n->child[i];

                    n->element[i] = n->element[j];
                    n->child[i] = n->child[j];

                    n->element[j] = currTrack;
                    n->child[j] = currChild;
                }
            }
        }
    }
    // recursively searches the tree for the node where the key should be inserted
    NODE findLeaf(NODE n, string track) {
        if (n->child[0] == NULL) {
            for (string s : n->element)
                if (track == s) {
                    return NULL;
                }
            return n;
        }
        for (int i = 0; i < MAXIMUM; i++) {
            if (n->element[i] >= track) {
                return findLeaf(n->child[i], track);
            }
            if (n->element[i] == "") {
                return findLeaf(n->child[i - 1], track);
            }
        }
        return findLeaf(n->child[MAXIMUM - 1], track);
    }

    // creates new node
    NODE newRootNode(NODE n, NODE current) {
        NODE root = createNode();

        root->element[0] = n->element[MINIMUM - 1];
        root->child[0] = n;
        root->element[1] = n->element[MAXIMUM-1];
        root->child[1] = current;

        n->parent = root;
        current->parent = root;

        return root;
    }
    // takes a node and adds to parent
    void appendToParent(NODE n, NODE current) {
        for (int i = 0; i < MAXIMUM; i++) {
            if (n->parent->element[i] == n->element[MAXIMUM - 1]) {
                n->parent->child[i] = current;
            }
            if (n->parent->element[i] == "") {
                n->parent->element[i] = n->element[MINIMUM - 1];
                n->parent->child[i] = n;
                break;
            }
        }
    }

    // updates parent of n to new value
    void updateParent(NODE n, string track, string top) {
        string current = "";
        for (int i = 0; i < MAXIMUM && (n->element[i] != ""); i++) {
            if (n->element[i] == top) {
                n->element[i] = track;
            }
            if (n->element[i] > current) {
                current = n->element[i];
            }
        }
        if (track > current && n->parent != NULL) {
            updateParent(n->parent, track, top);
        }
    }

    // if element is not full, inserts to tree and sort
    void insertElement(string artist, string track, int year, string genre) {
        string max;
        NODE current = findLeaf(root, track);
        if (!current) {
            cout << "Integer already inserted in the tree\n";
            return;
        }
        if (!isFull(current)) {
            cout << "Element inserted\n";
            int j = 0;
            for (string& i : current->element) {
                if (i == "") {
                    i = track;
                    current->artist = artist;
                    current->track = track;
                    current->year = year;
                    current->genre = genre;
                    break;
                }
                j++;
                max = i;
            }
            sortNode(current);
            if (j >= MAXIMUM && current->parent != NULL) {
                updateParent(current->parent, track, max);
            }
            return;
        }
        else {
            splitNode(current);
            insertElement(artist, track, year, genre);
        }
    }

    // checks if full
    static int isFull(NODE n) {
        if (n->element[MAXIMUM - 1] == "") {
            return 0;
        } else {
            return 1;
        }
    }

    // splits into 2 nodes
    void splitNode(NODE n) {
        NODE current;
        if (n->parent == NULL) {
            current = createNode();
            root = newRootNode(n, current);
        }
        else {
            if (isFull(n->parent)) {
                splitNode(n->parent);
            }

            current = createNode();
            current->parent = n->parent;
            appendToParent(n, current);
        }
        for (int i = 0; i < MINIMUM; i++) {
            current->element[i] = n->element[MINIMUM + i];
            n->element[MINIMUM + i] = -1;
            current->child[i] = n->child[MINIMUM + i];
            if (current->child[i] != NULL) {
                current->child[i]->parent = current;
            }
            n->child[MINIMUM + i] = NULL;
        }
        sortNode(n->parent);
    }

    void printTree() {
        NODE queue[50];
        int f = 0, r = -1, level = -1;
        queue[++r] = NULL;
        queue[++r] = root;
        while (f < r) {
            if (queue[f] == NULL) {
                cout<<"\nLevel: "<< ++level << endl;
                if (queue[r] != NULL) {
                    queue[++r] = NULL;
                }
                f++;
            } else {
                for (int i = 0; i < MAXIMUM; i++) {
                    if (queue[f]->element[i] != "") {
                        cout << queue[f]->element[i] << " ";
                    }
                    if (queue[f]->child[i] != NULL) {
                        queue[++r] = queue[f]->child[i];
                    }
                }
                cout << "      ";
                f++;
            }
        }
        cout << endl;
    }
};

class SplayTree {
public:
    class Node {
    public:
        string artist;
        string track;
        int year = 0;
        string genre;
        long height = 0;
        Node* left = nullptr;
        Node* right = nullptr;
        Node(string artist, string track, int year, string genre): artist(artist), track(track), year(year), genre(genre), height(0), left(nullptr), right(nullptr) {}
    };
    Node* root = nullptr;
    long numNodes = 0;

    /// Function Definitions:
    // Helper function to insert to properly set the value of root and prevent an infinite loop. Also calls splay to move the inserted node to the top.
    void insert (string artist, string track, int year, string genre) {
        root = insertFunction(root, artist, track, year, genre);
        root = splay(root, track);
    }
    // Method to construct the tree, adding new nodes.
    Node* insertFunction(Node* node, string artist, string track, int year, string genre) {
        // If traverses to a space where a node can be added, add the node.
        if (node == nullptr) {
            numNodes++;
            Node *tempNode = new Node(artist, track, year, genre);
            return tempNode;
        }
        // Traverse recursively left or right depending on where needs to travel to insert.
        if (track < node->track) {
            node->left = insertFunction(node->left, artist, track, year, genre);
        } else if (track > node->track) {
            node->right = insertFunction(node->right, artist, track, year, genre);
        }
        return node;
    }
    // Inserts every file from the musicdata.txt dataset into the program.
    void readFile() {
        // Reads the musicdata.txt file
        ifstream myFile;
        myFile.open("musicdata.txt");
        string firstLine = "";

        // While there are lines in the dataset left to read, continue to traverse and add each node. If repeat found, skip over it.
        if (myFile.is_open()) {
            getline(myFile, firstLine);
            while (myFile.good()) {
                // Artist name
                string artistName = "";
                getline(myFile, artistName, '\t');
                // Track name
                string trackName = "";
                getline(myFile, trackName, '\t');
                // Year
                string releaseYear = "";
                getline(myFile, releaseYear, '\t');
                // Genre
                string genreName = "";
                getline(myFile, genreName);
                insert(artistName, trackName, stoi(releaseYear), genreName);
            }
        }
    }
    // Helper function allowing a call of search without requiring the use of a node in main. Also used to print unsuccessful for faulty searches and call the splay function.
    string search(string searchThis) {
        // Call the BST-style search function.
        bool matchFound = false;
        searchFunction(root, searchThis, matchFound);
        // If a match to the track to be searched is found, call the splay function. Otherwise, output that no match was found.
        if (!matchFound) {
            cout << "No match to the inputted song found!" << endl;
        }
        else {
            root = splay(root, searchThis);
        }
        return searchThis;
    }
    // BST-style search function which iterates through the list of nodes to output the node matching the input.
    Node* searchFunction(Node *node, string& searchThis, bool& matchFound) {
        Node* resultNode = nullptr;
        // Check to see if a tree exists.
        if (node != nullptr) {
            // If goal is to output all the data for the searched node
            if (searchThis == node->artist || searchThis == node->track || (isdigit(searchThis[0]) && stoi(searchThis) == node->year) || searchThis == node->genre) {
                matchFound = true;
                // Search, as used to relay song information
                cout << "Artist: " << node->artist << ", Track: " << node->track << ", Release Year: " << node->year << ", Genre: " << node->genre << endl;
                return resultNode;
            }
            searchFunction(node->left, searchThis, matchFound);
            searchFunction(node->right, searchThis, matchFound);
        }
    }
    // Splay function - uses differing rotations to move the inputted track into the root of the tree.
    Node* splay(Node* node, string track) {
        // If node doesnt exist, return the root (since root is always called first).
        if (node == nullptr) {
            return node;
        }
        // If inputted track is less than the node variable's track...
        if (node->track > track) {
            // If the left node doesnt exist, return the current node.
            if (node->left == nullptr) {
                return node;
            }
            // Perform Zig-Zig rotation - double right rotation with recursive splay call.
            if (node->left->track > track) {
                node->left->left = splay(node->left->left, track);
                node = rotateRight(node);
            }
            // Perform Zag-Zig rotation - shift position of involved nodes once left and once right. Recursive splay call.
            else if (node->left->track < track) {
                node->left->right = splay(node->left->right, track);
                if (node->left->right != nullptr) {
                    node->left = rotateLeft(node->left);
                }
            }
            // Final two if statements serve two roles - complete Zig-Zig and Zag-Zig rotations,
            // As well as cover both the base case again by returning the current node and zig rotation by only rotating right.
            if (node->left != nullptr) {
                return rotateRight(node);
            }
            else {
                return node;
            }
        }
        // If inputted track is greater than the node variable's track...
        else {
            // If the left node doesnt exist, return the current node.
            if (node->right == nullptr) {
                return node;
            }
            // Perform Zig-Zag rotation - rotates nodes right once and left once.
            if (node->right->track > track) { // Zig Zag
                node->right->left = splay(node->right->left, track);
                if (node->right->left != nullptr) {
                    node->right = rotateRight(node->right);
                }
            }
            // Perform Zag-Zag rotation - rotate nodes left twice.
            else if (node->right->track < track) {
                node->right->right = splay(node->right->right, track);
                node = rotateLeft(node);
            }
            // Final two if statements serve two roles - complete Zig-Zag and Zag-Zag rotations,
            // As well as cover both the base case again by returning the current node and zig rotation by only rotating left.
            if (node->right != nullptr) {
                return rotateLeft(node);
            }
            else {
                return node;
            }
        }
    }
    // Helper function for left rotate
    Node* rotateLeft(Node* node) { // Left, Splay(x), x is right of parent and has no grandparent)
        // nodeR represents the inputted node's right child, nodeRL represents nodeR's left child.
        Node* nodeR = node->right;
        Node* nodeRL = nodeR->left;
        // Move original node so that it is nodeR's new left child.
        nodeR->left = node;
        // Move nodeR's original left child so that it is now the inputted node's right child.
        node->right = nodeRL;
        return nodeR;
    }
    // Helper function for right rotate
    Node* rotateRight(Node* node) { // Right, Splay(x), x is left of parent and has no grandparent
        // nodeL represents the inputted node's left child, nodeLR represents nodeL's right child.
        Node* nodeL = node->left;
        Node* nodeLR = nodeL->right;
        // Move original node so that it is nodeL's new right child.
        nodeL->right = node;
        // Move nodeL's original right child so that it is now the inputted node's left child.
        node->left = nodeLR;
        return nodeL;
    }
    // Function used to output the result of preorder traversal
    void printPreorder() {
        // Use findPreorder to fill vector with the results of a preorder traversal.
        vector<Node*> preorderResults;
        findPreorder(root, preorderResults);
        // Output results
        if (preorderResults.size() == numNodes && root != nullptr) {
            for (int i = 0; i < preorderResults.size(); i++) {
                cout << "Artist: " << preorderResults[i]->artist << ", Track: " << preorderResults[i]->track << ", Release Year: " << preorderResults[i]->year << ", Genre: " << preorderResults[i]->genre << endl;
            }
            return;
        }
        else {
            cout << "unsuccessful" << endl;
        }
    }
    // Function performing actual preorder traversal.
    void findPreorder(Node* node, vector<Node*>& preorderResults) {
        // Perform recursive traversal.
        if (node != nullptr) {
            preorderResults.push_back(node);
            findPreorder(node->left, preorderResults);
            findPreorder(node->right, preorderResults);
        }
    }
};

int main() {
    cout << "Music Sorter and Organizer" << endl;
    cout << "Created by Ryan Coveny and Emily Tran" << endl << endl;

    // Splay code
    SplayTree splayCall;

    while (true) {
        cout << "What would you like to do? (Input a number)" << endl;
        cout << "1. Read info from the database." << endl;
        cout << "2. Add a new song to the database." << endl;
        cout << "3. Display all music in the database." << endl;
        cout << "4. Search within the database." << endl;
        cout << "5. Exit program." << endl;

        string input = "";
        while (true) {
            getline(cin, input);
            if (input == "1") {
                splayCall.readFile();
                cout << "File read!" << endl;
                break;
            }
            if (input == "2") {
                string trackName;
                string artistName;
                string releaseYear;
                string genreName;

                cout << "What is the name of the song to be inserted?" << endl;
                getline(cin, trackName);
                cout << "What is the name of the song's artist?" << endl;
                getline(cin, artistName);
                cout << "What year was the song released?" << endl;
                while (true) {
                    cin >> releaseYear;
                    cin.ignore();
                    if (!isdigit(releaseYear[0])) {
                        cout << "Input a valid numerical year" << endl;
                    }
                    else {
                        break;
                    }
                }
                cout << "What genre does the song belong to?" << endl;
                getline(cin, genreName);

                splayCall.insert(artistName, trackName, stoi(releaseYear), genreName);
                break;
            }
            else if (input == "3") {
                cout << "How would you like the database to be displayed?" << endl;

                cout << "Displaying the database's information via Preorder Traversal..." << endl;

                splayCall.printPreorder();
                break;
            }
            else if (input == "4") {
                string trackName;

                cout << "What is the name of the song to be searched for?" << endl;
                getline(cin, trackName);

                splayCall.search(trackName);
                break;
            }
            else if (input == "5") {
                cout << "Closing program.";
                return 0;
            }
            else {
                cout << "Input a number between 1 and 4." << endl;
            }
        }
    }
}