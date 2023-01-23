#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define LENGTH 10000
#define MIN_NODE_KEY 2
#define MAX_NODE_KEY 5
#define MIN_NODE_CHILD 3
#define MAX_NODE_CHILD 6

//--------------- B Tree Structures ---------------//
typedef struct node_key
{
    int reg_key;
    int reg_index;
} node_key;

typedef struct node
{
    struct node_key node_keys[MAX_NODE_KEY];
    struct node *node_children[MAX_NODE_CHILD];
    struct node *parent;
    int current_num_keys;
    bool leaf;
} node;

typedef struct coordinates
{
    struct node *x;
    int y;
} coordinates;

//--------------- Functions Declaration ---------------//
node *create_node();
void traversal(node *no);
void traversal_2(node *no, FILE *data_file);
coordinates search(node *no, int k);
void split(node *x, int i);
void insert_2(node *no, int k, int id);
void insert_1(node **root, int k, int id);
node *predecessor(node *no);
node *successor(node *no);
node *fit(node *no, int i, node **root);
node *delete_2(node *no, int pos, node **root);
void delete_1(node *no, int k, node **root);
void destroy(node *root);
void queue_construct();
void queue_push(node *new_node);
void queue_pop();
int queue_empty();
int queue_full();
int pull_data_file(FILE *data_file, node **root);
void push_id_file(FILE *index_file, node *root);
void list(node *no, FILE *data_file);
void operations();

//--------------- Queue ---------------//
int front;
int back;
node *queue[LENGTH];
void queue_construct()
{
    back = -1;
    front = 0;
}

void queue_push(node *new_node)
{
    ++back;
    queue[back] = new_node;
}

void queue_pop()
{
    ++front;
}

int queue_empty()
{
    if (back < front)
        return 1;

    else
        return 0;
}

int queue_full()
{
    if (back > LENGTH - 1)
        return 1;

    else
        return 0;
}
//--------------- B Tree Functions ---------------//

node *create_node()
{
    node *no = (node *)malloc(sizeof(node));
    no->leaf = true;
    no->current_num_keys = 0;
    no->parent = NULL;
    for (int i = 0; i < MAX_NODE_CHILD; ++i)
        no->node_children[i] = NULL;

    for (int i = 0; i < MAX_NODE_KEY; ++i)
    {
        no->node_keys[i].reg_key = 123456789;
        no->node_keys[i].reg_index = 123456789;
    }

    return no;
}

void traversal(node *no)
{
    int i;
    if (no != NULL)
    {
        for (i = 0; i < no->current_num_keys; ++i)
        {
            traversal(no->node_children[i]);
            printf("(%d, %d) ", no->node_keys[i].reg_key, no->node_keys[i].reg_index);
        }
        traversal(no->node_children[i]);
    }
}

coordinates search(node *no, int k)
{
    coordinates pos;
    int i = 0;
    while (i < no->current_num_keys && k > no->node_keys[i].reg_key)
        ++i;

    if (i < no->current_num_keys && k == no->node_keys[i].reg_key)
    {
        pos.x = no;
        pos.y = i;
        return pos;
    }
    else if (no->leaf == true)
    {
        pos.x = NULL;
        pos.y = -1;
        return pos;
    }
    return search(no->node_children[i], k);
}

void split(node *x, int i)
{
    node *z = create_node();
    node *y = x->node_children[i];
    z->leaf = y->leaf;
    z->parent = y->parent;
    z->current_num_keys = MIN_NODE_KEY;

    for (int j = 1; j <= MIN_NODE_KEY; ++j)
    {
        z->node_keys[j - 1].reg_key = y->node_keys[j + MIN_NODE_KEY].reg_key;
        z->node_keys[j - 1].reg_index = y->node_keys[j + MIN_NODE_KEY].reg_index;
    }

    if (!y->leaf)
        for (int j = 0; j < MIN_NODE_CHILD; ++j)
        {
            z->node_children[j] = y->node_children[j + MIN_NODE_CHILD];
            y->node_children[j + MIN_NODE_CHILD]->parent = z;
            y->node_children[j + MIN_NODE_CHILD] = NULL;
        }

    y->current_num_keys = MIN_NODE_KEY;

    for (int j = MAX_NODE_CHILD - 1; j > i + 1; --j)
        x->node_children[j] = x->node_children[j - 1];

    x->node_children[i + 1] = z;

    for (int j = MAX_NODE_KEY - 1; j > i; --j)
    {
        x->node_keys[j].reg_key = x->node_keys[j - 1].reg_key;
        x->node_keys[j].reg_index = x->node_keys[j - 1].reg_index;
    }

    x->node_keys[i].reg_key = y->node_keys[MIN_NODE_KEY].reg_key;
    x->node_keys[i].reg_index = y->node_keys[MIN_NODE_KEY].reg_index;
    ++x->current_num_keys;
}

