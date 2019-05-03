#pragma once
#include <iostream>
#include <iomanip>

//Enumerated type for the color of nodes in RB-Tree
enum class Color { RED = 0, BLACK = 1 };

//Enumerated type for the order type of the tree. In this case, ascending or descending order.
enum class Order { ASC = 0, DES = 1 };

template<typename keyType>
class RB_Tree
{
private:

	//Red-Black tree node structure
    struct RB_Node
    {
        Color nodeColor;    //Color of the node. Either Color::RED or Color::BLACK
        keyType key;        //Data contained in the node
        RB_Node* parent;    //Pointer to the node's parent
        RB_Node* left;      //Pointer to the node's left child
        RB_Node* right;     //Pointer to the node's right child
    };

    RB_Node* root;      //Pointer to the root of the tree

    //Pointer to the tree's NIL node. The tree has a unique sentinel node called NIL node to represent all leaves
    //NIL's nodeColor is black and the rest of its attributes are immaterial
    RB_Node* const NIL;

    unsigned numRedNodes;    //Number of red nodes in the tree
    unsigned numBlackNodes;  //Number of black nodes in the tree

    //Private member functions
    void transplant(RB_Node* const, RB_Node* const);
    RB_Node* search(RB_Node*, const keyType&) const;
    RB_Node* minimum(RB_Node*) const;
    void leftRotate(RB_Node* const);
    void rightRotate(RB_Node* const);
    void insertFixup(RB_Node*);
    void deleteFixup(RB_Node*);
    void RB_insert(RB_Node*);
    void RB_delete(RB_Node*);
    int maximum(const int, const int) const;
    int calculateSubtreeHeight(const RB_Node* const) const;
	void copyTree(RB_Node*, RB_Node*, RB_Node*);
	void traverseInsert(const RB_Node* const, const RB_Node* const);
	bool compareSubtrees(const RB_Node*, const RB_Node*, RB_Node* const) const;
	void ascending(const RB_Node* const) const;
	void descending(const RB_Node* const) const;

public:
    //Default Constructor
    RB_Tree();

    //Copy Constructor
    RB_Tree(const RB_Tree&);

    //Destructor
    ~RB_Tree();

	//Public member functions
    void insert(const keyType x);
    bool remove(const keyType x);
    bool containsKey(const keyType x) const;
    bool isEmpty() const;
    unsigned getNumRedNodes() const;
    unsigned getNumBlackNodes() const;
    unsigned getNumNodes() const;
    int getTreeHeight() const;
    void statistics() const;
    void destroyTree();
	void displayTree(const Order) const;

    //Overloaded Operators
    RB_Tree<keyType>& operator=(const RB_Tree<keyType>&);
	RB_Tree<keyType> operator+(const RB_Tree<keyType>&) const;
	RB_Tree<keyType>& operator+=(const RB_Tree<keyType>&);
	bool operator==(const RB_Tree&) const;
	bool operator!=(const RB_Tree&) const;
};

//***************************************************
//		Private member function definitions
//***************************************************
/// <summary>
/// Replaces one subtree as a child of its parent with another subtree.
/// Replaces the subtree rooted at node u with the subtree rooted at node v
/// </summary>
/// <param name="u"> A pointer to the node being replaced </param>
/// <param name="v"> A pointer to the node replacing v in the tree </param>
template<typename keyType>
void RB_Tree<keyType>::transplant(RB_Node* const u, RB_Node* const v)
{
    //Check if we want to transplant the root, a left child or a right child
    if (u->parent == NIL)
    {
        //v becomes the new root
        root = v;
    }
    else if (u == u->parent->left)
    {
        //v becomes the left child of u's parent
        u->parent->left = v;
    }
    else
    {
        //v becomes the right child of u's parent
        u->parent->right = v;
    }

    //set v's parent to u's parent
    //Note: Even if v is NIL this operation will not cause problems
    v->parent = u->parent;
}

/// <summary>
/// Searches the tree for a specified key value. Returns NIL if the value is not found.
/// </summary>
/// <param name="traverse"> A pointer used to a node in the tree. This pointer traverses the tree being searched. </param>
/// <param name="keyValue"> The value being searched for in the tree. </param>
/// <returns> A pointer to the node containing the specified key value. </returns>
template<typename keyType>
typename RB_Tree<keyType>::RB_Node* RB_Tree<keyType>::search(RB_Node* traverse, const keyType& keyValue) const
{
	//Continue to search for the value as long as traverse does not point to NIL or 
	//the node with the key being searched for
    while (traverse != NIL && keyValue != traverse->key)
    {
		//Determine if the key being searched for might be in the current node's left or right subtree
        if (keyValue < traverse->key)
        {
            traverse = traverse->left;
        }
        else
        {
            traverse = traverse->right;
        }
    }

	//Return either NIL or a pointer to the node with the specified key
    return traverse;
}

