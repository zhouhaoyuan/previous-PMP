#include "hash_table.h"

void hash_table::init_hash_table()
{

    if(vector_node.size()==size)//judeg the vector_node is full or not
    {
        std::vector<table_node*> new_node;
        size_t new_size=get_size();
        new_node.resize(new_size);

        for(int i=0;i<vector_node.size();++i)
        {
            table_node *cur=vector_node[i];

            while(cur!=NULL)
            {
                size_t index=fun_index(cur->_key);
                table_node *tmp=cur;
                cur=cur->next;

                table_node *value=new_node[index];
                if(value==NULL)
                    new_node[index]=tmp;
                else
                {
                    tmp->next=value->next;
                    value->next=tmp;
                }
            }

        }
        vector_node.swap(new_node);
    }

}

void hash_table::show()
{
    for(unsigned int i=0;i<vector_node.size();++i)
    {
        std::cout<<"Num "<<i+1<<" : ";
        table_node *cur=vector_node[i];
        while(cur !=NULL)
        {
            std::cout<<cur->_value<<"-->";
            cur=cur->next;
        }
        std::cout<<" NULL"<<std::endl;
    }
}

