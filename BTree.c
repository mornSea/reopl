



C语言


头文件：

//实现对order序(阶)的B-TREE结构基本操作的封装。
//查找：search，插入：insert，删除：remove。
//创建：create，销毁：destory，打印：print。
#ifndef BTREE_H
#define BTREE_H

#ifdef __cplusplus
extern "C" {
#endif

////* 定义m序(阶)B 树的最小度数BTree_D=ceil(m)*/
/// 在这里定义每个节点中关键字的最大数目为:2 * BTree_D - 1，即序(阶)：2 * BTree_D.
#define BTree_D        2
#define ORDER        (BTree_D * 2) //定义为4阶B-tree,2-3-4树。最简单为3阶B-tree,2-3树。
//#define ORDER        (BTree_T * 2-1)//最简单为3阶B-tree,2-3树。

typedef int KeyType;
typedef struct BTNode{
int keynum;                        /// 结点中关键字的个数，keynum <= BTree_N
KeyType key[ORDER-1];                /// 关键字向量为key[0..keynum - 1]
struct BTNode* child[ORDER];        /// 孩子指针向量为child[0..keynum]
bool isLeaf;                    /// 是否是叶子节点的标志
}BTNode;

typedef BTNode* BTree;///定义BTree

///给定数据集data,创建BTree。
void BTree_create(BTree* tree, const KeyType* data, int length);

///销毁BTree，释放内存空间。
void BTree_destroy(BTree* tree);

///在BTree中插入关键字key。
void BTree_insert(BTree* tree, KeyType key);

///在BTree中移除关键字key。
void BTree_remove(BTree* tree, KeyType key);

///深度遍历BTree打印各层结点信息。
void BTree_print(const BTree tree, int layer=1);

/// 在BTree中查找关键字 key，
/// 成功时返回找到的节点的地址及 key 在其中的位置 *pos
/// 失败时返回 NULL 及查找失败时扫描到的节点位置 *pos
BTNode* BTree_search(const BTree tree, int key, int* pos);

#ifdef __cplusplus
}
#endif

#endif




实现文件：

//实现对order序(阶)的B-TREE结构基本操作的封装。

//查找：search，插入：insert，删除：remove。

//创建：create，销毁：destory，打印：print。

#include <stdlib.h>

#include <stdio.h>

#include <assert.h>

#include "btree.h"

 

//#define max(a, b) (((a) > (b)) ? (a) : (b))

#define cmp(a, b) ( ( ((a)-(b)) >= (0) ) ? (1) : (0) ) //比较a，b大小

#define DEBUG_BTREE

 

 

// 模拟向磁盘写入节点

void disk_write(BTNode* node)

{

//打印出结点中的全部元素，方便调试查看keynum之后的元素是否为0(即是否存在垃圾数据)；而不是keynum个元素。

    printf("向磁盘写入节点");

 for(int i=0;i<ORDER-1;i++){

  printf("%c",node->key[i]);

 }

 printf("\n");

}

 

// 模拟从磁盘读取节点

void disk_read(BTNode** node)

{

//打印出结点中的全部元素，方便调试查看keynum之后的元素是否为0(即是否存在垃圾数据)；而不是keynum个元素。

 printf("向磁盘读取节点");

 for(int i=0;i<ORDER-1;i++){

  printf("%c",(*node)->key[i]);

 }

 printf("\n");

}

 

// 按层次打印 B 树

void BTree_print(const BTree tree, int layer)

{

    int i;

    BTNode* node = tree;

 

    if (node) {

        printf("第 %d 层， %d node : ", layer, node->keynum);

 

  //打印出结点中的全部元素，方便调试查看keynum之后的元素是否为0(即是否存在垃圾数据)；而不是keynum个元素。

        for (i = 0; i < ORDER-1; ++i) {

  //for (i = 0; i < node->keynum; ++i) {

            printf("%c ", node->key[i]);

        }

 

        printf("\n");

 

        ++layer;

        for (i = 0 ; i <= node->keynum; i++) {

            if (node->child[i]) {

                BTree_print(node->child[i], layer);

            }

        }

    }

    else {

        printf("树为空。\n");

    }

}

 

// 结点node内对关键字进行二分查找。

int binarySearch(BTNode* node, int low, int high, KeyType Fkey)

{

 int mid;

 while (low<=high)

 {

  mid = low + (high-low)/2;

  if (Fkey<node->key[mid])

  {

   high = mid-1;

  }

  if (Fkey>node->key[mid])

  {

   low = mid+1;

  }

  if (Fkey==node->key[mid])

  {

   return mid;//返回下标。

  }

 }

 return 0;//未找到返回0.

}

 

//insert

/***************************************************************************************

   将分裂的结点中的一半元素给新建的结点，并且将分裂结点中的中间关键字元素上移至父节点中。

   parent 是一个非满的父节点

   node 是 tree 孩子表中下标为 index 的孩子节点，且是满的，需分裂。

*******************************************************************/

void BTree_split_child(BTNode* parent, int index, BTNode* node)

{

#ifdef DEBUG_BTREE

 printf("BTree_split_child!\n");

#endif

    assert(parent && node);

    int i;

 

    // 创建新节点，存储 node 中后半部分的数据

    BTNode* newNode = (BTNode*)calloc(sizeof(BTNode), 1);

    if (!newNode) {

        printf("Error! out of memory!\n");

        return;

    }

 

    newNode->isLeaf = node->isLeaf;

    newNode->keynum = BTree_D - 1;

 

    // 拷贝 node 后半部分关键字,然后将node后半部分置为0。

    for (i = 0; i < newNode->keynum; ++i){

        newNode->key[i] = node->key[BTree_D + i];

        node->key[BTree_D + i] = 0;

    }

 

    // 如果 node 不是叶子节点，拷贝 node 后半部分的指向孩子节点的指针，然后将node后半部分指向孩子节点的指针置为NULL。

    if (!node->isLeaf) {

        for (i = 0; i < BTree_D; i++) {

            newNode->child[i] = node->child[BTree_D + i];

            node->child[BTree_D + i] = NULL;

        }

    }

 

    // 将 node 分裂出 newNode 之后，里面的数据减半

    node->keynum = BTree_D - 1;

 

    // 调整父节点中的指向孩子的指针和关键字元素。分裂时父节点增加指向孩子的指针和关键元素。

    for (i = parent->keynum; i > index; --i) {

        parent->child[i + 1] = parent->child[i];

    }

 

    parent->child[index + 1] = newNode;

 

    for (i = parent->keynum - 1; i >= index; --i) {

        parent->key[i + 1] = parent->key[i];

    }

 

    parent->key[index] = node->key[BTree_D - 1];

    ++parent->keynum;

 

 node->key[BTree_D - 1] = 0;

 

    // 写入磁盘

     disk_write(parent);

     disk_write(newNode);

     disk_write(node);

}

 

void BTree_insert_nonfull(BTNode* node, KeyType key)