void insert_2(node *no, int k, int id)
{
    int i = no->current_num_keys - 1;

    if (no->leaf)
    {
        while (i >= 0 && k < no->node_keys[i].reg_key)
        {
            no->node_keys[i + 1].reg_key = no->node_keys[i].reg_key;
            no->node_keys[i + 1].reg_index = no->node_keys[i].reg_index;
            --i;
        }
        no->node_keys[i + 1].reg_key = k;
        no->node_keys[i + 1].reg_index = id;
        ++no->current_num_keys;
    }

    else
    {
        while (i >= 0 && k < no->node_keys[i].reg_key)
            --i;

        ++i;
        if (no->node_children[i]->current_num_keys == MAX_NODE_KEY)
        {
            split(no, i);

            if (k > no->node_keys[i].reg_key)
                ++i;
        }
        insert_2(no->node_children[i], k, id);
    }
}

void insert_1(node **root, int k, int id)
{
    if ((*root)->current_num_keys == MAX_NODE_KEY)
    {
        node *new_root = create_node();
        new_root->leaf = false;
        new_root->node_children[0] = (*root);
        (*root)->parent = new_root;
        split(new_root, 0);
        insert_2(new_root, k, id);
        (*root) = new_root;
    }

    else
        insert_2((*root), k, id);
}

node *predecessor(node *no)
{
    while (!no->leaf)
        no = no->node_children[no->current_num_keys - 1];

    return no;
}

node *successor(node *no)
{
    while (!no->leaf)
        no = no->node_children[0];

    return no;
}

