#include "./includes/ft_ping.h"

int loop = 1;

void  signalHandler() {
  loop = 0;
}

void  init_loop(int sockfd, struct options *options, struct sockaddr_in temp_addr) {
  char rbuffer[1500];
  struct timeval  tv;
  struct sockaddr_in r_addr;
  char pckt[options->size + sizeof(struct icmphdr)];
  struct icmphdr *pckt_hdr = (struct icmphdr *)pckt;
  struct timespec time_start, time_end;
  long double rtt_msec = 0;
  unsigned int addr_len;
  struct iphdr *ip, *inner_ip;
  struct icmphdr *icmp, *inner_icmp;
  fd_set  readfds;
  FD_ZERO(&readfds);
  FD_SET(sockfd, &readfds);
  char dst[options->size + sizeof(struct iphdr) + sizeof(struct icmphdr)];
  int msg_count = -1, flag = 1, msg_received_count = 0;
  while (loop) {
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    flag = 1;
    bzero(&pckt, sizeof(pckt));
    pckt_hdr->type = ICMP_ECHO;
    pckt_hdr->un.echo.id = htons(getpid());
    pckt_hdr->un.echo.sequence = msg_count++;
    pckt_hdr->checksum = checksum(&pckt, sizeof(pckt));
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    if (sendto(sockfd, &pckt, sizeof(pckt), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr)) <= 0) {
        printf("\nPacket Sending Failed!\n");
        flag = 0;
    }
    addr_len = sizeof(r_addr);
    while (flag && loop) {
      int ret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
      if (FD_ISSET(sockfd, &readfds)) {
        int test = recvfrom(sockfd, rbuffer, sizeof(rbuffer), 0, (struct sockaddr*)&r_addr, &addr_len);
          if (test > 0) {
          ip = (struct iphdr *) rbuffer;
          icmp = (struct icmphdr *)(rbuffer + 20);
          if (icmp->type) {
            inner_ip = (struct iphdr *)((uint8_t *)icmp + sizeof(struct icmphdr));
            inner_icmp = (struct icmphdr *)((uint8_t *)inner_ip + 20);
          }
          if ((htons(icmp->un.echo.id) == getpid() || (icmp->type && htons(inner_icmp->un.echo.id) == getpid())) && (icmp->type != 8)) {
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
            rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed;
            if (!(icmp->type == 0 && icmp->code == 0)) {
              char addrstr[INET_ADDRSTRLEN];
              inet_ntop(AF_INET, &r_addr.sin_addr, addrstr, sizeof(addrstr));
              print_icmp_error(icmp, ip, test, options->verbose, addrstr);
              if (options->verbose)
                printf("ICMP: type %d, code %d, size %ld, id 0x%x, seq 0x%04x\n", pckt_hdr->type, 0, options->size + sizeof(struct icmphdr), ntohs(inner_icmp->un.echo.id), msg_count);
            } else {
                printf("%ld bytes from %s: icmp_seq = %d ttl = %d time = %.3Lf ms\n", test - sizeof(struct iphdr), inet_ntop(AF_INET, &r_addr.sin_addr, dst, INET_ADDRSTRLEN), msg_count, ip->ttl, rtt_msec);
                msg_received_count++;
            }
            flag = 0;
            if (options->count != msg_count + 1)
              usleep(options->interval);
          }
        }
      } else if (ret <= 0)
        break;
    }
    if (options->count > 0 && options->count == msg_count + 1)
      break;
  }
  printf("--- %s ping statistics ---\n", options->hostname);
  printf("%d packets transmitted, %d packets received, %d%% packet loss\n", msg_count + 1, msg_received_count, (((msg_count + 1 - msg_received_count) * 100) / (msg_count + 1)));
}

int main(int argc, char **argv) {
  int sockfd;
  struct sockaddr_in temp_addr;
  struct options *options = NULL;
  options = init_options();
  if (!options)
    return (1);
  if (parse_args(argv, options))
    return (print_command_error(NULL, options, NULL));
   if (!options->hostname || argc < 2)
    return (print_command_error("ft_ping: missing host operand\n", options, NULL));
  struct addrinfo hints = {
      .ai_family = AF_INET,
      .ai_socktype = SOCK_RAW,
      .ai_protocol = IPPROTO_ICMP
  };
  struct addrinfo *res;
  char dst[options->size + sizeof(struct iphdr) + sizeof(struct icmphdr)];
  signal(SIGINT, signalHandler);
  if (getaddrinfo(options->hostname, 0, &hints, &res))
    return (print_command_error("ft_ping: unknown host\n", options, NULL));
  struct sockaddr_in *goodRes = (struct sockaddr_in *) res->ai_addr;
  temp_addr.sin_family = AF_INET;
  temp_addr.sin_port = 0;
  temp_addr.sin_addr.s_addr = inet_addr(inet_ntop(AF_INET, &goodRes->sin_addr, dst, INET_ADDRSTRLEN));
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0)
    return (print_command_error("Socket file descriptor not received!\n", options, res));
  if (set_socket_options(sockfd, options))
    return (close(sockfd), free(options), 1);
  if (options->verbose)
    printf("PING %s (%s): %d data bytes, id 0x%x = %d\n", options->hostname, inet_ntop(AF_INET, &goodRes->sin_addr, dst, INET_ADDRSTRLEN), options->size, getpid(), getpid());
  else
    printf("PING %s (%s): %d data bytes\n", options->hostname, inet_ntop(AF_INET, &goodRes->sin_addr, dst, INET_ADDRSTRLEN), options->size);
  init_loop(sockfd, options, temp_addr);
  return (free(options->hostname), free(options), free(res), close(sockfd), 0);
}