{

    assert(node);

 

    int i;

 

    // 节点是叶子节点，直接插入

    if (node->isLeaf) {

        i = node->keynum - 1;

        while (i >= 0 && key < node->key[i]) {

            node->key[i + 1] = node->key[i];

            --i;

        }

 

        node->key[i + 1] = key;

        ++node->keynum;

 

        // 写入磁盘

        disk_write(node);

    }

 

    // 节点是内部节点

    else {

        /* 查找插入的位置*/

        i = node->keynum - 1;

        while (i >= 0 && key < node->key[i]) {

            --i;

        }

 

        ++i;

 

        // 从磁盘读取孩子节点

        disk_read(&node->child[i]);

 

        // 如果该孩子节点已满，分裂调整值

        if (node->child[i]->keynum == (ORDER-1)) {

            BTree_split_child(node, i, node->child[i]);

   // 如果待插入的关键字大于该分裂结点中上移到父节点的关键字，在该关键字的右孩子结点中进行插入操作。

            if (key > node->key[i]) {

                ++i;

            }

        }

        BTree_insert_nonfull(node->child[i], key);

    }

}

 

void BTree_insert(BTree* tree, KeyType key)

{

#ifdef DEBUG_BTREE

 printf("BTree_insert:\n");

#endif

    BTNode* node;

    BTNode* root = *tree;

 

    // 树为空

    if (NULL == root) {

        root = (BTNode*)calloc(sizeof(BTNode), 1);

        if (!root) {

            printf("Error! out of memory!\n");

            return;

        }

        root->isLeaf = true;

        root->keynum = 1;

        root->key[0] = key;

 

        *tree = root;

 

        // 写入磁盘

        disk_write(root);

 

        return;

    }

 

    // 根节点已满，插入前需要进行分裂调整

    if (root->keynum == (ORDER-1)) {

        // 产生新节点当作根

        node = (BTNode*)calloc(sizeof(BTNode), 1);

        if (!node) {

            printf("Error! out of memory!\n");

            return;

        }

 

        *tree = node;

        node->isLeaf = false;

        node->keynum = 0;

        node->child[0] = root;

 

        BTree_split_child(node, 0, root);

 

        BTree_insert_nonfull(node, key);

    }

 

    // 根节点未满，在当前节点中插入 key

    else {

        BTree_insert_nonfull(root, key);

    }

}



测试文件：

//测试order序(阶)的B-TREE结构基本操作。

//查找：search，插入：insert，删除：remove。

//创建：create，销毁：destory，打印：print。

#include <stdio.h>
#include "btree.h"

void test_BTree_search(BTree tree, KeyType key)
{
    int pos = -1;
    BTNode*    node = BTree_search(tree, key, &pos);
    if (node) {
        printf("在%s节点（包含 %d 个关键字）中找到关键字 %c，其索引为 %d\n",
            node->isLeaf ? "叶子" : "非叶子",
            node->keynum, key, pos);
    }
    else {
        printf("在树中找不到关键字 %c\n", key);
    }
}

void test_BTree_remove(BTree* tree, KeyType key)
{
    printf("\n移除关键字 %c \n", key);
    BTree_remove(tree, key);
    BTree_print(*tree);
    printf("\n");
}

void test_btree()
{

    KeyType array[] = {
        'G','G', 'M', 'P', 'X', 'A', 'C', 'D', 'E', 'J', 'K',
        'N', 'O', 'R', 'S', 'T', 'U', 'V', 'Y', 'Z', 'F', 'X'
    };
    const int length = sizeof(array)/sizeof(KeyType);
    BTree tree = NULL;
    BTNode* node = NULL;
    int pos = -1;
    KeyType key1 = 'R';        // in the tree.
    KeyType key2 = 'B';        // not in the tree.

    // 创建
    BTree_create(&tree, array, length);

    printf("\n=== 创建 B- 树 ===\n");
    BTree_print(tree);
    printf("\n");

    // 查找
    test_BTree_search(tree, key1);
    printf("\n");
    test_BTree_search(tree, key2);

// 移除不在B树中的元素
test_BTree_remove(&tree, key2);
printf("\n");

    // 插入关键字
    printf("\n插入关键字 %c \n", key2);
    BTree_insert(&tree, key2);
    BTree_print(tree);
    printf("\n");

    test_BTree_search(tree, key2);

    // 移除关键字
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'M';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'E';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'G';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'A';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'D';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'K';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'P';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'J';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'C';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

    key2 = 'X';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'O';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'V';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'R';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'U';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'T';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

key2 = 'N';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);
key2 = 'S';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);
key2 = 'Y';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);
key2 = 'F';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);
key2 = 'Z';
    test_BTree_remove(&tree, key2);
    test_BTree_search(tree, key2);

// 销毁
BTree_destroy(&tree);
}

int main()
{
test_btree();

return 0;

}


BTree:

/**
* Created by Kali on 14-5-26.
*/
public class BTree<K extends Comparable<K>> {

    private final int degree;
    private AbstractBTreeNode<K> root;

    public BTree(int degree){
        if (degree < 2){
            throw new IllegalArgumentException("degree mustn't < 2");
        }
        this.degree = degree;
        root = new BTreeLeaf<>(degree);
    }

    public AbstractBTreeNode<K> getRoot(){
        return root;
    }
    /**
     * Insert a key into B-Tree.
     *
     * @param key key to insert.
     */
    public void insert(K key){
        AbstractBTreeNode<K> n = root;
        if (root.isFull()){
            AbstractBTreeNode<K> newRoot = new BTreeInternalNode<>(degree);
            newRoot.insertChild(n,0);
            newRoot.splitChild(0);
            n = newRoot;
            root = newRoot;
        }
        n.insertNotFull(key);
    }

    /**
     * Delete a key from B-Tree,if key doesn't exist in current tree,will effect nothing.
     *
     * @param key key to delete.
     */
    public void delete(K key){
        AbstractBTreeNode<K> node = root;
        node.deleteNotEmpty(key);
        if (node.nkey() == 0){
            //shrink
            root = node.getChild(0);
            if (root == null){
                root = new BTreeLeaf<>(degree);
            }
        }
    }

    @Override
    public String toString() {
       return AbstractBTreeNode.BTreeToString(this.root);
    }
}



测试：

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import yuanye.datastructure.btree.AbstractBTreeNode;
import yuanye.datastructure.btree.BTree;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.*;

import static org.junit.Assert.assertTrue;

/**
* BTree Tester.
*
* @author <Authors name>
* @version 1.0
* @since <pre>六月 1, 2014</pre>
*/
public class BTreeTest {

    private final List<Integer> keys = new ArrayList<>(12);
    private final List<List<Integer[]>> insertResults = new ArrayList<>(12);
    private final List<List<Integer[]>> deleteResults = new ArrayList<>(12);

    @Before
    public void before() throws Exception {
        int[] ks = new int[]{6, 18, 16, 22, 3, 12, 8, 10, 20, 21, 13, 17};
        for (int i = 0; i < ks.length; i++) {
            keys.add(i, ks[i]);
        }
    }

    @After
    public void after() throws Exception {
        keys.clear();
        insertResults.clear();
    }


    /**
     * Method: insert(K key)
     */
    @Test
    public void testInsert() throws Exception {
        BTree<Integer> tree = new BTree<>(2);
        fillInsertResult();
        for (int i = 0; i < keys.size(); i++) {
            tree.insert(keys.get(i));
            assertTrue(checkBTree(tree, insertResults.get(i)));
        }
    }

