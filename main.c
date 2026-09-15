/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tenomoto <tenomoto@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 12:42:44 by tenomoto          #+#    #+#             */
/*   Updated: 2026/09/15 12:42:44 by tenomoto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "codexion.h"

int	main(int argc, char **argv)
{
	struct s_args			args;
	struct s_shared_context	ctx;

	if (parse_args(argc, argv, &args) != 0)
		return (-1);
	if (init_context(&args, &ctx) != 0)
	{
		cleanup_context(&ctx);
		return (-1);
	}
	if (run_simulation(&ctx) != 0)
	{
		cleanup_context(&ctx);
		return (-1);
	}
	cleanup_context(&ctx);
	return (0);
}
