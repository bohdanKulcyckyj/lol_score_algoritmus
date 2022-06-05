#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    char name[50];
    int gamer_id;
    int kills;
    int assistance;
    int deaths;
    int rating;
    char division[50];
} Gamer;

typedef struct {
    int * blue_team_ids;
    int * red_team_ids;
    int * blue_team_stats;
    int * red_team_stats;
    char winner[50];
} Game;

void update_kad_blue(Game * game, int sizeBlueTeam, int sizeBlueStats, Gamer ** players_list, int list_length){
    if(sizeBlueStats / sizeBlueTeam == 3) {
        for(int i = 0; i < list_length; i++) {
            for(int j = 0; j < sizeBlueTeam; j++) {
                int index = j * 3;// 3 = K/A/D
                if(players_list[i]->gamer_id == game->blue_team_ids[j]) {
                    players_list[i]->kills += game->blue_team_stats[index];
                    players_list[i]->assistance += game->blue_team_stats[index+1];
                    players_list[i]->deaths += game->blue_team_stats[index+2];
                }
            }
        }
    } else {
        printf("Spatne vyplnena statistika K/A/D nebo pocet hracu u MODREHO tymu\n");
    }
}

void update_kad_red(Game * game, int sizeRedTeam, int sizeRedStats, Gamer ** players_list, int list_length){
    if(sizeRedStats / sizeRedTeam == 3) {
        for(int i = 0; i < list_length; i++) {
            for(int j = 0; j < sizeRedTeam; j++) {
                int index = j * 3;// 3 = K/A/D
                if(players_list[i]->gamer_id == game->red_team_ids[j]) {
                    players_list[i]->kills += game->red_team_stats[index];
                    players_list[i]->assistance += game->red_team_stats[index+1];
                    players_list[i]->deaths += game->red_team_stats[index+2];
                }
            }
        }
    } else {
        printf("Spatne vyplnena statistika K/A/D nebo pocet hracu u CERVENEHO tymu\n");
    }
}

void update_rating(Gamer ** players_list, int list_length, int teamSize, int * players_ids, int enemies_avg_rating, char winner[50], char team[50]){
    for(int i = 0; i < list_length; i++) {
        for(int k = 0; k < teamSize; k++) {
            if(players_list[i]->gamer_id == players_ids[k]) {
                int curr_rating = players_list[i]->rating;
                int k = 30;
                float ea = 1 / (1 + (pow(10, ((enemies_avg_rating - curr_rating)/400))));
                int sa = 0;

                if(!strcmp(team, winner)) {
                sa = 1;
                }

                int new_rating = curr_rating + k * (sa - ea);

                players_list[i]->rating = new_rating;
                
                if(new_rating < 1150) {
                    strcpy(players_list[i]->division, "Bronze");
                } else if(new_rating < 1500) {
                    strcpy(players_list[i]->division, "Silver");
                } else if(new_rating < 1850) {
                    strcpy(players_list[i]->division, "Gold");
                } else if(new_rating < 2200) {
                    strcpy(players_list[i]->division, "Platinum");
                } else {
                    strcpy(players_list[i]->division, "Diamond");
                }              
            }
        }
    }
}

void after_match_update(Game * game, int sizeBlueTeam, int sizeRedTeam, int sizeBlueStats, int sizeRedStats, Gamer ** players_list, int list_length) {
    int rating_prumer_blue = 0;
    int rating_prumer_red = 0;

    for(int j = 0; j < list_length;j++) {
        for(int i = 0; i < sizeBlueTeam;i++) {
            if(players_list[j]->gamer_id == game->blue_team_ids[i]) {
                rating_prumer_blue += players_list[j]->rating;

            }
        }

        for(int l = 0; l < sizeRedTeam;l++) {
            if(players_list[j]->gamer_id == game->red_team_ids[l]) {
                rating_prumer_red += players_list[j]->rating;
            }
        }
    }
    rating_prumer_blue /= sizeBlueTeam;
    rating_prumer_red /= sizeRedTeam;

    //update K/A/D
    update_kad_blue(game, sizeBlueTeam, sizeBlueStats, players_list, list_length);
    update_kad_red(game, sizeBlueTeam, sizeRedStats, players_list, list_length);

    //update ratingu po teamech
    update_rating(players_list, list_length, sizeBlueTeam, game->blue_team_ids, rating_prumer_red, game->winner, "blue");
    update_rating(players_list, list_length, sizeRedTeam, game->red_team_ids, rating_prumer_blue, game->winner, "red");
}

void readGames(char * pin, int * rows, Gamer ** players_list, int list_length) {
    FILE * fin = fopen(pin, "r");
    if (!fin) {
		printf("Chyba nacteni souboru\n");
	}

    char row[1000];
    int i = 1;
    int w = 1;
    int y = 1;
    Game helper;
    int stats_rows = 6;//6 radku patri k jedne hre
    int blue_players_count;
    int red_players_count;
    int blue_players_stats_count;
    int red_players_stats_count;
    int * array = (int*)malloc(sizeof(int));

    while(!feof(fin)) {
        fgets(row, 1000, fin);
        y = 1;

        char * tmp = strtok(row, ";,");
        tmp[strcspn(tmp, "\n")] = 0;

        if(w!=6 && w!=1) {
            while(tmp!=0) {
                array=(int*)realloc(array, (y+1)*sizeof(int));
                array[y-1] = atoi(tmp);
                tmp = strtok(0, ";,");
                y++;
            }
        } else {
            if(w==6) {
                strcpy(helper.winner, tmp);
                tmp = strtok(0, ";,");
            }
        }
        
        if(w==2) {
            blue_players_count = y - 1;
            helper.blue_team_ids = array;
        }
        if(w==3) {
            blue_players_stats_count = y - 1;
            helper.blue_team_stats = array;
        }
        if(w==4) {
            red_players_count = y - 1;
            helper.red_team_ids = array;
        }
        if(w==5) {
            red_players_stats_count = y - 1;
            helper.red_team_stats = array;
        }
        
        if(i % stats_rows == 0) {
            w = 1;
            //zavolat funkci pro update pro kazdou hru
            after_match_update(&helper, blue_players_count, red_players_count, blue_players_stats_count, red_players_stats_count, players_list, list_length);
        } else {
            w++;
        }

        array = 0;
        i++;
    }
    free(array);
}