    private <K extends Comparable<K>> boolean checkBTree(BTree<K> tree, List<K[]> result)
            throws NoSuchMethodException, InvocationTargetException, IllegalAccessException {

        AbstractBTreeNode<K> node = tree.getRoot();
        Queue<AbstractBTreeNode<K>> nodes = new LinkedList<>();

        Class<?> clazz = node.getClass();
        Method nChild = clazz.getDeclaredMethod("nchild", null);
        Method getChild = clazz.getDeclaredMethod("getChild", int.class);
        Method nKey = clazz.getDeclaredMethod("nkey", null);
        Method getKey = clazz.getDeclaredMethod("getKey", int.class);
        nChild.setAccessible(true);
        getChild.setAccessible(true);
        nKey.setAccessible(true);
        getKey.setAccessible(true);

        int nodeIndex = 0;
        while (node != null) {
            //add children
            int nchild = (Integer) nChild.invoke(node, null);
            for (int i = 0; i < nchild; i++) {
                Object n = (AbstractBTreeNode<K>) getChild.invoke(node, nchild);
                if (n != null) {
                    nodes.offer((AbstractBTreeNode<K>) n);
                }
            }
            K[] nodeKeys = result.get(nodeIndex++);
            //compare keys
            for (int i = 0; i < nodeKeys.length; i++) {
                if (!getKey.invoke(node, i).equals(nodeKeys[i])) {
                    return false;
                }
            }
            node = nodes.poll();
        }
        return true;
    }

    private void fillInsertResult() {

        //tree values after insert keys
        Integer[][][] res = {
                {{6}},                                                            // 6
                {{6, 18}},                                                        // 18
                {{6, 16, 18}},                                                    // 16
                {{16}, {6}, {18, 22}},                                            // 22
                {{16}, {3, 6}, {18, 22}},                                         // 3
                {{16}, {3, 6, 12}, {18, 22}},                                     // 12
                {{6, 16}, {3}, {8, 12}, {18, 22}},                                // 8
                {{6, 16}, {3}, {8, 10, 12}, {18, 22}},                            // 10
                {{6, 16}, {3}, {8, 10, 12}, {18, 20, 22}},                        // 20
                {{6, 16, 20}, {3}, {8, 10, 12}, {18}, {21, 22}},                  // 21
                {{16}, {6, 10}, {20}, {3}, {8}, {12, 13}, {18}, {21, 22}},        // 13
                {{16}, {6, 10}, {20}, {3}, {8}, {12, 13}, {17, 18}, {21, 22}},    // 17
        };

        for (int i = 0; i < 12; i++) {
            insertResults.add(Arrays.asList(res[i]));
        }
    }

    private void fillDeleteResult() {
        //tree values after insert keys
        Integer[][][] res = {
                {{16}, {10}, {20}, {3, 8}, {12, 13}, {17, 18}, {21, 22}},                  // 6
                {{10, 16, 20}, {3, 8}, {12, 13}, {17}, {21, 22}},                          // 18
                {{10, 13, 20}, {3, 8}, {12}, {17}, {21, 22}},                              // 16
                {{10, 13, 20}, {3, 8}, {12}, {17}, {21}},                                  // 22
                {{10, 13, 20}, {8}, {12}, {17}, {21}},                                     // 3
                {{13, 20}, {8, 10}, {17}, {21}},                                           // 12
                {{13, 20}, {10}, {17}, {21}},                                              // 8
                {{20}, {13, 17}, {21}},                                                    // 10
                {{17}, {13}, {21}},                                                        // 20
                {{13, 17}},                                                                // 21
                {{17}},                                                                    // 13
                {{}},                                                                      // 17
        };

        for (int i = 0; i < 12; i++) {
            deleteResults.add(Arrays.asList(res[i]));
        }
    }

    /**
     * Method: delete(K key)
     */
    @Test
    public void testDelete() throws Exception {
        BTree<Integer> bTree = new BTree<>(2);
        for (int key : keys) {
            bTree.insert(key);
        }
        fillDeleteResult();

        for (int i = 0; i < keys.size(); i++) {
            bTree.delete(keys.get(i));
            assertTrue(checkBTree(bTree, deleteResults.get(i)));
        }
    }

} 





C++普通版：

#include <iostream>

using namespace std;

static const int m = 3;  //定义最小度为3

static const int key_max = 2*m-1;//关键字最大个数

static const int key_min = m - 1;//关键字最小个数

static const int child_max = key_max + 1;//子节点最大个数(这里可以看出子节点数与关键字数有关)

static const int child_min = key_min + 1;//子节点最小个数

 

template <class T>

class BTree;//前置声明

/*

类名:BTnode

*/

template <class T>

class BTnode

{

  friend class BTree<T>;//友元函数

public:

 BTnode()

 {

  keyNum = 0;

  parent = NULL;//父节点初始化

        isleaf = true;

  int i ;

  for (i = 0;i < child_max;i++)//子树指针数组初始化

  {

   pchild[i] = NULL;

  }

  for (i = 0;i < key_max;i++)//关键字数组初始化

  {

   keyvalue[i] = '\0';

  }

 }

 private:

 bool isleaf;//判断节点是否是叶子节点

 int keyNum;//关键字个数

 BTnode<T>* parent;//指向父节点

 BTnode<T>* pchild[child_max];//子树指针数组

 T   keyvalue[key_max];//关键字数组

};

 

/*

类名:BTree

*/

template <class T>

class BTree

{

public:

   /*

     函数名:BTree

     函数作用:无参构造函数

     函数参数:无

     函数返回值: 无

     */

  

    BTree()

    {

       root = NULL;

    }

      /*

     函数名:BTree_insert

     函数作用:插入关键字

     函数参数:T value

     函数返回值: bool类型判断插入是否成功

     */

 

    bool BTree_insert(T value)

    {

      if (contain(value))//看树中是否有相同的关键字

      {

         return false;

      }

      else

      {

         if (root == NULL)

         {

           root = new BTnode<T>();

         }

         if (root->keyNum == key_max)

         {

             BTnode<T>* newnode = new BTnode<T>();

             newnode->pchild[0] = root;

             newnode->isleaf = false;//由上步操作可知newnode已经有子节点

             SplitBlock(newnode,0,root);//分块操作

             root = newnode;//更新根节点

         }

         Notfull_insert(root,value);//插入块未满的操作

         return true;

      }

    }

 

    /*

     函数名:SplitBlock

     函数作用:把节点分裂

     函数参数:BTnode<T>* node_parent,int child_index,BTnode<T>* node_child

     函数返回值: 无

     */

    

    void SplitBlock(BTnode<T>* node_parent,int child_index,BTnode<T>* node_child)

