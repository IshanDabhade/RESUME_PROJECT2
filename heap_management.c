#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct Node
{
    void *address;
    struct Node *prev;
    struct Node *next;
    int memorySize;
    int isAllocated;
} Node;

void DeleteNode(Node *ptr)
{
    ptr->prev = ptr->address = ptr->next = NULL;
    ptr->memorySize = ptr->isAllocated = 0;
    free(ptr);
    ptr = NULL;
}

Node *createNode()
{
    printf("inside createNode()\n");
    Node *nptr = (Node *)malloc(sizeof(Node));
    nptr->address = NULL;
    nptr->prev = NULL;
    nptr->next = NULL;
    nptr->memorySize = 0;
    nptr->isAllocated = 0;
    printf("createNode() over\n");
    return nptr;
}

void setNodeValues(Node *ptr, void *vptr, Node *prev, Node *next, int memorySize, int isAllocated)
{
    printf("\n\ninside setNodeValues()\n");
    ptr->address = vptr;
    ptr->prev = prev;
    ptr->next = next;
    ptr->memorySize = memorySize;
    ptr->isAllocated = isAllocated;
    printf("setNodeValues() over\n\n");
}

void *Allocate(Node *hptr, int asked)
{
    printf("\n\nInside Allocate()\n");
    Node *retptr = NULL;
    printf("asked = %d\n", asked);
    Node *ptr = hptr;
    int flag = 0;
    while (ptr != NULL && flag == 0)
    {
        if (ptr->memorySize >= asked && ptr->isAllocated == 0)
            flag = 1;
        else
            ptr = ptr->next;
    }
    printf("%p\n", ptr);
    if (ptr == NULL)
        printf("Cant give memory\n");
    else
    {
        Node *newptr = createNode();
        Node *tptr = ptr->next;

        newptr->address = ptr->address + asked;
        newptr->prev = ptr;
        newptr->next = tptr;
        newptr->memorySize = ptr->memorySize - asked;
        newptr->isAllocated = 0;
        if (tptr != NULL)
        {
            tptr->prev = newptr;
        }

        ptr->next = newptr;
        ptr->memorySize = asked;
        ptr->isAllocated = 1;
        retptr = ptr->address;
        printf("Allocate() over\n\n");
    }
    return retptr;
}

void Deallocate(Node *hptr, void **dptr)
{
    printf("\n\nInside Deallocate()\n");
    Node *ptr = hptr;
    int flag = 0;
    while (ptr != NULL && flag == 0)
    {
        if (ptr->address == *dptr)
        {
            flag = 1;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    if (ptr == NULL)
    {
        printf("Memory to be deallocated doesn't exist\n");
    }
    else
    {
        if (ptr->next != NULL)
        {
            if (ptr->next->isAllocated == 0)
            {
                Node *previous = ptr->prev;
                if (previous != NULL)
                {
                    if (previous->isAllocated == 0)
                    {
                        previous->memorySize += ptr->memorySize + ptr->next->memorySize;
                        previous->next = ptr->next->next;
                        if (ptr->next->next != NULL)
                        {
                            ptr->next->next->prev = previous;
                        }
                        DeleteNode(ptr->next);
                        DeleteNode(ptr);
                        *dptr = NULL;
                    }
                    else // previous->isAllocated == 1
                    {
                        ptr->memorySize += ptr->next->memorySize;
                        Node *temp = ptr->next;
                        ptr->next = ptr->next->next;
                        if (ptr->next != NULL)
                        {
                            ptr->next->prev = ptr;
                        }
                        DeleteNode(temp);
                        *dptr = NULL;
                    }
                }
                else // else previous == NULL  and next is free and non null
                {
                    ptr->memorySize += ptr->next->memorySize;
                    ptr->isAllocated = 0;
                    Node *temp = ptr->next;
                    ptr->next = ptr->next->next;
                    if (ptr->next != NULL)
                    {
                        ptr->next->prev = ptr;
                    }
                    ptr->prev == NULL;
                    DeleteNode(temp);
                    *dptr = NULL;
                }
            }
            else // next is full/allocated
            {
                Node *previous = ptr->prev;
                if (previous != NULL)
                {
                    if (previous->isAllocated == 0) // not allocated ie free
                    {

                        previous->memorySize += ptr->memorySize;
                        previous->next = ptr->next;
                        if (previous->next != NULL)
                        {
                            previous->next->prev = previous;
                        }
                        DeleteNode(ptr);
                        *dptr = NULL;
                    }
                    else
                    {
                        ptr->isAllocated = 0;
                        *dptr = NULL;
                    }
                }
                else // else previous == NULL and next is full/allocated
                {
                    ptr->isAllocated = 0;
                    ptr->address = NULL;
                    *dptr = NULL;
                }
            }
        }
        else // else ptr->next == NULL
        {
            Node *previous = ptr->prev;
            if (previous != NULL)
            {

                if (previous->isAllocated == 0) // not allocated ie free
                {

                    previous->memorySize += ptr->memorySize;
                    previous->next = ptr->next;
                    // previous->next->prev = previous;
                    // previous->address = NULL;
                    DeleteNode(ptr);
                    // free(*dptr);
                    *dptr = NULL;
                } // previous is full
                else
                {
                    ptr->isAllocated = 0;
                    // ptr->address = NULL;
                    //  ptr->prev == NULL;
                    // free(ptr);
                    //  free(*dptr);
                    *dptr = NULL;
                }
            }
            else // else previous == NULL and next is NULL
            {
                ptr->isAllocated = 0;
                *dptr = NULL;
            }
        }
    }
    printf("\nDeallocate Over\n\n");
}

int main()
{
    void *const_heap = malloc(10000);
    Node *hptr = createNode();
    setNodeValues(hptr, const_heap, NULL, NULL, 10000, 0); // Node*,address,prev,next,memorySize,isAllocatd

    char *cptr = (char *)Allocate(hptr, sizeof(char));
    int *iptr = (int *)Allocate(hptr, sizeof(int));
    float *fptr = (float *)Allocate(hptr, sizeof(float));
    char *cptr1 = (char *)Allocate(hptr, 10 * sizeof(char));
    float *fptr1 = (float *)Allocate(hptr, 20 * sizeof(float));

    for (int i = 0, j = 65; i < 8; i++, j++)
    {
        cptr1[i] = j;
    }

    printf("\n\n");
    for (int i = 0, j = 65; i < 8; i++, j++)
    {
        printf("%c ", cptr1[i]);
    }
    printf("\n\n");

    *iptr = 4;
    *cptr = '&';
    *fptr = 5;

    printf("%d ", *iptr);
    printf("%c ", *cptr);
    printf("%f ", *fptr);
    printf("%p ", cptr);
    printf("%p ", iptr);
    printf("%p ", fptr);
    printf("%p ", cptr1);
    printf("%p ", fptr1);

    for (int i = 0, j = 65; i < 8; i++, j++)
    {
        fptr1[i] = j;
    }

    printf("\n\n");
    for (int i = 0, j = 65; i < 8; i++, j++)
    {
        printf("%f ", fptr1[i]);
    }
    printf("\n\n");
    printf("%p ", iptr);
    Deallocate(hptr, &iptr);
    printf("%p ", iptr);
    printf("\n\n");
    printf("%p ", cptr1);
    Deallocate(hptr, &cptr1);
    printf("%p ", cptr1);
    printf("\n\n");
    printf("%p ", fptr);
    Deallocate(hptr, &fptr);
    printf("%p ", fptr);

    printf("\n\n");
}