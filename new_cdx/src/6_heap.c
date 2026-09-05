/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mennih < mennih@student.1337.ma>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 14:25:31 by mennih            #+#    #+#             */
/*   Updated: 2026/09/02 14:25:32 by mennih           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	req_less(const t_request *a, const t_request *b)
{
	if (a->priority != b->priority)
		return (a->priority < b->priority);
	return (a->coder_id > b->coder_id);
}

void	heap_swap(t_request *heap, int i, int j)
{
	t_request	tmp;

	tmp = heap[i];
	heap[i] = heap[j];
	heap[j] = tmp;
}

void	heap_sift_up(t_request *heap, int idx)
{
	int	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (req_less(&heap[idx], &heap[parent]))
		{
			heap_swap(heap, idx, parent);
			idx = parent;
		}
		else
			break ;
	}
}

void	heap_sift_down(t_request *heap, int size, int idx)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		smallest = idx;
		if (left < size && req_less(&heap[left], &heap[smallest]))
			smallest = left;
		if (right < size && req_less(&heap[right], &heap[smallest]))
			smallest = right;
		if (smallest == idx)
			break ;
		heap_swap(heap, idx, smallest);
		idx = smallest;
	}
}

int	heap_push(t_dongle *d, t_request req)
{
	t_request	*tmp;
	int			new_cap;

	if (d->heap_size >= d->heap_cap)
	{
		new_cap = d->heap_cap * 2;
		tmp = (t_request *)realloc(d->heap,
				(size_t)new_cap * sizeof(t_request));
		if (!tmp)
			return (-1);
		d->heap = tmp;
		d->heap_cap = new_cap;
	}
	d->heap[d->heap_size] = req;
	heap_sift_up(d->heap, d->heap_size);
	d->heap_size++;
	return (0);
}