Gamer ** readPlayers(char * pin, int * rows) {
    Gamer ** gamers = (Gamer**)malloc(sizeof(Gamer*));
    gamers[0] = (Gamer*)malloc(sizeof(Gamer));
    int num = 1;

    FILE * fin = fopen(pin, "r");
    if(!fin) {
        printf("Chyba nacteni souboru %s\n", pin);
        return gamers;
    }
    
    char row[1000];

    int i = 0;
    while(!feof(fin)) {
        if(i >= num) {
            gamers = (Gamer**)realloc(gamers, (num + 1) * sizeof(Gamer*));
            num++;
            gamers[i] = (Gamer*)malloc(sizeof(Gamer));
        }
        fgets(row, 1000, fin);

        char * tmp = strtok(row, ",");
        int w = 0;
        while(tmp != 0) {
            if(w == 0) {
                gamers[i]->gamer_id = atoi(tmp);
            } else if(w==1) {
                tmp[strcspn(tmp, "\n")] = 0;
                strcpy(gamers[i]->name, tmp);
            } else {
                tmp = 0;
                printf("Chyba - moc argumentu na jedno radku.\n");
            }
            tmp = strtok(0, ",");
            w++;
        }
        gamers[i]->rating = 1000;
        gamers[i]->kills = 0;
        gamers[i]->assistance = 0;
        gamers[i]->deaths = 0;

        i++;
    }

    fclose(fin);

    *rows = num;

    return gamers;
}

void printHTML(char * pin, Gamer** players_list, int players_list_length) {
    char htmlHeader[] = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<link rel=\"stylesheet\" href=\"style.css\">\n<title>League of Legends - Players stats</title>\n</head>\n";
    //to rewrite previous data
    FILE * fin = fopen(pin, "w");
    if(!fin) {
        printf("Chyba nacteni souboru %s\n", pin);
    }

    fprintf(fin, htmlHeader);
    fclose(fin);
    //to assert new data
    fin = fopen(pin, "a");
    if(!fin) {
        printf("Chyba nacteni souboru %s\n", pin);
    }

    fprintf(fin, "<body>\n");
    fprintf(fin, "\t<main>\n");
    fprintf(fin, "\t\t<table>\n");

    fprintf(fin, "\t\t<thead>\n");
    fprintf(fin, "\t\t<tr>\n");

    fprintf(fin, "\t\t<th class=\"player-name-h\">Name</th>\n");
    fprintf(fin, "\t\t<th class=\"player-kad-h\">K/A/D</th>\n");
    fprintf(fin, "\t\t<th class=\"player-rating-h\">Rating</th>\n");
    fprintf(fin, "\t\t<th class=\"player-division-h\">Division</th>\n");

    fprintf(fin, "\t\t</tr>\n");
    fprintf(fin, "\t\t</thead>\n");

    fprintf(fin, "\t\t<tbody>\n");
    for(int i = 0; i < players_list_length; i++) {
        fprintf(fin, "\t\t<tr>\n");
        
        fprintf(fin, "\t\t<td class=\"player-name\">%s</td>\n", players_list[i]->name);
        fprintf(fin, "\t\t<td class=\"player-kad\">%d/%d/%d</td>\n", players_list[i]->kills, players_list[i]->assistance, players_list[i]->deaths);
        fprintf(fin, "\t\t<td class=\"player-rating\">%d</td>\n", players_list[i]->rating);
        fprintf(fin, "\t\t<td class=\"player-division\">%s</td>\n", players_list[i]->division);

        fprintf(fin, "\t\t</tr>\n");
    }
    fprintf(fin, "\t\t</tbody>\n");

    fprintf(fin, "\t\t</table>\n");
    fprintf(fin, "\t</main>\n");
    fprintf(fin, "</body>\n");
    fprintf(fin, "</html>");

    fclose(fin);
}

int main(int argc, char ** argv) {
	char * gamesFilePath = 0;
	char * gamersFilePath = 0;
    char * htmlFilePath = 0;

    if(argc == 4) {
        gamesFilePath = argv[1];
        gamersFilePath = argv[2];
        htmlFilePath = argv[3];
    }

	if (gamesFilePath == 0 || gamersFilePath == 0 || htmlFilePath == 0) {
		printf("Chyba vstupu.\n");
		return 1;
	}
	//else {
	//	printf("Cesta k souboru her: %s\n", gamesFilePath);
    //  printf("Cesta k souboru hracu: %s\n", gamersFilePath);
	//	printf("Cesta k vystupnimu souboru html: %s\n", htmlFilePath);
	//}

    int gamers_list_length;
    //nacteni vsech hracu a nainicializovani pocatecnich hodnot
    Gamer ** gamers_list = readPlayers(gamersFilePath, &gamers_list_length);

    //precteni vsech her a aplikovani na kazdou hru funkci ktera updatuje state
    readGames(gamesFilePath, &gamers_list_length, gamers_list, gamers_list_length);
    
    //vysledek zapisuje do html souboru
    printHTML(htmlFilePath, gamers_list, gamers_list_length);

    for(int i = 0; i < gamers_list_length; i++) {
        free(gamers_list[i]);
    }
    free(gamers_list);

    return 0;
}