    {

       BTnode<T>* node_right = new BTnode<T>();

       node_right->isleaf = node_child->isleaf;

       node_right->keyNum = key_min;

       int i;

       //node_right拷贝关键字

       for (i = 0;i < key_min;i++)

       {

         node_right->keyvalue[i] = node_child->keyvalue[i+child_min];

       }

       //判断分裂的节点是否是叶子节点,如果不是的话就要把它的孩子赋值过去

       if (!node_child->isleaf)

       {

          for (i = 0;i < child_min;i++)

          {

             node_right->pchild[i] = node_child->pchild[i+child_min];

             node_child->pchild[i+child_min]->parent = node_right->pchild[i];

          }

       }

       node_child->keyNum = key_min;//更新子节点的关键字数

       //把父节点关键字和子指针往后移 倒序赋值

       for (i = node_parent->keyNum;i > child_index;i--)

       {

          node_parent->keyvalue[i] = node_parent->keyvalue[i-1];

          node_parent->pchild[i+1] = node_parent->pchild[i];

          node_child->pchild[i]->parent = node_parent->pchild[i+1];

       }

       node_parent->keyNum++;//更新父节点关键字数

       node_parent->pchild[child_index+1] = node_right;

       node_right->parent = node_parent->pchild[child_index+1];

       //把中间的那个关键字上移到父节点

       node_parent->keyvalue[child_index] = node_child->keyvalue[key_min];

    }

 

   

    /*

    函数名:Notfull_insert

    函数作用:往没有满的节点中增加关键字

    函数参数:BTnode<T>* node,T value

    函数返回值:无

    */

    void Notfull_insert(BTnode<T>* node,T value)

    {

 

      int node_keynum = node->keyNum;//获取节点关键字数

      if (node->isleaf)//node是叶子节点

      {

        while (node_keynum > 0 && value < node->keyvalue[node_keynum-1])

        {

         node->keyvalue[node_keynum] = node->keyvalue[node_keynum-1];//把关键字往后移动

         --node_keynum;

        }

        node->keyvalue[node_keynum] = value;

        node->keyNum++;

      }

      else//node是内部节点

      {

         while (node_keynum > 0 && value < node->keyvalue[node_keynum-1])

         {

           --node_keynum;

         }

         //在比它小和比它大中间那个子节点中找合适位置,

         //如果它比所有的都小则在第一个子节点中寻找

         BTnode<T>* node_child = node->pchild[node_keynum];

         if (node_child->keyNum == key_max)

         {

           SplitBlock(node,node_keynum,node_child);

           if (value > node->keyvalue[node_keynum])//插入值和子节点中间的值比较

           {

             node_child = node->pchild[node_keynum+1];

           }

         }

         Notfull_insert(node_child,value);//继续往下寻找合适的位置

      }

    }

    

    /*

    函数名:contain

    函数作用:查找是否有相同元素在数中

    函数参数:T value

    函数返回值:bool类型

    */

   bool contain(T value)

   {

        if (BTree_find(root,value) != NULL)

      return true;

    return false;

   }

   

    /*

    函数名:BTree_find

    函数作用:查找是否有相同元素在数中

    函数参数:BTnode<T>* node,T value

    函数返回值: BTnode<T>*

    */

   BTnode<T>* BTree_find(BTnode<T>* node,T value)

   {

       if (node == NULL)//当前节点为空的时候

       {

          return NULL;

       }

       else//当前节点不为空

       {

         int i;

         //在比它小和比它大的中间子节点中寻找相等的

         for (i = 0; i < node->keyNum ;i++)

         {

            if (value <= node->keyvalue[i])

            {

               break; 

            }

         }

 

         //校验当前的关键字是否等于查找的关键字

         if (i < node->keyNum && value == node->keyvalue[i])//i是下标最大值keyNum-1

         {

            return node;

         }

         else

         {

          //如果之前比查找关键没有相等的关键字并且当前节点是叶子节点的话

          //那么在B树中没有一样的关键字(因为后面比关键字更大)

           if (node->isleaf)

           {

            return NULL;

           }

           else//如果不是叶子节点则在下面的子节点中寻找

           {

             return BTree_find(node->pchild[i],value);//这里的return别忘了

           }

         }

       }

   }

  

    /*

    函数名:printpoint

    函数作用:打印元素

    函数参数:BTnode<T>* node,int count

    函数返回值:无

    */

   void printpoint(BTnode<T>* node,int count)

   {

      if (node != NULL)//判断元素是否为空

      {

         int i,j;

          //每个节点从小到大打印

         for (i = 0;i < node->keyNum; i++)

         {

          //判断是否叶节点,不是的话则要往子节点中寻找打印

           if (!node->isleaf)//判断是否是叶子节点

           {

             printpoint(node->pchild[i],count-3);

           }

           for (j = count;j >= 0; j--)

           {

             cout<<"-";

           }

           cout<<"|"<<node->keyvalue[i]<<"|"<<endl;

        }

        if (!node->isleaf)//子节点数比关键字数多一个

        {

             printpoint(node->pchild[i],count-3);

        }

      }

   }

    /*

    函数名:printpoint

    函数作用:printpoint无参函数传递值过去

    函数参数:无

    函数返回值:无

    */

   void printpoint()

   {

     printpoint(root,key_max*5);

   }

    /*

    函数名:BTree_delete

    函数作用:删除函数

    函数参数:T value

    函数返回值:bool类型

    */

   bool BTree_delete(T value)

   {

      if (!contain(value))

      {

          return false;

      }

      if (root->keyNum == 1)

      {

             if (root->isleaf)

             {

                delete root;

                root = NULL;

                return true;

             }

             else//当根节点只有一个关键字时且不为叶子节点

             {

                BTnode<T>* node_child1 = root->pchild[0];

                BTnode<T>* node_child2 = root->pchild[1];

                //减少一层树的高度

                if (node_child1->keyNum == key_min && node_child2->keyNum == key_min)

                {

                    MergeBlock(root,0);

                    delete root;

                    root = node_child1;

                }

             }

      }

      BTree_deletebalance(root,value);

   }

     /*

    函数名:MergeBlock

    函数作用:合并块

    函数参数:BTnode<T>* node_parent,int child_index

    函数返回值:无

    */

   void MergeBlock(BTnode<T>* node_parent,int child_index)

   {

      BTnode<T>* node_child1 = node_parent->pchild[child_index];

      BTnode<T>* node_child2 = node_parent->pchild[child_index+1];

      //将父节点的节点对应的关键字下移

      node_child1->keyvalue[key_min] = node_parent->keyvalue[child_index];

      node_child1->keyNum = key_max;

      int i;

      for (i = 0;i < key_min;i++)

      {

        node_child1->keyvalue[child_min+i] = node_child2->keyvalue[i];

      }

      //判断node_child1是否是叶子节点,不是叶子节点则要将node_child2的子节点赋值给node_child1

      if (!node_child1->isleaf)

      {

        for (i = 0; i < child_min;i++)

        {

           node_child1->pchild[i+child_min] = node_child2->pchild[i];

        }

      }

      //因为父节点下移一个关键字,则关键字后的所有值要往前移动一个

      node_parent->keyNum--;

      for (i = child_index;i <  node_parent->keyNum;i++)

      {

         node_parent->keyvalue[i] = node_parent->keyvalue[i+1];

         //子节点也要往前移动一位

         node_parent->pchild[i+1] = node_parent->pchild[i+2];

      }

      delete node_child2;

      node_child2 = NULL;

   }

      /*

    函数名:getprev

    函数作用:在左边的兄弟节点中找一个最大的

    函数参数:BTnode<T>* node

    函数返回值:左边兄弟的最大的关键字

    */

