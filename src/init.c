#include "../includes/ft_ping.h"

int parse_args(char **argv, struct options *option) {
  int i = 1;
  int y;
  char *value;
  int int_value;
  while(argv[i]) {
    y = 0;
    if (argv[i][0] == '-') {
      while(argv[i][y] && argv[i][y] =='-')
        y++;
      if (y == 1) {
        if (argv[i][y] && argv[i][y] == 'v') {
          option->verbose = 1;
          y++;
        }
        if (argv[i][y] && argv[i][y] == '?')
          return (print_helper());
        if (argv[i][y] && (argv[i][y] == 's' || argv[i][y] == 'T' || argv[i][y] == 'i'  || argv[i][y] == 'c')) {
          value = (argv[i][y + 1]) ? strdup(argv[i] + y + 1) : strdup(argv[i + 1]);
          if (!value)
            return (printf("ft_ping: no space left\n"), 1);
          if (!ft_isNumeric(value)) {
            int_value = atoi(value);
            if (argv[i][y] == 's') {
              if (int_value < 0 || int_value > 65399)
                return (free(value), printf("ft_ping: option value too big: %d\n", int_value), 1);
              option->size = int_value;
            }
            else if (argv[i][y] == 'T') {
              if (int_value < 0 || int_value > 255)
                return (free(value), printf("ft_ping: option value too big: %d\n", int_value), 1);
              option->tos = int_value;
            }
            else if (argv[i][y] == 'i')
              option->interval = int_value * 1000000;
            else
              option->count = int_value;
          }
          else
            return (printf("ft_ping: invalid value (\'%s\' near \'%s\')\n", value, value), free(value), 1);
          if (!argv[i][y + 1])
            i++;
          free(value);
        }
        else if (argv[i][y])
          return (printf("ft_ping: unrecognized option \'%s\'\n", argv[i]), 1);
      } else if (y == 2)
        if (!strncmp(argv[i], "--ttl", 5)) {
          value = (argv[i][5]) ? strdup(argv[i] + 5) : strdup(argv[i + 1]);
          if (!ft_isNumeric(value)) {
            int_value = atoi(value);
            if (int_value < 0 || int_value > 255)
                return (printf("ft_ping: option value too big: %d\n", int_value), free(value),  1);
            option->ttl = int_value;
          }
          else
            return (printf("ft_ping: invalid value (\'%s\' near \'%s\')\n", value, value), free(value),  1);
          if (!argv[i][5])
            i++;
          free(value);
        }
        else
          return (printf("ft_ping: unrecognized option \'%s\'\n", argv[i]), 1);
      else
        return (printf("ft_ping: unrecognized option \'%s\'\n", argv[i]), 1);
    } else {
      if (!option->hostname)
        option->hostname = strdup(argv[i]);
      if (!option->hostname)
        return (printf("ft_ping: no space left\n"), 1);
    }
    i++;
  }
  return (0);
}

int print_helper() {
  printf("Usage:  ./ft_ping [OPTION...] HOST...\nSend ICMP ECHO_REQUEST packets to network hosts.\n\n Options:\n");
  printf("    -v                verbose output\n");
  printf("    -c [NUMBER]       stop after sending NUMBER packets\n");
  printf("    -i [NUMBER]       wait NUMBER seconds between sending each packets\n");
  printf("    -T [NUMBER]       set type of service (TOS) to NUMBER\n");
  printf("    -s [NUMBER]       send NUMBER data octets\n");
  printf("    --ttl [NUMBER]    specify NUMBER as time-to-live\n");
  return (1);
}

struct options *init_options() {
  struct options *options = malloc(sizeof(struct options));
  if (!options)
    return (printf("Can't malloc options\n"), NULL);
  options->verbose = 0;
  options->tos = 60;
  options->ttl = 64;
  options->size = 56;
  options->interval = 1000000;
  options->count = 0;
  options->hostname = NULL;
  return (options);
}

int set_socket_options(int sockfd, struct options *options) {
    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS, &options->tos, sizeof(options->tos)) != 0) {
      printf("\nSetting socket options to TTL failed!\n");
      return 1;
    }
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &options->ttl, sizeof(options->ttl)) != 0) {
      printf("\nSetting socket options to TTL failed!\n");
      return 1;
    }
  return (0);
}
