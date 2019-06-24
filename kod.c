#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <stdlib.h>
#include <signal.h>



float signalQuality(float value){
    // percent = 100 x (1 - (PdBm_max - PdBm) / (PdBm_max - PdBm_min)) - wzor z internetu
    float qualityInPercent = 100 * (1 - (-20 - value) / (-20 - (-85))); 
    return qualityInPercent;
}

int sfd;

int main(int argc, char const *argv[])
{
    int signal_level;
    float signal_percent;
    struct iwreq wrq;
    struct iw_statistics stats;

    // stworzenie gniazda aby wywolac polecenie ioctl
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

	ioctl(sfd, SIOCGIWSTATS, &wrq);
        signal_level = stats.qual.level - 256; 
        signal_percent = signalQuality(signal_level);
        printf("Poziom sygnalu [dBm] =  %d (%.2f %%)\n", signal_level, signal_percent);

        // pobranie wartosci mocy karty bezprzewodowej
        ioctl(sfd, SIOCGIWTXPOW, &wrq);
        printf("Obecna wartosc Tx-Power     [dBm] =   %d\n", wrq.u.txpower.value);


        if (signal_percent < atoi(argv[3])) {
            wrq.u.txpower.value++;
            ioctl(sfd, SIOCSIWTXPOW, &wrq);
            printf("Zaktualizowana wartosc Tx-Power [dBm] =   %d\n", wrq.u.txpower.value);
        }
        else if (signal_percent > atoi(argv[4])) {
            wrq.u.txpower.value--;
            ioctl(sfd, SIOCSIWTXPOW, &wrq);
            printf("Zaktualizowana wartosc Tx-Power [dBm] =   %d\n", wrq.u.txpower.value);
        }

        printf("\n\n");
        sleep(atoi(argv[2]));
    }

    close(sfd);
    return 0;
}