   T getprev(BTnode<T>* node)

   {

    //在比节点位置小的节点中找一个最大的值最为

      while (!node->isleaf)

      {

         node = node->pchild[node->keyNum];

      }

      node->keyNum--;

      return node->keyvalue[node->keyNum-1];

   }

      /*

    函数名:getnext

    函数作用:在右边的兄弟节点中找一个最小的

    函数参数:BTnode<T>* node

    函数返回值:右边兄弟的最小的关键字

    */

   T getnext(BTnode<T>* node)

   {

     //在比节点位置大的节点中找一个最小的

     while (!node->isleaf)

      {

         node = node->pchild[0];

      }

     return node->keyvalue[0];

   }

    /*

    函数名:BTree_deletebalance

    函数作用:用递归删除并做修复

    函数参数:BTnode<T>* node

    函数返回值:无

    */

   void BTree_deletebalance(BTnode<T>* node,T value)

   {

     int i;

     //在当前节点中找合适坐标使得value在这个区间内

     for(i = 0;i < node->keyNum && value > node->keyvalue[i];i++)

     {

 

     }

     //如果当前i的关键字等于value

     if (i < node->keyNum && value == node->keyvalue[i])

     {

      //判断当前节点是否是叶子节点,如果是的话直接删除,下面的情况保证了如果value在叶子节点的话,叶子节点keyNum一定是>=child_min

        if (node->isleaf)

        {

           node->keyNum--;

           //把i后面的都往前移动一位

           for (;i < node->keyNum;i++)

           {

             node->keyvalue[i] = node->keyvalue[i+1];

           }

           return;

        }

        else//当前节点为内节点

        {

          //里面关键字都比value小的节点

          BTnode<T>* node_left = node->pchild[i];

          //里面关键字都比value大的节点

          BTnode<T>* node_right = node->pchild[i+1];

          if (node_left->keyNum >= child_min)//左子节点中的关键字数大于等于child_min

          {

             T prev_replace = getprev(node_left);

             //传递不平衡点让pre_replace

             BTree_deletebalance(node_left,prev_replace);

             //让前继的关键字替换当前删除的关键字

             node->keyvalue[i] = prev_replace;

             return;

          }

          else if (node_right->keyNum >= child_min)//右子节点中的关键字数大于等于child_min

          {

             T next_replace = getnext(node_right);

             //在左边中找到最大的那个递归替换

             BTree_deletebalance(node_right,next_replace);

             //让前继的关键字替换当前删除的关键字

             node->keyvalue[i] = next_replace;

             return;

          }

          else//左右子节点中的关键字数都等于key_min

          {

            //合并两个子节点

            MergeBlock(node,i);

            //在合并的节点中删除value值

            BTree_deletebalance(node_left,value);

          }

        }

     }

     else//关键字不在当前节点(下面步骤保证了遍历的所有节点关键字数都是大于等于child_min)

     {

       //在(<<value<<)的区间找

        BTnode<T>* node_child = node->pchild[i];//这里i = node->keyNum

        BTnode<T>*  node_left = NULL;

        BTnode<T>*  node_right = NULL;

        if (node_child->keyNum == key_min)//当前节点只有两个关键字,补给当前节点使之大于等于child_min

        {  

            //判断是否有左右兄弟节点

            if (i >0)

            {

                node_left = node->pchild[i-1];

            }

            if (i <= node->keyNum-1)

            {

               node_right = node->pchild[i+1];

            }

           int j;

           //当前左兄弟的关键字数大于等于child_min

           if (node_left && node_left->keyNum >= child_min)

           {   

                //把父节点的i-1对应的关键字下移

               for (j = node_child->keyNum;j > 0; j--)

               {

                 node_child->keyvalue[j] = node_child->keyvalue[j-1];

               }

               node_child->keyvalue[0] = node->keyvalue[i-1];

               //如果子节点的左兄弟节点不是叶子节点

               if (!node_left->isleaf)

               {

                //把左兄弟最右边的子节点指针赋值给node_child

                  for (j = node_child->keyNum+1;j > 0;j--)

                  {

                    node_child->pchild[j-1]->parent = node_child->pchild[j]->parent;

                    node_child->pchild[j] = node_child->pchild[j-1];

 

                  }

                  node_left->pchild[node_left->keyNum]->parent =  node_child->pchild[0];

                  node_child->pchild[0] = node_left->pchild[node_left->keyNum];

               }

               node_child->keyNum++;

               node->keyvalue[i-1] = node_left->keyvalue[node_left->keyNum-1];

               node_left->keyNum--;

           }

           else if (node_right && node_right->keyNum >= child_min)

           {

               //把父节点的i对应的关键字下移

               node_child->keyvalue[node_child->keyNum] = node->keyvalue[i];

               node_child->keyNum++;

               //把右兄弟节点最小的关键字赋值给父节点的i位置

               node->keyvalue[i] = node_right->keyvalue[0];

               node_right->keyNum--;

               //把右兄弟的关键字往前移动一位

               for (j = 0;j < node_right->keyNum;j++)

               {

                 node_right->keyvalue[j] = node_right->keyvalue[j+1];

               }

               //如果右兄弟不是叶子节点则需要把右兄弟最左边的子节点指针赋值给node_child

               if (!node_right->isleaf)

               {

                node_right->pchild[0]->parent = node_child->pchild[node_child->keyNum]->parent;

                 node_child->pchild[node_child->keyNum] = node_right->pchild[0];

                  for (j = 0;j < node_right->keyNum+1;j++)

                  {

                    node_right->pchild[j+1]->parent = node_right->pchild[j]->parent;

                    node_right->pchild[j] = node_right->pchild[j+1];

 

                  }

                 

               }

           }

           else if (node_left)//node_left只有child_min-1个关键字

           {

            //把左兄弟合并

             MergeBlock(node,i-1);

             //更新子节点

             node_child = node_left;

           }

           else if (node_right)

           {

            //把右兄弟合并,子节点无需更新

             MergeBlock(node,i);

           }

        }

        BTree_deletebalance(node_child,value);

     }

   }

private:

 BTnode<T>* root;//根节点

};






c++迷你版

/*

* C++ Program to Implement B-Tree

*/

#include<stdio.h>

#include<conio.h>

#include<iostream>

using namespace std;

struct BTreeNode

{

    int *data;

    BTreeNode **child_ptr;

    bool leaf;

    int n;

}*root = NULL, *np = NULL, *x = NULL;

BTreeNode * init()

{

    int i;

    np = new BTreeNode;

    np->data = new int[5];

    np->child_ptr = new BTreeNode *[6];

    np->leaf = true;

    np->n = 0;

    for (i = 0; i < 6; i++)

    {

        np->child_ptr[i] = NULL;

    }

    return np;

}

void traverse(BTreeNode *p)

{

    cout<<endl;

    int i;

    for (i = 0; i < p->n; i++)

    {

        if (p->leaf == false)

        {

            traverse(p->child_ptr[i]);

        }

        cout << " " << p->data[i];

    } 

    if (p->leaf == false)

    {

        traverse(p->child_ptr[i]);

    }

    cout<<endl;

}

void sort(int *p, int n)

