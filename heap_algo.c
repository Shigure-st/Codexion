/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_algo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:44 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:44 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	shift_up(t_heap *queue)
{
	int			parent;
	int			curent;
	t_heap_data	tmp;

	curent = queue->size;
	parent = (curent - 1) / 2;
	while (curent != 0
		&& queue->data[parent].priority > queue->data[curent].priority)
	{
		tmp = queue->data[parent];
		queue->data[parent] = queue->data[curent];
		queue->data[curent] = tmp;
		curent = parent;
		parent = (curent - 1) / 2;
	}
}

int	get_min_child(t_heap *queue, int left, int right)
{
	if (right >= queue->size)
		return (left);
	else
	{
		if (queue->data[left].priority < queue->data[right].priority)
			return (left);
		else
			return (right);
	}
}

void	shift_down(t_heap *queue)
{
	int			parent;
	int			child;
	t_heap_data	tmp;

	parent = 0;
	while ((parent * 2) + 1 < queue->size)
	{
		child = get_min_child(queue, (parent * 2) + 1, (parent * 2) + 2);
		if (queue->data[parent].priority > queue->data[child].priority)
		{
			tmp = queue->data[parent];
			queue->data[parent] = queue->data[child];
			queue->data[child] = tmp;
			parent = child;
		}
		else
			break ;
	}
}