node *fit(node *no, int i, node **root)
{
    node *x = no->node_children[i];
    node *left_sib;
    node *right_sib;

    if (i == 0)
        left_sib = NULL;
    else
        left_sib = no->node_children[i - 1];
    if (i == MAX_NODE_CHILD - 1)
        right_sib = NULL;
    else
        right_sib = no->node_children[i + 1];

    if (left_sib != NULL && left_sib->current_num_keys > MIN_NODE_KEY)
    {
        for (int j = MAX_NODE_KEY - 1; j > 0; --j)
        {
            x->node_keys[j].reg_key = x->node_keys[j - 1].reg_key;
            x->node_keys[j].reg_index = x->node_keys[j - 1].reg_index;
        }
        x->node_keys[0].reg_key = no->node_keys[i - 1].reg_key;
        x->node_keys[0].reg_index = no->node_keys[i - 1].reg_index;
        ++x->current_num_keys;

        no->node_keys[i - 1].reg_key = left_sib->node_keys[left_sib->current_num_keys - 1].reg_key;
        no->node_keys[i - 1].reg_index = left_sib->node_keys[left_sib->current_num_keys - 1].reg_index;
        --left_sib->current_num_keys;

        for (int j = MAX_NODE_CHILD - 1; j > 0; --j)
            x->node_children[j] = x->node_children[j - 1];
        x->node_children[0] = left_sib->node_children[left_sib->current_num_keys + 1];
        left_sib->node_children[left_sib->current_num_keys + 1] = NULL;
    }

    else if (right_sib != NULL && right_sib->current_num_keys > MIN_NODE_KEY)
    {
        x->node_keys[x->current_num_keys].reg_key = no->node_keys[i].reg_key;
        x->node_keys[x->current_num_keys].reg_index = no->node_keys[i].reg_index;
        ++x->current_num_keys;

        no->node_keys[i].reg_key = right_sib->node_keys[0].reg_key;
        no->node_keys[i].reg_index = right_sib->node_keys[0].reg_index;

        for (int j = 1; j < MAX_NODE_KEY; ++j)
        {
            right_sib->node_keys[j - 1].reg_key = right_sib->node_keys[j].reg_key;
            right_sib->node_keys[j - 1].reg_index = right_sib->node_keys[j].reg_index;
        }
        --right_sib->current_num_keys;

        x->node_children[x->current_num_keys] = right_sib->node_children[0];
        for (int j = 1; j < MAX_NODE_CHILD; ++j)
            right_sib->node_children[j - 1] = right_sib->node_children[j];
        right_sib->node_children[MAX_NODE_CHILD - 1] = NULL;
    }

    else
    {
        if (left_sib != NULL)
        {
            left_sib->node_keys[left_sib->current_num_keys].reg_key = no->node_keys[i - 1].reg_key;
            left_sib->node_keys[left_sib->current_num_keys].reg_index = no->node_keys[i - 1].reg_index;

            for (int j = 1; j <= MIN_NODE_KEY; ++j)
            {
                left_sib->node_keys[j + MIN_NODE_KEY].reg_key = x->node_keys[j - 1].reg_key;
                left_sib->node_keys[j + MIN_NODE_KEY].reg_index = x->node_keys[j - 1].reg_index;
            }
            left_sib->current_num_keys = MAX_NODE_KEY;

            for (int j = 0; j < MIN_NODE_CHILD; ++j)
            {
                left_sib->node_children[j + MIN_NODE_CHILD] = x->node_children[j];
                if (x->node_children[j] != NULL)
                    x->node_children[j]->parent = left_sib;
            }
            no->node_children[i] = NULL;
            free(x);

            for (int j = i; j < MAX_NODE_KEY; ++j)
            {
                no->node_keys[j - 1].reg_key = no->node_keys[j].reg_key;
                no->node_keys[j - 1].reg_index = no->node_keys[j].reg_index;
            }
            --no->current_num_keys;

            for (int j = i + 1; j < MAX_NODE_CHILD; ++j)
                no->node_children[j - 1] = no->node_children[j];
            no->node_children[MAX_NODE_CHILD - 1] = NULL;

            if (no->parent == NULL && no->current_num_keys == 0)
            {
                left_sib->parent = NULL;
                (*root) = left_sib;
                no = left_sib;
            }
        }
        else
        {
            x->node_keys[x->current_num_keys].reg_key = no->node_keys[i].reg_key;
            x->node_keys[x->current_num_keys].reg_index = no->node_keys[i].reg_index;
            for (int j = 1; j <= MIN_NODE_KEY; ++j)
            {
                x->node_keys[j + MIN_NODE_KEY].reg_key = right_sib->node_keys[j - 1].reg_key;
                x->node_keys[j + MIN_NODE_KEY].reg_index = right_sib->node_keys[j - 1].reg_index;
            }
            x->current_num_keys = MAX_NODE_KEY;

            for (int j = 0; j < MIN_NODE_CHILD; ++j)
            {
                x->node_children[j + MIN_NODE_CHILD] = right_sib->node_children[j];
                if (right_sib->node_children[j] != NULL)
                    right_sib->node_children[j]->parent = x;
            }
            no->node_children[i + 1] = NULL;
            free(right_sib);

            for (int j = i + 1; j < MAX_NODE_KEY; ++j)
            {
                no->node_keys[j - 1].reg_key = no->node_keys[j].reg_key;
                no->node_keys[j - 1].reg_index = no->node_keys[j].reg_index;
            }
            --no->current_num_keys;

            for (int j = i + 2; j < MAX_NODE_CHILD; ++j)
                no->node_children[j - 1] = no->node_children[j];
            no->node_children[MAX_NODE_CHILD - 1] = NULL;

            if (no->parent == NULL && no->current_num_keys == 0)
            {
                x->parent = NULL;
                (*root) = x;
                no = x;
            }
        }
    }

    return no;
}

