#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"

int detected_file(const char *path) { return !access(path, F_OK); }

int config_edit(char *CONF) {
    char ip[20],username[32];
    int port;
    if (!detected_file(CONF)) {
        // Init
        FILE *fpw = fopen(CONF, "w");
        printf("target ip: ");
        scanf("%s",ip);
        printf("target username: ");
        scanf("%s",username);
        printf("target port (default 22): ");
        int port;
        if (scanf("%d",&port)!=1) {
            port=22;
        }
        fprintf(fpw, "%s\n%s\n%d\n",ip,username,port);
        fclose(fpw);
        printf("config saved.\n");
        return 0;
    }
    // edit
    FILE *fpr = fopen(CONF, "r");
    fscanf(fpr, "%s\n%s\n%d\n",ip,username,&port);
    fclose(fpr);
    char opt;
    do {
        printf("Choose the option to change:\n"
               "(1) ip (currently %s)\n"
               "(2) username (currently %s)\n"
               "(3) port (currently %d)\n"
               "(0) finish\n"
               "> ",ip,username,port);
        getchar(); // clear cache
        opt = getchar();
        if (opt == '1') {
            printf("New ip: ");
            scanf("%s",ip);
            continue;
        }
        if (opt == '2') {
            printf("New username: ");
            scanf("%s",username);
            continue;
        }
        if (opt == '3') {
            printf("New port (default 22): ");
            if (scanf("%d",&port)!=1) {
                port=22;
            }
            continue;
        }
        if (opt == '0') {
            break;
        }
        puts("Unknown option\n");
    } while (opt != '0');
    // replace old config
    system("rm -rf $HOME/.swpcnf");
    FILE *fpw = fopen(CONF, "w");
    fprintf(fpw, "%s\n%s\n%d\n",ip,username,port);
    fclose(fpw);
    printf("config saved.\n");
    return 0;
}

int exec_sync(char *CONF) {
    // pull or push with config
    FILE *fpr = fopen(CONF, "r");
    char ip[20],username[32],pos[127],cmd[127];
    int port;
    while (1) {
        fscanf(fpr, "%s\n%s\n%d\n",ip,username,&port);
        printf("Using config to sync:\n\n"
               "\tip: %s\n"   
               "\tusername: %s\n"
               "\tport: %d\n\n"
               "[ L (pull) | S (push) | E (edit) ] ",ip,username,port);
        fflush(stdin);
        char opt = getchar();
        // locate
        sprintf(pos, "%s@%s:/home/%s/",username,ip,username);
        if (!strcmp("root", username)) {
            sprintf(pos, "%s@%s:/%s/",username,ip,"root");
        }
        if (opt == 'L' | opt == 'l') {
            // clean local swaps
            fflush(stdin);
            fflush(stdout);
            printf("Clear local swaps? [y/N] ");
            char opt_clear = getchar();
            if (opt_clear == 'Y' | opt_clear == 'y') {
                system("rm $HOME/swap/ -r");
            } // bugs
            sprintf(cmd, "scp -r -P %d %s/swap $HOME/",port,pos);
            // pull
            for (int i = 0; i < 3; ++i) {
                printf("\r$LOCAL/swap/ <- %s in %d...",pos,3-i);
                fflush(stdout);
                sleep(1);
            }
            printf("\r\n");
            fclose(fpr);
            system(cmd);
            return 0;
        }
        if (opt == 'S' | opt == 's') {
            // clean opponent swaps
            sprintf(cmd, "scp -r -P %d $HOME/swap/ %s",port,pos);
            char *clear_swap = (char *)malloc(127);
            sprintf(clear_swap, "/home/%s/swap/",username);
            if (!strcmp("root", username)) {
                sprintf(clear_swap, "/root/swap/");
            }
            sprintf(clear_swap, "ssh -p %d -l %s %s rm -r swap/",port,username,ip);
            printf("Clear receivers swaps, CTRL+C to skip\n");
            system(clear_swap);
            // push
            for (int i = 0; i < 3; ++i) {
                printf("\r$LOCAL/swap/ -> %s in %d...",pos,3-i);
                fflush(stdout);
                sleep(1);
            }
            printf("\r\n");
            fclose(fpr);
            system(cmd);
            return 0;
        }
        if (opt == 'E' | opt == 'e') {
            config_edit(CONF);
            fclose(fpr);
            return 0;
        }
        printf("Aborted.\n");
        return 0;
    }
}

int add_swap(int argc, char *argv[]) {
    // Add files to swap
    char cmd[255];
    for (int i = 1; i<argc; ++i) {
        sprintf(cmd, "cp -r \"%s\" $HOME/swap/",argv[i]);
        system(cmd);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc!=1) {
        add_swap(argc,argv);
        printf("Added files to $HOME/swap/.\n");
        return 0;
    }
    const char *HOME = getenv("HOME");
    char *CONF;
    sprintf(CONF, "%s/.swpcnf",HOME);
    if (!detected_file(CONF)) {
        config_edit(CONF);
        return 0;
    }
    exec_sync(CONF);
    return 0;
}