{

    int i, j, temp;

    for (i = 0; i < n; i++)

    {

        for (j = i; j <= n; j++)

        {

            if (p[i] > p[j])

            {

                temp = p[i];

                p[i] = p[j];

                p[j] = temp;

            }

        }

    }

}

int split_child(BTreeNode *x, int i)

{

    int j, mid;

    BTreeNode *np1, *np3, *y;

    np3 = init();

    np3->leaf = true;

    if (i == -1)

    {

        mid = x->data[2];

        x->data[2] = 0;

        x->n--;

        np1 = init();

        np1->leaf = false;

        x->leaf = true;

        for (j = 3; j < 5; j++)

        {

            np3->data[j - 3] = x->data[j];

            np3->child_ptr[j - 3] = x->child_ptr[j];

            np3->n++;

            x->data[j] = 0;

            x->n--;

        }

        for (j = 0; j < 6; j++)

        {

            x->child_ptr[j] = NULL;

        }

        np1->data[0] = mid;

        np1->child_ptr[np1->n] = x;

        np1->child_ptr[np1->n + 1] = np3;

        np1->n++;

        root = np1;

    }

    else

    {

        y = x->child_ptr[i];

        mid = y->data[2];

        y->data[2] = 0;

        y->n--;

        for (j = 3; j < 5; j++)

        {

            np3->data[j - 3] = y->data[j];

            np3->n++;

            y->data[j] = 0;

            y->n--;

        }

        x->child_ptr[i + 1] = y;

        x->child_ptr[i + 1] = np3; 

    }

    return mid;

}

void insert(int a)

{

    int i, temp;

    x = root;

    if (x == NULL)

    {

        root = init();

        x = root;

    }

    else

    {

        if (x->leaf == true && x->n == 5)

        {

            temp = split_child(x, -1);

            x = root;

            for (i = 0; i < (x->n); i++)

            {

                if ((a > x->data[i]) && (a < x->data[i + 1]))

                {

                    i++;

                    break;

                }

                else if (a < x->data[0])

                {

                    break;

                }

                else

                {

                    continue;

                }

            }

            x = x->child_ptr[i];

        }

        else

        {

            while (x->leaf == false)

            {

            for (i = 0; i < (x->n); i++)

            {

                if ((a > x->data[i]) && (a < x->data[i + 1]))

                {

                    i++;

                    break;

                }

                else if (a < x->data[0])

                {

                    break;

                }

                else

                {

                    continue;

                }

            }

                if ((x->child_ptr[i])->n == 5)

                {

                    temp = split_child(x, i);

                    x->data[x->n] = temp;

                    x->n++;

                    continue;

                }

                else

                {

                    x = x->child_ptr[i];

                }

            }

        }

    }

    x->data[x->n] = a;

    sort(x->data, x->n);

    x->n++;

}

int main()

{

    int i, n, t;

    cout<<"enter the no of elements to be inserted\n";

    cin>>n;

    for(i = 0; i < n; i++)

    {

          cout<<"enter the element\n";

          cin>>t;

          insert(t);

    }

    cout<<"traversal of constructed tree\n";

    traverse(root);

    getch();

}





java：

AbstractBTreeNode：

import java.util.LinkedList;

import java.util.Queue;

 

/**

 * Created by Kali on 14-5-26.\

 * Abstract node.

 */

public abstract class AbstractBTreeNode<K extends Comparable<K>> {

 

    protected final int degree;

 

    AbstractBTreeNode(int degree) {

        if (degree < 2) {

            throw new IllegalArgumentException("degree must >= 2");

        }

        this.degree = degree;

    }

 

    /**

     * If the node is leaf.

     *

     * @return true if is leaf,false if not.

     */

     abstract boolean isLeaf();

 

    /**

     * Search key in the B-Tree.

     *

     * @param key the key to search

     * @return key in the B-tree or null if key does not exist in the tree.

     */

    abstract K search(K key);

 

    /**

     * Insert a key in to a node when the node is not full.

     *

     * @param key the key to insert

     * @throws java.lang.RuntimeException if node is full

     */

    abstract void insertNotFull(K key);

 

    /**

     * <p>Delete a key when the {@code keys >= degree}.</p>

     * <p>If key to delete does not exist in current node,internal node will find a subtree tree the key

     * may exist in,find the key in subtree and delete;the leaf will do nothing if the key to delete

     * does not exist.</p>

     *

     * @param key the key to delete.

     */

    abstract void deleteNotEmpty(K key);

 

    /**

     * <p>Insert a key in to B-Tree.</p>

     * <p>Insert a key into current node.</p>

     *

     * @param key the key to insert

     * @throws java.lang.RuntimeException if current is full.

     */

    void insertKey(K key) {

        checkNotFull();

        int i = this.nkey();

        //move keys

        while (i > 0 && key.compareTo(this.getKey(i - 1)) < 0) {

            this.setKey(this.getKey(i - 1), i);

            i--;

        }

        this.setKey(key, i);

        this.setNKey(this.nkey() + 1);

    }

 

    /**

     * <p>Get a key with index of {@code idx} in current node</p>

     *

     * @param idx idx of key to get.

     * @return key of given index

     * @throws java.lang.RuntimeException if {@code idx < 0 } or {@code idx >= degree *2 -1}

     */

     abstract K getKey(int idx);

 

    /**

     * <p>Delete given key in current node.</p>

     *

     * @param key the key to delete.

     * @return the key deleted or null if key does not exist.

     */

    protected K deleteKey(K key) {

        int index = indexOfKey(key);

        if (index >= 0) {

            return this.deleteKey(index);

        }

        return null;

    }

 

    /**

     * <p>Delete a key with given index.</p>

     *

     * @param index index of key to delete

     * @return key deleted or null if index is invalid.

     * @throws java.lang.RuntimeException if index is invalid

     */

    protected K deleteKey(int index) {

        if (index < 0 || index >= this.nkey()) {

            throw new RuntimeException("Index is invalid.");

        }

        K result = this.getKey(index);

        while (index < this.nkey() - 1) {

            this.setKey(this.getKey(index + 1), index);

            index++;

        }

        this.setKey(null, this.nkey() - 1);

        this.setNKey(this.nkey() - 1);

        return result;

 

    }

 

    /**

     * <p>Check if current exists given key</p>

     *

     * @param key key to check

     * @return true is given key exists in current node.

     */

     boolean existsKey(K key) {

        return indexOfKey(key) >= 0;

    }

 

    /**

     * Replace one key with newKey

     *

     * @param oldKey the key to replace

     * @param newKey the new key to insert in

     */

    protected void replaceKey(K oldKey, K newKey) {

        int index = indexOfKey(oldKey);

        if (index >= 0) {

            setKey(newKey, index);

        }

    }

 

    /**

     * Replace given index key with a new key

     *

     * @param newKey the new key to insert in

     * @param oldKeyIndex old key index

     * @return the key be replaced or null if index is invalid

     */

    protected abstract K setKey(K newKey, int oldKeyIndex);

 

    /**

     * Set one of current child with given index.

     *

     * @param sub child subtree

     * @param index index of child to set

     */

    protected abstract void setChild(AbstractBTreeNode<K> sub, int index);

 

