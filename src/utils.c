/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: enorie <enorie@student.42.fr>              #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-10-24 08:37:06 by enorie            #+#    #+#             */
/*   Updated: 2025-10-24 08:37:06 by enorie           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/ft_ping.h"

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int ft_isNumeric(char *str) {
  while(*str) {
    if (!isdigit(*str))
      return (1);
    str++;
  }
  return (0);
}