node *delete_2(node *no, int pos, node **root)
{
    node *left_child;
    node *right_child;

    if (no->leaf)
    {
        for (int i = pos + 1; i < MAX_NODE_KEY; ++i)
        {
            no->node_keys[i - 1].reg_key = no->node_keys[i].reg_key;
            no->node_keys[i - 1].reg_index = no->node_keys[i].reg_index;
        }
        --no->current_num_keys;
    }

    else
    {
        int key, id;
        left_child = no->node_children[pos];
        right_child = no->node_children[pos + 1];

        if (left_child != NULL && left_child->current_num_keys > MIN_NODE_KEY)
        {
            node *pred = predecessor(left_child);
            key = pred->node_keys[pred->current_num_keys - 1].reg_key;
            id = pred->node_keys[pred->current_num_keys - 1].reg_index;
            delete_1((*root), key, root);
            no->node_keys[pos].reg_key = key;
            no->node_keys[pos].reg_index = id;
        }

        else if (right_child != NULL && right_child->current_num_keys > MIN_NODE_KEY)
        {
            node *succ = successor(right_child);
            key = succ->node_keys[0].reg_key;
            id = succ->node_keys[0].reg_index;
            delete_1((*root), key, root);
            no->node_keys[pos].reg_key = key;
            no->node_keys[pos].reg_index = id;
        }

        else
        {
            key = no->node_keys[pos].reg_key;
            left_child = no->node_children[pos];
            right_child = no->node_children[pos + 1];

            left_child->node_keys[left_child->current_num_keys].reg_key = no->node_keys[pos].reg_key;
            left_child->node_keys[left_child->current_num_keys].reg_index = no->node_keys[pos].reg_index;
            for (int i = 1; i <= MIN_NODE_KEY; ++i)
            {
                left_child->node_keys[i + MIN_NODE_KEY].reg_key = right_child->node_keys[i - 1].reg_key;
                left_child->node_keys[i + MIN_NODE_KEY].reg_index = right_child->node_keys[i - 1].reg_index;
            }
            left_child->current_num_keys = MAX_NODE_KEY;

            for (int i = 0; i < MIN_NODE_CHILD; ++i)
            {
                left_child->node_children[i + MIN_NODE_CHILD] = right_child->node_children[i];
                if (right_child->node_children[i] != NULL)
                    right_child->node_children[i]->parent = NULL;
            }
            no->node_children[pos + 1] = NULL;
            free(right_child);

            for (int i = pos + 1; i < MAX_NODE_KEY; ++i)
            {
                no->node_keys[i - 1].reg_key = no->node_keys[i].reg_key;
                no->node_keys[i - 1].reg_index = no->node_keys[i].reg_index;
            }
            --no->current_num_keys;

            for (int i = pos + 2; i < MAX_NODE_CHILD; ++i)
                no->node_children[i - 1] = no->node_children[i];
            no->node_children[MAX_NODE_CHILD - 1] = NULL;

            if (no->parent == NULL && no->current_num_keys == 0)
            {
                free(no);
                left_child->parent = NULL;
                (*root) = left_child;
                no = left_child;
            }
            delete_1((*root), key, root);
        }
    }

    return no;
}

void delete_1(node *no, int k, node **root)
{
    int i = no->current_num_keys;
    int flag = 0;
    do
    {
        --i;
        if (k == no->node_keys[i].reg_key)
            flag = 1;
    } while (i >= 0 && k < no->node_keys[i].reg_key);
    ++i;

    if (flag)
        no = delete_2(no, i - 1, root);

    else
    {
        node *aux = no;
        if (no->node_children[i]->current_num_keys == MIN_NODE_KEY)
            no = fit(no, i, root);

        i = no->current_num_keys - 1;
        while (i >= 0 && k < no->node_keys[i].reg_key)
            --i;
        ++i;

        if (no != aux)
            delete_1(no, k, root);

        else
            delete_1(no->node_children[i], k, root);
    }
}

void destroy(node *root)
{
    queue_construct();
    if (root)
        queue_push(root);

    int i;
    while (!queue_empty())
    {
        node *no = queue[front];
        queue_pop();
        i = 0;
        while (no->node_children[i] != NULL)
        {
            queue_push(no->node_children[i]);
            ++i;
        }
        free(no);
    }
}

//--------------- File's Functions ---------------//

int pull_data_file(FILE *data_file, node **root)
{
    int x = 0;
    int key;
    char str[10];
    fseek(data_file, 0, SEEK_SET);
    while (fgets(str, 6, data_file))
    {
        if (str[0] != '#')
        {
            key = atoi(str);
            insert_1(root, key, x);
        }
        ++x;
        fseek(data_file, x * 192, SEEK_SET);
    }

    return x;
}

void push_id_file(FILE *index_file, node *root)
{
    queue_construct();
    if (root)
        queue_push(root);
    int count_1 = 1;
    int count_2 = 0;
    int i, j;
    while (!queue_empty())
    {
        node *no = queue[front];
        queue_pop();
        fprintf(index_file, "No %d: ", count_2);
        j = 0;
        if (no->node_children[j] != NULL)
        {
            fprintf(index_file, "%d ", count_1);
            ++count_1;
        }
        else
            fprintf(index_file, "-1 ");

        ++j;
        for (i = 0; i < MAX_NODE_KEY; ++i)
        {
            if (i < no->current_num_keys)
                fprintf(index_file, "<%d, %d> ", no->node_keys[i].reg_key, no->node_keys[i].reg_index);
            else
                fprintf(index_file, "<*****, -1> ");

            if (no->node_children[j] != NULL)
            {
                fprintf(index_file, "%d ", count_1);
                ++count_1;
            }
            else
                fprintf(index_file, "-1 ");

            ++j;
        }
        fprintf(index_file, "\n");

        j = 0;
        while (j <= no->current_num_keys && no->node_children[j] != NULL)
        {
            queue_push(no->node_children[j]);
            ++j;
        }

        ++count_2;
    }
}

