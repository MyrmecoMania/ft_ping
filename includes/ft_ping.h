/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: enorie <enorie@student.42.fr>              #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-10-24 08:37:23 by enorie            #+#    #+#             */
/*   Updated: 2025-10-24 08:37:23 by enorie           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ifaddrs.h>
#include <ctype.h>


struct options {
  int   ttl;
  int   tos;
  int   size;
  int   interval;
  int   verbose;
  int   count;
  char  *hostname;
};

int parse_args(char **argv, struct options *option);
struct options *init_options();
int set_socket_options(int sockfd, struct options *options);
unsigned short checksum(void *b, int len);
int ft_isNumeric(char *str);
void  print_icmp_error(struct icmphdr *icmp, struct iphdr *ip, int bytes, int verbose, char *str);
int print_command_error(char *str, struct options *options, struct addrinfo *res);
int print_helper();

#endif
