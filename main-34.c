#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char name[50];
    int health;
    int strength;
    int magic;
} Player;

typedef struct {
    char itemName[50];
    int strengthBonus;
    int magicBonus;
    char type[20];  
} Item;

Item itemsDatabase[9] = {
    {"Sword of Strength", 5, 0, "weapon"},
    {"Ice Sword", 5, 2, "weapon"},
    {"Health Amulet", 0, 0, "amulet"},
    {"Gauntlet of Fire", 3, 6, "weapon"},
    {"Staff of the Void", 0, 7, "weapon"},
    {"Almighty Sword", 7, 0, "weapon"},
    {"Decay Sword", 2, 3, "weapon"},
    {"Sword of Strength", 5, 0, "weapon"},
    {"Staff of Magic", 0, 5, "weapon"}
};

#define MAX_INVENTORY_SIZE 10

typedef struct {
    Item *weapon;  
    Item items[MAX_INVENTORY_SIZE];
    int itemCount;
} Inventory;

typedef struct {
    char name[50];
    int health;
    int strength;
    int magic;
    char ability[100];  
} Boss;

void initializePlayer(Player *player) {
    printf("Enter your character's name: ");
    if (fgets(player->name, sizeof(player->name), stdin)) {  
        player->name[strcspn(player->name, "\n")] = 0;  
    }
    player->health = 100;
    player->strength = 15;
    player->magic = 10;
}


void printPlayerStats(const Player *player) {
    printf("\nPlayer Stats:\n");
    printf("Name: %s\n", player->name);
    printf("Health: %d\n", player->health);
    printf("Strength: %d\n", player->strength);
    printf("Magic: %d\n", player->magic);
    printf("\n");
}

void equipWeapon(Player *player, Inventory *inventory, Item *newWeapon) {
    if (inventory->weapon != NULL) {
        player->strength -= inventory->weapon->strengthBonus;
        player->magic -= inventory->weapon->magicBonus;
        printf("Unequipping %s...\n", inventory->weapon->itemName);
    }
    inventory->weapon = newWeapon;
    player->strength += newWeapon->strengthBonus;
    player->magic += newWeapon->magicBonus;
    printf("Equipping %s...\n", newWeapon->itemName);
    printPlayerStats(player);
}

void addItemToInventory(Player *player, Inventory *inventory, const Item *item) {
    if (inventory->itemCount < MAX_INVENTORY_SIZE) {
        inventory->items[inventory->itemCount] = *item;
        printf("Added %s to your inventory.\n", item->itemName);
        inventory->itemCount++;

        if (strcmp(item->type, "weapon") == 0 && inventory->weapon == NULL) {
            equipWeapon(player, inventory, &inventory->items[inventory->itemCount - 1]);
        }
    } else {
        printf("Your inventory is full. Cannot add %s.\n", item->itemName);
    }
}

void displayInventory(const Inventory *inventory) {
    printf("Inventory:\n");
    for (int i = 0; i < inventory->itemCount; i++) {
        printf("%d: %s (Type: %s, Strength: %d, Magic: %d)\n", i, inventory->items[i].itemName, inventory->items[i].type, inventory->items[i].strengthBonus, inventory->items[i].magicBonus);
    }
    printf("\n");
}

void savePlayerData(const Player *player, const Inventory *inventory) {
    FILE *file = fopen("playerdata.txt", "w");
    if (file != NULL) {

        fprintf(file, "Player Name: %s\nHealth: %d\nStrength: %d\nMagic: %d\n", 
                player->name, player->health, player->strength, player->magic);

        int equippedWeaponIndex = -1;
        if (inventory->weapon) {
            for (int i = 0; i < inventory->itemCount; i++) {
                if (inventory->weapon == &inventory->items[i]) {
                    equippedWeaponIndex = i;
                    break;
                }
            }
        }
        fprintf(file, "Equipped Weapon Index: %d\n", equippedWeaponIndex);


        fprintf(file, "Inventory Item Count: %d\n", inventory->itemCount);
        for (int i = 0; i < inventory->itemCount; i++) {
            fprintf(file, "Item %d: %s, Strength Bonus: %d, Magic Bonus: %d, Type: %s\n", 
                    i, inventory->items[i].itemName, inventory->items[i].strengthBonus, 
                    inventory->items[i].magicBonus, inventory->items[i].type);
        }
        fclose(file);
    } else {
        printf("Failed to save game data.\n");
    }
}