/// <summary>
/// Returns a pointer to the node with the smallest value in the tree.
/// </summary>
/// <param name="traverse"> Pointer that traverses the tree until the minimum is reached. </param>
/// <returns> Pointer to the node with the smallest value in the tree. </returns>
template<typename keyType>
typename RB_Tree<keyType>::RB_Node* RB_Tree<keyType>::minimum(RB_Node* traverse) const
{
	//As long as the current node contains a left child, move the pointer to the left child
    while (traverse->left != NIL)
    {
        traverse = traverse->left;
    }

	return traverse;
}

//NOTE: An exception is thrown if the pivot's right child is NIL
/// <summary>
/// Performs a left rotation about the pivot node. Assumes the pivot's right child is not NIL.
/// Only node pointers are changed, no other attributes are affected.
/// The pivot's right child takes the pivot's position in the tree. The pivot becomes the left child of its right child.
/// The left subtree of the pivot's right child becomes the pivot's right subtree.
/// </summary>
/// <param name="pivot"> A pointer to the node the rotation takes place about. </param>
template<typename keyType>
void RB_Tree<keyType>::leftRotate(RB_Node* const pivot)
{
    //If pivot is the NIL node, the rotation does nothing
    if (pivot != NIL)
    {
        RB_Node* const prc{ pivot->right };    //The pivot's right child. Becomes the root of the rotated subtree

        //If pivot's right child is the NIL node then the rotation will break the tree at the position
        //of the pivot. An exception is thrown to prevent the rotated subtree from being cut off.
        if (prc == NIL)
        {
            throw std::exception{ "ERROR: The pivot's right child cannot be NIL." };
        }

        //prc's left subtree becomes the pivot's right subtree
        pivot->right = prc->left;

        //If prc's left subtree is not NIL, the pivot becomes the parent of prc's left subtree
        if (prc->left != NIL)
        {
            prc->left->parent = pivot;
        }

        //Determine if the pivot is the root, a left child, or a right child. The pivot's right child becomes the
        //tree's new root if the pivot is the tree's root, otherwise it becomes the child of the pivot's parent
        if (pivot->parent == NIL)
        {
            root = prc;
        }
        else if (pivot == pivot->parent->left)
        {
            pivot->parent->left = prc;
        }
        else //if (pivot == pivot->parent->right)
        {
            pivot->parent->right = prc;
        }

        //The pivot's parent becomes the parent of the pivot's right child
        prc->parent = pivot->parent;

        //The pivot becomes the left child of prc
        prc->left = pivot;
        pivot->parent = prc;
    }
}

//NOTE: An exception is thrown if the pivot's left child is NIL
/// <summary>
/// Performs a right rotation about the pivot node. Assumes the pivot's left child is not NIL.
/// Only node pointers are changed. No other attributes are affected.
/// The pivot's left child takes the pivot's position in the tree. The pivot becomes the right child of its left child.
/// The right subtree of the pivot's left child becomes the pivot's left subtree.
/// </summary>
/// <param name="pivot"> A pointer to the node the rotation takes place about. </param>
template<typename keyType>
void RB_Tree<keyType>::rightRotate(RB_Node* const pivot)
{
    //If pivot is the NIL node, the rotation does nothing
    if (pivot != NIL)
    {
        RB_Node* plc{ pivot->left };    //The pivot's left child. Becomes the root of the rotated subtree

        //If pivot's left child is the NIL node then the rotation will break the tree at the position
         //of the pivot. An exception is thrown to prevent the rotated subtree from being cut off.
        if (plc == NIL)
        {
            throw std::exception{ "ERROR: The pivot's left child cannot be NIL." };
        }

        //The right subtree of plc becomes pivot's left subtree
        pivot->left = plc->right;

        //If plc's right subtree is not NIL, its parent becomes the pivot
        if (plc->right != NIL)
        {
            plc->right->parent = pivot;
        }

        //Determine if the pivot is the root of the tree, a right child, or a left child. If the pivot is
        //the root, plc becomes the new root, otherwise the parent of the pivot becomes plc's parent
        if (pivot->parent == NIL)
        {
            root = plc;
        }
        else if (pivot == pivot->parent->left)
        {
            pivot->parent->left = plc;
        }
        else //if (pivot == pivot->parent->right)
        {
            pivot->parent->right = plc;
        }

        //The pivot's parent becomes plc's parent
        plc->parent = pivot->parent;

        //The pivot becomes plc's right child
        plc->right = pivot;
        pivot->parent = plc;
    }
}

