

#include "codexion.h"

void	heap_remove(t_dongle *d, int idx)
{
	d->heap_size--;
	if (idx < d->heap_size)
	{
		d->heap[idx] = d->heap[d->heap_size];
		heap_sift_up(d->heap, idx);
		heap_sift_down(d->heap, d->heap_size, idx);
	}
}

int	heap_find(t_dongle *d, int coder_id)
{
	int	i;

	i = 0;
	while (i < d->heap_size)
	{
		if (d->heap[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}