void loadPlayerData(Player *player, Inventory *inventory) {
    FILE *file = fopen("playerdata.txt", "r");
    if (file != NULL) {
    
        if (fscanf(file, "Player Name: %[^\n]\n", player->name) == 1 &&
            fscanf(file, "Health: %d\nStrength: %d\nMagic: %d\n", &player->health, &player->strength, &player->magic) == 3) {
            int equippedWeaponIndex;
            if (fscanf(file, "Equipped Weapon Index: %d\n", &equippedWeaponIndex) == 1) {
                
                if (fscanf(file, "Inventory Item Count: %d\n", &inventory->itemCount) == 1 && inventory->itemCount <= MAX_INVENTORY_SIZE) {
                    for (int i = 0; i < inventory->itemCount; i++) {
                        if (fscanf(file, "Item %*d: %[^,], Strength Bonus: %d, Magic Bonus: %d, Type: %[^,\n]\n",
                                   inventory->items[i].itemName, &inventory->items[i].strengthBonus, &inventory->items[i].magicBonus, inventory->items[i].type) != 4) {
                            break;
                        }
                    }
                    if (equippedWeaponIndex != -1 && equippedWeaponIndex < inventory->itemCount) {
                        inventory->weapon = &inventory->items[equippedWeaponIndex];
                    } else {
                        inventory->weapon = NULL;
                    }
                }
            }
        } else {
            initializePlayer(player); 
        }
        fclose(file);
    } else {
        printf("No saved data found or unable to open file. Creating new character...\n");
        initializePlayer(player);
    }
}


void initializeBoss(Boss *boss) {
    strcpy(boss->name, "Dark Overlord");
    boss->health = 200;
    boss->strength = 25;
    boss->magic = 20;
    strcpy(boss->ability, "Fireball: deals extra magic damage");
}

void bossFight(Player *player, Boss *boss) {
    printf("\nA wild %s appears! %s\n", boss->name, boss->ability);

    while (boss->health > 0 && player->health > 0) {
        printf("Choose your action:\n1. Attack\n2. Heal\nEnter choice: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            int playerAttack = rand() % player->strength;
            boss->health -= playerAttack;
            printf("You attack the %s for %d damage! Boss remaining health: %d\n", boss->name, playerAttack, boss->health);

            if (boss->health > 0) {
                int bossAttack = rand() % boss->strength;
                player->health -= bossAttack;
                printf("The %s attacks you for %d damage! Your remaining health: %d\n", boss->name, bossAttack, player->health);
            }
        } else if (choice == 2) {
            player->health += 20;
            if (player->health > 100) player->health = 100;
            printf("You healed! Your health is now %d.\n", player->health);
        }

        if (boss->health <= 0) {
            printf("You have defeated the %s!\n", boss->name);
        }
        if (player->health <= 0) {
            printf("You have been defeated by the %s...\n", boss->name);
            break;
        }
    }
}

int main() {
    Player player;
    Inventory inventory = {0};
    char choice;
    int loop = 1, dropChance, drop;
    int monsterDefeats = 0;
  
  srand(time(NULL));
  loadPlayerData(&player, &inventory); 
  printPlayerStats(&player);

    while (loop) {
        if (player.health > 0) {
            printf("Do you want to fight a monster? (y/n): ");
            scanf(" %c", &choice);
            getchar(); 

            if (choice == 'y' || choice == 'Y') {
                int monsterHealth = 30;
                int monsterStrength = 10;
                while (monsterHealth > 0 && player.health > 0) {
                    int playerAttack = rand() % player.strength;
                    monsterHealth -= playerAttack;
                    printf("You hit the monster for %d damage!\n", playerAttack);

                    if (monsterHealth > 0) {
                        int monsterAttack = rand() % monsterStrength;
                        player.health -= monsterAttack;
                        printf("The monster hits you for %d damage! Your remaining health: %d\n", monsterAttack, player.health);
                    }

                    if (monsterHealth <= 0) {
                        printf("You defeated the monster!\n");
                        monsterDefeats++;
                        dropChance = rand() % 100;
                        if (dropChance < 30) {
                            drop = rand() % 9;
                            addItemToInventory(&player, &inventory, &itemsDatabase[drop]);
                            printf("You have found an item: %s\n", itemsDatabase[drop].itemName);
                            displayInventory(&inventory);
                        }

                        if (monsterDefeats == 5 || monsterDefeats == 7) {
                            Boss boss;
                            initializeBoss(&boss);
                            bossFight(&player, &boss);
                        }
                    }

                    if (player.health <= 0) {
                        printf("You are defeated by the monster...\n");
                        break;
                    }
                }
            } else if (choice == 'n' || choice == 'N') {
                printf("You chose not to fight. Saving game...\n");
                savePlayerData(&player, &inventory);
                loop = 0;
            } else {
                printf("Invalid input, please type 'y' or 'n'.\n");
            }
        }

        if (player.health <= 0) {
            printf("You have died. Saving your game before it ends...\n");
            savePlayerData(&player, &inventory);  

            printf("Would you like to start a new game? (y/n): ");
            scanf(" %c", &choice);
            getchar(); 

            if (choice == 'y' || choice == 'Y') {
                printf("Starting a new game...\n");
                memset(&inventory, 0, sizeof(inventory)); 
                initializePlayer(&player); 
                monsterDefeats = 0; 
            } else {
                printf("Exiting the game.\n");
                break; 
            }
        }
    }

    return 0;
}