/// <summary>
/// Restores the properties of Red-Black trees after an insertion is made.
/// </summary>
/// <param name="insertedNode"> A pointer to the node being inserted </param>
template<typename keyType>
void RB_Tree<keyType>::insertFixup(RB_Node* insertedNode)
{
    //Increment the number of red nodes
    ++numRedNodes;

    //Note: The inserted node is always red.
    //If the inserted node's parent is red we are in violation the property 4
    while (insertedNode->parent->nodeColor == Color::RED)
    {
        //Check if the parent of the inserted node is a left child or right child
        if (insertedNode->parent == insertedNode->parent->parent->left)
        {
            //Create a pointer to the inserted node's uncle
            RB_Node* rightUncle = insertedNode->parent->parent->right;

            //Case 1: The inserted node's uncle is red
            //Recolor the node's parent and uncle to be black, recolor the node's grandparent to be red. Preserves property 5
            //By changing the node's grandparent to red, we may have introduced violations of property 2 or 4
            //further up the tree. Change the inserted node pointer to point to the nodes grandparent and check for a violation
            if (rightUncle->nodeColor == Color::RED)
            {
                insertedNode->parent->nodeColor = Color::BLACK;
                rightUncle->nodeColor = Color::BLACK;
                insertedNode->parent->parent->nodeColor = Color::RED;
                insertedNode = insertedNode->parent->parent;

                //Case 1 produces a net change of one more black node and one less red node
                ++numBlackNodes;
                --numRedNodes;
            }
            //Else, the inserted node's uncle is black
            else
            {
                //Case 2: The inserted node's uncle is black and the inserted node is a right child
                //Point to the inserted node's parent and perform a left rotate with the inserted node's parent as the pivot
                //This changes our violation to case 3 and we fall through to case 3
                if (insertedNode == insertedNode->parent->right)
                {
                    insertedNode = insertedNode->parent;
                    leftRotate(insertedNode);
                }

                //Case 3: The inserted node's uncle is black and the inserted node is a left child
                //Color the inserted node's parent black and the grandparent red then rotate right with
                //the grandparent as the pivot. The recoloring and rotation preserve property 5. After case 3 we do not
                //have two red nodes in a row and the loop will terminate. Since we do not point to the root in case two
                //and three property two holds. Case 2 and 3 fix the only violation of property 4.
                insertedNode->parent->nodeColor = Color::BLACK;
                insertedNode->parent->parent->nodeColor = Color::RED;
                rightRotate(insertedNode->parent->parent);
            }
        }
        //Same as the first if block but with left and right exchanged.
        else //if (insertedNode->parent == insertedNode->parent->parent->right)
        {
            RB_Node* leftUncle = insertedNode->parent->parent->left;

            //Case 1
            if (leftUncle->nodeColor == Color::RED)
            {
                insertedNode->parent->nodeColor = Color::BLACK;
                leftUncle->nodeColor = Color::BLACK;
                insertedNode->parent->parent->nodeColor = Color::RED;
                insertedNode = insertedNode->parent->parent;

                //Case 1 produces a net change of one more black node and one less red node
                ++numBlackNodes;
                --numRedNodes;
            }
            else
            {
                //Case 2
                if (insertedNode == insertedNode->parent->left)
                {
                    insertedNode = insertedNode->parent;
                    rightRotate(insertedNode);
                }

                //Case 3
                insertedNode->parent->nodeColor = Color::BLACK;
                insertedNode->parent->parent->nodeColor = Color::RED;
                leftRotate(insertedNode->parent->parent);
            }
        }
    }

    //If the root color is red at this point, we recolor it black
    if (root->nodeColor == Color::RED)
    {
        ++numBlackNodes;
        --numRedNodes;
    }

    //Color the root black since at termination property 2 is the only property not guaranteed
    root->nodeColor = Color::BLACK;
}