void list(node *no, FILE *data_file)
{
    int i;
    char str[200];
    if (no != NULL)
    {
        for (i = 0; i < no->current_num_keys; ++i)
        {
            list(no->node_children[i], data_file);
            printf("Informacoes do Produto: %d", no->node_keys[i].reg_key);
            fseek(data_file, no->node_keys[i].reg_index * 192, SEEK_SET);
            fgets(str, 192, data_file);
            int count_1 = 0;
            for (int i = 0; i < strlen(str); ++i)
            {
                if (str[i] != '@' && count_1 == 0)
                    continue;

                else if (str[i] == '@' && count_1 == 0)
                {
                    printf("\nNome: ");
                    ++count_1;
                    continue;
                }
                else if (str[i] == '@' && count_1 == 1)
                {
                    printf("\nMarca: ");
                    ++count_1;
                    continue;
                }
                else if (str[i] == '@' && count_1 == 2)
                {
                    printf("\nDescricao: ");
                    ++count_1;
                    continue;
                }
                else if (str[i] == '@' && count_1 == 3)
                {
                    printf("\nAno: ");
                    ++count_1;
                    continue;
                }
                else if (str[i] == '@' && count_1 == 4)
                {
                    printf("\nPreco: ");
                    ++count_1;
                    continue;
                }
                else if (str[i] == '@' && count_1 == 5)
                {
                    printf("\n\n");
                    break;
                }
                printf("%c", str[i]);
            }
        }
        list(no->node_children[i], data_file);
    }
}

//--------------- User's Functions ---------------//
void operations()
{
    printf("\n-OPERACOES-\n");
    printf("Inserir -> 1\n");
    printf("Remover -> 2\n");
    printf("Buscar -> 3\n");
    printf("Atualizar -> 4\n");
    printf("Listar -> 5\n");
    printf("Finalizar -> 0\n");
}