    /**

     * Insert a child at given index.

     *

     * @param sub child subtree to insert

     * @param index index of child to insert

     */

    protected void insertChild(AbstractBTreeNode<K> sub, int index) {

        int i = this.nchild();

        while (i > index) {

            this.setChild(this.getChild(i - 1), i);

            i--;

        }

        this.setChild(sub, index);

        this.setNChild(this.nchild() + 1);

    }

 

    /**

     * Get child with given index.

     *

     * @param index index of child to get

     * @return child subtree of null if index is invalid

     */

     abstract AbstractBTreeNode<K> getChild(int index);

 

    /**

     * Delete given child in current node.

     *

     * @param child child subtree to delete.

     */

    protected void deleteChild(AbstractBTreeNode<K> child) {

        int index = -1;

        for (int i = 0; i < nchild(); i++) {

            if (this.getChild(i) == child) {

                index = i;

                break;

            }

        }

        if (index >= 0) {

            deleteChild(index);

        }

    }

 

    /**

     * Delete child with given index

     *

     * @param index index of child to delete

     * @return child subtree or null if index is invalid

     */

    protected AbstractBTreeNode<K> deleteChild(int index) {

        AbstractBTreeNode<K> result = null;

        if (index >= 0 && index < this.nchild()) {

            result = this.getChild(index);

            while (index < this.nchild() - 1) {

                this.setChild(this.getChild(index + 1), index);

                index++;

            }

            this.setChild(null, index);

            this.setNChild(this.nchild() - 1);

        }

        return result;

    }

 

    /**

     * Split a full child to two child node.

     *

     * @param child child index to split

     * @throws java.lang.RuntimeException is child to spilt is not full

     */

    protected abstract void splitChild(int child);

 

    /**

     * Split current node to two node.

     *

     * @param newNode new node

     * @return middle of current node before split

     * @throws java.lang.RuntimeException if current node is not full.

     */

    protected abstract K splitSelf(AbstractBTreeNode<K> newNode);

 

    /**

     * Merge current node with another .

     *

     * @param middle middle key of the two node

     * @param sibling sibling node to merge

     * @throws java.lang.RuntimeException if keys of either node exceed degree-1.

     */

    protected abstract void merge(K middle, AbstractBTreeNode<K> sibling);

 

    /**

     * Set key amount of current node.

     *

     * @param nkey key amount to set

     * @return old key amount

     */

    protected abstract int setNKey(int nkey);

 

    /**

     * Key amount of current node.

     *

     * @return key amount of current node.

     */

     abstract int nkey();

 

    /**

     * Child amount of current node.

     *

     * @return child amount.

     */

     abstract int nchild();

 

    /**

     * Set child amount of current node.

     *

     * @param nchild child amount.

     * @return old child amount.

     */

    protected abstract int setNChild(int nchild);

 

    /**

     * Get index of given key.

     *

     * @param key the key to get.

     * @return index of key or -1 if key does not exist in current node.

     */

    protected int indexOfKey(K key) {

        for (int i = 0; i < this.nkey(); i++) {

            if (key.equals(this.getKey(i))) {

                return i;

            }

        }

        return -1;

    }

 

    /**

     * Check whether current node is full.

     *

     * @return true if current node is full,else false.

     */

    protected boolean isFull() {

        return nkey() == degree * 2 - 1;

    }

 

    /**

     * Check current node is not full.

     *

     * @throws java.lang.RuntimeException if current is full.

     */

    protected final void checkNotFull() {

        if (isFull()) {

            throw new RuntimeException(this.toString() + " is full.");

        }

    }

 

    /**

     * Recursively traverse the B-Tree,constitute a string.

     *

     * @param root root of B-Tree.

     * @param <K> Type of key of B-Tree

     * @return String of B-Tree.

     */

    static <K extends Comparable<K>> String BTreeToString(AbstractBTreeNode<K> root){

        StringBuffer sb = new StringBuffer();

        AbstractBTreeNode node;

        Queue<AbstractBTreeNode> queue = new LinkedList<>();

        queue.add(root);

        String newLine = System.getProperty("line.separator");

        while (!queue.isEmpty()){

            node = queue.poll();

            sb.append(node).append(newLine);

            int i = 0;

            while (node.getChild(i) != null){

                queue.offer(node.getChild(i));

                i++;

            }

        }

        return sb.toString();

    }

}


BTreeInternalNode：


/**

* Created by Kali on 14-5-26.

*/
class BTreeInternalNode<K extends Comparable<K>> extends AbstractBTreeNode<K> {

    private final Object[] keys;
    private final AbstractBTreeNode<K>[] children;
    private int nkey = 0;
    private int nchild = 0;

    BTreeInternalNode(int degree) {
        super(degree);
        keys = new Object[2 * degree - 1];
        children = new AbstractBTreeNode[2 * degree];
    }

    @Override
    protected boolean isLeaf() {
        return false;
    }


    @Override
    K getKey(int idx) {
        return (K) keys[idx];
    }

    @Override
    protected K setKey(K newKey, int oldKeyIndex) {
        K old = (K) keys[oldKeyIndex];
        keys[oldKeyIndex] = newKey;
        return old;
    }

    @Override
    protected void setChild(AbstractBTreeNode<K> sub, int index) {
        children[index] = sub;
    }

    @Override
    AbstractBTreeNode<K> getChild(int index) {
        if (index >= 0 && index < children.length) {
            return children[index];
        }
        return null;
    }

    @Override
    protected int setNKey(int nkey) {
        int old = this.nkey;
        this.nkey = nkey;
        return old;
    }

    @Override
    int nkey() {
        return nkey;
    }

    @Override
    int nchild() {
        return nchild;
    }

    @Override
    protected int setNChild(int nchild) {
        int old = this.nchild;
        this.nchild = nchild;
        return old;
    }


    @Override
    K search(K key) {
        int index = indexOfKey(key);
        if (index >= 0) {
            return (K) keys[index];
        }
        index = 0;
        while (key.compareTo((K) keys[index++]) > 0) ;
        return children[index].search(key);
    }

    @Override
    void insertNotFull(K key) {
        checkNotFull();
        int i = 0;
        while (i < nkey && key.compareTo((K) keys[i]) >= 0) {
            i++;
        }
        if (this.children[i].isFull()) {
            this.splitChild(i);
            if (key.compareTo((K) this.keys[i]) > 0) {
                i++;
            }
        }

        this.children[i].insertNotFull(key);
    }

