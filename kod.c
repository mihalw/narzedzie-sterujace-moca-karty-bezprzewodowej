#include <stdio.h>
#include <unistd.h>
#include "iwlib.h"
#include <sys/ioctl.h>	
#include <net/if.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

	int sfd, wynikIoctla, wynikIoctla2;
	struct iwreq wrq;

	sfd = socket(PF_INET, SOCK_DGRAM, 0);
	// strncpy (DOKAD, SKAD, ROZMIAR)
	strncpy(wrq.ifr_name, argv[1], IFNAMSIZ);
	wynikIoctla = ioctl(sfd, SIOCGIFFLAGS, &wrq.ifr_name);

	printf("txpower poczatkowy(powinno byc kolo 20): %d\n", wrq.u.txpower.value);
	printf("wynik ioctl: %d\n", wynikIoctla);

	wynikIoctla2 = ioctl(sfd, SIOCSIFFLAGS, wrq.u.txpower.value, 100);
	printf("txpower po zmianie(powinno byc 100): %d\n", wrq.u.txpower.value);
	printf("wynik ioctla numer 2: %d\n", wynikIoctla2);

	return 0;
}
