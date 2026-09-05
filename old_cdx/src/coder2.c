

#include "codexion.h"

void	put_dongles(t_coder *c)
{
	if (c->sim->n == 1)
	{
		dongle_release(c->left, c->sim);
		return ;
	}
	dongle_release(c->left, c->sim);
	dongle_release(c->right, c->sim);
}
