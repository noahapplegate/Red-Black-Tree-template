#include "RB_Tree.h"
#include <iostream>

int main()
{
    RB_Tree<int> t1;

	//TEST isEMPTY (empty)
    if (t1.isEmpty())
    {
        std::cout << "tree empty\n";
    }

    std::cout << "inserting into tree\n";

	//TEST INSERT
    t1.insert(15);
    t1.insert(10);
    t1.insert(20);
    t1.insert(25);
    t1.insert(30);
    t1.insert(17);
    t1.insert(16);
    t1.insert(18);
    t1.insert(5);
    t1.insert(8);

	//TEST OPERATOR=
	RB_Tree<int> t2;
	t2 = t1;

	//TEST COPY CONSTRUCTOR
	RB_Tree<int> t3{ t2 };

	//TEST DESTROY TREE
	t1.destroyTree();

	t1.insert(15);
	t1.insert(17);
	t1.insert(25);
	t1.insert(13);
	t1.insert(5);
	t1.insert(10);

	//TEST CASCADING ASSIGNMENT
	t3 = t2 = t1;

    t1.insert(35);
    t1.insert(29);

	//TEST getTREEHEIGHT
    std::cout << t1.getTreeHeight() << std::endl;

	//TEST isEmpty (not empty)
    if (t1.isEmpty())
    {
        std::cout << "tree empty\n";
    }

    //TEST REMOVE
    t1.remove(8);		//Not in tree
    t1.remove(15);
    t1.remove(17);
    t1.remove(5);
    t1.remove(35);
    t1.remove(25);
    t1.remove(10);

	//13, 29 in tree
	//TEST CONTAINSKEY (in tree)
    if (t1.containsKey(13))
    {
        std::cout << "13 in the tree\n";
    }

	//TEST CONTAINSKEY (not in tree)
    if (t1.containsKey(1))
    {
        std::cout << "1 in the tree\n";
    }

	//TEST GETTREEHEIGHT
	//height 1
	std::cout << t1.getTreeHeight() << std::endl;

	t1.remove(13);	//remove root
	t1.remove(29);

	//height -1
	std::cout << t1.getTreeHeight() << std::endl;

	//TEST STATISTICS
	t1.statistics();

	t2 = t1;

    //height 2
    std::cout << t1.getTreeHeight() << std::endl;

    t1.insert(5);

    //height 3
    std::cout << t1.getTreeHeight() << std::endl;

    t1.statistics();

    t1.destroyTree();
	t2.destroyTree();
	t3.destroyTree();

	t1.insert(15);
	t1.insert(24);
	t1.insert(14);
	t1.insert(10);
	t1.insert(9);
	t1.insert(5);
	t1.insert(12);

	t2.insert(3);
	t2.insert(17);
	t2.insert(10);
	t2.insert(10);

	t3.insert(19);
	t3.insert(21);
	t3.insert(4);
	
	//TEST ADDITION
	RB_Tree<int> t4;

	t4 = t1 + t2 + t3;

	//TEST DISPLAYTREE (ascending/descending/empty)
	t4.displayTree(Order::ASC);
	std::cout << std::endl;
	t4.displayTree(Order::DES);

	t1.destroyTree();

	t1.displayTree(Order::ASC);

	//TEST OPERATOR==
	std::cout << (t2 == t3);
	t1 = t4;
	std::cout << (t1 == t4);
	
	//compare empty trees
	t4.destroyTree();
	RB_Tree<int> t5;
	std::cout << (t4 == t5);

	std::cout << (t1 == t1);

	t2 = t1;
	t2.insert(56);

	std::cout << (t1 == t2);

	std::cout << std::endl;

	//TEST OPERATOR!=

	std::cout << (t1 != t3);

	//TEST OPERATOR+=
	t1.destroyTree();
	t2.destroyTree();

	t1.insert(15);
	t1.insert(10);
	t1.insert(20);
	t1.insert(30);

	t2.insert(17);
	t2.insert(21);
	t2.insert(25);
	t2.insert(13);

	t1 += t2;

    return 0;
}