//ADD COMMENTS, REWRITE
template<typename keyType>
void RB_Tree<keyType>::deleteFixup(RB_Node* x)
{
    RB_Node* w;
    while (x != root && x->nodeColor == Color::BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            //case1 transforms to case 2,3,4
            if (w->nodeColor == Color::RED)
            {
                w->nodeColor = Color::BLACK;
                x->parent->nodeColor = Color::RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            //case 2
            if (w->left->nodeColor == Color::BLACK && w->right->nodeColor == Color::BLACK)
            {
                w->nodeColor = Color::RED;
                x = x->parent;

                //Adjust the red and black node counts
                ++numRedNodes;
                --numBlackNodes;
            }
            else
            {
                //case 3 transforms to case 4
                if (w->right->nodeColor == Color::BLACK)
                {
                    w->left->nodeColor = Color::BLACK;
                    w->nodeColor = Color::RED;
                    rightRotate(w);
                    w = x->parent->right;
                }

                //case 4
                w->nodeColor = x->parent->nodeColor;
                x->parent->nodeColor = Color::BLACK;
                w->right->nodeColor = Color::BLACK;
                leftRotate(x->parent);
                x = root;

                //Adjust the red and black node counts
                --numRedNodes;
                ++numBlackNodes;
            }
        }
        else
        {
            w = x->parent->left;

            //Case 1
            if (w->nodeColor == Color::RED)
            {
                w->nodeColor = Color::BLACK;
                x->parent->nodeColor = Color::RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }

            //Case 2
            if (w->right->nodeColor == Color::BLACK && w->left->nodeColor == Color::BLACK)
            {
                w->nodeColor = Color::RED;
                x = x->parent;

                //Adjust the red and black node counts
                ++numRedNodes;
                --numBlackNodes;
            }
            else
            {
                //Case 3
                if (w->left->nodeColor == Color::BLACK)
                {
                    w->right->nodeColor = Color::BLACK;
                    w->nodeColor = Color::RED;
                    leftRotate(w);
                    w = x->parent->left;
                }

                //Case 4
                w->nodeColor = x->parent->nodeColor;
                x->parent->nodeColor = Color::BLACK;
                w->left->nodeColor = Color::BLACK;
                rightRotate(x->parent);
                x = root;

                //Adjust the red and black node counts
                ++numBlackNodes;
                --numRedNodes;
            }
        }
    }

    //Check if we need to adjust our counter
    if (x->nodeColor != Color::BLACK)
    {
        --numRedNodes;
        ++numBlackNodes;
    }

    x->nodeColor = Color::BLACK;
}

/// <summary>
/// Inserts a node into the red black tree
/// </summary>
/// <param name="insertedNode"> A pointer to the node being inserted in the tree </param>
template<typename keyType>
void RB_Tree<keyType>::RB_insert(RB_Node* insertedNode)
{
    //Pointer to trail the inserted node, points to the parent when the insertion takes place
    RB_Node* trailing = NIL;

    //Pointer used to traverse the tree, we stop traversing when this pointer points to NIL
    RB_Node* traverse = root;

    //Find the position to insert the node
    while (traverse != NIL)
    {
        trailing = traverse;
        if (insertedNode->key < traverse->key)
        {
            traverse = traverse->left;
        }
        else
        {
            traverse = traverse->right;
        }
    }

    //Set the inserted node's parent to point to the trailing pointer
    insertedNode->parent = trailing;

    //Check if the inserted node is the root, a left child, or a right child and insert the node into the tree
    if (trailing == NIL)
    {
        root = insertedNode;
    }
    else if (insertedNode->key < trailing->key)
    {
        trailing->left = insertedNode;
    }
    else
    {
        trailing->right = insertedNode;
    }

	//Restore RedBlack Tree properties
    insertFixup(insertedNode);
}

