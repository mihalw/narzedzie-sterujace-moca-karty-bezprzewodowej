#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_QUALITY -20
#define MIN_QUALITY -85
#define SLEEP_TIME 2

float signalQuality(float value){
    // percent = 100 x (1 - (PdBm_max - PdBm) / (PdBm_max - PdBm_min)) - wzor z internetu
    float qualityInPercent = 100 * (1 - (MAX_QUALITY - value) / (MAX_QUALITY - (MIN_QUALITY))); 
    return qualityInPercent;
}

int main(int argc, char const *argv[])
{
    int sfd, signal_level;
    float signal_percent;
    struct iwreq wrq;
    struct iw_statistics stats;

    // stworzenie gniazda aby wywolac pozniej polecenie ioctl
    if((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(&wrq, 0, sizeof(wrq)); 
        memset(&stats, 0, sizeof(stats)); 
        strncpy(wrq.ifr_name, argv[1], IFNAMSIZ);

        wrq.u.data.pointer = &stats;
        wrq.u.data.length = sizeof(stats);

	int wynik3;
	wynik3 = ioctl(sfd, SIOCGIWSTATS, &wrq);
	printf("Wynik ioctla: %d\n", wynik3);

        signal_level = stats.qual.level - 256; 
        signal_percent = signalQuality(signal_level);
        printf("Poziom sygnalu =  %d (%.2f %%) dBm\n", signal_level, signal_percent);

        // pobranie wartosci mocy karty bezprzewodowej
	int wynik2;
        wynik2 = ioctl(sfd, SIOCGIWTXPOW, &wrq);
	printf("Wynik ioctla: %d\n", wynik2);
        printf("Obecna wartosc Tx-Power = %d dBm\n", wrq.u.txpower.value);

        if (signal_percent < atoi(argv[2])) {
            wrq.u.txpower.value++;
	    int wynik;
            wynik =ioctl(sfd, SIOCSIWTXPOW, &wrq);
	    printf("Wynik ioctla: %d\n", wynik);
            printf("Zaktualizowana wartosc Tx-Power = %d dBm\n", wrq.u.txpower.value);
        }
        else if (signal_percent > atoi(argv[3])) {
            wrq.u.txpower.value--;
	    int wynik;
            wynik = ioctl(sfd, SIOCSIWTXPOW, &wrq);
	    printf("Wynik ioctla: %d\n", wynik);
            printf("Zaktualizowana wartosc Tx-Power = %d dBm\n", wrq.u.txpower.value);
        }

        printf("\n\n");
        sleep(SLEEP_TIME);
    }

    close(sfd);
    return 0;
}