//--------------- Main Program ---------------//
int main(int argc, char *argv[])
{
    if (argv[1] == NULL || argv[2] == NULL)
    {
        printf("ERRO: Forneca os arquivos de Dados e Indices, nessa ordem.\n");
        return 0;
    }

    node *root = NULL;
    root = create_node();

    FILE *data_file;
    FILE *index_file;
    data_file = fopen(argv[1], "r+");
    index_file = fopen(argv[2], "w+");
    if (data_file == NULL)
    {
        printf("ERRO: Arquivo de Dados nao encontrado!\n");
        return 0;
    }
    if (index_file == NULL)
    {
        printf("ERRO: Arquivo de Indices nao encontrado!\n");
        return 0;
    }

    int x = pull_data_file(data_file, &root);
    int k = 10000 + x;
    int id = x;
    char key[6];
    char name[25];
    char description[120];
    char brand[20];
    char price[15];
    char year[5];
    char registry[192];
    int pk;
    char str[200];
    char new_price[10];
    int count_1;
    int count_2;
    coordinates ans;
    int operation;
    printf("\n--------------- Arvore Inicial (Key, ID) ---------------\n");
    traversal(root);
    printf("\n--------------- Arvore Inicial (Key, ID) ---------------\n\n");
    while (1)
    {
        data_file = fopen(argv[1], "r+");
        fseek(data_file, 0, SEEK_END);

        operations();
        scanf("%d", &operation);
        if (operation == 1)
        {
            scanf("%*c");

            printf("\nInforme o nome:\n");
            scanf("%[^\n]", name);
            scanf("%*c");

            printf("Informe a marca:\n");
            scanf("%[^\n]", brand);
            scanf("%*c");

            printf("Informe a descricao:\n");
            scanf("%[^\n]", description);
            scanf("%*c");

            printf("Informe o ano de fabricacao:\n");
            scanf("%[^\n]", year);
            scanf("%*c");

            printf("Informe o preco:\n");
            scanf("%[^\n]", price);
            scanf("%*c");

            registry[0] = '\0';
            sprintf(key, "%d", k);
            strcat(registry, key);
            strcat(registry, "@");
            strcat(registry, name);
            strcat(registry, "@");
            strcat(registry, brand);
            strcat(registry, "@");
            strcat(registry, description);
            strcat(registry, "@");
            strcat(registry, year);
            strcat(registry, "@");
            strcat(registry, price);
            strcat(registry, "@");

            for (int i = strlen(registry); i < 192; ++i)
                registry[i] = '*';
            registry[192] = '\0';

            fprintf(data_file, "%s", registry);
            insert_1(&root, k, id);

            ++k;
            ++id;
            printf("\nProduto Inserido!\n");
            fclose(data_file);
        }
        else if (operation == 2)
        {
            printf("\nInforme a Chave\n");
            scanf("%d", &pk);
            ans = search(root, pk);
            if (ans.y == -1)
                printf("\nProduto Inexistente!\n");

            else
            {
                fseek(data_file, ans.x->node_keys[ans.y].reg_index * 192, SEEK_SET);
                fprintf(data_file, "%c", '#');
                delete_1(root, pk, &root);
                printf("\nProduto Removido!\n");
            }

            fclose(data_file);
        }
        else if (operation == 3)
        {

            printf("\nInforme a Chave\n");
            scanf("%d", &pk);
            ans = search(root, pk);
            if (ans.y == -1)
                printf("\nProduto Inexistente!\n");
            else
            {
                printf("\nInformacoes do Produto:");
                fseek(data_file, ans.x->node_keys[ans.y].reg_index * 192, SEEK_SET);
                fgets(str, 192, data_file);

                count_1 = 0;
                for (int i = 0; i < strlen(str); ++i)
                {
                    if (str[i] != '@' && count_1 == 0)
                        continue;

                    else if (str[i] == '@' && count_1 == 0)
                    {
                        printf("\nNome: ");
                        ++count_1;
                        continue;
                    }
                    else if (str[i] == '@' && count_1 == 1)
                    {
                        printf("\nMarca: ");
                        ++count_1;
                        continue;
                    }
                    else if (str[i] == '@' && count_1 == 2)
                    {
                        printf("\nDescricao: ");
                        ++count_1;
                        continue;
                    }
                    else if (str[i] == '@' && count_1 == 3)
                    {
                        printf("\nAno: ");
                        ++count_1;
                        continue;
                    }
                    else if (str[i] == '@' && count_1 == 4)
                    {
                        printf("\nPreco: ");
                        ++count_1;
                        continue;
                    }
                    else if (str[i] == '@' && count_1 == 5)
                    {
                        printf("\n\n");
                        break;
                    }
                    printf("%c", str[i]);
                }
            }
            fclose(data_file);
        }
        else if (operation == 4)
        {
            printf("\nInforme a Chave:\n");
            scanf("%d", &pk);
            ans = search(root, pk);
            if (ans.y == -1)
                printf("\nProduto Inexistente!\n");

            else
            {
                printf("Informe o Novo Preco:\n");
                scanf("%s", new_price);
                fseek(data_file, ans.x->node_keys[ans.y].reg_index * 192, SEEK_SET);
                count_1 = 0;
                count_2 = 0;
                while (count_1 != 5)
                {
                    if (fgetc(data_file) == '@')
                        ++count_1;
                    ++count_2;
                }
                fseek(data_file, ans.x->node_keys[ans.y].reg_index * 192 + count_2, SEEK_SET);
                fprintf(data_file, "%s@", new_price);
                count_1 = 192 - (count_2 + strlen(new_price) + 1);
                fseek(data_file, ans.x->node_keys[ans.y].reg_index * 192 + count_2 + strlen(new_price) + 1, SEEK_SET);
                for (int i = 0; i < count_1; ++i)
                    fprintf(data_file, "*");

                printf("\nPreco Atualizado!\n");
            }
            printf("\n");
            fclose(data_file);
        }
        else if (operation == 5)
        {
            printf("\n");
            list(root, data_file);
            fclose(data_file);
        }
        else if (operation == 0)
        {

            printf("\n--------------- Arvore Final (Key, ID) ---------------\n");
            traversal(root);
            printf("\n--------------- Arvore Final (Key, ID) ---------------\n\n");
            push_id_file(index_file, root);
            destroy(root);
            fclose(data_file);
            return 0;
        }
        else
        {
            printf("\nComando Invalido!\n");
            fclose(data_file);
        }
    }

    fclose(data_file);
    return 0;
}