//ADD COMMENTS, REWRITE
//NOTE: Memory is freed in this function
template<typename keyType>
void RB_Tree<keyType>::RB_delete(RB_Node* nodeToDelete)
{
    RB_Node* y = nodeToDelete;
    RB_Node* replacement;
    Color originalColor = nodeToDelete->nodeColor;

    if (nodeToDelete->left == NIL)
    {
        replacement = nodeToDelete->right;
        transplant(nodeToDelete, nodeToDelete->right);
    }
    else if (nodeToDelete->right == NIL)
    {
        replacement = nodeToDelete->left;
        transplant(nodeToDelete, nodeToDelete->left);
    }
    else
    {
        y = minimum(nodeToDelete->right);
        originalColor = y->nodeColor;
        replacement = y->right;

        if (y->parent == nodeToDelete)
        {
            replacement->parent = y;
        }
        else
        {
            transplant(y, y->right);
            y->right = nodeToDelete->right;
            y->right->parent = y;
        }

        transplant(nodeToDelete, y);
        y->left = nodeToDelete->left;
        y->left->parent = y;

        //The node y will be recolored with the color of the node we are deleting
        //If the node colors are different, then y will change color and the counters are adjusted
        if (y->nodeColor != nodeToDelete->nodeColor)
        {
            //y changes color to the color of the node being deleted
            if (nodeToDelete->nodeColor == Color::BLACK)
            {
                ++numBlackNodes;
                --numRedNodes;
            }
            else
            {
                ++numRedNodes;
                --numBlackNodes;
            }
        }

        y->nodeColor = nodeToDelete->nodeColor;
    }

    //Adjust the appropriate counter before removing a node
    if (nodeToDelete->nodeColor == Color::BLACK)
    {
        --numBlackNodes;
    }
    else
    {
        --numRedNodes;
    }

	//Free allocated memory
    delete nodeToDelete;

    if (originalColor == Color::BLACK)
    {
        deleteFixup(replacement);
    }
}

/// <summary>
/// Compares two integers and returns the beg
/// </summary>
/// <param name="a"> First integer being compared. </param>
/// <param name="b"> Second integer being compared. </param>
/// <returns> The maximum of the two integers. </returns>
template<typename keyType>
int RB_Tree<keyType>::maximum(const int a, const int b) const
{
    return (a > b) ? a : b;
}

/// <summary>
/// Calculates the height of the tree rooted at the node passed as a paramater.
/// </summary>
/// <param name="subtreeRoot"> Pointer to the root of the subtree height is being calculated for. </param>
/// <returns> The height of the subtree as an int. </returns>
template<typename keyType>
int RB_Tree<keyType>::calculateSubtreeHeight(const RB_Node* const subtreeRoot) const
{
	//If our subtree is empty, return height of -1
    if (subtreeRoot == NIL)
    {
        return -1;
    }
	//Otherwise, return the maximum of the height of the left and right subtrees of the current node
    else
    {
        return maximum(calculateSubtreeHeight(subtreeRoot->left), calculateSubtreeHeight(subtreeRoot->right)) + 1;
    }
}

/// <summary>
/// Allocates a new red black tree by copying the structure of another.
/// </summary>
/// <param name="copyTo_parent"> Pointer to the parent of node being copied to. </param>
/// <param name="copyFrom"> Pointer to the node being copied. </param>
/// <param name="copyFrom_NIL"> Pointer to the NIL node in the tree being copied from. </param>
template<typename keyType>
void RB_Tree<keyType>::copyTree(RB_Node* copyTo_parent, RB_Node* copyFrom, RB_Node* copyFrom_NIL)
{
	//If the node from the subtree we are copying from is that tree's NIL node, there is nothing to copy
	if (copyFrom == copyFrom_NIL)
	{
		return;
	}

	//The node being copied is not NIL so allocate a new node and perform the copy
	RB_Node* copyTo{ new RB_Node };
	copyTo->key = copyFrom->key;
	copyTo->nodeColor = copyFrom->nodeColor;
	copyTo->parent = copyTo_parent;
	copyTo->left = NIL;
	copyTo->right = NIL;
	
	//If the tree being copied to happens to be empty, set the root to the new node that was allocated
	if (root == NIL)
	{
		root = copyTo;
	}

	//Check if the node being copied to is a left or right child and link it accordingly
	if (copyTo->key < copyTo_parent->key)
	{
		copyTo_parent->left = copyTo;
	}
	else
	{
		copyTo_parent->right = copyTo;
	}

	//Recursively call the copySubtree function twice with the node being copied to as the copyTo_parent in the next calls
	//and the left and right children of the node being copied as the copyFrom node in the next calls
	copyTree(copyTo, copyFrom->left, copyFrom_NIL);
	copyTree(copyTo, copyFrom->right, copyFrom_NIL);
}

/// <summary>
/// Perform a LNR traversal of some tree and insert each node in that tree into THIS tree.
/// </summary>
/// <param name="traverse"> Pointer to a node in the tree being traversed. </param>
/// <param name="traverseTreeNIL"> Pointer to the NIL node in the tree being traversed. </param>
template<typename keyType>
void RB_Tree<keyType>::traverseInsert(const RB_Node* const traverse, const RB_Node* const traverseTreeNIL)
{
	// If node is NIL, return recursively to the function called from.
	if (traverse == traverseTreeNIL)
		return;

	// Recursively call the function to the left of the tree.
	traverseInsert(traverse->left, traverseTreeNIL);

	// Insert a new node into THIS tree with the key value of the traverse node
	insert(traverse->key);

	// Recursively call the function to the right of the tree.
	traverseInsert(traverse->right, traverseTreeNIL);
}

