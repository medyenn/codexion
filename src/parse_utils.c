

#include "codexion.h"

bool	is_valid_uint(const char *s)
{
	if (!s || *s == '\0')
		return (false);
	if (*s == '+')
		s++;
	if (*s == '\0')
		return (false);
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return (false);
		s++;
	}
	return (true);
}

long long	safe_atoll(const char *s)
{
	long long	result;

	if (*s == '+')
		s++;
	result = 0;
	while (*s)
	{
		if (result > (long long)INT_MAX)
			return (-1);
		result = result * 10 + (*s - '0');
		s++;
	}
	return (result);
}
