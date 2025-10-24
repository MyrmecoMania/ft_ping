/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: enorie <enorie@student.42.fr>              #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-10-24 08:37:18 by enorie            #+#    #+#             */
/*   Updated: 2025-10-24 08:37:18 by enorie           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../includes/ft_ping.h"

void  print_icmp_error(struct icmphdr *icmp, struct iphdr *ip, int bytes, int verbose, char *str) {
  printf("%ld bytes from %s: ", bytes - sizeof(struct iphdr), str);
  if (icmp->type == 3) {
    switch (icmp->code) {
      case 0: printf("Destination network unreachable\n"); break;
      case 1: printf("Destination host unreachable\n"); break;
      case 3: printf("Destination port unreachable\n"); break;
    }
  } else if (icmp->type == 11) {
    if (icmp->code == 0) printf("Time to live exceeded\n");
    if (icmp->code == 1) printf("Fragment reassembly time exceeded\n");
  } else if (icmp->type == 12) {
    if (icmp->code == 0) printf("Pointer indicates the error\n");
    if (icmp->code == 1) printf("Missing a required option\n");
    if (icmp->code == 2) printf("Bad length\n");
  }
  if (verbose) {
    struct in_addr src, dst;
    src.s_addr = ip->saddr;
    dst.s_addr = ip->daddr;
    printf("IP Hdr Dump:\n %x%x%02x %04x %04x %04x %02x%02x %04x %04x %.4x %04x %.4x\n", ip->version, ip->ihl, ip->tos, ntohs(ip->tot_len), ntohs(ip->id), ntohs(ip->frag_off), ip->ttl, ip->protocol, ntohs(ip->check), ntohs(ip->saddr), ntohl(ip->saddr) & 0xFFFF, ntohs(ip->daddr), ntohl(ip->daddr) & 0xFFFF);
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src        Dst\n");
    printf(" %1d  %1d  %02x %04x %04x   %1x %04x  %02x  %02x %04x %s %s\n", ip->version, ip->ihl, ip->tos, ntohs(ip->tot_len), ntohs(ip->id), (ntohs(ip->frag_off) & 0xE000) >> 13, ntohs(ip->frag_off) & 0x1FFF,  ip->ttl, ip->protocol, ntohs(ip->check), inet_ntoa(src), inet_ntoa(dst));
  }
}

int print_command_error(char *str, struct options *options, struct addrinfo *res) {
  if (str)
    printf("%s", str);
  if (options->hostname)
    free(options->hostname);
  if (options)
    free(options);
  if (res)
    freeaddrinfo(res);
  return (1);
}