/// <summary>
/// Compares two trees by comparing the two nodes being pointed to and then recursively comparing the
/// left and right subtrees of these nodes.
/// </summary>
/// <param name="t1"> Pointer to a node THIS tree which is being compared. </param>
/// <param name="t2"> Pointer to a node in the second tree being compared. </param>
/// <param name="t2NIL"> Pointer to the second tree's NIL node. </param>
/// <returns> True if the two nodes are the same and their subtrees are the same, otherwise false. </returns>
template<typename keyType>
bool RB_Tree<keyType>::compareSubtrees(const RB_Node* t1, const RB_Node* t2, RB_Node* const t2NIL) const
{			
			//Both nodes have the same key
	return (t1->key == t2->key) &&
		   //AND both nodes have the same left subtrees
		   ((t1->left == NIL && t2->left == t2NIL) ||
		   (t1->left != NIL && t2->left != t2NIL && compareSubtrees(t1->left, t2->left, t2NIL))) &&
		   //AND both nodes have the same right subtrees
		   ((t1->right == NIL && t2->right == t2NIL) ||
		   (t1->right != NIL && t2->right != t2NIL && compareSubtrees(t1->right, t2->right, t2NIL)));
}

/// <summary>
/// Initially a node that points to the root node will then traverse the tree
/// in a ascending order, following the LNR (Left-Node-Right) order.
/// </summary>
/// <param name="node"> A pointer to a node that will traverse the tree </param>
template<typename keyType>
void RB_Tree<keyType>::ascending(const RB_Node* const node) const
{
	// If node is NIL, return recursively to the function called from.
	if (node == NIL)
		return;

	// Recursively call the function to the left of the tree.
	ascending(node->left);

	// Display node's key value.
	std::cout << node->key << std::endl;

	// Recursively call the function to the right of the tree.
	ascending(node->right);
}

/// <summary>
/// Initially a node that points to the root node will then traverse the tree
/// in a descending order, RNL (Right-Node-Left) order.
/// </summary>
/// <param name="node"> A pointer to a node that will traverse the tree </param>
template<typename keyType>
void RB_Tree<keyType>::descending(const RB_Node* const node) const
{
	// If node is NIL, return recursively to the function called from.
	if (node == NIL)
		return;

	// Recursively call the function to the right of the tree.
	descending(node->right);

	// Display node's key value.
	std::cout << node->key << std::endl;

	// Recursively call the function to the left of the tree.
	descending(node->left);
}

//***************************************
//			Default Contructor
//***************************************
/// <summary>
/// Constructor for RB_Tree. Allocates a NIL node and points root to NIL. The NIL node is colored black
/// </summary>
template<typename keyType>
RB_Tree<keyType>::RB_Tree() : NIL{ new RB_Node }, numBlackNodes{ 0 }, numRedNodes{ 0 }
{
    NIL->nodeColor = Color::BLACK;
    root = NIL;
}

//***************************************
//			Copy Constructor
//***************************************
/// <summary>
/// Red-Black Tree copy constructor. Sets up an empty tree then copies data from the tree parameter.
/// </summary>
/// <param name="right"> Constant reference to the tree being copied. </param>
template<typename keyType>
RB_Tree<keyType>::RB_Tree(const RB_Tree& right) : 
	NIL{ new RB_Node }, numBlackNodes{ right.numBlackNodes }, numRedNodes{ right.numRedNodes }
{
	//Set up empty tree
	NIL->nodeColor = Color::BLACK;
	root = NIL;

	//Copy data from the tree parameter
	copyTree(root, right.root, right.NIL);
}

//*********************************
//			Destructor
//*********************************
//NOTE: Memory is deallocated in this function
/// <summary>
/// Red-Black Tree destructor. Destroys the tree and frees the allocated memory. The memory allocated to the NIL node is
/// deallocated in the destructor.
/// </summary>
template<typename keyType>
RB_Tree<keyType>::~RB_Tree()
{
	//Destroy the tree, leaving only the NIL node
	destroyTree();

	//Free up the memory allocated for the NIL node
	delete NIL;
}