    @Override
    void deleteNotEmpty(K key) {
        //key in this node
        if (this.existsKey(key)) {
            int index = indexOfKey(key);
            AbstractBTreeNode<K> node;
            //predecessor child could delete
            if ((node = children[index]).nkey() >= degree) {
                K repKey = node.getKey(node.nkey() - 1);                  //maximum key in predecessor
                node.deleteNotEmpty(repKey);
                setKey(repKey, index);
            }
            //follow child could delete a key
            else if ((node = children[index + 1]).nkey() >= degree) {
                K repKey = node.getKey(0);                              //minimum key in follow
                node.deleteNotEmpty(repKey);
                setKey(repKey, index);
            }

            //merge predecessor with follow
            else {
                node = children[index];
                node.merge(key, children[index + 1]);
                this.deleteKey(index);
                this.deleteChild(index + 1);
                node.deleteNotEmpty(key);
            }
    }

    //key may exist in child
    else{
        int i = 0;
        //find proper child the key may exists in
        while (i < nkey) {
            if (key.compareTo((K) keys[i]) < 0)
                break;
            i++;
        }
        AbstractBTreeNode<K> target = children[i];
        //child has enough key
        if (target.nkey() >= degree) {
            target.deleteNotEmpty(key);
        } else {
            AbstractBTreeNode<K> sibling;
            //try to find replacement from predecessor
            if (i > 0 && (sibling = children[i - 1]).nkey() >= degree) {
                if (!target.isLeaf()) {
                    AbstractBTreeNode<K> sub = sibling.deleteChild(nchild); //last child
                    target.insertChild(sub, 0);
                }
                K repKey = sibling.deleteKey(sibling.nkey() - 1);    //maximum key
                repKey = setKey(repKey, i - 1);
                target.insertKey(repKey);
                target.deleteNotEmpty(key);
            }
            //try to find replacement from follower
            else if (i < nkey && (sibling = children[i + 1]).nkey() >= degree) {
                if (!target.isLeaf()) {
                    AbstractBTreeNode<K> sub = sibling.deleteChild(0);  //first child
                    target.insertChild(sub, target.nchild());
                }
                K repKey = sibling.deleteKey(0);                    //minimum key
                repKey = setKey(repKey, i);
                target.insertKey(repKey);
                target.deleteNotEmpty(key);
            }
            //merge child with one of it's sibling
            else {
                //merge with predecessor sibling
                if (i > 0) {
                    K repKey = this.deleteKey(i - 1);
                    sibling = children[i - 1];
                    sibling.merge(repKey, target);
                    this.deleteChild(target);
                    sibling.deleteNotEmpty(key);
                } else {
                    K repKey = this.deleteKey(i);
                    sibling = children[i + 1];
                    target.merge(repKey, sibling);
                    deleteChild(i + 1);
                    target.deleteNotEmpty(key);
                }
            }
        }
    }

}

    @Override
    protected void splitChild(int child) {
        AbstractBTreeNode<K> old = children[child];
        AbstractBTreeNode<K> neo = old.isLeaf()
                ? new BTreeLeaf<>(degree)
                : new BTreeInternalNode<>(degree);
        K middle = old.splitSelf(neo);
        this.insertKey(middle);
        this.insertChild(neo, child + 1);
    }

    @Override
    protected K splitSelf(AbstractBTreeNode<K> newNode) {
        if (!(newNode instanceof BTreeInternalNode)) {
            throw new RuntimeException("Instance not match.");
        }
        if (!isFull()) {
            throw new RuntimeException("Node is not full");
        }

        K middle = (K) keys[degree - 1];
        BTreeInternalNode<K> node = (BTreeInternalNode) newNode;
        int i = 0;
        while (i < degree - 1) {
            node.keys[i] = this.keys[i + degree];
            this.keys[i + degree] = null;
            i++;
        }
        this.keys[degree - 1] = null;

        i = 0;
        while (i < degree) {
            node.children[i] = this.children[i + degree];
            this.children[i + degree] = null;
            i++;
        }

        this.nkey = degree - 1;
        node.nkey = degree - 1;
        this.nchild = degree;
        node.nchild = degree;
        return middle;
    }

    @Override
    protected void merge(K middle, AbstractBTreeNode<K> sibling) {
        if (!(sibling instanceof BTreeInternalNode)) {
            throw new RuntimeException("Sibling is not leaf node");
        }
        BTreeInternalNode node = (BTreeInternalNode) sibling;
        this.insertKey(middle);
        for (int i = 0; i < node.nkey(); i++) {
            this.insertKey((K) node.keys[i]);
        }
        for (int i = 0; i < node.nchild(); i++) {
            insertChild(node.children[i], i + degree);
        }
    }


    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append(" internal node ---- ").append("size: ").append(nkey).append(" keys:").append("[");
        for (int i = 0; i < nkey; i++) {
            sb.append(keys[i]).append(",");
        }
        sb.append("]");
        return sb.toString();
    }
}


BTreeLeaf:


/**
* Created by Kali on 14-5-26.
*/
public class BTreeLeaf<K extends Comparable<K>> extends AbstractBTreeNode<K> {

    private final Object[] keys;
    private int nkey;

    BTreeLeaf(int degree){
        super(degree);
        keys = new Object[2*degree - 1];
    }


    @Override
    protected boolean isLeaf() {
        return true;
    }

    @Override
    K search(K key) {
        int index = indexOfKey(key);
        if (index >=0)
            return (K) keys[index];
        return null;
    }

    @Override
    K getKey(int idx) {
        return (K) keys[idx];
    }

    @Override
    protected K setKey(K newKey, int oldKeyIndex) {
        K old = (K) keys[oldKeyIndex];
        keys[oldKeyIndex] = newKey;
        return old;
    }

    @Override
    protected void setChild(AbstractBTreeNode<K> sub, int index) {
        throw new RuntimeException("Could not set child of leaf node.");
    }

    @Override
    AbstractBTreeNode<K> getChild(int index) {
        return null;
    }

    @Override
    protected void splitChild(int child) {
        throw new  RuntimeException("Could not split child of leaf node.");
    }

    @Override
    protected int setNKey(int nkey) {
        int old = this.nkey;
        this.nkey = nkey;
        return old;
    }

    @Override
    int nkey() {
        return nkey;
    }

    @Override
    int nchild() {
        return 0;
    }

    @Override
    protected int setNChild(int nchild) {
        throw new RuntimeException("Could set NChild of leaf node.");
    }

    @Override
    void insertNotFull(K key) {
        checkNotFull();
        this.insertKey(key);
    }

    @Override
    void deleteNotEmpty(K key) {
        this.deleteKey(key);
    }

    @Override
    protected K splitSelf(AbstractBTreeNode<K> newNode) {
        if (! (newNode instanceof BTreeLeaf)){
            throw new RuntimeException("Instance not match.");
        }
        if (!isFull()){
            throw new RuntimeException("Node is not full");
        }

        K middle = (K)keys[degree -1];
        BTreeLeaf<K> node = (BTreeLeaf)newNode;
        int i = 0;
        while (i < degree-1){
            node.keys[i] = this.keys[i + degree];
            this.keys[i + degree] = null;
            i++;
        }
        this.keys[degree -1] = null;
        this.nkey = degree -1;
        node.nkey = degree -1;
        return middle;
    }

    @Override
    protected void merge(K middle, AbstractBTreeNode<K> sibling) {
        if ( !(sibling instanceof BTreeLeaf)){
            throw new RuntimeException("Sibling is not leaf node");
        }
        BTreeLeaf node = (BTreeLeaf)sibling;
        this.insertKey(middle);
        for (int i = 0; i < node.nkey(); i++){
            this.insertKey((K)node.keys[i]);
        }
    }

    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append("leaf----").append("size: ").append(nkey).append(" keys:").append("[");
        for(int i = 0; i < nkey; i++){
            sb.append(keys[i]).append(",");
        }
        sb.append("]");
        return sb.toString();
    }

}




