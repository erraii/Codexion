#include "codexion.h"
#include <stdio.h>

static void	run_test(t_scheduler scheduler)
{
	t_heap		heap;
	t_request	first;
	t_request	second;

	heap.size = 0;
	first.coder_id = 1;
	first.sequence = 1;
	first.deadline = 500;
	second.coder_id = 2;
	second.sequence = 2;
	second.deadline = 300;
	heap_push(&heap, first, scheduler);
	heap_push(&heap, second, scheduler);
	printf("First coder: %d\n", heap_peek(&heap)->coder_id);
	heap_pop(&heap);
	printf("Second coder: %d\n", heap_peek(&heap)->coder_id);
}

int	main(void)
{
	printf("FIFO test:\n");
	run_test(POLICY_FIFO);
	printf("EDF test:\n");
	run_test(POLICY_EDF);
	return (0);
}