//*********************************************
//			Public Member Functions
//*********************************************
//NOTE: Memory is allocated in this function
/// <summary>
/// Inserts a node into the Red-Black tree with a specified key value.
/// </summary>
/// <param name="x"> The key value of the node being insterted. </param>
template<typename keyType>
void RB_Tree<keyType>::insert(const keyType x)
{
    //Allocate a new node, initialize it with the data passed to the function
    RB_Node* newNode = new RB_Node;

    newNode->parent = NIL;
    newNode->left = NIL;
    newNode->right = NIL;
    newNode->nodeColor = Color::RED;
    newNode->key = x;

    //Insert the node into the tree
    RB_insert(newNode);
}

/// <summary>
/// Attempts to remove a node with the specified key value from the tree. If such a node does not exist
/// nothing happens.
/// </summary>
/// <param name="x"> The key value of the node to be removed from the tree. </param>
/// <returns> Returns true if a node was removed, otherwise false. </returns>
template<typename keyType>
bool RB_Tree<keyType>::remove(const keyType x)
{
	//Search for the node to delete. Returns NIL if the node does not exist.
    RB_Node* nodeToDelete = search(root, x);

	//If the node exists, delete it and return true
    if (nodeToDelete != NIL)
    {
        RB_delete(nodeToDelete);
		return true;
    }

	//The node was not found, return false
	return false;
}

/// <summary>
/// Determines if the key value specified is in the tree. The value of the pointer returned from search
/// is compared to NIL. If the pointer is not NIL, the element was found and the expression evaluates to true. 
/// If the pointer is NIL then the search was unsuccessful the expression evaluates to false.
/// </summary>
/// <param name="keyValue"> The key value that is searched for in the tree </param>
/// <returns> True if the key value passed is in the tree, otherwise false </returns>
template<typename keyType>
bool RB_Tree<keyType>::containsKey(const keyType keyValue) const
{
    return (search(root, keyValue) != NIL);
}

/// <summary>
/// Checks to see if the tree is empty. The tree is empty if the root is NIL.
/// </summary>
/// <returns> True if the tree is empty, otherwise false </returns>
template<typename keyType>
bool RB_Tree<keyType>::isEmpty() const
{
    return root == NIL;
}

/// <summary>
/// Accessor function for the numRedNodes member
/// </summary>
/// <returns> The number of red nodes in the Red-Black tree. </returns>
template<typename keyType>
unsigned RB_Tree<keyType>::getNumRedNodes() const
{
    return numRedNodes;
}

/// <summary>
/// Accessor function for the numBlackNodes member.
/// </summary>
/// <returns> The number of black nodes in the Red-Black tree. </returns>
template<typename keyType>
unsigned RB_Tree<keyType>::getNumBlackNodes() const
{
    return numBlackNodes;
}

/// <summary>
/// Gets the total number of nodes in the tree by adding the number of red and black nodes together.
/// </summary>
/// <returns> The total number of nodes in the Red-Black tree. </returns>
template<typename keyType>
unsigned RB_Tree<keyType>::getNumNodes() const
{
    return numRedNodes + numBlackNodes;
}

/// <summary>
/// Calculates the height of the Red-Black tree.
/// </summary>
/// <returns> The height of the Red-Black tree as an int. -1 is returned if the tree is empty. </returns>
template<typename keyType>
int RB_Tree<keyType>::getTreeHeight() const
{
    return calculateSubtreeHeight(root);
}

/// <summary>
/// Displays statistics about the Red-Black tree including total nodes, height, and number of red and black nodes.
/// </summary>
template<typename keyType>
void RB_Tree<keyType>::statistics() const
{
    std::cout << "Red-Black Tree Statistics\n";
    std::cout << "-------------------------\n";
    std::cout << std::setw(25) << "Total Nodes: " << getNumNodes() << std::endl;
    std::cout << std::setw(25) << "Tree Height: " << getTreeHeight() << std::endl;
    std::cout << std::setw(25) << "Number of Red Nodes: " << getNumRedNodes() << std::endl;
    std::cout << std::setw(25) << "Number of Black Nodes: " << getNumBlackNodes() << std::endl;
}

/// <summary>
/// Destroys the Red-Black tree, leaving only the NIL node as the root.
/// </summary>
template<typename keyType>
void RB_Tree<keyType>::destroyTree()
{
	//While the tree is not empty, delete the root
    while (root != NIL)
    {
        RB_delete(root);
    }
}

