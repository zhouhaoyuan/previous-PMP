#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>

struct table_node
{
    int _key;
    double _value;
    struct table_node *next;

    table_node(const int key,const double value)
        :_key(key),_value(value),next(NULL)
    {}
};

class hash_table
{

private:
    std::vector<table_node*> vector_node;
    size_t size;
    int i=0;

private:
    size_t fun_index(const int key)
    {
        return key % vector_node.size();
    }

    size_t get_size();

    void init_hash_table();


public:

    hash_table(size_t sz=0):size(sz)
    {}

    void insert(const int key,const double value);

    void show();

    void remove(const int &key);

    double find(const int &key);

    bool isNull(){
        if(vector_node.empty())
            return true;
    }
};

inline size_t hash_table::get_size()
{
    const int len=22;
    size_t arr[len]={53,97,193,389,769,1543,3079,6151,12289,24593,
                           49157,98317,196613,393241,786433,1572869,3145739,
                           6291469,12582917,25165843,50331653,100663319};
    return arr[i++];
}

inline void hash_table::insert(const int key,const double value)
{
    init_hash_table();

    table_node *tmp= new table_node(key,value);

    size_t index = fun_index(key);
    table_node *cur=vector_node[index];

    if(cur==NULL)
        vector_node[index]=tmp;
    else if(cur!=NULL)
    {
        tmp->next=cur->next;
        cur->next=tmp;
    }
    size++;
}
inline void hash_table::remove(const int &key)
{
    size_t index=fun_index(key);
    table_node *cur = vector_node[index];
    if(cur !=NULL)
    {
        if(cur->next==NULL)
        {
            delete vector_node[index];
            vector_node[index]=NULL;
        }
        else
        {
            table_node *tmp=cur->next;
            cur->next=tmp->next;
            delete tmp;
        }
    }
}
inline double hash_table::find(const int &key)
{
    size_t index=fun_index(key);
    table_node *cur = vector_node[index];
    if(cur !=NULL)
    {
        if(cur->next==NULL)
        {
            return cur->_value;
        }
        else
        {
            return cur->_value;
        }
    }
}


#endif // HASH_TABLE_H
