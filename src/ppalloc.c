
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define align4(x) (((((x) - 1 ) >> 2) <<2)+4) // for 4 byte alignment 
#define BLOCK_SIZE sizeof(struct s_block) // block size macro


typedef struct s_block* t_block; // type for struct to make our linked list simpler

void *base=NULL; // starting point of heap initialized to null

struct s_block {
	size_t size; //size
	t_block next; //pointer to next block
	t_block prev;
	int free; // is the block free flag
	void *ptr;
	char data[1];// allows to do pointer artithmetic with byte precision
};



/* First fit algorithm function to find a fitting and free chunk */
t_block find_block(t_block *last, size_t size) {
	t_block b=base; // starting point of heap
	while(b && !(b->free && b->size >= size)) {
		*last = b;
		b = b->next;
	}
	return (b);
}



/* Function to extend the heap */
t_block extend_heap(t_block last, size_t s) {
	t_block b;
	b = sbrk(0); //current break
	if (sbrk(BLOCK_SIZE + s) == (void*)-1)
		return (NULL); // in case sbrk fails
	b->size = s;
	b->next = NULL;
	if (last) last->next = b; 
	b->free = 0;
	return (b);

}
/* If a block can fit data AND another block, we split it to get two blocks */
void split_block(t_block b, size_t s) {
	t_block new;
	new = (t_block) (b->data + s);
	new->size = b->size - s - BLOCK_SIZE;
	new->next = b->next;
	new->free = 1;
	b->size = s; 
	b->next = new; // the original block's "next" now points to our new block

}


t_block fusion(t_block b) {
	if (b->next && b->next->free) {
		b->size += b->next->size + BLOCK_SIZE;
		b->next = b->next->next;
		if (b->next) b->next->next;
		//b->next->free = 0;
	} 
}


t_block get_block(void *p) {
	char *tmp;
	tmp = p;
	return (p = tmp -= BLOCK_SIZE);
}


/* Function to check if pointer is valid */
int valid_addr(void *p) {
	if (base)
	{ 
		if (p>base && p<sbrk(0))
		{
			return (p == (get_block(p))->ptr);
		}
	}
	return (0);
}


void copy_block(t_block src, t_block dst)
{
	int *sdata, *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for (i=0; i*4<src->size && i*4<dst->size; i++) ddata[i] = sdata[i];
}

void *ppalloc(size_t size) {
	t_block b,last;
	size_t s;
	s = align4(size);
	if (base) {
		//Find a block
		last = base;
		b = find_block(&last,s);
		if (b) {
			//check to see if we can split
			if ((b->size - s) >= (BLOCK_SIZE + 4))
				split_block(b,s);
			b->free=0;
		} else {
			//extend the heap
			b = extend_heap(last, s);
			if (!b)
				return (NULL);

		}
	} else {
			// first run
			b = extend_heap(NULL, s);
			if (!b)
				return (NULL);
			base = b;
	}
  return (b->data);
}


// simply inits the memory with 0s
void *ppcalloc(size_t number, size_t size) {
	size_t *new;
	size_t s4, i;
	new = ppalloc(number * size);
	if (new) {
		s4 = align4(number * size) << 2;
		for (i=0; i<s4; i++)
			new[i] = 0;
	}
   return (new);
}


void free (void *p)
{
	t_block b;
	if (valid_addr(p))
	{
		b = get_block(p);
		b->free = 1;

		if (b->prev && b->prev->free)
			b = fusion(b->prev);

		if (b->next) fusion(b);
		else 
			{
				if (b->prev) b->prev->next = NULL;
				else base = NULL;
				brk(b);
			}
	}
}



void *realloc(void *p, size_t size)
{
	size_t s;
	t_block b, new;
	void *newp;
	if (!p) return (ppalloc(size)); // If realloc is called with NULL pointer we just malloc
	if (valid_addr(p))
	{
		s = align4(size);
		b = get_block(p);
		if (b->size >= s)
		{
			printf("pussy 2");
			if (b->size - s >= (BLOCK_SIZE +4))
				split_block(b,s);
		}
		else
		{
			if (b->next && b->next->free && (b->size + BLOCK_SIZE + b->next->size) >= s)
			{
				fusion(b);
				if (b->size - s >= (BLOCK_SIZE + 4))
					split_block(b,s);
			}
			else
			{
				newp = ppalloc(s);
				if (!newp)
				{ 
					printf("balls");
					return (NULL);
				}
				new = get_block(newp);

				copy_block(b, new);

				free(p);
				printf("pussy");
				return (newp);
			}
		}
		printf("peos");
		return (p);
	}
	printf("cocks");
	return (NULL);
}