/// <summary>
/// Determines the type of way the tree wants to be displayed. As of now, the two options
/// are ascending and descending. If more ways to order tree was to be added, add more types
/// to the Order enumerator class and account for them within the displayTree function.
/// </summary>
/// <param name="ord"> Specifies the order in which the tree is displayed. </param>
template<typename keyType>
void RB_Tree<keyType>::displayTree(const Order ord) const
{
	// Set the node to its root value.
	RB_Node* node = root;

	// If the node is NIL, no tree.
	if (node == NIL)
	{
		return;
	}
	// If the value of x is 0, place the tree in ascending order.
	else if (ord == Order::ASC)
	{
		ascending(node);
	}
	// If the value of x is 1, place the tree in descending order.
	else if (ord == Order::DES)
	{
		descending(node);
	}
	// If the value does not exist within the enumerator class Order, display the following:
	else
	{
		std::cout << "Order not within the enumerator class." << std::endl;
	}
}


//************************************************
//				Overloaded Operators
//************************************************
/// <summary>
/// Assigns one tree to another. The exact structure of the Red-Black tree is copied.
/// </summary>
/// <param name="right"> The tree on the right hand side of an assignment statement. (leftTree = rightTree) </param>
/// <returns> A reference to the tree that has been assigned to. </returns>
template<typename keyType>
RB_Tree<keyType>& RB_Tree<keyType>::operator=(const RB_Tree<keyType>& right)
{
	//this = right

	//Check for self assignment
	if (this != &right)
	{
		//Destroy the tree being assigned to, which is overwritten by a new tree.
		this->destroyTree();

		//Copy the number of red and black nodes
		numBlackNodes = right.numBlackNodes;
		numRedNodes = right.numRedNodes;

		//Copy the right tree to the left tree
		copyTree(root, right.root, right.NIL);	
	}

	//Return constant reference to the tree that was assigned to. Allows for cascading assignment.
	return *this;
}

/// <summary>
/// Adds THIS tree and the right tree parameter. Red-Black tree addition (x + y) is defined as the tree that results
/// from starting with tree x and then inserting nodes from tree y in LNR order.
/// </summary>
/// <param name="right"> The tree that is the right summand in an addition operation. </param>
/// <returns> A copy of the sum of the two trees. This enables cascading. </returns>
template<typename keyType>
RB_Tree<keyType> RB_Tree<keyType>::operator+(const RB_Tree<keyType>& right) const
{
	//sum = this + right, return sum

	//Initialize the sum to this tree
	RB_Tree<keyType> sumTree{ *this };

	//Insert each node from the right tree into the sum tree
	sumTree.traverseInsert(right.root, right.NIL);

	//Return a constant reference to the sum tree. Supports cascading
	return sumTree;
}

/// <summary>
/// Performs the addition THIS + right and then assigns the result to THIS.
/// </summary>
/// <param name="right"> The tree on the right hand side of the += operator. </param>
/// <returns> A reference to THIS tree after the assignment has been done. </param>
template<typename keyType>
RB_Tree<keyType>& RB_Tree<keyType>::operator+=(const RB_Tree<keyType>& right)
{
	return *this = (*this + right);
}

/// <summary>
/// Compares two trees to determine if they are equal.
/// Equality is defined as having the same structure, colors, and keys.
/// </summary>
/// <param name="right"> The tree on the right of the equality operation being compared to THIS tree. </param>
/// <returns> True if the trees are the same, otherwise false. </returns>
template<typename keyType>
bool RB_Tree<keyType>::operator==(const RB_Tree& right) const
{
	//Check for self comparisson
	if (this != &right)
	{			
			   //Both trees have the same number of red and black nodes
		return (numRedNodes == right.numRedNodes && numBlackNodes == right.numBlackNodes) &&
			   //AND both trees are empty OR both trees are the same
			   ((root == NIL && right.root == right.NIL) ||
			    (root != NIL && right.root != right.NIL && compareSubtrees(root, right.root, right.NIL)));
	}
	else
	{
		return true;
	}
}

/// <summary>
/// Compares two trees to determine if they are not equal. Returns the NOT of the operator== function.
/// Equality is defined as having the same structure, colors, and keys.
/// </summary>
/// <param name="right"> Tree on the right hand side of the not equal operator. </param>
/// <returns> True if the trees are equal, otherwise false. </returns>
template<typename keyType>
bool RB_Tree<keyType>::operator!=(const RB_Tree& right) const
{
	return !(*this == right);
}