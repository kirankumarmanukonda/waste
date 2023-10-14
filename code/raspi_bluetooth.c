#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <gtk/gtk.h>


#include <unistd.h>
#include <fcntl.h>

void rec_img(char *);
void sen(char *);
int raspi_init(void);
// char buffer[10];
int readd;
int s, status;

int raspi_init()
{
    struct sockaddr_rc addr = {0};

    char dest[18] = "D8:3A:DD:17:61:C7"; // replace with the address of the remote device

    //E4:5F:01:C2:0E:2F white
    //E4:5F:01:CE:D9:15 green device2
    //D8:3A:DD:17:61:C7 black(new device) angio
    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);



    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t)1;
    str2ba(dest, &addr.rc_bdaddr);


    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    
    /************************       working        **********************/
    struct timeval tv;
    tv.tv_sec = 2;  // timeout in seconds
    tv.tv_usec = 0; // timeout in microseconds
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0)
    {
        perror("setsockopt failed");
    }
    
    printf("status %d",status);
    return status;
}

extern void prev_lsci(void);

void rec_img(char *color_photo)
{
    while (gtk_events_pending())
        gtk_main_iteration();

    char *filename = strdup(color_photo); // replace with the name you want to give to the received image


    //prev_lsci();
   
    sleep(1);
    char buf[8192];
    size_t n;
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("fopen");
        //exit(1);
    } 

    while ((n = recv(s, buf, sizeof(buf), 0)) > 0)
    {
        while (gtk_events_pending())
            gtk_main_iteration();
        if (fwrite(buf, 1, n, fp) < n)
        {
            //perror("fwrite");
            printf("Image receiving Done\n");
            // exit(1);
            break;
        }


    }

    while (gtk_events_pending())
        gtk_main_iteration();
    fclose(fp);


    
}

void sen(char *cmd)
{
    status = write(s, cmd, strlen(cmd));

    if (status < 0)
    {
        perror("Error receiving message");
    }
}


